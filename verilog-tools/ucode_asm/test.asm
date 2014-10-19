; This is a comment

	.width	64	; Instruction word size in bits
	.size	32	; Total size of microprogram in words

; Define some instructions

	.insn	foo 0x0800000000000000 src[15:8], dest[7:0]
	.insn	bar 0x1000000000000000 src[15:8], bra[7:0]

; Here is the program

	.org	0x04

start:	foo src=0x55, dest=0xAA
	bar src=0x55, bra=end
end:
