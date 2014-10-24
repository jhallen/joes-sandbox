;  Tiny fat filesystem boot loader
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

; Don't touch this code.  I guarantee that you'll break it :-)
;
; BP must not be touched by anyone but the ld fnuction.  BP
; contains the current sector loaded at 0:7E00.
;
; If you change the address of this buffer, you will break the directory
; search code, which depends on the fact that when you increment a pointer
; to the end of the buffer, the sign bit becomes set.  (I used to put this
; buffer at FE00 and check for the zero bit, but Linux loads there).
;
; Functions:
;  putc		write a character to console
;  puts		write a zero terminated string to console
;  getfat	uses ld to return a word from the fat: si=fat[si]
;  ld		loads sector no. given in AX into buffer 0:7E00
;  getsec	loads a number of sectors at an address.  Takes into
;               account track and 64K boundaries.
;
; Everything else is in main:
;  Fix diskette parameter table
;  Search directory for file to load
;  Load file given starting FAT no.

; Build this as follows:
;   ml /c bootfat.asm    - or -    masm bootfat.asm;
;   link /tiny bootfat.obj;
; Install to floppy:
;   debug bootfat.com
;   - w 7c00 0 0 1
;   - q

text    segment byte
	assume  cs:text,ds:text,ss:text,es:text
	org     078h
f1:	
	dw      1 dup (?)		; Diskette parm table interrupt vec.
f2:
	dw      1 dup (?)

	org     0100h
start:                                  ; Entry point must be here or link
					; will not generate .com file.

	org	07c00h			; For linux, we can just stay at 7c00

; Warning: we could be 0:7c00 or 07c0:0, so don't access any data with CS:
; All local jumps are relative, so it should be ok otherwise.

boot:   jmp     cntd                    ; Skip over parameter table
					; This sequence must be EB 3C 90
					; or Bill will not like us.  Some
					; kind of idiotic virus protection?

	db	'Winblows'		; Windblows vfat fs overwrites this
					; so we can't use the space.  Suck.

; FAT superblock

; Parameters for 1.44M disk
; More derived parameters at end of this file

bps:
	dw      512                     ; Bytes per sector
spc:
	db      1                       ; Sectors per cluster
rsv:
	dw      1                       ; Number of sectors in boot record.
fat:
	db      2                       ; Number of FAT tables
dir:
	dw      224                     ; Number of directory entries
nls:
	dw      2880                    ; Number of logical sectors if < 65536
typ:
	db      0f0h                    ; Media type
nfs:
	dw      9                       ; Number of FAT sectors
spt:
	dw      18                      ; Sectors per track
hds:
	dw      2                       ; Number of heads
nhs:
	dw      0                       ; Number of hidden sectors
nhs2:
	dw      0
nlsb:
	dw      0                       ; Number of sectors if >= 65536
	dw      0
drive:
	db      0                       ; Drive number

	db	0			; Wasted reserved byte
	db	029h			; Extended boot magic no.
	dd	0			; Volume ID
	db	'           '		; 11-byte volume label
	db	'For Rent'		; 8 wasted reserved bytes

; The Code
; This must be at offset 3E

; Set stack and segment values

cntd:
; Set stack for linux
	mov	bx,9000h
	mov	ss,bx
	mov	sp,4000h-12
	xor	dx,dx
	mov     ds,dx
	mov     es,dx
	mov	bp,dx
	cld

; Set new drive parameter table... the purpose of this is to increase the
; number of sectors per track to 18 (it's ok if it's set to more sectors
; than are actually on the disk) and to make us step faster.
	lds     si,DWORD PTR f1
	mov     di,offset buffer
	mov     cx,11
ui1:    lodsb
	cmp     BYTE PTR ES:[di],dl
	jz      ui
	mov     al,ES:[di]
ui:     stosb
	loop    ui1
	push    es
	pop     ds
	mov     WORD PTR f1,offset buffer
	mov     WORD PTR f2,dx

; Look through root directory for matching name

;	mov     ax,WORD PTR dirs        ; Root directory sector number
	mov	ax,19-1			; -1 here because AX is incremented
					; first time through loop.
