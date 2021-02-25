;
; DA_2A_T2.asm
;
; Created: 2/23/2021 4:22:58 PM
; Author : ElmerOMejia
;


; Replace with your application code

.INCLUDE "M328pbDEF.INC"

LDI R19, 40


CBI DDRC, 3  ; set PC3 as input
SBI PORTC, 3 ; enable pull up
SBI DDRB, 2  ; set PB2 as output

loop:
SBIS PINC, 3 ; skip next if set
jmp skip

SBI PORTB, 2 ; set PB2 as output
forty:
CALL delay
dec R19
brne forty
CBI PORTB, 2
jmp loop

skip:
jmp loop


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
