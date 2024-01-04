		PUBLIC  MOJA_PROC_W_ASM
			PUBLIC INIT_ASM
		PUBLIC  ZM
		
DATSEG      	SEGMENT DATA
		RSEG    DATSEG
ZM:	
        DS 1
 				
PRGSEG  SEGMENT CODE
		RSEG    PRGSEG
INIT_ASM:

		mov ZM,#10b
		ret
		
MOJA_PROC_W_ASM:

        mov A,ZM
		mov dptr,#8008h
        movx @DPTR,A

        RET

		END
