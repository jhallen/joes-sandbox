# Classic CPU Performance Comparison


|CPU       |Year|Example use              |Speed grades    |Performance         |
|----------|----|-------------------------|----------------|--------------------|
|[8080](#intel-8080), 8085|1974|Altair 8800, IMSAI 8080, TRS-80 Model 100|2, 3, 5, 6 MHz  |80 KB/s - 240 KB/s (downwards), 81.6 KB/s - 245 KB/s (upwards)|
|[6800](#motorola-6800), 6802|1974|SWTPC 6800, ET3400       |1, 1.5, 2 MHz   |89.9 KB/s - 179.8 KB/s (downwards), 76.2 KB/s - 152.3 KB/s (upwards) | 
|[SC/MP](#national-semiconductor-sc/mp)     |1974|                         |.5, 1 MHz       |22.6 KB/s           |
|[6502](#6502)      |1975|Apple 1, 2; Commodore Pet, Vic-20, 64; Atari VCS, 400/800, NES|1, 2, 3 MHz     |66.7 KB/s - 200 KB/s |
|[1802](#rca-1802)      |1975|COSMAC ELF               |3.2, 5 MHz      |113.6 KB/s          |
|[F8](#fairchild-f8)        |1975|Channel F                |                |                    |
|[2650](#signetics-2650)      |1975|                         |416 KHz, 666 KHz|68 KB/s             |
|[Z80](#zilog-z80)       |1976|TRS-80 Model 1, 2, 3, 4; Sinclair ZX 81, Spectrum|2.5, 4, 6 MHz| 119 KB/s - 286 KB/s |
|[9900](#ti-9900)      |1976|TI-99/4A                 |3 MHz           |171 KB/s            |
|[6801, 6803](#motorola-6801-6803)|1977|TRS-80 MC-10             |1, 1.5, 2 MHz   |119.4 KB/s - 238.8 KB/s (downwards), 106.7 KB/s - 213 KB/s (updwards) |
|[6809](#motorola-6809)      |1978|TRS-80 Color Computer    |1, 1.5, 2 MHz   |169 KB/s - 338 KB/s (495 KB/s for reversing copy) |
|[8086](#intel-8086)      |1978|                         |5, 8 MHz        |588 KB/s  - 941 KB/s |
|[8088](#intel-8088)      |1979|IBM PC                   |5, 8 MHz        |400 KB/s  - 640 KB/s |
|[68000](#motorola-68000)     |1980|Apple Macintosh, Amiga, Atari ST, TRS-80 Model 12/16|4, 6, 8 MHz |889 KB/s - 1778 KB/s |
|[68008](#motorola-68008)     |1982|Sinclair QL              |8, 10 MHz       |870 KB/s - 1087 KB/s |
|[65816](#65816)     |1983|Apple 2 GS, SNES         |2.8, 14 MHz     |400 KB/s - 2000 KB/s |

## Intel 8080

### Downwards copying

~~~asm

inner:
	mov	b, (hl)		; 7  Load a byte
	dec	hl		; 5  Decrement pointer
	mov	c, (hl)		; 7  Load a byte
	dec	hl		; 5  Decrement pointer
	push	bc		; 11 Store two bytes

	dec	de		; 5  Count
	jnz	inner		; 10 Loop

~~~

50 cycles for 2 bytes: 25 cycles / byte (up to 240 KB/s).

### Upwards copying

~~~asm

inner:
	pop	bc		; 10 Load two bytes
	mov	(hl), c		; 7  Store one byte
	inc	hl		; 5  Incremenet destination pointer
	mov	(hl), b		; 7  Store one byte
	inc	hl		; 5  Increment destination pointer

	dec	de		; 5  Count
	jnz	inner		; 10 Loop

~~~

49 cycles for 2 bytes: 24.5 cycles / byte (up to 245 KB/s).

## Motorola 6800



~~~asm
inner:
	lda	7,x		; 5 Load two bytes
	psha			; 3 Store one byte
	lda	6,x		; 5 Load two bytes
	psha			; 3 Store one byte
	lda	5,x		; 5 Load two bytes
	psha			; 3 Store one byte
	lda	4,x		; 5 Load two bytes
	psha			; 3 Store one byte
	lda	3,x		; 5 Load two bytes
	psha			; 3 Store one byte
	lda	2,x		; 5 Load two bytes
	psha			; 3 Store one byte
	lda	1,x		; 5 Load two bytes
	psha			; 3 Store one byte
	lda	0,x		; 5 Load two bytes
	psha			; 3 Store one byte

	ldb	xval+1		; 3 (zero page)
	subb	#8		; 2
	stb	xval+1		; 4
	bcs	noborrow	; 4
	dec	xval		; 6 (extended) [infrequent!]
noborrow:
	ldx	xval		; 4 get source pointer
	cpx	final		; 4 (direct)
	bne	inner		; 4
~~~

Reasonable unrolling: 89 cycles for 8 bytes: 11.125 cycles / byte (up to 179.8 KB/s)

Maximum unrolling: 8 cycles per byte (up to 250 KB/s)

### Upwards copying

~~~asm
inner:
	pula			; 4 Load one byte
	sta	0,x		; 6 Store one  byte
	pula
	sta	1,x
	pula
	sta	2,x
	pula
	sta	3,x
	pula
	sta	4,x
	pula
	sta	5,x
	pula
	sta	6,x
	pula
	sta	7,x

	ldb	xval+1		; 3 (zero page)
	addb	#8		; 2
	stb	xval+1		; 4
	bcc	nocarry		; 4
	inc	xval		; 6 (extended- there is no direct!) [infrequent!]
nocarry:
	ldx	xval		; 4 get dest pointer
	cpx	final		; 4 (direct)
	bne	inner		; 4
~~~

Reasonable unrolling: 105 cycles for 8 bytes: 13.125 cycles / byte (up to 152.3 KB/s)

Maximum unrolling: 10 cycles per byte (up to 200 KB/s)

## National Semiconductor SC/MP

SC/MP is interesting in the instruction timing does not vary base on
addressing mode (at least not that it says in the datasheet).

~~~asm
inner:
	ld	@1(P2)		; 18
	st	@1(P3)		; 18
	ld	@1(P2)		; 18
	st	@1(P3)		; 18
	ld	@1(P2)		; 18
	st	@1(P3)		; 18
	ld	@1(P2)		; 18
	st	@1(P3)		; 18
	dld	(P1)		; 22
	jnz	inner		; 11
~~~

Reasonable unrolling: 177 cycles for 4 bytes: 44.25 cycles / byte (up to 22.6 KB/s).

## 6502

The fastest copy is with self-modified code.  Use the "ABS, X" or "ABS, Y"
indexing mode.  The caller stuffs the source an destination addresses into
the instructions using these addressing modes.

~~~asm

; Put source address at src+1 and src+2
; Put destination address at dest+1 and dest+2
; Put byte count at count+0 and count+1

upcopy:

	ldx	#0		; Start index at zero

outer:	lda	count+1		; Check upper half of count
	beq	lastpage	; Branch if less than 256 bytes left
	ldy	#0		; Copy 256 bytes...
	jmp	inner
lastpage:
	ldy	count+0		; Lower half of count
	bne	inner		; Branch if we have bytes to copy...
	rts			; Otherwise we are done!

inner:
src:	lda	$0000,x		; 4  Load one byte
dest:	sta	$0000,x		; 5  Store one byte
	inx			; 2  Increment index
	dey			; 2  Decrement count
	bne	inner		; 2  Loop

; Next page
	inc	src+2		; Increment upper half of source address
	inc	dest+2		; Increment upper half of destination address
	jmp	outer		; Loop
~~~

15 cycles per byte: (up to 200 KB/s)

The inner loop can be reduced by one instruction if we overload the index and the byte count, but the setup code is more complex.

~~~asm

inner:
src:	lda	$0000,x		; 4  Load one byte
dest:	sta	$0000,x		; 5  Store one byte
	inx			; 2  Increment index
	bne	inner		; 2  Loop
~~~

13 cycles per byte: (up to 231 KB/s)

The "(ind),y" mode could be used in a way that seems to allow unrolling...

~~~asm

; Setup: load size/8 into x

inner:

	lda	(src),y		; 5  Load one byte
	sta	(dest),y	; 6  Store one byte
	iny			; 2  Increment index

	lda	(src),y
	sta	(dest),y
	iny

	lda	(src),y
	sta	(dest),y
	iny

	lda	(src),y
	sta	(dest),y
	iny

	etc...
~~~

Again 13 cycles per byte for the copying part, but the overhead will
overwhelm it.

## RCA 1802

This is the first CMOS microprocessor and is often used on spacecraft.

8 cycles per machine cycle.

~~~asm
	lda			; 2
	stn			; 2
	inc			; 2
	lda			; 2
	stn			; 2
	inc			; 2
	lda			; 2
	stn			; 2
	inc			; 2
	lda			; 2
	stn			; 2
	inc			; 2

	dec			; 2
	glo			; 2
	bnz inner		; 2

	ghi			; 2
	bnz inner		; 2
~~~

22 machine cycles for 4 bytes: 5.5 machine cycles per byte (44 clock cycles
per byte).  Up to 113.6 KB / sec.

## Signetics 2650

~~~asm
inner:
	lod			; 3
	sto			; 3
	lod			; 3
	sto			; 3
	lod			; 3
	sto			; 3
	lod			; 3
	sto			; 3

	addi			; 3
	bct			; 3
	addi			; 3..

	addi			; 3
	bct			; 3
	addi			; 3..

	bdr			; 3

~~~

39 cycles for 4 bytes: 9.75 cycles per byte.  Up to 68 KB / sec.

## Zilog Z80

~~~asm
	ldir			; (de) <- (hl)
~~~

21 cycles / byte using the dedicated block move instruction.

## TI 9900

This is an early 16-bit microprocessor.

~~~asm

inner:
	mov	(a)+, (b)+	; 30
	mov	(a)+, (b)+	; 30
	mov	(a)+, (b)+	; 30
	mov	(a)+, (b)+	; 30
	dec			; 10
	jne	inner		; 10
~~~

140 cycles for 8 bytes: 17.5 cycles per byte (up to 171.4 KB/s).

## Motorola 6801, 6803

These are enhanced versions of the 6800.

### Downwards copying

~~~asm
inner:
	ldd	6,x		; 5 Load two bytes
	pshb			; 3 Store one byte
	psha			; 3 Store one byte
	ldd	4,x		; 5
	pshb			; 3
	psha			; 3
	ldd	2,x		; 5
	pshb			; 3
	psha			; 3
	ldd	0,x		; 5
	pshb			; 3
	psha			; 3

	ldd	xval		; 4 Get source pointer
	addd	#-8		; 4 Adjust
	std	xval		; 4 (direct)
	ldx	xval		; 4 (direct)
	cpx	final		; 4 (direct)
	bne	inner		; 3 Loop
~~~

Reasonable unrolling: 67 cycles for 8 bytes: 8.375 cycles / byte (up to 239 KB / s)

### Upwards copying

~~~asm
inner:
	pulb			; 4 Load one byte
	pula			; 4 Load one byte
	std	0,x		; 5 Store two bytes
	pulb			; 4 Load one byte
	pula			; 4 Load one byte
	std	2,x		; 5 Store two bytes
	pulb			; 4 Load one byte
	pula			; 4 Load one byte
	std	4,x		; 5 Store two bytes
	pulb			; 4 Load one byte
	pula			; 4 Load one byte
	std	6,x		; 5 Store two bytes

	ldd	xval		; 4 Get source pointer
	addd	#8		; 4 Adjust destination pointer
	std	xval		; 4 (zero page)
	ldx	xval		; 4 (zero page)
	cpx	final		; 4 (direct)
	bne	inner		; 3 Loop
	
~~~

Reasonable unrolling: 75 cycles for 8 bytes: 9.375 cycles / byte (up to 213 KB / s)

## Motorola 6809

### Downwards copying

Use stack blasting for the stores: 6809 has an efficient multi-register push
instruction.  Use indexing for the loads (6809 also has auto-pre decrement,
but indexing is faster).  Use "load effective address" to adjust the source
pointer.  Keep a loop counter in the direct page.

~~~asm

inner:
	ldu	10,y		; 6   Load 2 bytes (5-bit offset)
	ldx	8,y		; 6   Load 2 bytee (5-bit offset)
	ldd	6,y		; 6   Load 2 bytes (5-bit offset)
	pshs	d,x,u		; 11  Store 6 bytes
	ldu	4,y		; 6   Load 2 bytes (5-bit offset)
	ldx	2,y		; 6   Load 2 bytes (5-bit offset)
	ldd	,y		; 5   Load 2 bytes (no offset)
	pshs	d,x,u		; 11  Store 6 bytes
        leay	-12,y		; 5   Adjust source address (5-bit offset)
        dec	counter		; 6   Count (using Direct Addressing)
        bne	inner		; 3   Loop
~~~

Reasonable unrolling: 71 cycles for 12 bytes: 5.916 cycles / byte (up to 338 KB / s)

Maximum unrolling: 29 cycles per 6 bytes: 4.833 cycles / byte (up to 414 KB / s)

### Upwards copying

Use stack blasting for the loads: 6809 has an efficient multi-register pop
instruction.  Use indexing for the stores (6809 also has auto-pre increment,
but indexing is faster).  Use "load effective address" to adjust the
destination pointer.  Keep a loop counter in the direct page.

~~~asm

inner:
	puls    d,x,u		; 11  Load 6 bytes
	std     ,y		; 5   Store 2 bytes (no offset)
	stx     2,y             ; 6   Store 2 bytes (5-bit offset)
        stu     4,y             ; 6   Store 2 bytes (5-bit offset)
	puls	d,x,u           ; 11  Load 6 bytes
	std     6,y             ; 6   Store 2 bytes
	stx	8,y             ; 6   Store 2 bytes
	stu	10,y            ; 6   Store 2 bytes
        leay	12,y		; 5   Adjust destination address (5-bit offset)
        dec	counter		; 6   Count (using Direct Addressing)
        bne	inner		; 3   Loop
~~~

Reasonable unrolling: 71 cycles for 12 bytes: 5.916 cycles / byte (up to 338 KB / s)

Maximum unrolling: 29 cycles per 6 bytes: 4.833 cycles / byte (up to 414 KB / s)

### Reversing copy

This is the fastest copy for 6809, but it reverses the data (by 16-bit
words) as it copies.

~~~asm

inner:
	pulu	d,x,y		; 11  Load 6 bytes
	pshs	d,x,y		; 11  Store 6 bytes
	pulu	d,x,y
	pshs	d,x,y
	pulu	d,x,y
	pshs	d,x,y
	pulu	d,x,y
	pshs	d,x,y
	dec	counter		; 6  Count
	bne	inner		; 3  Loop
~~~

Reasonable unrolling: 97 cycles for 24 bytes: 4.042 cycles / byte (up to 494.8 KB/s)

Maximum unrolling: 22 cycles for 6 bytes: 3.666 cycles / byte (up to 545.6 KB/s)

## Intel 8086

This 16-bit processor has a 16-bit bus.

~~~asm
	rep movsw		; 9+17n
~~~

17 cycles for 2 bytes: 8.5 cycles / byte (up to 941KB / s).  There is a
penalty if the words are unaligned.

## Intel 8088

This 16-bit processor has an 8-bit bus.

~~~asm
	rep movsw		; 9+25n
~~~

25 cycles for 2 bytes: 12.5 cycles / byte (up to 640KB / s)

## Motorola 68000

This 16-bit processor has a 32-bit architecture and a 16-bit bus.

~~~asm
inner:
	movem			; 8 + 8n  Load multiple registers
	movem			; 8 + 8n  Store multiple registers
	dbnz inner		; 10  Count and loop
~~~

Moving 12 32-not words at a time.

48 bytes in 218 cycles: 4.5 cycles / byte.

## Motorola 68008

This 16-bit processor has a 32-bit architecture and an 8-bit bus.

~~~asm
inner:
	movem			; 16 + 16n  Load multiple registers
	movem			; 24 + 16n  Store multiple registers
	dbnz inner		; 18  Count and loop
~~~

For example, moving 12 32-not words at a time.

48 bytes in 442 cycles: 9.2 cycles / byte.  Up to 1.09 MB / s.

## 65816

This is an enhanced 6502 that inclues 16-bit features including a dedicated
block move instruction.

~~~asm
	mvp			; 7 cycles / byte
~~~

7 cycles / byte using the dedicated instruction.
