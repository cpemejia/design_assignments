;
; DA_2A_T2.asm
;
; Created: 2/23/2021 4:22:58 PM
; Author : ElmerOMejia
;


.INCLUDE "M328pbDEF.INC"

.cseg
.ORG 0x00

LDI R16, HIGH(RAMEND) 
OUT SPH, R16
LDI R16, LOW(RAMEND) 
OUT SPL, R16
SBI PORTC, 3 ; enable pull up
SBI DDRB, 3 ; set PB3 output
;SBI DDRB, 2  ; set PB2 as output
loop:
SBIS PINC, 3
RJMP pressed

CBI PORTB, 3 ; turn on portB led
LDI R20, 15  ; delay 15x
CALL delay   ; call delay subroutine
SBI PORTB, 3 ; turn off portB led
LDI R20, 5   ; delay 5x
CALL delay   ; call delay subroutine

RJMP loop    ; endless loop

pressed:
SBI PORTB, 3 ; led pb3 off
SBI DDRB, 2  ; set PB2 as output
CBI PORTB, 2 ; led pb2 on
LDI R20, 40  ; 2s delay
CALL delay   
SBI PORTB, 2 ; led pb2 off


SBIS PINC, 3
RJMP pressed

RJMP loop
