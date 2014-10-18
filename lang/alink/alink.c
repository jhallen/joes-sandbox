/* Absolute linker for MS-DOS .OBJ files or Xenix .o files
   Copyright (C) 1991 Joseph H. Allen

ALINK is free software; you can redistribute it and/or modify it under the
terms of the GNU General Public License as published by the Free Software
Foundation; either version 1, or (at your option) any later version.  

ALINK is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
A PARTICULAR PURPOSE.  See the GNU General Public License for more details.  

You should have received a copy of the GNU General Public License along with
ALINK; see the file COPYING.  If not, write to the Free Software Foundation,
675 Mass Ave, Cambridge, MA 02139, USA.  */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>

/* Modes & Flags */

int quiet = 0;			/* Set to supress messages */
int dump = 0;			/* Set for debugging (object module dump) */
int errval = 0;			/* Set if there was an error */
char *outfile = NULL;		/* Output file name or NULL for no output file */
char *listfile = NULL;		/* List file name or NULL for no listing */

/* Safe malloc/realloc */

void *aalloc(unsigned size)
{
	void *rt;
	if (size)
		rt = malloc(size);
	else
		rt = malloc(size);
	if (rt == NULL) {
		fprintf(stderr, "Out of memory\n");
		exit(1);
	}
	return rt;
}

void *acalloc(unsigned x, unsigned y)
{
	void *rt;
	rt = calloc(x, y);
	if (rt == NULL) {
		fprintf(stderr, "Out of memory\n");
		exit(1);
	}
	return rt;
}

void *aealloc(void *blk, unsigned size)
{
	void *rt;
	if (blk == NULL)
		return aalloc(size);
	if (size)
		rt = realloc(blk, size);
	else
		rt = realloc(blk, 1);
	if (rt == NULL) {
		fprintf(stderr, "Out of memory\n");
		exit(1);
	}
	return rt;
}

/* List of object modules */

struct module {
	char *file;		/* File containing object module */
	char *name;		/* Name of object module */
	unsigned long begin;	/* Offset into file where object module begins */
} **modules;
unsigned nmodules = 0;

/* List of libraries */

#define LTSIZE 1024

struct library {
	char *file;		/* Name of file containing library */
	unsigned pgsize;	/* Library page size */
	unsigned size;		/* Size of dictionary */
	struct entry *htab[LTSIZE];	/* Hash table */
} **libraries;
unsigned nlibraries = 0;

struct entry {
	struct entry *next;
	char *name;
	unsigned val;
};

/* Hash function */

unsigned long hash(char *n)
{
        unsigned char *name = (unsigned char *)n;
	unsigned long accu = 0;
	while (*name)
		accu = accu * 34857 + *name++;
	return accu;
}

/* Copy a size/string field from .OBJ file to buffer.  String gets zero
 * terminated
 */

void ncpy(char *d, unsigned char *s)
{
	unsigned len = *s++;
	while (len--)
		*d++ = *(char *)s++;
	*d = 0;
}

/***********/
/* A label */
/***********/

unsigned nlabels = 0;

struct label {
	struct label *next;	/* Next label with same hash value */
	char *name;		/* Name of this label */
	struct module *module;	/* Object module where this was defined */
	/* NULL if not yet defined */
	int ext;		/* 0=name, 1=label, -1=undefined label */
	unsigned long offset;	/* Value of label */

	/* Segment label is relative to */
	struct segment *seg;

	/* Group label is relative to: must be zero if 'seg' is zero */
	/* What is this for? */
	struct segment *group;

	/* If 'seg' and 'group' above are zero, absolute paragraph number which label
	 * is relative to.  Usually (always?) this will be zero for EQU constants */
	unsigned frame;

	/* List of references to the label */
	/* Used to generate cross-reference listing */
	struct module **ref;
	unsigned nref;
	unsigned bksiz;
};

/***************************************/
/* A segment and/or group and/or class */
/***************************************/

struct segment {
	struct segment *next;	/* Next segment/group with same hash value */
	char *name;		/* Name of this segment/group */

	/* Segment bound to name */
	struct module *module;	/* Module where this was first defined */
	/* NULL if no segment bound to this name */
	int alignment;		/* 0=Absolute, 1=byte, 2=word, 3=para, 4=page */
	int combine;		/* 0=private, 2,4,5,7=concatenate, 6=union */
	unsigned frame;		/* Paragraph number if alignment==0 */

	/* During first pass for a given module, 'nlength' is length added to
	 * segment for just that module.  'length' is length of segment from
	 * previous modules */
	unsigned long length;	/* Segment size */
	unsigned long nlength;	/* New length for current module */

	/* Same as above, but for second pass */
	unsigned long pos;	/* Current position within segment */
	unsigned long inc;	/* Increment to get next position */

	unsigned long addr;	/* Address assigned to this segment by 'position' */
	int set;		/* Set if address is assigned */

	unsigned long ldofst;	/* Segment gets placed in output file at addr+ldofst */
	/* This is for allowing ROM aliases */

	/* A 64K segment may only have 2K of data in it (for example).  These record
	 * where the the 2K is in the segment (for example).  This is done in the
	 * first pass.
	 */
	unsigned long start;	/* Lowest code offset in segment */
	unsigned long end;	/* Highest code offset in segment */

	/* This is allocated before second pass using above information.  Size of
	 * malloc block is end-start.  First byte in this block is absolute
	 * address addr+start.
	 */
	unsigned char *buf;	/* Buffer where code and data goes in second pass */

	/* Modules contributing to this segment */
	/* For listing */
	struct module **contrib;
	unsigned *contribo;
	int ncontrib;
	int contribp;

	/* Group bound to name */
	struct module *gmodule;	/* obj file where this was defined/NULL if undefined */
	struct segment **seglst;	/* Malloc array of segments in the group */
	unsigned lstsiz;	/* Number of segments in the group */
	unsigned long gaddr;	/* Group base address.  This is the 'addr' of the
				   lowest segment in the group */

	/* Class bound to name */
	struct segment **clst;	/* List of segments in class */
	unsigned csiz;		/* Number of segments in list */
};

/* Hash table of labels */

#define NLABELS 512
struct label *labels[NLABELS];

/* Label lookup function.  Creates new structure if requested label is not
 * found */

struct label *labelfind(char *name)
{
	unsigned hval = (hash(name) % NLABELS);
	struct label *l;
	for (l = labels[hval]; l; l = l->next)
		if (!strcmp(name, l->name))
			return l;
	l = (struct label *) acalloc(1, sizeof(struct label));
	l->name = strdup(name);
	l->next = labels[hval];
	if (listfile) {
		l->ref = (struct module **) aalloc((l->bksiz = 10) * sizeof(struct module *));
		l->nref = 0;
	}
	++nlabels;
	labels[hval] = l;
	return l;
}

