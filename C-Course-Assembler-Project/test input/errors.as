;invalid labal name
3label: add 3,@r4
label!: add 3,@r4

;label already declared in file
loop: clr @r4
loop: add 3,@r4

;invalid directive oprand
.data "hello"
.string 2,3,4
.extern invaild!label
.string "hello""

;missing comma
add 3 @r4

;extrenous text after end of command
sub 3,@r4 as

;invalid source operand
add 3label,@r4
;invalid dest opernd
add 4,@r9

;missing operands 
add 4

;too much oprends
stop 4

;invalid source operand type
lea @r4,@r4
;invalid dest oprenad type
lea label,45

;undefined command name
mul 4,@r4

;entry label not defined
.entry test