;	mov     dx,WORD PTR dir         ; Number of root directory entries
	mov	dx,224

; First enter here with bx negative.  BX should be 9000h from setting of
; stack above.

more:	add	bx,32
	jns	loop1
	inc	ax

loop0:  call    ld                      ; Make sure it's loaded
					; address in BX
loop1:  mov     di,bx
	mov     si,offset nam
	mov     cx,11
	repz cmpsb
	jz      found

next:   dec     dx                      ; Out of entries?
	jnz	more			; branch if

error:	mov     si,offset msg		; Missing OS error
	call	puts			; Write string in SI to console
	xor	ah,ah
	int	16h			; Wait for keypress
	int	19h			; Reboot

found:	mov	si,offset nam		; Output "Loading" message.
	call	puts
	mov     di,26[bx]               ; First cluster of file.
	mov     bx,0F600h		; Set ES:BX to load area.  ES is
					; 0 here.

; Load file starting with cluster no. in DI
; This function is a bit complex... we're counting up the number of
; contiguous clusters in cl, loading that amount, and then doing the
; next segment of contiguous clusters.

up1:    xor     cx,cx                   ; Determine number of clusters to load
	push    di                      ; Remember starting cluster No.
; CL sectors to load
; DI current cluster no.
uu:
;	add     cl,BYTE PTR spc         ; increment by sectors per cluster
	add	cl,1
	mov     ax,di                   ; AX and DI have current cluster no.

; 16-bit fat
;	call    gtfat                   ; Get 16-bit fat entry in di
; DI has next cluster, AX has current cluster
;	cmp     di,0fff0h               ; End of file?
;	db	24 dup ( 090h )

; 12-bit fat
ff12:   shl     di,1                    ; Multiply di by 3/2
	add     di,ax
	shr     di,1                    ; DI has offset into FAT
	call    gtfat                   ; DI=FAT[DI]
	test    ax,1                    ; Even or odd?
	jz      evn
	shr     di,1
	shr     di,1
	shr     di,1
	shr     di,1
evn:	and	di,0fffh
	cmp     di,0ff0h                ; End of file?

oo1:    jb      ooo                     ; ...branch if not
	xor     di,di                   ; yes, set next clust no to zero
					; so we are no longer contiguous
ooo:
; AX has current cluster
; DI has next cluster
	cmp     cl,124                  ; Never do more than this at a time
	jae     dn                      ; ...branch if we hit limit
	inc     ax                      ; next cluster
	cmp     ax,di                   ; Still contiguous?
	jz      uu                      ; branch if
dn:     pop     ax                      ; Get starting cluster
	push    di                      ; Save beginning of next segment

;	mov     dl,BYTE PTR spc         ; Convert cluster no. to sector no.
;	mov     dh,0
	mov	dx,1
	mul     dx
;	add	ax,WORD PTR firstc
	add	ax,31
	adc     dx,0                    ; carry.
up:     call    getsec                  ; Load sectors at es:bx

	pop     di
	or      di,di                   ; More to load?
	jnz     up1                     ; ...branch if

; Everything's loaded.  Now we boot Linux.
; Copy A00 bytes from F600 to 90000
; Stack was set to 9000:3FF4
; DI is zero here.
	mov     si,0F600h
	push	ss
	pop	es
	mov     cx,0500h
	rep movsw
; Jump to setup.S
	db      0eah			; Far jump
	dw      0
	dw      9020h

; Get a word from the FAT: DI=FAT[DI]
; Enter: Offset to FAT word in DI.  It's ok if you cross sector boundaries.
; Exit: DI=Value of requested FAT work.

gtfat:
	push    es
	push    ax
	push    bx
	push    cx
	push    dx

	push    di
	call    gtfat1
	mov     dl,[bx+di]	; Lower fat entry byte
	pop     di

; 12-bit fat:
	inc     di

	call    gtfat1
; 16-bit fat:
;	inc	di

	mov     dh,[bx+di]	; Upper fat entry byte
	push    dx		; Return it in DI
	pop     di

	pop     dx
	pop     cx
	pop     bx
	pop     ax
	pop     es
	retn