/* Hash table of segments/groups */

#define NSEGMENTS 512
struct segment *segments[NSEGMENTS];

/* Group/ public segment lookup function.  Creates new structure if given
 * name is not found */

struct segment *segmentfind(char *name)
{
	unsigned hval = (hash(name) % NSEGMENTS);
	struct segment *l;
	for (l = segments[hval]; l; l = l->next)
		if (!strcmp(name, l->name))
			return l;
	l = (struct segment *) acalloc(1, sizeof(struct segment));
	l->start = 0xffffffff;
	l->name = strdup(name);
	l->next = segments[hval];
	segments[hval] = l;
	return l;
}

/* Private segment creation function.  'seg' is (possible unused) public
 * segment structure bound to private segment name.  'obj' is name of object
 * file where private segment is defined */

struct segment *segprivate(struct segment *seg, struct module *obj)
{
	struct segment *l;
	l = (struct segment *) acalloc(1, sizeof(struct segment));
	l->name = seg->name;
	l->module = obj;
	l->next = seg->next;
	seg->next = l;
	return l;
}

/* Find private segment */

struct segment *private1find(char *name, struct module *module)
{
	struct segment *seg;
	for (seg = segmentfind(name); seg != NULL; seg = seg->next)
		if (!strcmp(name, seg->name) && module == seg->module)
			return seg;
	return 0;
}

/* Find private segment: name should be given as SEGMENT(MODULE) */

struct segment *privatefind(char *name)
{
	struct segment *seg = 0;
	int x, y, z;
	char c1, c2;
	for (x = 0; name[x] && name[x] != '('; ++x);
	if (!name[x])
		return 0;
	c1 = name[x];
	name[x] = 0;
	++x;
	for (y = x; name[y] && name[y] != ')'; ++y);
	c2 = name[y];
	name[y] = 0;
	for (z = 0; z != nmodules; ++z)
		if (!strcmp(modules[z]->name, name + x))
			break;
	if (z != nmodules)
		seg = private1find(name, modules[z]);
	name[x - 1] = c1;
	name[y] = c2;
	return seg;
}

/************************************/
/* Macros to help access .obj files */
/************************************/

#define SIZ(x) (pgm[x]+(pgm[x+1]<<8))
#define SIZINC(x) ((x)+=2, pgm[x-2]+(pgm[x-1]<<8))
#define IDX(x) ((pgm[x]&0x80)?((pgm[x]&0x7F)<<8)+pgm[x+1]:pgm[x])
#define IDXINC(x) \
 ((pgm[x]&0x80)?(x+=2,((pgm[x-2]&0x7F)<<8)+pgm[x-1]):pgm[(x)++])

/**********************************************************/
/* Indexable array of names relative to current .obj file */
/* (Limits are those imposed by LINK)                     */
/**********************************************************/

struct segment *namearray[256];
int nameidx;

struct segment *grouparray[22];
int groupidx;

struct segment *segarray[256];
int segidx;

struct label *externarray[1024];
int externidx;

/* Function to determine size of repeated data block */

unsigned iteratedsz(unsigned char *pgm, unsigned *x, int n)
{
	unsigned amnt = 0;
	while (n--) {
		unsigned rpt = SIZINC(*x);
		unsigned blk = SIZINC(*x);
		if (!blk) {
			amnt += rpt * pgm[*x];
			*x += pgm[*x] + 1;
		} else
			amnt += iteratedsz(pgm, x, blk);
	}
	return amnt;
}

/* Function to generate repeated data block */

void iterated(unsigned char *pgm, unsigned *x, int n, unsigned char **ptr)
{
	while (n--) {
		unsigned rpt = SIZINC(*x);
		unsigned blk = SIZINC(*x);
		if (!blk) {
			while (rpt--)
				memcpy(*ptr, pgm + *x + 1, pgm[*x]), *ptr += pgm[*x];
			*x += pgm[*x] + 1;
		} else
			iterated(pgm, x, blk, ptr);
	}
}

/* Function to add stupid common variables */

void addbss(char *name, struct module *module, unsigned long size)
{
	struct segment *seg = segmentfind("_bss");
	struct label *pub;
	if (seg->module == NULL) {
		seg->module = module;
		seg->alignment = 2;
		seg->combine = 2;
	}
	pub = labelfind(name);
	if (pub->module == NULL) {
		/* Alignment? */
		pub->module = module;
		pub->offset = seg->length + seg->nlength;
		seg->nlength += size;
		pub->seg = seg;
	}
}

void addfar(char *name, struct module *module, unsigned long size)
{
	struct segment *seg = segmentfind("_bss");
	struct label *pub;
	if (seg->module == NULL) {
		seg->module = module;
		seg->alignment = 2;
		seg->combine = 2;
	}
	pub = labelfind(name);
	if (pub->module == NULL) {
		/* Alignment? */
		pub->module = module;
		pub->offset = seg->length + seg->nlength;
		seg->nlength += size;
		pub->seg = seg;
	}
}

unsigned char *pgm = 0;
unsigned pgmbksiz = 0;

