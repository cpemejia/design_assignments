;
; 16b_8b_div.asm
;
; Created: 2/12/2021 5:10:34 PM
; Author : ElmerOMejia
;

.def low16 = R16 ; lower 16 bit 
.def high16 = R17 ; upper 16 bit
.def div8  = R18 ; 8 bit divisor
.def lowres  = R19 ; lower 16 bit result
.def highres  = R20 ; upper 16 bit result
.def remainder = R21 ; remainder
.def temp  = R23; temporary value

Start:

	ldi temp,0xFF ; 0xFFFF
	mov high16,temp
	mov low16,temp
	ldi temp,0xFF ; 0xFF
	mov div8,temp
	inc lowres  ; begin at 1

p2:
	clc      ; carry clear
	rol low16 ; rotate 16bit left
	rol high16 
	rol remainder
	brcs subtract; go to subtract if carry is set
	cp remainder, div8 ; compare if temp to div8
	brcs p3  ; if temp is less than div8 go to p4

subtract:
	sub remainder, div8; subtract temp by div8
	sec      ; set carry
	rjmp result  ; go to result and set result
p3:
	clc      ; carry clear
result:
	rol lowres  ; rotate left result to include carry
	rol highres
	brcc p2  ; go to p2 if carry is not set
done: jmp done;
