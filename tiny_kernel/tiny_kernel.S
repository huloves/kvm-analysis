/*
 * write "Hello"(ASCII) to port 0x996
 * compile: 
 */
.section ".text"
start:
        mov x5, 0x48
        mov x4, 0x996
        strb w5, [x4] 
        mov x5, 0x65 
        strb w5, [x4] 
        mov x5, 0x6c
        strb w5, [x4] 
        strb w5, [x4] 
        mov x5, 0x6f
        strb w5, [x4] 
        mov x5, 0x0a
        strb w5, [x4] 
        ret