void pass1(FILE * fd, char *objname)
{
	struct module *module;
	unsigned x, nxt;
	char buf[256];
	int d;

	module = (struct module *) aalloc(sizeof(struct module));
	module->file = objname;
	module->begin = ftell(fd);
	if (!nmodules)
		modules = (struct module **) aalloc(sizeof(struct module *) * ++nmodules);
	else
		modules = (struct module **) aealloc(modules, sizeof(struct module *) * ++nmodules);
	modules[nmodules - 1] = module;

	/* Clear indexes */
	externidx = segidx = groupidx = nameidx = 0;

      loop:
	d = getc(fd);
	if (d == -1)
		return;
	nxt = getc(fd);
	nxt += (getc(fd) << 8);
	if (nxt > pgmbksiz)
		pgm = (unsigned char *) aealloc(pgm, pgmbksiz = nxt);
	fread(pgm, 1, nxt, fd);
	--nxt;
	x = 0;

	/* Process .OBJ file */
	switch (d) {
	case 0x88:		/* Comment */
	case 0x8E:		/* Type definition */
	case 0x94:		/* Line number */
	case 0x9C:		/* Fixup (skip on first pass) */
		break;		/* Just ignore 'em */

	case 0x80:		/* Module name */
		ncpy(buf, pgm);
		module->name = strdup(buf);
		break;

	case 0xA0:		/* Code/Data (only record start & size) */
		{
			unsigned sz = nxt;
			unsigned ox = x;
			struct segment *seg = segarray[IDXINC(x) - 1];
			unsigned ofst = SIZINC(x);
			sz -= x - ox;
			if (ofst + seg->length < seg->start)
				seg->start = ofst + seg->length;
			if (ofst + sz + seg->length > seg->end)
				seg->end = ofst + sz + seg->length;
		}
		break;

	case 0xA2:		/* 'Dup' data (only record start & size) */
		{
			struct segment *seg = segarray[IDXINC(x) - 1];
			unsigned ofst = SIZINC(x);
			unsigned sz = iteratedsz(pgm, &x, 1);
			if (ofst + seg->length < seg->start)
				seg->start = ofst + seg->length;
			if (ofst + sz + seg->length > seg->end)
				seg->end = ofst + sz + seg->length;
		}
		break;

	case 0x8A:		/* End of module */
		{
			int z;
			for (z = 0; z != segidx; ++z) {
				segarray[z]->length += segarray[z]->nlength, segarray[z]->nlength = 0;
				if (segarray[z]->length > 65536)
					fprintf(stderr, "Segment overflow \'%s\'\n", segarray[z]->module->name), errval = 1;
			}
		}
		return;

	case 0x96:		/* List of names (segment, group or class) */
		for (; x != nxt; x += pgm[x] + 1) {
			ncpy(buf, pgm + x);
			namearray[nameidx++] = segmentfind(buf);
		}
		break;

	case 0x8C:		/* External names */
		for (; x != nxt; x += pgm[x] + 1, IDXINC(x)) {
			struct label *label;
			ncpy(buf, pgm + x);
			label = externarray[externidx++] = labelfind(buf);
			if (!label->ext)
				label->ext = 1;
			if (listfile != NULL) {
				if (label->bksiz == label->nref)
					label->ref = (struct module **)
					    aealloc(label->ref, (label->bksiz += 20) * sizeof(struct module *));
				label->ref[label->nref++] = module;
			}
		}
		break;

	case 0x9A:		/* Group definition */
		{
			struct segment *grp;
			unsigned y = IDXINC(x);
			grp = namearray[y - 1];
			if (grp->seglst != NULL) {
				for (y = 0; x != nxt; ++y) {
					unsigned z;
					struct segment *seg = segarray[++x, IDXINC(x) - 1];
					for (z = 0; z != grp->lstsiz; ++z)
						if (grp->seglst[z] == seg)
							goto over;
					grp->seglst = (struct segment **)
					    aealloc(grp->seglst, sizeof(struct segment *) * ++grp->lstsiz);
					grp->seglst[z] = seg;
				      over:;
				}
			} else {
				unsigned ox = x;
				for (y = 0; x != nxt; ++x, IDXINC(x))
					++y;
				grp->lstsiz = y;
				grp->seglst = (struct segment **) acalloc(y, sizeof(struct segment *));
				for (y = 0, x = ox; x != nxt;)
					grp->seglst[y++] = segarray[++x, IDXINC(x) - 1];
				grp->gmodule = module;
			}
			grouparray[groupidx++] = grp;
		}
		break;

	case 0x98:		/* Segment definition */
		{
			int align, combine;
			unsigned frame = 0;
			int ofst = 0;
			long len = 0;
			int nidx, cidx, oidx;
			struct segment *seg;
			align = (pgm[x] >> 5);
			combine = (7 & (pgm[x] >> 2));
			if (pgm[x] & 2)
				len = 0x10000;
			++x;
			if (!align)
				frame = SIZINC(x), ofst = pgm[x++];
			if (!len)
				len = SIZ(x);
			x += 2;
			nidx = IDXINC(x);
			cidx = IDXINC(x);
			oidx = IDXINC(x);

			seg = namearray[nidx - 1];
			if (!combine) {	/* Private segment */
				struct segment *class;
				int z;
				seg = segprivate(seg, module);
				segarray[segidx++] = seg;
				seg->alignment = align;
				seg->combine = combine;
				seg->frame = frame;
				seg->ncontrib++;
				seg->nlength = len;
				class = namearray[cidx - 1];
				for (z = 0; z != class->csiz; ++z)
					if (seg == class->clst[z])
						goto ovr;
				class->clst = (struct segment **) aealloc(class->clst, ++class->csiz * sizeof(struct segment *));
				class->clst[z] = seg;
			      ovr:;
			} else {
				segarray[segidx++] = seg;
				if (seg->module != NULL) {	/* Segment already defined.  Check for mismatch */
					if (align > seg->alignment && seg->alignment)
						seg->alignment = align;
					if (combine != seg->combine)
						fprintf(stderr,
							"Segment mismatch (combine) %s(%s) %s\n", seg->name, module->name, seg->module->name), errval = 1;
					if (frame != seg->frame)
						fprintf(stderr, "Segment mismatch (at) %s(%s) %s\n", seg->name, module->name, seg->module->name), errval = 1;
					if (seg->combine == 6) {
						if (len > seg->length)
							seg->length = len;
					} else if (len) {
						if (!seg->nlength) {
							switch (align) {
							case 2:
								seg->length = ((seg->length + 1) & ~1);
								break;
							case 3:
								seg->length = ((seg->length + 15) & ~15);
								break;
							case 4:
								seg->length = ((seg->length + 255)
									       & ~255);
								break;
                                                        }
						} else {
							switch (align) {
							case 2:
								seg->nlength = ((seg->nlength + 1) & ~1);
								break;
							case 3:
								seg->nlength = ((seg->nlength + 15) & ~15);
								break;
							case 4:
								seg->nlength = ((seg->nlength + 255)
										& ~255);
								break;
							}
                                                }
                                        }
					seg->nlength += len;
					seg->ncontrib++;
				} else {	/* New public segment */
					struct segment *class;
					int z;
					seg->alignment = align;
					seg->combine = combine;
					seg->frame = frame;
					seg->nlength = len;
					seg->module = module;
					seg->ncontrib++;
					class = namearray[cidx - 1];
					for (z = 0; z != class->csiz; ++z)
						if (seg == class->clst[z])
							goto ovr1;
					class->clst = (struct segment **)
					    aealloc(class->clst, ++class->csiz * sizeof(struct segment *));
					class->clst[z] = seg;
				      ovr1:;
				}
			}
		}
		break;

	case 0x90:		/* Public names */
		{
			unsigned gidx;
			unsigned sidx;
			unsigned frame = 0;
			gidx = IDXINC(x);
			sidx = IDXINC(x);
			if (!gidx && !sidx)
				frame = SIZINC(x);
			while (x != nxt) {
				struct label *label;
				ncpy(buf, pgm + x);
				x += pgm[x] + 1;
				label = labelfind(buf);
				if (label->module != NULL)
					fprintf(stderr, "Multiply defined symbol: %s(%s) (%s)\n", label->name, module->name, label->module->name), errval = 1;
				else {
					label->frame = frame;
					label->module = module;
					if (sidx) {
						label->seg = segarray[sidx - 1];
						label->offset = SIZ(x) + label->seg->length;
					} else
						label->offset = SIZ(x);
					if (gidx)
						label->group = grouparray[gidx - 1];
				}
				x += 2;
				IDXINC(x);
			}
		}
		break;

	case 0xB0:		/* Common variable */
		while (x != nxt) {
			ncpy(buf, pgm + x);
			x += pgm[x] + 1;
			IDXINC(x);
			if (pgm[x++] == 0x61) {
				unsigned long size;
				if (!pgm[x & 0x80])
					size = pgm[x++];
				else
					switch (pgm[x++]) {
					case 0x81:
						size = pgm[x] + (pgm[x + 1] << 8);
						x += 2;
						break;
					case 0x84:
						size = pgm[x] + (pgm[x + 1] << 8) + (pgm[x + 2] << 16);
						x += 3;
						break;
					case 0x88:
						size = pgm[x] + (pgm[x + 1] << 8) + (pgm[x + 2] << 16) + (pgm[x + 3] << 24);
						x += 4;
						break;
					}
				addbss(buf, module, size);
			} else {
				unsigned long size;
				unsigned long amnt;
				if (!pgm[x & 0x80])
					size = pgm[x++];
				else
					switch (pgm[x++]) {
					case 0x81:
						size = pgm[x] + (pgm[x + 1] << 8);
						x += 2;
						break;
					case 0x84:
						size = pgm[x] + (pgm[x + 1] << 8) + (pgm[x + 2] << 16);
						x += 3;
						break;
					case 0x88:
						size = pgm[x] + (pgm[x + 1] << 8) + (pgm[x + 2] << 16) + (pgm[x + 3] << 24);
						x += 4;
						break;
					}
				if (!pgm[x & 0x80])
					amnt = pgm[x++];
				else
					switch (pgm[x++]) {
					case 0x81:
						amnt = pgm[x] + (pgm[x + 1] << 8);
						x += 2;
						break;
					case 0x84:
						amnt = pgm[x] + (pgm[x + 1] << 8) + (pgm[x + 2] << 16);
						x += 3;
						break;
					case 0x88:
						amnt = pgm[x] + (pgm[x + 1] << 8) + (pgm[x + 2] << 16) + (pgm[x + 3] << 24);
						x += 4;
						break;
					}
				size *= amnt;
				addfar(buf, module, size);
			}
		}
		break;

	default:
		fprintf(stderr, "Unknown record type %2.2X in object module \'%s\'\n", d, module->name);
		errval = 1;
	}
	goto loop;
}

/* Target types */

char *targtab[] = {
	"a byte", "an ofst", "a seg", "a seg:ofst", "? illegal", "an ofst",
	"? 6", "? 7"
};

/* Linker 2nd pass.  Process single module */

void dopass2(FILE * fd, struct module *module)
{
	unsigned x, nxt;
	char buf[256];
	struct segment *last;
	unsigned char *lastptr;
	int d;

	struct {
		int method;
		struct segment *seg;
		struct label *label;
	} threads[8];

	/* Clear indexes */
	externidx = segidx = groupidx = nameidx = 0;

      loop:
	d = getc(fd);
	if (d == -1)
		return;
	nxt = getc(fd), nxt += (getc(fd) << 8);
	fread(pgm, 1, nxt, fd);
	--nxt;
	x = 0;

	/* Process .OBJ file */
	switch (d) {
	case 0x80:		/* Module name */
	case 0x88:		/* Comment */
	case 0x8E:		/* Type definition */
	case 0x94:		/* Line number */
	case 0x90:		/* Public name */
	case 0xB0:		/* Common variable */
		break;		/* Just ignore 'em */

	case 0xA0:		/* Code/Data */
		{
			unsigned sz = nxt;
			unsigned ox = x;
			struct segment *seg = segarray[IDXINC(x) - 1];
			unsigned ofst = SIZINC(x);
			last = seg;
			sz -= x - ox;
			lastptr = seg->buf + ofst + seg->pos - seg->start;
			memcpy(seg->buf + ofst + seg->pos - seg->start, pgm + x, sz);
		}
		break;

	case 0xA2:		/* 'Dup' data */
		{
			struct segment *seg = segarray[IDXINC(x) - 1];
			unsigned ofst = SIZINC(x);
			unsigned char *ptr = seg->buf + ofst + seg->pos - seg->start;
			last = seg;
			lastptr = ptr;
			iterated(pgm, &x, 1, &ptr);
		}
		break;

	case 0x9c:		/* Fixup */
		{
			unsigned long seg, off, frame;
			while (x != nxt) {	/* Process each subrecord... */
				char c = pgm[x++];
				if (!(c & 128)) {	/* We have a thread field.  Record thread. */
					int idx = (c & 3) + (4 & (c >> 4));	/* Calculate thread no. so that
										   0-3 are for targets and 4-7 are
										   for frames */
					threads[idx].method = (7 & (c >> 2));	/* Save method code */
					if (!(c & 64))
						threads[idx].method &= 3;	/* Upper bit of Thread method comes
										   from fixup.  It should be zero
										   here. */
					/* Get method data */
					switch (threads[idx].method) {
					case 0:	/* Segment */
						threads[idx].seg = segarray[IDXINC(x) - 1];
						break;
					case 1:	/* Group */
						threads[idx].seg = grouparray[IDXINC(x) - 1];
						break;
					case 2:	/* Label */
						threads[idx].label = externarray[IDXINC(x) - 1];
						break;
					}
				} else {	/* We have a fixup field */
					/* Fixup 'datum' is the object that we are going modify.
					   'Target' is the value we are going to stick into the object.
					   'Frame' is what we need to make 'Target' relative to. */
					int locat = pgm[x++] + ((c & 3) << 8);	/* Offset to datum to be fixed */
					int rel = !(c & 64);	/* rel is set for self-relative fix */
					int what = (7 & (c >> 2));	/* Type of datum being fixed */
					int fmethod;	/* Method for calculating frame */
					struct segment *fseg;
					struct label *flabel;
					int tmethod;	/* Method for calculating target */
					struct segment *tseg;
					struct label *tlabel;
					unsigned disp;	/* Displacement */
					c = pgm[x++];

					/* Get Frame */
					if (c & 128) {	/* It comes from a thread */
						int idx = ((c >> 4) & 3) + 4;
						fmethod = threads[idx].method;
						fseg = threads[idx].seg;
						flabel = threads[idx].label;
					} else {	/* It's defined here */
						fmethod = (c >> 4);
						switch (fmethod) {
						case 0:	/* Frame is segment address */
							fseg = segarray[IDXINC(x) - 1];
							break;
						case 1:	/* Frame is group address */
							fseg = grouparray[IDXINC(x) - 1];
							break;
						case 2:	/* Frame is a symbol */
							flabel = externarray[IDXINC(x) - 1];
							break;
						}
					}

					/* Get target */
					if (c & 8) {	/* It comes from a thread */
						int idx = c & 3;
						tmethod = threads[idx].method + (c & 4);
						tseg = threads[idx].seg;
						tlabel = threads[idx].label;
					} else {	/* It's defined here */
						tmethod = (c & 7);
						switch (tmethod & 3) {
						case 0:	/* Target is segment address */
							tseg = segarray[IDXINC(x) - 1];
							break;
						case 1:	/* Target is group address */
							tseg = grouparray[IDXINC(x) - 1];
							break;
						case 2:	/* Target is a label */
							tlabel = externarray[IDXINC(x) - 1];
							break;
						}
					}
					/* Get displacement if it's not zero */
					if (tmethod >= 4)
						tmethod -= 4, disp = 0;
					else
						disp = SIZINC(x);

					/* Handle frame methods 4 & 5 (method 5 needed the target) */
					switch (fmethod) {
					case 4:	/* Frame is segment containing fixup datum */
						fmethod = 0;
						fseg = last;
						break;
					case 5:	/* Frame is same as target */
						fmethod = tmethod;
						fseg = tseg;
						flabel = tlabel;
						break;
					}

					/* Now we have all information needed to do the fixup */
					if (dump) {
						if (dump)
							printf
							    ("Fixup %s at %lx %s",
							     targtab[what], locat + last->pos + last->addr, (rel ? "self-rel" : "seg-rel"));
						switch (fmethod) {
						case 0:
							printf(" frame=segment %s", fseg->name);
							break;
						case 1:
							printf(" frame=group %s", fseg->name);
							break;
						case 2:
							printf(" frame=label %s", flabel->name);
							break;
						}
						switch (tmethod) {
						case 0:
							printf(" target=segment %s disp=%x", tseg->name, disp);
							break;
						case 1:
							printf(" target=group %s disp=%x", tseg->name, disp);
							break;
						case 2:
							printf(" target=label %s disp=%x", tlabel->name, disp);
							break;
						}
						printf("\n");
					}

					/* Get target segment/offset */
					switch (tmethod) {
					case 0:
						seg = (tseg->addr >> 4);
						off = tseg->pos;	/* Should this be zero? */
						break;
					case 1:
						seg = (tseg->gaddr >> 4);
						off = tseg->pos;	/* Should this be zero? */
						break;
					case 2:
						if (tlabel->seg != NULL) {
							seg = (tlabel->seg->addr >> 4);
							off = tlabel->offset;
						} else {
							seg = tlabel->frame;
							off = tlabel->offset;
						}
					}

					/* Get frame segment/offset */
					switch (fmethod) {
					case 0:
						frame = (fseg->addr >> 4);
						break;
					case 1:
						frame = (fseg->gaddr >> 4);
						break;
					case 2:
						if (flabel->seg != NULL)
							if (flabel->group != NULL)
								frame = (flabel->group->gaddr >> 4);
							else
								frame = (flabel->seg->addr >> 4);
						else
							frame = flabel->frame;
						break;
					}

					if (dump)
						printf("Before xlat: Target=%lx:%lx Frame=%lx\n", seg, off, frame);

					/* Make seg/off relative to frame */
					off += (seg - frame) * 16;
					/* Add in displacement */
					off += disp;
					/* Record fact that off is now relative to frame */
					seg = frame;

					if (dump)
						printf("After xlat: Target=%lx:%lx\n", seg, off);

					/* Adjust for self relative */
					/* Segment for this had better match segment fixup is for... */
					if (rel) {
						/* Boy is this stupid.  Masm should stick these offsets in the insn. */
						switch (what) {
						case 5:
						case 1:	/* Offset */
							off -= 2;
							break;

						case 0:	/* Low-byte */
							off -= 1;
							break;

						case 2:	/* Segment */
							off -= 2;
							break;

						case 3:	/* seg:ofset */
							off -= 4;
							break;
						}

						/* We're self relative: subtract insn's offset (locat+last->pos) relative
						   to insn's frame with respect to target's frame.  */
						off -= locat + last->pos + ((last->addr >> 4) - seg) * 16;
					}

					if (dump)
						printf("After rel: Target=%lx:%lx\n", seg, off);

					/* Add datum to seg/off.  Tasm stores offsets in the image,
					 * Masm stores offsets in the fixup */
					switch (what) {
					case 5:
					case 1:
						off += (lastptr[locat + 1] << 8);

					case 0:
						off += lastptr[locat];
						break;

					case 2:
						seg += lastptr[locat] + (lastptr[locat + 1] << 8);
						break;

					case 3:
						off += (lastptr[locat + 1] << 8);
						off += lastptr[locat];
						seg += lastptr[locat + 2] + (lastptr[locat + 3] << 8);
						break;
					}

					/* Store final target value in datum */
					switch (what) {
						/* Fix an offset */
					case 5:
					case 1:
						lastptr[locat + 1] = (off >> 8);
						lastptr[locat] = off;
						break;

						/* Fix only low byte of an offset */
					case 0:
						lastptr[locat] = off;
						break;

						/* Fix a segment */
					case 2:
						lastptr[locat] = seg;
						lastptr[locat + 1] = (seg >> 8);
						break;

						/* Fix a pointer */
					case 3:
						lastptr[locat] = off;
						lastptr[locat + 1] = (off >> 8);
						lastptr[locat + 2] = seg;
						lastptr[locat + 3] = (seg >> 8);
						break;

					default:
						fprintf(stderr, "Unknown fixup object\n"), errval = 1;
						break;
					}
				}
			}
		}
		break;

	case 0x8A:		/* End of module */
		{
			int z;
			for (z = 0; z != segidx; ++z)
				segarray[z]->pos += segarray[z]->inc, segarray[z]->inc = 0;
		}
		return;

	case 0x96:		/* List of names (segment, group or class) */
		for (; x != nxt; x += pgm[x] + 1) {
			ncpy(buf, pgm + x);
			namearray[nameidx++] = segmentfind(buf);
		}
		break;

	case 0x8C:		/* External names */
		for (; x != nxt; x += pgm[x] + 1, IDXINC(x)) {
			ncpy(buf, pgm + x);
			externarray[externidx++] = labelfind(buf);
		}
		break;

	case 0x9A:		/* Group definition */
		{
			struct segment *grp;
			unsigned y = IDXINC(x);
			grp = namearray[y - 1];
			grouparray[groupidx++] = grp;
		}
		break;

	case 0x98:		/* Segment definition */
		{
			int align, combine;
			unsigned frame = 0;
			int ofst = 0;
			long len = 0;
			int nidx, cidx, oidx;
			struct segment *seg;
			align = (pgm[x] >> 5);
			combine = (7 & (pgm[x] >> 2));
			if (pgm[x] & 2)
				len = 0x10000;
			++x;
			if (!align)
				frame = SIZINC(x), ofst = pgm[x++];
			if (!len)
				len = SIZ(x);
			x += 2;
			nidx = IDXINC(x);
			cidx = IDXINC(x);
			oidx = IDXINC(x);

			seg = namearray[nidx - 1];
			if (!combine)
				seg = private1find(seg->name, module);
			segarray[segidx++] = seg;
			seg->contrib[seg->contribp++] = module;

			if (seg->combine != 6) {
				seg->contribo[seg->contribp - 1] = seg->pos;
				if (len) {
					if (!seg->inc) {
						switch (align) {
						case 2:
							seg->pos = ((seg->pos + 1) & ~1);
							break;
						case 3:
							seg->pos = ((seg->pos + 15) & ~15);
							break;
						case 4:
							seg->pos = ((seg->pos + 255) & ~255);
							break;
                                                }
					} else {
						switch (align) {	/* I think this can never happen */
							/* Except maybe with common blocks */
						case 2:
							seg->inc = ((seg->inc + 1) & ~1);
							break;
						case 3:
							seg->inc = ((seg->inc + 15) & ~15);
							break;
						case 4:
							seg->inc = ((seg->inc + 255) & ~255);
							break;
						}
                                        }
                                }
				seg->inc += len;
			} else
				seg->contribo[seg->contribp - 1] = 0;
		}
		break;

	}
	goto loop;
}

