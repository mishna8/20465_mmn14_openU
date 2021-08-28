

;2 op reg&reg
add r4 , r6

;2 op reg&imm
sub r3, #-7

;2 op reg&dir
add r1 ,sun

;2 op imm&dir
sun: lea #48, W

;2 op imm&imm
lab: cmp #56 , #-56

.extern W

;2 op dir&dir
MAIN: add l5,lab


;1 op reg
t: clr r0

L8: .data 6,-9

www: .string "hello"

;1 op imm
 dec #90

;1 op dir 
not www

;1 op 9 rela
bne %t

;1 op 9 reg
jmp r3

LIST: .data 6,-9

.entry MAIN

; 1 op 9 dir
bne L8

; 1 op 9 imm 
bne #1

;0 op
end: stop

l5: .string "string"


