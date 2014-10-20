_TEXT           SEGMENT BYTE PUBLIC 'CODE'
                ASSUME  CS:_TEXT,CS:_TEXT

                public  _indos
                public  _inexec
_stackp         dw      _stack
_indos          dw      0
_inexec         dw      0

                public  _odos
                public  _oi20
                public  _oi27
                public  _oi25
                public  _oi26
_stack          dw      128 dup (?)
_odos           dd      (?)
_oi20           dd      (?)
_oi27           dd      (?)
_oi25           dd      (?)
_oi26           dd      (?)

                public  _i20chain
_i20chain:      inc     _indos
                jmp     cs:[_oi20]

                public  _i27chain
_i27chain:      inc     _indos
                jmp     cs:[_oi27]

                public  _i25chain
_i25chain:
                pushf                           ; Save regs
                push    ax
                push    bp
                push    si
                push    ds
                push    cs
                pop     ds
                mov     bp,sp                   ; Point to stack
                inc     _indos
                cli
                mov     ax,10[bp]               ; Save return address
                mov     si,_stackp
                mov     [si],ax
                inc     si
                inc     si
                mov     ax,12[bp]
                mov     [si],ax
                inc     si
                inc     si
                mov     _stackp,si
                mov     ax,offset _cntd1
                mov     10[bp],ax
                mov     12[bp],cs
                pop     ds
                pop     si
                pop     bp
                pop     ax
                popf
                jmp     cs:[_oi25]
                public  _i26chain
_i26chain:
                pushf                           ; Save regs
                push    ax
                push    bp
                push    si
                push    ds
                push    cs
                pop     ds
                mov     bp,sp                   ; Point to stack
                inc     _indos
                cli
                mov     ax,10[bp]               ; Save return address
                mov     si,_stackp
                mov     [si],ax
                inc     si
                inc     si
                mov     ax,12[bp]
                mov     [si],ax
                inc     si
                inc     si
                mov     _stackp,si
                mov     ax,offset _cntd1
                mov     10[bp],ax
                mov     12[bp],cs
                pop     ds
                pop     si
                pop     bp
                pop     ax
                popf
                jmp     cs:[_oi26]

_cntd1:         pushf                           ; Flags
                pushf                           ; Space for return address
                pushf
                push    ax
                push    bp
                push    si
                push    ds
                mov     bp,sp
                push    cs
                pop     ds
                dec     _indos
                cli
                mov     si,_stackp
                dec     si
                dec     si
                mov     ax,[si]
                mov     10[bp],ax
                dec     si
                dec     si
                mov     ax,[si]
                mov     8[bp],ax
                mov     _stackp,si
                pop     ds
                pop     si
                pop     bp
                pop     ax
                iret

                public  _doschain
vrx:            inc     _indos
                jmp     over
_doschain:
                pushf                           ; Save regs
                push    ax
                push    bp
                push    si
                push    ds
                push    cs
                pop     ds
                mov     bp,sp                   ; Point to stack
                mov     _inexec,0
                cmp     ah,1
                jnz     vv1
; character input with echo
                jmp     vr1
vv1:            cmp     ah,2
                jnz     vv2
; character output
                jmp     vr1
vv2:            cmp     ah,6
                jnz     vv3
; Direct console I/O
                jmp     vr1
vv3:            cmp     ah,7
                jnz     vv4
; Unfiltered character input without echo
                jmp     vr1
vv4:            cmp     ah,8
                jnz     vv5
; Character input without echo
                jmp     vr1
vv5:            cmp     ah,9
                jnz     vv6
; Output string
                jmp     vr1
vv6:            cmp     ah,10
                jnz     vv7
; Buffered keyboard input
                jmp     vr1
vv7:            cmp     ah,11
                jnz     vv8
; Get input status
                jmp     vr1
vv8:            cmp     ah,12
                jnz     vv9
; Dump input
                jmp     vr1
vv9:            cmp     ah,63
                jnz     vv10
; Read file or device
                jmp     vr1
vv10:           cmp     ah,64
                jnz     vv11
; Write to file or device
                jmp     vr1
vv11:           cmp     ah,0
                jz      vrx
                cmp     ah,49
                jz      vrx
                cmp     ah,76
                jz      vrx
                cmp     ah,75
                jnz      vr1
                mov     _inexec,50
                jmp     vr
vr1:            inc     _indos
vr:             cli
                mov     ax,10[bp]               ; Save return address
                mov     si,_stackp
                mov     [si],ax
                inc     si
                inc     si
                mov     ax,12[bp]
                mov     [si],ax
                inc     si
                inc     si
                mov     _stackp,si
                mov     ax,offset _cntd
                mov     10[bp],ax
                mov     12[bp],cs

over:
                pop     ds
                pop     si
                pop     bp
                pop     ax
                popf
                jmp     cs:[_odos]

_cntd:          pushf                           ; Flags
                pushf                           ; Space for return address
                pushf
                push    ax
                push    bp
                push    si
                push    ds
                mov     bp,sp
                push    cs
                pop     ds
                cmp     _inexec,0
                jnz     ooo
                dec     _indos
ooo:            mov     _inexec,0
                cli
                mov     si,_stackp
                dec     si
                dec     si
                mov     ax,[si]
                mov     10[bp],ax
                dec     si
                dec     si
                mov     ax,[si]
                mov     8[bp],ax
                mov     _stackp,si
                extrn   _dos:near
                pop     ds
                pop     si
                pop     bp
                pop     ax
                jmp     cs:_dos

_TEXT           ENDS
		END