/* Second pass of linker */

void pass2(void)
{
	int x;
	for (x = 0; x != nmodules; ++x) {
		FILE *fd = fopen(modules[x]->file, "rb");
		fseek(fd, modules[x]->begin, 0);
		dopass2(fd, modules[x]);
		fclose(fd);
	}
}

/* Show segments and classes */

void showsegs(void)
{
	struct segment *seg;
	int x;

	printf("\n");
	printf("Position the segments\n\n");
	printf("Each line should look like this: XXXXX[,YYYYY] NameList\n");
	printf("       Where: XXXXX is the starting address in hexadecimal\n");
	printf("              YYYYY is address you want segment loaded if different\n");
	printf("                    from XXXXX\n");
	printf("         and: NameList is space seperated list of segments or class names\n");
	printf("              Give private segments in this format: NAME(MODULE)\n");
	printf("\nThe segments will be placed in the order given in the NameList\n");
	printf("Entry will terminate when all segments are entered\n\n");

	printf("Segments:");
	for (x = 0; x != NSEGMENTS; ++x)
		for (seg = segments[x]; seg; seg = seg->next)
			if (seg->module != NULL) {
				printf(" %s", seg->name);
				if (!seg->combine)
					printf("(%s)", seg->module->name);
			}
	printf("\n");

	printf("Classes:");
	for (x = 0; x != NSEGMENTS; ++x)
		for (seg = segments[x]; seg; seg = seg->next)
			if (seg->clst != NULL) {
				int z;
				printf(" \'%s\'(", seg->name);
				for (z = 0; z != seg->csiz; ++z) {
					printf("%s", seg->clst[z]->name);
					if (!seg->clst[z]->combine)
						printf("(%s)", seg->clst[z]->module->name);
					if (z != seg->csiz - 1)
						printf(",");
				}
				printf(")");
			}
	printf("\n\n");
}

