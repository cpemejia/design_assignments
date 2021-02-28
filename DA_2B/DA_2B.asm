;
; DA_2B.asm
;
; Created: 2/27/2021 1:32:34 AM
; Author : ElmerOMejia
;


; Replace with your application code

.include <m328pbdef.inc>
.ORG 0x00 ; reset location
RJMP MAIN
.ORG 0x02 ; external interrupt 0 location
RJMP EX0_ISR

MAIN:
	LDI R20, HIGH(RAMEND) ; initialize stack
	OUT SPH, R20
	LDI R20, LOW(RAMEND)
	OUT SPL, R20

	LDI R21, 0x02 ; INT0 is falling edge
	STS EICRA, R21 ; set 2 into EICRA register
	SBI DDRB, 3  ; set PB3 as output
	SBI PORTD, 2   ; pull up on PD2
	LDI R21, 1<<INT0; INT0 enabled
	OUT EIMSK,  R21
	SEI; enable interrupts

loop:
	CBI PORTB, 3 ; turn on portB
	LDI R22, 15  ; delay 15x
	CALL delay   ; call delay subroutine
	SBI PORTB, 3 ; turn off portB
	LDI R22, 5   ; delay 5x
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
DEC R18     ; decrement R18
BRNE L3     ; if R18 not zero go to L3
DEC R17     ; decrement R17
BRNE L2     ; if R17 not zero, go to L2
DEC R16     ; decrement R16
BRNE L1     ; if R16 not zero, got to L1
DEC R22     ; decrement R22
BRNE delay  ; if R22 not zero got to delay
RET         ; return 

EX0_ISR:
SBI PORTB, 3 ; led pb3 off
SBI DDRB, 2  ; output PB2
CBI PORTB, 2 ; led pb2 on
LDI R22, 40  ; 2s delay
CALL delay   
SBI PORTB, 2 ; led pb2 off
RETI
