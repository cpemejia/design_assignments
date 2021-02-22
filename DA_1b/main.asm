; DA_1B.asm
;
; Created: 2/19/2021 9:14:33 PM
; Author : ElmerOMejia
;

.EQU STARTADDS = 0x0400	; mem address 0x400
.EQU ODDS = 0x500        ; mem address 0x500
.EQU EVENS = 0x600       ;mem address 0x600


.ORG 00		        ; begin at memory 0
CLR R1                   ; clear register R1
LDI R16, 0x70            ; load immediate decimal value 112 into R16
MOV R14, R16             ; move R16 into R14
LDI R16, 0x21            ; load immediate decimal value 33 into R16
MOV R15, R16             ; move R16 into R16
LDI XL, LOW(STARTADDS)   ; x pointer to point to low address 0x00
LDI XH, HIGH(STARTADDS)  ; x pointer to point to high address 0x04
LDI YL, LOW(ODDS)        ; y pointer to point to low address 0x00
LDI YH, HIGH(ODDS)       ; y pointer to point to high address 0x05
LDI ZL, LOW(EVENS)       ; z pointer to point to low address 0x00
LDI ZH, HIGH(EVENS)      ; z pointer to point to high address 0x06
 
L1: 
ST X+, R15               ; store R15 where x is pointed to 
MOV R17, R15             ; move R15 into R17
ADD R18, R17             ; running sum for total
ADC R19, R1              ; add carry to R19 if carry flag set
MOV R13, R15             ; move R15 into R13
ROR R13                  ; rotate R13 right 
BRCS odd                 ; after rotate, if last bit 1, then number is odd
JMP even                 ; else its even

even:
ST Z+, R15	        ; if even store where Z is pointed to 
MOV R23, R15             ; move R15 into R23
ADD R24, R23             ; running sum for even
ADC R25, R1              ; add carry to R25 if carry flag set
INC R15                  ; increment R15

JMP count_dec            ; jump to decrement counter

odd:                     
ST Y+, R15               ; if odd store where Z is pointed to
MOV R20, R15             ; move R15 into R20
ADD R21, R20             ; running sum for odd
ADC R22, R1              ; add carry to R22 if carry flag set
INC R15                  ; increment R15
JMP count_dec            ; jump to decrement counter

count_dec:

DEC R14                  ; decrement counter
BRNE L1                  ; if not equal to zero, loop to L1

END: JMP END             ; endless loop
