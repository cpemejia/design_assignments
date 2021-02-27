;
; DA_2A_T1.asm
;
; Created: 2/23/2021 4:22:58 PM
; Author : ElmerOMejia
;


.INCLUDE "M328pbDEF.INC"

SBI DDRB, 3 ; set PB 3 output
LDI R19, 15 ; load immediate value 15 into R19
LDI R20, 5  ; load immediate value 5 into R20

loop: 
SBI PORTB, 3 ; turn on portB

fifteen:     ; loop for 0.75 s delay
CALL delay   ; call delay subroutine
dec R19      ; dec counter
BRNE fifteen ; if not zero, loop to fifteen label
CBI PORTB, 3 ; turn off portB

five:        ; loop for 0.25s delay
CALL delay   ; call delay subroutine
dec R20      ; dec counter
BRNE five    ; if not zero, loop to fiteen label

rjmp loop    ; endless loop


delay:       ; delay for a 0.05s delay
LDI R16, 3   ; load immediate 3 to R16
L1:          ; label 1
LDI R17, 105 ; load immediate 105 into R17
L2:
LDI R18, 255 ; load immediate 255, into R18
L3:
NOP          ; NOP for more delay
NOP
NOP
NOP
NOP
NOP
NOP
DEC R18
BRNE L3
DEC R17
BRNE L2
DEC R16
BRNE L1
ret
