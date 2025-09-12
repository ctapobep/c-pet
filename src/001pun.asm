main:
        sub     sp, sp, #32
        stp     x29, x30, [sp, #16]
        add     x29, sp, #16
        mov     w8, wzr
        str     w8, [sp, #8]
        stur    wzr, [x29, #-4]
        adrp    x0, .L.str
        add     x0, x0, :lo12:.L.str
        bl      printf
        ldr     w0, [sp, #8]
        ldp     x29, x30, [sp, #16]
        add     sp, sp, #32
        ret

.L.str:
        .asciz  "To C or not to C, that's the question"