/* Output the linked program */

char zeros[1024];

void emit(char *outfile)
{
	FILE *fd;
	int x;
	unsigned long addr = 0;
	struct segment *seg, *lowseg = 0, *prevseg;
	unsigned long lowest;

	/* Find lowest segment */
	lowest = 0xffffffff;
	lowseg = 0;
	for (x = 0; x != NSEGMENTS; ++x)
		for (seg = segments[x]; seg; seg = seg->next)
			if (seg->buf != NULL && seg->addr + seg->ldofst + seg->start < lowest)
				lowseg = seg, lowest = seg->addr + seg->ldofst + seg->start;

	/* Write it out */
	fd = fopen(outfile, "wb");
	if (fd == NULL) {
		fprintf(stderr, "Couldn\'t open output file\n"), errval = 1;
		return;
	}

	if (lowseg != NULL) {
		prevseg = lowseg;
		fwrite(lowseg->buf, lowseg->end - lowseg->start, 1, fd);
		addr = lowest + lowseg->end - lowseg->start;
		lowseg->buf = 0;

	      loop:

		lowest = 0xffffffff;
		lowseg = 0;
		for (x = 0; x != NSEGMENTS; ++x)
			for (seg = segments[x]; seg; seg = seg->next)
				if (seg->buf != NULL && seg->addr + seg->ldofst + seg->start < lowest)
					lowseg = seg, lowest = seg->addr + seg->ldofst + seg->start;

		if (lowseg != NULL) {
			unsigned long naddr = lowseg->addr + lowseg->ldofst + lowseg->start;
			if (naddr < addr) {
				fprintf(stderr, "Segment \'%s\' overlaps segment \'%s\'\n", lowseg->name, prevseg->name);
				errval = 1;
			} else {
				prevseg = lowseg;
				while (naddr > addr + 1024) {
					fwrite(zeros, 1024, 1, fd);
					addr += 1024;
				}
				if (naddr != addr)
					fwrite(zeros, naddr - addr, 1, fd);
				addr = naddr;
				fwrite(lowseg->buf, lowseg->end - lowseg->start, 1, fd);
				addr += lowseg->end - lowseg->start;
			}
			lowseg->buf = 0;
			goto loop;
		}
	}

	fclose(fd);
}

