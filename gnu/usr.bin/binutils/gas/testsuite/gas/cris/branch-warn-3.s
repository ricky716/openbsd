; Test warning for expansion of branches.
; FIXME: Warnings currently have the line number of the last
; line, which is not really good.

;  { dg-do assemble { target cris-*-* } }
;  { dg-options "-N" }

 .text
start:
 nop
 .space 32768,0
 ba start ; { dg-warning "32-bit conditional branch generated" "" { target cris-*-* } { 13 } }
 nop
