;
; DA_2A_T1.asm
;
; Created: 2/23/2021 4:22:58 PM
; Author : ElmerOMejia
;

;
; DA_2A.asm
;
; Created: 2/22/2021 10:04:43 PM
; Author : ElmerOMejia
;

.INCLUDE "M328pbDEF.INC"

.cseg
.ORG 0x00

LDI R16, LOW(RAMEND) 
OUT SPL, R16
LDI R16, HIGH(RAMEND) 
OUT SPH, R16


SBI DDRB, 3 ; set PB 3 output

loop:
SBI PORTB, 3 ; turn on portB
LDI R20, 15  ; delay 15x
CALL delay   ; call delay subroutine
CBI PORTB, 3 ; turn off portB
LDI R20, 5   ; delay 5x
CALL delay   ; call delay subroutine

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
DEC R20
BRNE delay
ret


