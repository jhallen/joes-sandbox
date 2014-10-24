;Master Boot Record
;Copyright (C) 1998 Joseph H. Allen

;This file is part of MBR (Master Boot Record)

;MBR is free software; you can redistribute it and/or modify it under the 
;terms of the GNU General Public License as published by the Free Software 
;Foundation; either version 1, or (at your option) any later version.  

;MBR is distributed in the hope that it will be useful, but WITHOUT ANY 
;WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS 
;FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more 
;details.  

;You should have received a copy of the GNU General Public License along with 
;MBR; see the file COPYING.  If not, write to the Free Software Foundation, 
;675 Mass Ave, Cambridge, MA 02139, USA.  */ 

; Assemble with MASM: ml /c boot.asm
; Link with ALINK (get from ftp://ftp.worcester.com/pub/joe/alink.tar.Z)
; Set segments to: 0 SEGZERO
;                  600 SEGCODE
;                  7B0 SEGPART
;                  7C00 SEGBOOT

; Put all segments in a group since we're going to be
; referring to them all with the same segment register value
zero	group	segzero,segcode,segpart,segboot

; This segment is placed at zero to
; force group 'zero' to start at 00000
; Also we find interrupt vectors here
segzero	segment	byte public
	db	070h dup ( ? )
tickofs	dw	?
tickseg	dw	?

segzero	ends

; Code segment located at 00600
; BIOS loads us at 07C00 but we move ourself here since some
; operating systems expect to find the partition table here.
segcode	segment	byte public
	assume	cs:zero, ss:zero, es:zero, ds:zero

; We start off here.

code:

; This code is actually running at 07C00, so be careful about addressing

; Set up stack and copy boot record to 00600
	xor	ax,ax			; Initialize stack to 07c00
	mov	ss,ax			; No need for cli/sti here
	mov	sp,offset boot		; since loads into ss pause ints.
	mov	es,ax			; Move ourself from 7c00 to 600
	mov	ds,ax			; load area to partition table area.
	mov	si,sp
	mov	di,offset code
	mov	cx,0100h
	cld
	rep movsw
	db	0eah			; Far jump to new location for rest
	dw	offset cont		; of the code.
	dw	seg cont

cont:	
; Now we're at our natural location
	mov	ax,offset timer		; Install tick interrupt handler
	call	instvec

; Prompt user for partition to boot
uloop:	mov	si,offset prompt	; Prompt user
	call	puts
	call	gets			; Get response string at 07c00
	mov	si,offset names		; Point to name table
srch:	lodsw				; Get partition table entry address
	cmp	al,0ffh			; End of table?
	je	notfound		; ...yes
	mov	bp,ax			; Save entry address
	mov	di,offset boot		; Point to user's response
	cmp	byte ptr [di],0		; User just hit return or timeout?
	je	found			; ...yes
strlop:	lodsb				; Compare strings
	scasb
	jne	fail			; ...no match
	cmp	al,0			; End of string?
	jne	strlop			; ...nope
found:					; We found the matching entry
	xor	ax,ax
	cmp	byte ptr [bp],080h	; already active?
	mov	si,offset part		; Clear all active flags.
	mov	[si],al
	mov	16[si],al
	mov	32[si],al
	mov	48[si],al
	mov	byte ptr [bp],080h	; Set active flag on this entry.
	je	wload			; ...yes, don't have to write sector
	cmp	write,0			; Write flag set?
	je	wload			; ...no, don't rewrite boot sector
	mov	di,5			; Write sector - retry 5 times
	mov	dx,080h			; C: drive
	mov	cx,001h			; Sector no. 1
wretry:	mov	ax,0301h		; Write sect cmd, 1 sect to write
	mov	bx,offset code		; Point to 00600
	int	13h
	jnc	wload			; Success?
	xor	ax,ax			; ...nope.  Reset drive
	int	13h
	dec	di
	jnz	wretry			; More retries?
	mov	si,offset err		; ...nope.  Give message
	call	puts

wload:	mov	di,5			; Retry 5 times
	mov	dx,[bp]			; Partition entry has parms for
	mov	cx,[bp+2]		; sector read function.
retry:	mov	ax,0201h		; Read 1 sector
	mov	bx,offset boot		; into 07c00
	int	13h
	jnc	run			; Success?
	xor	ax,ax			; no... Reset drive
	int	13h
	dec	di
	jnz	retry			; More retries?
	mov	si,offset err		; ...nope.  Give message
	call	puts
; Fall into partition entry lister (maybe user pick a working partition)
; and retry again.

notfound:				; ...given entry not found.
	mov	si,offset choose	; Give message.
	call	puts
	mov	si,offset names		; List entry names.
nlop:	lodsw
	cmp	al,0ffh			; Done?
	je	uloop			; ...yes
	call	puts			; Print name
	call	crlf			; CR-LF
	jmp	nlop

fail:	cmp	al,0			; ...no string match.  Advance
	je	srch			; to next string.  Do it this way
	lodsb				; cause we might already be there.
	jmp	fail

; Partition boot sector is loaded at 07c00: jump to it
run:	les	ax,orgvec		; Restore tick handler
	call	instvec
	mov	si,bp			; si points to partition entry
;	jmp	zero:boot		; this is broken in alink? masm?
	db	0eah
	dw	offset boot
	dw	seg boot

; Subroutines

; Print backspace-space-backspace sequence
; Enter: al must contain 8
; Exit: trashes ax, bx, si
backup:	mov	si,offset spbs
; fall into doputs

; puts: Write NUL terminated string to TTY
; Enter: Address of string in SI
; Exit: SI points to after the terminating NUL of the string
; Trashes: AH, BX (AL guarenteed to have zero)
doputs:	call	putc			; write character to TTY
puts:	lodsb				; Get next character
	cmp	al,0			; End of string?
	jnz	doputs
	retn				; done.

; crlf: Output crlf sequence
; Trashes: AX, BX
crlf:	mov	al,13
	call	putc
	mov	al,10
; fall into putc

; putc: Write character to TTY
; Enter: AL has character to write
; Trashes: AH, BX (AH has 0e, BX has 7)
putc:	mov	ah,0eh
	mov	bx,7h
	int	10h
	retn

; getc Get character from keyboard
; Exit: scan code in AH, character in AL
getc:	xor	ah,ah
	int	016h
	retn

; gets: Read in a string from the user using line editing
; Exit: string at 07c00
; Trashes: BX SI AX
gets:	mov	di,offset boot		; Input buffer
gtlop:	mov	ax,delay		; Get timeout delay
	cmp	ax,0			; No timeout?
	je	noto			; ...no timeout
	mov	count,ax		; Set counter
lop:	cmp	count,0			; Check for timer countdown
	jne	gets1
	mov	di,offset boot
rtn:	call	crlf
	mov	byte ptr [di],0
	retn
gets1:	mov	ah,1
	int	16h
	jz	lop
noto:	call	getc			; Get character
	cmp	al,13
	je	rtn
	cmp	al,8
	jne	get1
	cmp	di,offset boot		; Got backspace
	je	gtlop
	dec	di
	call	backup
	jmp	gtlop
get1:	cmp	al,32			; Character in range?
	jb	gtlop
	stosb				; ...yes, so save it.
	call	putc
	jmp	gtlop

; Install bx:ax into tick interrupt vector.  Save original values in
; orgvec.

instvec:cli
	xchg	ax,tickofs
	mov	word ptr orgvec[0],ax
	mov	ax,es
	xchg	ax,tickseg
	mov	word ptr orgvec[2],ax
	sti
	retn

; Strings

spbs	db	32,8,0
prompt	db	10,"mbr: ",0
choose	db	10,"choose: ",13,10,10,0
err	db	"disk error",13,10,0

; timer interrupt service routine

timer:	cmp	cs:count,0		; Decrement counter unless it's zero
	je	idone
	dec	cs:count
idone:
	db	0eah			; Long jmp - operand follows

; Variables

orgvec	dd	0			; Original vector address
count	dw	0			; Tick counter

; Parameters set by installer

delay	dw	0		; Timeout delay in ticks
write	db	0		; Set if we should write back table

; Partition names

;names	dw	offset part
;	db	"linux",0
;	dw	offset part1
;	db	"dos",0
;	dw	offset part2
;	db	"win",0
;	dw	offset part3
;	db	"4",0
;	db	0ffh
names	db	0ffh

segcode	ends

segpart	segment	byte public	; located at 7B0
	db	14 dup ( ? )	; Skip

; There are 4 16-byte partition entries
; beginning at 7BE
; Each entry looks like this:
;
;  0	boot flag		0 for non-bootable, 80 for bootable
;  1	starting head		starting head number of partition
;  2	starting sector		low 6 bits is starting sector.  upper 2
;				bits is upper two bits of track number
;  3	starting track		low 8 bits of track number
;  4	id			system ID byte
;  5    ending head		last head of partition
;  6    ending sector
;  7	ending track
;  8-11 prefix			no. sectors to skip before partition
; 12-15 size			no. sectors in partition 

part	db	16 dup ( ? )	; The partition table itself at 7BE
part1	db	16 dup ( ? )
part2	db	16 dup ( ? )
part3	db	16 dup ( ? )

	db	055H		; Magic number at 7FE
	db	0AAH

segpart	ends

; 7c00 is where the bios initially loads us.
segboot	segment	byte public
boot:
	db	512 dup ( ? )
segboot	ends

	end
