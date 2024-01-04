LOOP:
    MOV DPTR,#8008H
    ; ALTERNATIVE APPROACH
    ; PTIO EQU 8008H
    ; MOV DPTR,#PTIO
    MOVX A,@DPTR
    JB ACC.0,X0_OFF
    
    mov A,R5
    
    JZ SET128
    JNZ SET0
    
    SET128:
    MOV A,#128
    JNZ OMITSET0 
    
    SET0:
    MOV A,#0
    
    OMITSET0:
    MOV DPTR,#8009H
    MOVX @DPTR,A
    MOV R5,A
    
    MOV DPTR,#-65025
    ; ALTERNATIVE APPROACH
    ; MOV R7,#-255
    ; MOV R6,#-255
    ; MOV DPL,R7
    ; MOV DPH,R6
    WAIT_LOOP:
    NOP
    NOP
    NOP
    NOP
    INC DPTR
    MOV A,DPH
    ORL A,DPL
    JNZ WAIT_LOOP
    MOV DPTR,#8009H
    X0_OFF:
    MOV A,#0
    MOVX @DPTR,A
    SJMP LOOP
    END