/* Bind a segment to an address */

unsigned long bind(struct segment *seg, unsigned long addr, unsigned long ld)
{
	if (seg->set) {
		printf("Segment \'%s\' is already assigned\n", seg->name);
	} else {
		if (seg->alignment == 4)
			addr = ((addr + 255) & ~255);
		else
			addr = ((addr + 15) & ~15);
		seg->addr = addr;
		seg->ldofst = ld;
		addr += seg->length;
		seg->set = 1;
		if (seg->end > seg->start)
			seg->buf = (unsigned char *) aalloc(seg->end - seg->start);
	}
	return addr;
}

/* Position the segments */

void position(void)
{
	char buf[512];
	unsigned long addr;
	unsigned long ld;
	struct segment *seg;
	int x, y;

	/* Position absolute segments */
	/* And allocate contrib buffers */
	for (x = 0; x != NSEGMENTS; ++x)
		for (seg = segments[x]; seg; seg = seg->next)
			if (seg->module != NULL) {
				if (seg->alignment == 0)
					bind(seg, (unsigned long) seg->frame << 4, 0L);
				seg->contrib = (struct module **) aalloc(seg->ncontrib * sizeof(struct module *));
				seg->contribo = (unsigned *) aalloc(seg->ncontrib * sizeof(unsigned));
			}

	/* Get segments/classes to bind from stdin */
	goto start;
      loop1:
	if (!quiet)
		printf(">");
	fgets(buf, sizeof(buf) - 1, stdin);
	if (buf[0] && buf[strlen(buf) - 1] == '\n')
        	buf[strlen(buf) - 1] = 0;
	if (buf[0] && buf[strlen(buf) - 1] == '\r')
        	buf[strlen(buf) - 1] = 0;
	x = 0;
	while (buf[x] == ' ' || buf[x] == '\t')
		++x;
	for (y = x; buf[y] && buf[y] != ' ' && buf[y] != '\t'; ++y);
	sscanf(buf + x, "%lx", &addr);
	ld = addr;
	sscanf(buf + x, "%*x,%lx", &ld);
	ld -= addr;
      loop:
	for (x = y; buf[x] == ' ' || buf[x] == '\t'; ++x);
	for (y = x; buf[y] && buf[y] != ' ' && buf[y] != '\t'; ++y);
	if (x != y) {
		char c = buf[y];
		buf[y] = 0;
		seg = segmentfind(buf + x);
		if (seg->clst != NULL) {
			int z;
			for (z = 0; z != seg->csiz; ++z)
				addr = bind(seg->clst[z], addr, ld);
		} else {
			if (seg->module == NULL)
				seg = privatefind(buf + x);
			if (seg == NULL)
				printf("No such segment \'%s\'\n", buf + x);
			else
				addr = bind(seg, addr, ld);
		}
		buf[y] = c;
		goto loop;
	}
      start:
	for (x = 0; x != NSEGMENTS; ++x)
		for (seg = segments[x]; seg; seg = seg->next)
			if (seg->module != NULL && !seg->set)
				goto loop1;
}

/* Do group associations */

void groups(void)
{
	int x;
	struct segment *seg;
	for (x = 0; x != NSEGMENTS; ++x)
		for (seg = segments[x]; seg; seg = seg->next)
			if (seg->gmodule != NULL) {
				int y;
				unsigned long lowest = 0xFFFFF;
				unsigned long highest = 0;
				for (y = 0; y != seg->lstsiz; ++y) {
					if (seg->seglst[y]->addr < lowest)
						lowest = seg->seglst[y]->addr;
					if (seg->seglst[y]->addr + seg->seglst[y]->length > highest)
						highest = seg->seglst[y]->addr + seg->seglst[y]->length;
				}
				if (highest - lowest > 65536)
					fprintf(stderr, "Error: Group overflow \'%s\'\n", seg->name), errval = 1;
				seg->gaddr = lowest;
			}
}

/* Generate listing */

int cmplab(const void *x, const void *y)
{
	struct label **a = (struct label **) x;
	struct label **b = (struct label **) y;
	return strcmp((*a)->name, (*b)->name);
}

void show(char *listfile)
{
	int x, y;
	struct label *l;
	struct segment *seg;
	char sbuf[256];
	FILE *fd;
	struct label **lbls;
	fd = fopen(listfile, "w+");
	if (fd == NULL) {
		fprintf(stderr, "Couldn\'t open listing file\n"), errval = 1;
		return;
	}

	fprintf(fd, "Segments                Address Size Data at offsets\n--------                ------- ---- ---------------\n");
	for (x = 0; x != NSEGMENTS; ++x)
		for (seg = segments[x]; seg; seg = seg->next)
			if (seg->module != NULL) {
				int z;
				if (seg->start < seg->end)
					fprintf(fd, "%-23s %7.5lX %4lX (%lX-%lX)\n", seg->name, seg->addr, seg->length, seg->start, seg->end - 1);
				else
					fprintf(fd, "%-23s %7.5lX %4lX\n", seg->name, seg->addr, seg->length);
				for (z = 0; z != seg->contribp; ++z)
					fprintf(fd, "          %4x %s\n", seg->contribo[z], seg->contrib[z]->name);
			}

	fprintf(fd, "\nGroups                  Address Contents\n------                  ------- --------\n");
	for (x = 0; x != NSEGMENTS; ++x)
		for (seg = segments[x]; seg; seg = seg->next)
			if (seg->seglst != NULL) {
				int z;
				fprintf(fd, "%-23s %7.5lX", seg->name, seg->gaddr);
				for (z = 0; z != seg->lstsiz; ++z) {
					fprintf(fd, " %s", seg->seglst[z]->name);
					if (!seg->seglst[z]->combine)
						fprintf(fd, "(%s)", seg->seglst[z]->module->name);
				}
				fprintf(fd, "\n");
			}

	if (nlabels) {
		lbls = (struct label **) aalloc(nlabels * sizeof(char *));

		for (x = 0, y = 0; x != NLABELS; ++x)
			for (l = labels[x]; l; l = l->next)
				lbls[y++] = l;
		qsort(lbls, nlabels, sizeof(struct label *), cmplab);

		fprintf(fd, "\nLabels                  Ofst Segment References\n------                  ---- ------- ----------\n");
		for (x = 0; x != nlabels; ++x) {
			l = lbls[x];
			if (l->module != NULL)
				sprintf(sbuf, "%s(%s)", l->name, l->module->name);
			else
				sprintf(sbuf, "%s(UNDEFINED)", l->name);
			fprintf(fd, "%-23s %4lX", sbuf, l->offset);
			if (l->seg != NULL)
				if (!l->seg->combine)
					fprintf(fd, " %s(%s)", l->seg->name, l->seg->module->name);
				else
					fprintf(fd, " %7s", l->seg->name);
			else
				fprintf(fd, " %7X", l->frame);
			for (y = 0; y != l->nref; ++y)
				fprintf(fd, " %s", l->ref[y]->name);
			fprintf(fd, "\n");
		}
	}
	fclose(fd);
}

