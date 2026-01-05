.global context_switch
context_switch:
    push %ebx
    push %esi
    push %edi
    push %ebp
    mov 20(%esp), %eax   # old_esp
    test %eax, %eax
    jz skip_save
    mov %esp, (%eax)
skip_save:
    mov 24(%esp), %esp   # new_esp
    pop %ebp
    pop %edi
    pop %esi
    pop %ebx
    ret