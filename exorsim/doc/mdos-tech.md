<h3>Filesystem</h3>

<p>MDOS uses single sided or double sided 8-inch soft sectored single
density diskettes with the IBM 3740 format: 77 cylinders of 26 sectors of
128 bytes.  2002 sectors for a single sided diskette and 4004 sectors for a
double sided diskette.</p>

<p>Space is allocated in clusters.  Each cluster has four sectors and is 512
bytes.</p>

<p>Total clusters on a single sided diskette is 500, with two unused sectors
left over at the end of the disk.  Double-sided diskettes have exactly 1001
clusters.</p>

<p>These sectors are reserved:</p>

<dl>
<dt>Sector 0<dd>Diskette ID block
<dt>Sector 1<dd>Cluster Allocation Table (or CAT) bitmap.  1 means
allocated.  Bit 7 of byte 0 is the first cluster.
<dt>Sector 2<dd>Lock-out cluster allocation table
<dt>Sectors 3 - 22<dd>Directory - 20 sectors
<dt>Sectors 23 - 24<dd>Boot block and MDOS RIB
</dl>

<p>Each directory entry is 16 bytes, so there can be up to 160 files on a
diskette.  Each directory entry looks like this:</p>

<dl>
<dt>0 - 7<dd>File name
<dt>8 - 9<dd>Suffix
<dt>10 - 11<dd>Sector number of first cluster
<dt>12 - 13<dd>File attributes
<dt>14 - 15<dd>Unused
</dl>

<p>Sectors within a file are addressed by LSN- Logical Sector Number.  LSN
$FFFF is the first sector of the first cluster.  It contains the the file's
RIB- Retrieval Information Block.  The first data sector is LSN $0000.</p>

<p>Files are composed of up to 57 contiguous "extents", where each extent
can have up to 32 clusters.  This works out to a maximum file size of 912 KB
(but the largest disk is 500.5 KB).</p>

<p>The RIB contains a list of SDWs- 16-bit words which give the starting
cluster number and size of each extent.  The RIB also contains the load
address and starting execution address of binary files.</p>
