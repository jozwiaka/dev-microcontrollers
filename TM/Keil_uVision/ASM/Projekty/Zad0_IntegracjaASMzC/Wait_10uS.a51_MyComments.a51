		PUBLIC  _WAIT_10US
		PUBLIC  _LONG_PARAM_LIST
		PUBLIC ?_LONG_PARAM_LIST?BYTE
		
DATSEG      	SEGMENT DATA
		RSEG    DATSEG
		?_LONG_PARAM_LIST?BYTE:
//Dyrektywa rezerwujaca 8 bajtow
		DS 8
 				
PRGSEG  SEGMENT CODE
		RSEG    PRGSEG
_WAIT_10US:
		MOV DPL,R7
		MOV DPH,R6
WAIT_U:
		NOP
		NOP
		NOP
		NOP
		INC DPTR ;INCrement
        ;1. Licznik pętli opóźniające, którą będę inkrementował w każdej iteracji aż się przepełni
		MOV A,DPH ;MOVe
        ;Wkladam wartosc DPH do A
		ORL A,DPL ;ORLogic
        ;Suma logiczna A=DPH i DPL; A=A+DPL
		JNZ WAIT_U ;JumpNotZero
        ;1. Jesli A!=0 to powtarza petle
        ;2. Sprawdzenie niezerowego stanu rejestru DPTR poprzez weryfikacje stanu akumulatora ustalonego poprzednia instrukcja sumy logicznej
        ;3. Wykonanie skoku do kolejnej iteracji petli jesli akumulator jest rozny od zera (oparciu o powyzsze instrukcji rowniez jesli DPTR jest rozny od zera)
		RET ;RETurn
_LONG_PARAM_LIST:
		MOV R7,?_LONG_PARAM_LIST?BYTE+7
		RET

		END
