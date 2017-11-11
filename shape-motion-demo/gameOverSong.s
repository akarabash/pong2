    .file "gameOverSong.s"
    .arch msp430g2553
    .p2align 1, 0
    
    .data

STATE: .byte 0
        
.text
JUMP: .word STATE1
      .word STATE2
      .word STATE3
      .word STATE4
      .word STATE5

    .global START
    
START: mov #5, r12
        cmp.b &STATE, r12
        JL default
        
        mov.b &STATE, r12
        add r12, r12
        mov JUMP(r12), r0 
        

STATE1: mov #2000, r12
        CALL #buzzer_set_period,
        mov.b #1, &STATE
        JMP end

STATE2: mov #5000, r12
        CALL #buzzer_set_period,
        mov.b #2, &STATE
        JMP end
        
STATE3: mov #3000, r12
        CALL #buzzer_set_period,
        mov.b #3, &STATE
        JMP end
        
STATE4: mov #4000, r12
        CALL #buzzer_set_period,
        mov.b #4, &STATE
        JMP end
        
STATE5: mov #3000, r12
        CALL #buzzer_set_period,
        mov.b #5, &STATE
        JMP end
        
default: mov #0, r12
        CALL #buzzer_set_period
        
end:
    pop r0