gtfat1: push    di
	pop     ax

; 12-bit fat
	mov     cl,9

; 16-bit fat
;	mov	cl,8

	shr     ax,cl
;	add     ax,WORD PTR rsv
	add	ax,1

; 16-bit fat
;	shl	di,1
; 12-bit fat
	nop
	nop

	and     di,1FFh
; Fall into ld

; Load sector into buffer if it isn't already there
; Since this is only used for FAT and directory, only 16-bits are needed
; for the sector number
;
; Enter: AX=sector number
; Exit:  ES:BX=Sector loaded into buffer at 0:7e00h
;        SI=trashed

ld:	push	ax
	push    cx
	push    dx
	xor     dx,dx
	mov     es,dx
	mov     bx,7e00h
	cmp	ax,bp
	jz      ex
	mov	bp,ax
	mov     cl,1
	push	bx
	call    getsec
	pop	bx
ex:	pop     dx
	pop     cx
	pop	ax
	retn

; Load sectors.  This handles crossing of track boundaries and 64K
; boundaries.
;
; Enter:
; ES:BX = Address to load sectors at.  ES*16+BX must be divisible by the
;         sector size.  Lower 12 bits of ES must be zero.
; DX:AX = Starting sector to load (0 is first sector of partition)
; CL = number of sectors to load (1 - 255)
;
; Exit:
; ES:BX = Incremented by number of bytes loaded
; SI = 0
; CX = trashed
; AX:DX trashed

getsec:
;	add     ax,WORD PTR nhs         ; Offset sector number to start of
	add	ax,0
;	adc     dx,WORD PTR nhs2        ; partition.
	adc	dx,1000h
	xor     ch,ch
	mov     si,cx                   ; DI has no. sectors to load

; Get more sectors...
gsu:    push    dx                      ; Current sector no. on stack
	push    ax

; Load sectors only to end of current track 
	div	WORD PTR spcy		; Divide by sectors per cyl.
	mov	ax,dx			; Have to do this because divide
	xor	dx,dx			; by spt can overflow.
	div     WORD PTR spt            ; Divide by sectors per track- DX
					; has sector no. within track
;	mov     ax,WORD PTR spt         ; Sectors per track -
	mov	ax,18
	sub     ax,dx                   ; sector wthin track gives no. left
	cmp     ax,si                   ; Do we need less than this?
	jb      gs1                     ; ...branch if not, and read em all
	mov     ax,si                   ; just read the requesed no.
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
	push    si                      ; Save regs...
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

	div     BYTE PTR spt            ; divide by sectors per track
	mov     dh,al
	inc     ah

; Now DH has head number and AH has sector number

;	mov     dl,BYTE PTR drive       ; DL=drive number
	mov	dl,0
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
	add     bx,ax                   ; add to address - end of segment?
	jnz     good1                   ; ...branch if
	mov     ax,es                   ; bump up to next 64K segment
	add     ax,1000h
	mov     es,ax
good1:  pop     dx                      ; Get old sector num
	pop     ax
	pop     si                      ; Get old sector count
	add     ax,cx                   ; update sector num
	adc     dx,0
	sub     si,cx                   ; decrement count, any more to do?
	jnz     gsu                     ; ...branch if
	retn

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

msg:
	db      'No OS',13,10,0		; Missing OS message

	org     07e00h-28

nam:
	db      'VMLINUZ    ',0		; Name of file to load and zero
					; terminated message to print.
;firstc:
;	dw      31                      ; Sector number of cluster 0
;			; First valid cluster number is 2.  This corresponds
;			; to first sector after the fats.  So this
;			; value is computed as rsv+fat*nfs+dir*32/512-2
;dirs:
;	dw	19			; Sector no. of first directory
;			; Calculated as: rsv+fat*nfs
;
spcy:
	dw      36                      ; Sectors per cylinder
			; Calculated as: hds*spt
buffer:
	db      0,0,0,0,36,0,0,0,1,0,0	; Diskette parm table
ver:
	db	1			; Software version no.
	db      055h,0aah               ; Magic number

text    ends
	end     start
