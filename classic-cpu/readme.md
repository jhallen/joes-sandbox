# Classic CPU Memcpy Comparison

Here is a table of the bandwidths you can expect to get from memmove/memcpy
on various classic CPUs.  These operations are important- for example, they
are the key operation for scrolling the screen in a video game without any
acceleration hardware (such as Spectrum ZX).

|CPU       |Year|Example use              |Speed grades    |Cycles/Byte|Performance (slowest speed grade)|
|----------|----|-------------------------|----------------|------|--------------|
|[8008](#intel-8008)|1972|                |250, 500 KHz    |89    |2.8 KB/s      |
|[8080](#intel-8080), 8085|1974|Altair 8800, IMSAI 8080, TRS-80 Model 100|2, 3, 5, 6 MHz|25, 24.5  |80 KB/s (downwards), 81.6 KB/s (upwards)|
|[6800](#motorola-6800), 6802|1974|SWTPC 6800, ET3400       |1, 1.5, 2 MHz|11.25, 13.125   |89.9 KB/s (downwards), 76.2 KB/s (upwards) | 
|[SC/MP](#national-semiconductor-sc/mp)     |1974|                         |.5, 1 MHz     |44.25  |11.3 KB/s           |
|[6502](#6502)      |1975|Apple 1, 2; Commodore Pet, Vic-20, 64; Atari VCS, 400/800, NES|1, 2, 3 MHz|13     |76.9 KB/s |
|[1802](#rca-1802)      |1975|COSMAC ELF               |3.2, 5 MHz|44| 113.6 KB/s          |
|[F8](#fairchild-f8)        |1975|Channel F                |    |            |                    |
|[2650](#signetics-2650)      |1975|                         |416 KHz, 666 KHz|9.75  |68 KB/s             |
|[Z80](#zilog-z80)       |1976|TRS-80 Model 1, 2, 3, 4; Sinclair ZX 81, Spectrum|2.5, 4, 6 MHz|21 | 119 KB/s |
|[9900](#ti-9900)      |1976|TI-99/4A                 |3 MHz           |17.5     |171 KB/s            |
|[6801, 6803](#motorola-6801-6803)|1977|TRS-80 MC-10             |1, 1.5, 2 MHz|8.375, 9.375   |119.4 KB/s (downwards), 106.7 KB/s (upwards) |
|[6809](#motorola-6809), HD63C09      |1978|TRS-80 Color Computer    |1, 1.5, 2 MHz, 3 MHz|5.833, 4  |171 KB/s, 250 KB/s for reversing copy |
|[8086](#intel-8086)      |1978|                         |5, 8 MHz |8.5       |588 KB/s |
|[8088](#intel-8088)      |1979|IBM PC                   |5, 8 MHz |12.5       |400 KB/s |
|[68000](#motorola-68000)     |1980|Apple Macintosh, Amiga, Atari ST, TRS-80 Model 12/16|4, 6, 8 MHz|4.5 |889 KB/s |
|[68008](#motorola-68008)     |1982|Sinclair QL              |8, 10 MHz |9.2      |870 KB/s |
|[80286](#intel-80286)        |1982|                         |4, 6, 8, 12.5 MHz |2         |2000 KB/s       |
|[G65SC816](#65816)     |1983|Apple 2 GS, SNES         |2, 3, 4, 6, 8, 14 MHz |7    |286 KB/s |
|[68HC11](#68HC11)|1984|             |1, 2, 3 MHz |8.125, 7.25     |123 KB/s (downwards), 138 KB/s (upwards) |
|[80386](#intel-80386)        |1985|                         |12, 16, 20 MHz |1         |12000 KB/s       |
|[VL86C010](#VL86C010)|1986|Acorn RISC Machine|10, 12 MHz |.6 |16667 KB/s |

## Intel 8008

It's very slow because there is only a single pointer register (HL), and
there are no 16-bit operations.

~~~asm
; Use self modified code
; High part of source and dest address in sahigh and dahigh

; b has low part of source address
; c has low part of dest address
; hl is pointer
; de is loop count (inverse of it)
; [inf] means infrequent

inner:

; Move byte from source to dest
	ld	l, b		; 5
sahigh:	ld	h, #srchigh	; 8
	ld	a, (hl)		; 8
	ld	l, c		; 5
dahigh:	ld	h, #desthigh	; 8
	ld	(hl), a		; 7

; Increment source
	inc	b		; 5
	jnz	sk1		; 11

	ld	h, #mypage	; 8 [inf]
	ld	l, #sahigh+1	; 8 [inf]
	ld	a, (hl)		; 8 [inf]
	add	a, #1		; 5 [inf]
	ld	(hl), a		; 7 [inf]
sk1:

; Increment dest
	inc	d		; 5
	jnz	sk2		; 11

	ld	h, #mypage	; 8 [inf]
	ld	l, #dahigh+1	; 8 [inf]
	ld	a, (hl)		; 8 [inf]
	add	a, #1		; 8 [inf]
	ld	(hl), a		; 7 [inf]
sk2:

; Increment inverse of count (so we can use z flag- inc does not affect carry)
	inc	e		; 5
	jnz	inner		; 11
	inc	d		; 5 [inf]
	jnz	inner		; 11 [inf]

; All done!

; Total = 89 cycles
~~~

## Intel 8080

8080 is much faster because we can use the stack pointer as one of the data
pointers and there are 16-bit operations.

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

### Downwards copying

~~~asm
inner:
	lda	7,x		; 5 Load one bytes
	psha			; 3 Store one byte
	lda	6,x
	psha
	lda	5,x
	psha
	lda	4,x
	psha
	lda	3,x
	psha
	lda	2,x
	psha
	lda	1,x
	psha
	lda	0,x
	psha

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
	ldd	4,x
	pshb
	psha
	ldd	2,x
	pshb
	psha
	ldd	0,x
	pshb
	psha

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
	pulb
	pula
	std	2,x
	pulb
	pula
	std	4,x
	pulb
	pula
	std	6,x

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
	cmpy	#final		; 5   Check (stuff final here)
        bne	inner		; 3   Loop
~~~

Reasonable unrolling: 70 cycles for 12 bytes: 5.833 cycles / byte (up to 343 KB / s)

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
	cmpy	#final		; 5   Check (stuff final here)
        bne	inner		; 3   Loop
~~~

Reasonable unrolling: 70 cycles for 12 bytes: 5.833 cycles / byte (up to 343 KB / s)

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
	cmpu	#final		; 5  Check
	bne	inner		; 3  Loop
~~~

Reasonable unrolling: 96 cycles for 24 bytes: 4 cycles / byte (up to 500 KB/s)

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

Moving 12 32-bit words at a time.

48 bytes in 218 cycles: 4.5 cycles / byte.

## Motorola 68008

This 16-bit processor has a 32-bit architecture and an 8-bit bus.

~~~asm
inner:
	movem			; 16 + 16n  Load multiple registers
	movem			; 24 + 16n  Store multiple registers
	dbnz inner		; 18  Count and loop
~~~

For example, moving 12 32-bit words at a time.

48 bytes in 442 cycles: 9.2 cycles / byte.  Up to 1.09 MB / s.

## 65816

This is an enhanced 6502 that inclues 16-bit features including a dedicated
block move instruction.

~~~asm
	mvp			; 7 cycles / byte
~~~

7 cycles / byte using the dedicated instruction.

## 68HC11

### Downwards copying

~~~asm
inner:
	ldx	6,y		; 6 Load two bytes
	pshx			; 4 Save two bytes
	ldx	4,y
	pshx
	ldx	2,y
	pshx
	ldx	0,y
	pshx

	ldd	xval		; 4 Get source pointer
	addd	#-8		; 4 Adjust
	std	xval		; 4 (direct)
	ldy	xval		; 5 (direct)
	cpy	#final		; 5 (direct)
	bne	inner		; 3 Loop
~~~

Reasonable unrolling: 65 cycles for 8 bytes: 8.125 cycles / byte (up to 369 KB / s)

### Upwards copying

~~~asm
inner:
	puly			; 6 Load two bytes
	sty	0,x		; 6 Store two bytes
	puly
	sty	2,x
	puly
	sty	4,x
	puly
	sty	6,x

	abx			; 3 Adjust destination poiner
	cpx	#final		; 4 Check
	bne	inner		; 3 Loop
~~~

Reasonable unrolling: 58 cycles for 8 bytes: 7.25 cycles / byte (up to 414 KB / s)

## VL86C010

Use load and store multiple.

~~~asm
inner:
	ldm			; 11 Load 40 bytes (10 registers)
	stm			; 11 Save 40 bytes (10 registers)
	sub	r11, #1		;  1 Count
	b	inner		;  1 Loop
~~~

24 cycles for 40 bytes: .6 cycles / byte.
