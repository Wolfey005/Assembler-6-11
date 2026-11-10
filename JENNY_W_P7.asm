mov dx 0
;                 Dx ---> Running Total
mov cx 0
;                 cx --->counter

get
;                 get num  in to AX ---> user input
add dx ax
;                 add to running total
mov [150] dx
;                 save total in memory location 150

mov ax [150]
;                  load total into AX
put
;                  display running total

add cx 1
;                 add 1 to counter total
cmp cx 10
;                 compare 10 values
jb [5]

halt