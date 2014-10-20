;   Assembly language utilities
;   Copyright (C) 1991 Joseph H. Allen
;
; JTERM - Joe's pass-through printing terminal emulator
;
; JTERM is free software; you can redistribute it and/or modify it under the
; terms of the GNU General Public License as published by the Free Software
; Foundation; either version 1, or (at your option) any later version.
;
; JTERM is distributed in the hope that it will be useful, but WITHOUT ANY
; WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
; A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
;
; You should have received a copy of the GNU General Public License
; along with JTERM; see the file COPYING.  If not, write to
; the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.

_TEXT   SEGMENT BYTE PUBLIC 'CODE'
        ASSUME  CS:_TEXT

; Set a block of memory to a word
; void fbsetw(short far *dest,short count,short value)

        PUBLIC  _fbsetw
_fbsetw PROC    near
        push    bp
        mov     bp,sp
        push    di

        mov     ax,10[bp]
        mov     es,6[bp]
        mov     di,4[bp]
        mov     cx,8[bp]
        cld
        rep stosw


        pop     di
        pop     bp
        ret
_fbsetw ENDP

; Copy a block of memory in the forward direction
; void fbfwrdw(short far *dest,short far *src,short count)

        PUBLIC  _fbfwrdw
_fbfwrdw        PROC    near
        push    bp
        mov     bp,sp
        push    si
        push    di
        push    ds

        mov     si,8[bp]
        mov     ds,10[bp]
        mov     di,4[bp]
        mov     es,6[bp]
        mov     cx,12[bp]
        cld
        rep movsw


        pop     ds
        pop     di
        pop     si
        pop     bp
        ret
_fbfwrdw        ENDP

; Copy a block of memory in the backward direction
; void fbbkwdw(short far *dest,short far *src,short count)

        PUBLIC  _fbbkwdw
_fbbkwdw        PROC    near
        push    bp
        mov     bp,sp
        push    si
        push    di
        push    ds

        mov     si,8[bp]
        mov     ds,10[bp]
        mov     di,4[bp]
        mov     es,6[bp]
        mov     cx,12[bp]
        add     si,cx
        add     si,cx
        add     di,cx
        add     di,cx
        dec     si
        dec     si
        dec     di
        dec     di
        std
        rep movsw
        cld

        pop     ds
        pop     di
        pop     si
        pop     bp
        ret
_fbbkwdw        ENDP

_TEXT   ENDS
        END
        
