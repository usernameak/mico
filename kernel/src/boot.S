.section .isr_vector, "x"
    .arm
    reset:      b   micoKMainARM
    irq:        b   irq

.section .text
    .arm
    .global micoKMainARM
    micoKMainARM:
        ldr r0, =(micoKMain)
        bx  r0
