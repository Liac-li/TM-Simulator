; get given 2 numbers' greatest common divisor
; Input: a string of 0's and 1's, eg '111011' for '3, 2'

; finite set of states
#Q = {0,cp,cp1,mh,mh2,cmp,del1,del2,accept,halt_reject}

; finite set of input
#S = {0,1}

; finite set of tape symbols
#G = {0,1,_}

; the start state
#q0 = 0

; the blank symbol
#B = _

; the set of final states
#F = {halt_reject}

; the number of tapes
#N = 2

; transitions
; state 0: begin
0 ** ** ** cp 
0 __ __ ** accept

; copy the string of int to the 2nd tape
cp 1_ 1_ r* cp
cp 0_ __ r* cp1
cp1 1_ _1 rr cp1
cp1 __ __ ll mh

; mh move head to right end 
mh _* _* l* mh
mh 11 ** ** cmp

mh2 *_ *_ *l mh2 ; move tape 2 head
mh2 11 ** ** cmp

; compare length and do minus
cmp 11 11 ll cmp
cmp _1 _1 rr del2 ; 2nd is greater
cmp 1_ 1_ rr del1 ; 1st is greater
cmp __ ** ** accept ; the same

; del tape 1's
del1 11 _1 rr del1
del1 __ ** ll mh

del2 11 1_ rr del2
del2 __ ** ll mh2

; accept 
accept ** ** ** halt_reject
