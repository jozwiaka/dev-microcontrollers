PRGSEG  SEGMENT CODE
RSEG    PRGSEG

PUBLIC _GETADC

_GETADC:
MOV DPTR,#-100
WAIT_LOOP:
NOP
NOP
NOP
NOP
INC DPTR
MOV A,DPH
ORL A,DPL
JNZ WAIT_LOOP

MOV A,#0
ANL A,R7
JNZ POT0
MOV A,#1
ANL A,R7
JNZ POT1
MOV A,#2
ANL A,R7
JNZ POT2
POT0:
MOV DPTR,#8005H
SJMP KONIEC_WYBORU_POTENCJOMETRU
POT1:
MOV DPTR,#8006H
SJMP KONIEC_WYBORU_POTENCJOMETRU
POT2:
MOV DPTR,#8007H
SJMP KONIEC_WYBORU_POTENCJOMETRU
KONIEC_WYBORU_POTENCJOMETRU:
MOVX A,@DPTR
MOV R7,A
RET
END
