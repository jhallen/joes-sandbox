# Classic CPU Performance Comparison


|CPU       |Year|Example use              |Speed grades    |Performance         |
|----------|----|-------------------------|----------------|--------------------|
|8080, 8085|1974|                         |2, 3, 5, 6 MHz  |80 KB/s - 240 KB/s  |
|6800, 6802|1974|                         |1, 1.5, 2 MHz   |94 KB/s - 188 KB/s (downwards), 79 KB/s - 158 KB/s (upwards) | 
|6502      |1975|Apple 2, C64, Atari 800  |1, 2, 3 MHz     |66.7 KB/s - 200 KB/s |
|Z80       |1976|ZX Spectrum              |                |          |
|6801, 6803|1977|TRS-80 MC-10             |1, 1.5, 2 MHz   |127 KB/s - 254 KB/s (downwards), 112.7 KB/s - 225 KB/s (updwards) |
|6809      |1978|TRS-80 Color Computer    |1, 1.5, 2 MHz   |169 KB/s - 338 KB/s (495 KB/s for reversing copy) |
|6811      |1984|                         |1, 1.5, 2 MHz   |                    |

## 8080

### Downwards copying

~~~asm

inner:
	mov	c, (hl)		; 7
	dcx	h		; 5
	mov	b, (hl)		; 7
	dcx	h		; 5
	push	bc		; 11

	dcx	de		; 5
	jnz	inner		; 10

~~~

50 cycles for 2 bytes: 25 cycles / byte (up to 240 KB/s).

## 6800

### Downwards copying

~~~asm
inner:
	ldab	xval+1		; 3 (zero page)
	subb	#8		; 2
	stab	xval+1		; 4
	bcs	noborrow	; 4
	dec	xval		; 6 (extended)
noborrow:

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

	cpx	final		; 4 (zero page)
	bne	inner		; 4
~~~

Reasonable unrolling: 85 cycles for 8 bytes: 10.625 cycles / byte (up to 188.2 KB/s)

Maximum unrolling: 8 cycles per byte (up to 250 KB/s)

### Upwards copying

~~~asm
inner:
	ldab	xval+1		; 3 (zero page)
	addb	#8		; 2
	stab	xval+1		; 4
	bcc	nocarry		; 4
	dec	xval		; 6 (extended)
nocarry:

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

	cpx	final		; 4 (zero page)
	bne	inner		; 4
~~~

Reasonable unrolling: 101 cycles for 8 bytes: 12.625 cycles / byte (up to 158.4 KB/s)

Maximum unrolling: 10 cycles per byte (up to 200 KB/s)

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

## 6801, 6803

### Downwards copying

~~~asm
inner:
	std	xval		; 4 (zero page)
	ldx	xval		; 4 (zero page)

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
	bne	inner		; 3 Loop
~~~

Reasonable unrolling: 63 cycles for 8 bytes: 7.875 cycles / byte (up to 254 KB / s)

### Upwards copying

~~~asm
inner:
	std	xval		; 4 (zero page)
	ldx	xval		; 4 (zero page)

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
	bne	inner		; 3 Loop
	
~~~

Reasonable unrolling: 71 cycles for 8 bytes: 8.875 cycles / byte (up to 225 KB / s)

## 6809

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
