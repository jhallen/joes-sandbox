;  Tiny ext2 filesystem boot loader
;  Copyright (C) 1998 Joseph H. Allen

; This program is free software; you can redistribute it and/or modify it
; under the terms of the GNU General Public License as published by the Free
; Software Foundation; either version 1, or (at your option) any later
; version.

; This program is distributed in the hope that it will be useful, but WITHOUT
; ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
; FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
; more details.

; You should have received a copy of the GNU General Public License along with
; this program; see the file COPYING.  If not, write to the Free Software
; Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.

; This loads a file called vmlinuz from the root directory to 0F600,
; copies the part between 0F600 - 10000 to 90000 - 90A00 and
; then jumps to segment 9020, offset 0.  It also sets the stack to segment
; 9000, offset 3ff4 (this is what Linus' bootsector does).

; Functions:
;  putc		write a character to console
;  puts		write a zero terminated string to console
;  ld0		loads block no. given in DX:AX into buffer 0:8400
;  ld1		loads block no. given in DX:AX into buffer 0:8800
;  ld2		loads block no. given in DX:AX into buffer 0:8C00
;  getblks	loads a number of blocks at an address
;  getfile	loads a file at 0:f600, given its inode no.
;  bumpes	add 1000h to ES and clear BX if carry is set.

; Build this as follows with masm 5.00:
;   masm bootext2.asm;
;   link /tiny bootext2.obj;
;
; Write to floppy like this:
;   debug bootext2.com
;   - w 7c00 0 0 2
;   - q

; Check list of possible limits (most caused by divide overflows, etc.):

;  root directory must be less than 64K (ok)

; Sector size is fixed at 512 bytes
; Block size is fixed at 1K bytes (this won't be a problem until we have
; frags).

;  ipg must be less than 64K (ok: 8K is probably the max)
;   max group no. is 64K-1 (ok: 64K*8MB -> 512GB).
;  gdpb must be less than 64K (ok, it's 32)
;   max group descriptor block no. is 64K-1 (ok)
;  must be less than 64K inodes per block (ok, it's 8)
;   max inode block no. w/in group is 64K-1 (ok)

; spcy must be less than 64K (ok, it's: 16K max)
;  must be less than 64K cylinders (ok, it's 1K)
; spt must be less than 64K (ok, it's: 63 max)
;  must be less than 64K heads (ok, max is 256)
; spt must be less than 256 (ok, max is 63)
;  max head number is 255 (ok)

text    segment byte
	assume  cs:text,ds:text,ss:text,es:text
	org     078h
f1:
	dw      1 dup (?)		; Interrupt vector with address of
f2:
	dw      1 dup (?)		; diskette parm table.

	org     0100h
start:                                  ; Entry point must be here or link
					; will not generate .com file.

	org	07c00h
boot:
	db	0eah			; Far jump to cntd.  Old Leading
	dw	offset cntd		; Edge's don't start at 0:7c00 -
	dw	0			; They use 07c0:0 instead.
version:
	db	2			; Software version no.
magic:
	dw	0F137h			; Magic number
; Drive parameters
nhs:
	dw	0			; Number of hidden sectors (starting
nhs2:
	dw	0			; offset to this partition).
spt:
	dw      18			; Sectors per track
spcy:
	dw      36			; Sectors per cylinder
; Parameter block for int13 call to load rest of boot record
rsv:
	db	2			; No. sectors in boot record
	db	2			; Parm for int 13h
seccyl:
	db	1			; Sector no., upper bits of cyl no.
	db	0			; Cylinder no.
drive:
	db	0			; Drive no.
	db	0			; Head no.
; ext2fs parameters
ipg:
	dw	256			; Inodes per group
gdpb:
	dw	32			; Block descriptors per block
ipb:
	dw	8			; Inodes per block
namlen:
	dw	7			; Length of name


buffer:
	db	0,0,0,0,36,0,0,0,1,0,0	; Diskette parm table
nam:
	db      'vmlinuz',0,0,0,0,0,0	; Name of file to load (12 chars
					; max) and zero terminated message
					; to print.
msg:
	db      'No OS',13,10,0		; Missing operating system message.
err:
	db	'Error',13,10,0		; Disk read error.

; Variables

bk0:
	dd	-1			; Block no. at 8400
bk1:
	dd	-1			; Block no. at 8800
bk2:
	dd	-1			; Block no. at 8C00

; Missing OS message printer
missing:
	mov     si,offset msg		; Missing OS error
error:	call	puts			; Write string in SI to console
	xor	ah,ah
	int	16h			; Wait for keypress
	int	19h			; Reboot

; Output zero terminated string
; Enter: SI points to string.
; Exit: AL equals 0, SI points after string.

putsl:	call	putc
puts:	lodsb
	or	al,al
	jnz	putsl
	retn

; Output a character
; Enter: AL has character.

putc:	push	bx
	mov     ah,0eh
	mov     bx,7
	int     10h
	pop	bx
	retn

; Set stack and segment values
; The rest of the code assumes that these are set this way

; Set stack for linux
cntd: 	mov	dx,9000h
	mov	ss,dx			; No need for cli...sti here.
	mov	sp,4000h-12		; Load of SS stops interrupts for
	xor	dx,dx			; next instruction.
	mov     ds,dx
	mov     es,dx
	mov	bp,dx
	cld

; Load the rest of us.
	mov	ax,WORD PTR rsv		; Get parms for int13 to load
	mov	cx,WORD PTR seccyl	; rest of boot record.
	mov	dx,WORD PTR drive
	mov	bx,offset boot		; Address
	int	13h			; Read the sectors
	jnc	go			; If no error, continue
	mov	si,offset err
	jmp	error

; Set new drive parameter table... the purpose of this is to increase the
; number of sectors per track to 18 (it's ok if it's set to more sectors
; than are actually on the disk) and to make us step faster.
go:	mov	si,offset hi
	call	puts
	lds     si,DWORD PTR f1
	mov     di,offset buffer
	mov     cx,11
ui1:    lodsb
	cmp     BYTE PTR ES:[di],0
	jz      ui
	mov     al,ES:[di]
ui:     stosb
	loop    ui1
	push    es
	pop     ds
	mov     WORD PTR f1,offset buffer
	mov     WORD PTR f2,0

; Get root directory
	mov	ax,2		; Inode no. of root
	mov	dx,0
	call	getfile		; Get it
	push	bx		; Save ending address
	mov	es,dx		; Point to it (dx has zero from 'getfile')
	mov	bx,0f600h

; Search through it
find:	mov	si,offset nam	; Point to name
	mov	cx,WORD PTR namlen	; Name length
	cmp	cx,6[bx]	; Same length?
	jne	nxtentry
	mov	di,bx		; Point to name in entry
	add	di,8
	repz cmpsb		; Names match?
	jnz	nxtentry	; branch if not

	mov	si,offset nam	; Print name we're about to load
	call	puts

	mov	ax,[bx]		; Get inode no. of file
	mov	dx,2[bx]
	call	getfile		; Load the file
; We're done!
; Everything's loaded.  Now we boot Linux.
; Copy A00 bytes from F600 to 90000
; Stack was set to 9000:3FF4
golinux:
	mov	di,0
	mov     si,0F600h
	push	ss
	pop	es
	mov     cx,0500h
	rep movsw
; Jump to setup.S
	db      0eah			; Far jump
	dw      0
	dw      9020h

nxtentry:
	add	bx,4[bx]	; Advance to next record
	call	bumpes
	mov	bp,sp
	cmp	bx,[bp]
	jne	find
	jmp	missing

; Load file given inode no. in dx:ax
; File is loaded at f600 (address returned in bx:es).
getfile:
; First part... get inode address in SI, given inode no. in DX:AX
	sub	ax,1		; Oops, first inode no. is 1
	sbb	dx,0
	div	WORD PTR ipg	; Divide by inodes per group
			; AX has group no.
			; DX has inode no. within group
	push	dx		; Save inode no. w/in group
	xor	dx,dx
	div	WORD PTR gdpb
			; AX has group descriptor block no.
			; DX has group no. w/in group descriptors block
	push	dx		; Save group no. w/in group descriptors block
	xor	dx,dx
	add	ax,2		; First group descriptor block is block 2
	adc	dx,0
	call	ld0		; BX points to group descriptors block.
	pop	ax		; Get group no. w/in group descriptors block
	mov	cl,5
	shl	ax,cl		; Mult by group descriptor size
	add	bx,ax		; Point to group descriptor
	pop	ax		; Get inode no. within group
	xor	dx,dx
	div	WORD PTR ipb	; Divide by inodes per block
			; AX has inode block no. w/in group
			; DX has inode no. w/in block
	push	dx
	xor	dx,dx
	add	ax,8[bx]	; Add first inode block in group
	adc	dx,10[bx]
	call	ld0		; Get block containing inode.  BX points to it.
	pop	ax		; Get inode no. w/in block
	mov	cl,7		; Mult inode no. by size of inode
	shl	ax,cl
	add	bx,ax		; Index to inode within block
; Now we have inode
	mov	si,bx		; Put it in SI...
	mov	bx,0f600h	; Address to load file.
	mov	ax,28[si]	; Get file size in sectors
	inc	ax		; Convert to no. blocks.
	shr	ax,1
	mov	di,ax		; Save in di.
; Do direct pointers
	add	si,40		; Point to first direct pointer in inode
	mov	cx,12		; 12 direct pointers
	call	stepn		; Do it
; Do 1st level indirection
	mov	cx,1		; Just one indirection pointer to do
	call	istepn
; Do 2nd level indirection
	cmp	di,0
	je	donget
	mov	ax,[si]
	mov	dx,2[si]
	push	bx
	push	es
	call	ld2
	mov	si,bx
	pop	es
	pop	bx
	call	istepbk
; Finish up
donget:	xor	ax,ax
	mov	dx,ax
	call	lazyld
	mov	BYTE PTR count,-1
	retn

; Try to extend current extent with block in DX:AX.  If we can't, load the
; last extent at ES:BX and start a new extent.  To finish loading last extent,
; call with DX:AX=0.  Decrements DI.

lazyld:
;	call	hex
	cmp	BYTE PTR count,-1
	je	new
	add	WORD PTR lastlo,1	; inc does not set carry
	adc	WORD PTR lasthi,0
	inc	BYTE PTR count
	cmp	BYTE PTR count,63
	je	nxtext
	cmp	ax,WORD PTR lastlo
	jne	nxtext
	cmp	dx,WORD PTR lasthi
	je	lazy
nxtext:	push	dx
	push	ax
	push	cx
	mov	dx,WORD PTR firsthi
	mov	ax,WORD PTR firstlo
	mov	cl,BYTE PTR count
	call	getblks
	pop	cx
	pop	ax
	pop	dx
new:	mov	WORD PTR firstlo,ax
	mov	WORD PTR firsthi,dx
	mov	WORD PTR lastlo,ax
	mov	WORD PTR lasthi,dx
	mov	BYTE PTR count,0
lazy:	dec	di
	retn

	org	07dfeh
	db      055h,0aah               ; Magic number at end of first sect.

; Variables for lazyld
count:
	db	-1	; Current no. blocks in contiguous extent
firstlo:
	dw	0	; Block no. of first block in extent
firsthi:
	dw	0
lastlo:
	dw	0	; Block no. of last block in extent
lasthi:
	dw	0

; Step through array of 256 direct pointers, and load block specified at
; each.
; SI points to array.
; AX:DX, CX trashed
; Decrements DI by no. blocks
stepbk:	mov	cx,256		; 256 pointers in a block
; Step through cx pointers
stepn:	cmp	di,0		; Done?
	je	stepd
	mov	ax,[si]		; Low part
	mov	dx,2[si]	; High part
	call	lazyld		; Get next block
	add	si,4		; Advance
	loop	stepn		; Until done
stepd:	retn

; Step through indirection block
; SI points to pointers.
; AX:DX, CX trashed
; Decrements DI by no. blocks.
istepbk:
	mov	cx,256		; 256 indirection pointers
istepn:	cmp	di,0		; Done?
	je	istepd		; Branch if
	mov	ax,[si]
	mov	dx,2[si]
	push	si
	push	cx
	push	bx
	push	es
	call	ld1		; Load block into buffer 1
	mov	si,bx
	pop	es
	pop	bx
	call	stepbk
	pop	cx
	pop	si
	add	si,4
	loop	istepn
istepd:	retn
	
; Load block into buffer 0, 1 or 2 (use ld0, ld1, or ld2).
;
; Enter: DX:AX=block no.
; Exit:  ES:BX=buffer address.

ld0:	push	si
	mov	bx,8400h
	mov	si,offset bk0
	jmp	ld

ld1:	push	si
	mov	bx,8800h
	mov	si,offset bk1
	jmp	ld

ld2:	push	si
	mov	bx,8c00h
	mov	si,offset bk2

ld:	push	bx
	push    cx
	xor	cx,cx
	mov	es,cx
	cmp	ax,[si]
	jnz	doit
	cmp	dx,2[si]
	jz	ldok
doit:	mov	[si],ax
	mov	2[si],dx
	mov     cl,1
	call    getblks
ldok:	pop	cx
	pop	bx
	pop	si
	retn

; Load blocks.  This handles crossing of track boundaries and 64K
; boundaries.
;
; Enter:
; ES:BX = Address to load sectors at.  ES*16+BX must be divisible by the
;         sector size.  Lower 12 bits of ES must be zero.
; DX:AX = Starting block no. to load (0 is first block of partition).  If
;         0 if given for DX:AX, 1 block is cleared and ES:BX are advanced
;         by 1 block.
; CL = number of blocks to load (1 - 63)
;
; Exit:
; ES:BX = Incremented by number of bytes loaded
; CX = trashed

getblks:
	push	di
	push    ax                      ; Save block number for caller
	push    dx
	or	ax,ax			; Block no. is zero?
	jne	noclr
	or	dx,dx
	jne	noclr
	mov	cx,200h			; Just clear the block
	mov	di,bx
	xor	ax,ax
	rep stosw
	add	bx,400h
	call	bumpes
	jmp	getdone

noclr:	shl	ax,1			; Convert block no. to sect. no.
	rcl	dx,1
	add     ax,WORD PTR nhs		; Offset sector number to start of
	adc     dx,WORD PTR nhs2	; partition.
	shl	cl,1			; Convert no. block to no. sects
	xor     ch,ch
	mov     di,cx                   ; DI has no. sectors to load

; Get more sectors...
gsu:    push    dx                      ; Current sector no. on stack
	push    ax

; Load sectors only to end of current track 
	div	WORD PTR spcy		; Divide by sectors per cylinder
	mov	ax,dx			; Sector within cylinder to AX
	xor	dx,dx
	div     WORD PTR spt		; Divide by sectors per track- DX
					; has sector no. within track
	mov     ax,WORD PTR spt		; Sectors per track -
	sub     ax,dx                   ; sector wthin track gives no. left
	cmp     ax,di                   ; Do we need less than this?
	jb      gs1                     ; ...branch if not, and read em all
	mov     ax,di                   ; just read the requesed no.
gs1:    

; Make sure we don't cross 64K boundary (stupid DMA controller...)
	push    ax                      ; no secs to read on stack
	mov     cl,9                    ;
	shl     ax,cl                   ; mult by 200h
	add     ax,bx                   ; add current offset. do we hit 64K?
	jnc     gsok                    ; ...branch if not

; We would cross a 64K boundary... read only exactly to it instead        
	pop     ax                      ; zap no secs.
	mov     ax,bx                   ; current offset in BX
	neg     ax                      ; no. bytes left to 64K
	shr     ax,cl                   ; convert to no. secs.
	push    ax

; Print a progress indicator '.'
	mov	al,02eh
	call	putc

gsok:   pop     cx                      ; No. secs to read in CX

	pop     ax                      ; Get back starting sector no.
	pop     dx

; Now we know how many sectors to read, so read them
	push    di                      ; Save regs...
	push    ax
	push    dx
	mov     ch,2                    ; Code for int 13h read
	push    cx                      ; Save AX parm for int 13h

; Convert sector number to cylinder/head/sector numbering needed for
; int 13h

; Divide by number of sectors per cylinder

	div     WORD PTR spcy
	mov     cx,ax
	mov     ax,dx

; Now CX has cylinder number and AX has sector number within cylinder

	div	BYTE PTR spt		; divide by sectors per track
	mov     dh,al
	inc     ah

; Now DH has head number and AH has sector number

	mov     dl,BYTE PTR drive	; DL=drive number
	xchg    ch,cl                   ; CH=lower 8 bits of cylinder number
	ror     cl,1
	ror     cl,1
	add     cl,ah                   ; CL=upper 2 bits are MSBs of cylinder
	pop     ax                      ; AL=number of sectors to read

; All registers set - read the sectors
rrz:    push    ax                      ; Save code and count for retries
	int     13h                     ; Read sectors
	jnc     good                    ; ...branch if no error
	pop     ax                      ; Retry forever
	jmp     rrz
good:   

; Got the sectors - now update address and count        
	pop     cx
	xor     ch,ch                   ; CX has no. sectors read
	mov     ax,0200h                ; Mult by 512
	mul	cx                      ; AX has no. bytes read
	add     bx,ax
	call	bumpes
	pop     dx                      ; Get old sector num
	pop     ax
	pop     di                      ; Get old sector count
	add     ax,cx                   ; update sector num
	adc     dx,0
	sub     di,cx                   ; decrement count, any more to do?
	jnz     gsu                     ; ...branch if
getdone:
	pop     dx                      ; restore sector no. for caller
	pop     ax
	pop	di
	retn

; Bump ES when BX hits 0
bumpes:	jnz	nobump
	mov	bx,es
	add	bx,1000h
	mov	es,bx
	xor	bx,bx
nobump:	retn

; Grettings from second boot sector

hi:
	db	'ext2fs boot loader v.2',13,10,0

; Print regs in hex

hex:	push	ax
	push	dx
	mov	bp,sp
	mov	al,dh
	call	hexd
	mov	al,dl
	call	hexd
	mov	al,3[bp]
	call	hexd
	mov	al,2[bp]
	call	hexd
	mov	al,ch
	call	hexd
	mov	al,cl
	call	hexd
	mov	al,13
	call	putc
	mov	al,10
	call	putc
	pop	dx
	pop	ax
	retn

hexd:	push	ax
	shr	al,1
	shr	al,1
	shr	al,1
	shr	al,1
	call	hex1
	pop	ax
	and	al,15
hex1:	cmp	al,10
	jb	easy
	add	al,65-48-10
easy:	add	al,48
	jmp	putc

	org	8000h			; Ext2 superblock gets loaded here.
super:

text    ends
	end     start