/* Note that a library was given on command line */

void library(FILE * fd, char *name)
{
	struct library *l;
	unsigned char *dict;
	unsigned long pos;
	l = (struct library *) acalloc(sizeof(struct library), 1);
	l->file = name;
	getc(fd);
	l->pgsize = getc(fd);
	l->pgsize += (getc(fd) << 8);
	l->pgsize += 3;
	pos = getc(fd);
	pos += (getc(fd) << 8);
	pos += ((long) getc(fd) << 16);
	pos += ((long) getc(fd) << 24);
	l->size = getc(fd);
	l->size += (getc(fd) << 8);
	l->size <<= 9;
	if (!l->size) {
		fprintf(stderr, "Library \'%s\' has no public symbols\n", name);
		errval = 1;
		free(l);
		return;
	}
	fseek(fd, pos, 0);
	dict = (unsigned char *) aalloc(l->size);
	fread(dict, 1, l->size, fd);
	libraries = (struct library **) aealloc(libraries, ++nlibraries * sizeof(struct library *));

	/* Make our own hash table out of library's dictionary */

	{
		int y, h, q;
		char buf[256];
		for (y = 0; y != l->size; y += 512) {
			for (h = 0; h != 37; ++h) {
				q = dict[y + h] * 2;
				if (q) {
					unsigned hval;
					struct entry *e;
					ncpy(buf, dict + y + q);
					hval = hash(buf);
					e = (struct entry *)
					    aalloc(sizeof(struct entry));
					e->name = strdup(buf);
					e->next = l->htab[hval % LTSIZE];
					l->htab[hval % LTSIZE] = e;
					q += dict[y + q] + 1;
					e->val = dict[y + q] + (dict[y + q + 1] << 8);
				}
			}
		}
	}
	free(dict);

	libraries[nlibraries - 1] = l;
}

/* Find symbol in libraries and if found, load module associated with it */

void getlib(char *name)
{
	int x;
	unsigned hval;
	struct library *l;
	struct entry *e;
	hval = hash(name) % LTSIZE;
	for (x = 0; x != nlibraries; ++x) {
		l = libraries[x];
		for (e = l->htab[hval]; e != NULL; e = e->next)
			if (!strcmp(name, e->name)) {
				unsigned long off;
				FILE *fd;
				off = e->val;
				off *= l->pgsize;
				fd = fopen(l->file, "rb");
				fseek(fd, off, 0);
				pass1(fd, l->file);
				fclose(fd);
				return;
			}
	}
}

/* Try to eliminate undefined symbols by looking in libraries */

void resolve(void)
{
	int x;
	struct label *label;
      loop:
	for (x = 0; x != NLABELS; ++x)
		for (label = labels[x]; label; label = label->next)
			if (label->ext == 1 && label->module == NULL) {
				getlib(label->name);
				if (label->module == NULL)
					fprintf(stderr, "Undefined symbol \'%s\'\n", label->name);
				label->ext = -1;
				goto loop;
			}
}

/* Process a file name */

void dofile(char *objname)
{
	FILE *fd = fopen(objname, "rb");
	int c;

	if (fd == NULL) {
		fprintf(stderr, "Couldn\'t open file \'%s\'\n", objname);
		errval = 1;
		return;
	}

	while (1)
		switch (c = getc(fd)) {
		case 0x80:
			ungetc(c, fd);
			pass1(fd, objname);
			break;

		case 0xF0:
			ungetc(c, fd);
			library(fd, objname);
			fclose(fd);
			return;

		case -1:
			fclose(fd);
			return;

		default:
			{
				int bksiz, len;
				char *name;
			      loop:
				name = (char *) aalloc(bksiz = 13);
				ungetc(c, fd);
				do
					c = getc(fd);
				while (c == ' ' || c == '\t' || c == '\r' || c == '\n');
				if (c != -1) {
					for (len = 0; c != -1 && c != ' ' && c != '\t' && c != '\r' && c != '\n'; c = getc(fd)) {
						if (len == bksiz - 1)
							name = (char *)
							    aealloc(name, bksiz += 13);
						name[len++] = c;
					}
					name[len] = 0;
					dofile(name);
					goto loop;
				}
				fclose(fd);
				return;
			}
		}
}

int main(int argc, char *argv[])
{
	if (argc < 2)
		fprintf(stderr, "\
 Absolute linker for generating simple binary image files - Version 1.1\n\
 Written by Joseph H. Allen\n\
 \n\
 Syntax: alink [-q] [-d] [-l listfile] [-o outputfile] files\n\
 \n\
 '-q'     Suppresses messages\n\
 '-d'     Debug mode (dumps object modules)\n\
 \n\
 'files'  List names of object files and libraries to link together.  May\n\
          also include text files containing whitespace separated lists of\n\
          more file names\n\
 "), errval = 1;
	else {
		int x;
		for (x = 1; argv[x]; ++x)
			if (!strcmp(argv[x], "-q"))
				quiet = 1;
			else if (!strcmp(argv[x], "-l"))
				if (argv[x + 1] == NULL)
					fprintf(stderr, "Missing listing file name\n"), exit(1);
				else
					listfile = argv[++x];
			else if (!strcmp(argv[x], "-d"))
				dump = 1;
			else if (!strcmp(argv[x], "-o"))
				if (argv[x + 1] == NULL)
					fprintf(stderr, "Missing output file name\n"), exit(1);
				else
					outfile = argv[++x];
			else
				dofile(argv[x]);	/* First pass process file */
		resolve();	/* Link in necessary library object modules */
		if (!quiet)
			showsegs();	/* List segments for user */
		position();	/* Get segment address and ordering info. */
		groups();	/* Determine group addresses */
		pass2();	/* Load and Fixup object modules */
		if (listfile != NULL)
			show(listfile);	/* Generate listing file */
		if (outfile != NULL)
			emit(outfile);	/* Emit output file */
	}
	return errval;
}
