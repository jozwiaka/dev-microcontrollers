;Oznaczenie pamieci
;BIT, CODE, DATA, IDATA, XDATA, NUMBER

;Wejscie
;Arg Number               char, 1-byte ptr       int, 2-byte ptr                long, float                         generic ptr
;    1                          R7                   R6 & R7                       R4�R7                              R1�R3
;                                             (MSB in R6,LSB in R7)           (MSB in R4,LSB in R7)      (Mem type in R3, MSB in R2, LSB in R1)
;    2                          R5                   R4 & R5                       R4�R7                              R1�R3
;                                             (MSB in R4,LSB in R5)           (MSB in R4,LSB in R7)      (Mem type in R3, MSB in R2, LSB in R1)
;    3                          R3                   R2 & R3                     ----------                           R1�R3
;                                             (MSB in R2,LSB in R3)                                      (Mem type in R3, MSB in R2, LSB in R1)

;Wyjscie
;bit                                 - Carry bit
;char, unsigned char, 1-byte pointer - R7 
;int, unsigned int, 2-byte ptr       - R6 & R7 MSB in R6, LSB in R7. 
;long,  unsigned long                - R4-R7 MSB in R4, LSB in R7. 
;float R4-R7                         - 32-Bit IEEE format. 
;generic pointer                     - R1-R3 Memory type in R3, MSB R2, LSB R1. 

;Upublicznie (udostepnienie na zewnatrz) symbolu zwiazanego z procedura przyjmujaca argumenty wejsciowe (wymaga uzupelnienia symbolu dodatkowym znakiem "podlogi")
		PUBLIC  _WAIT_10US
		PUBLIC  _LONG_PARAM_LIST
;Upublicznienie symbolu segmentu danych, wykorzystywanego do przekazywania argumentow wejsciowych do funkcji LONG_PARAM_LIST
		PUBLIC ?_LONG_PARAM_LIST?BYTE
		
;Zdefiniowanie segmentu danych przeznaczonego do odeberania argumentow wejsiowych funkcji LONG_PARAM_LIST
DATSEG      	SEGMENT DATA
		RSEG    DATSEG
		?_LONG_PARAM_LIST?BYTE:
//Dyrektywa rezerwujaca 8 bajtow
		DS 8
 				
PRGSEG  SEGMENT CODE
		RSEG    PRGSEG
;Definicja procedury realizujacej opoznienie czasowe
_WAIT_10US:
; Odebranie argumentu wejsciowego podanego przy wywolaniu procedury
		MOV DPL,R7   ;Umieszcza mlodszy bajty argumentu wejsciowego w mlodszym bajcie rejestru DPTR (DPL)
		MOV DPH,R6   ;Umieszcza starszy bajt argumentu wejsciowego w starszym bajcie rejestru DPTR (DPH)
;Petla opozniajaca wykorzystujaca wartosc rejestru DPTR jako zmienna iteracyjna			
WAIT_U:
; czterokrotne wykonanie instrukcji NOP, implementujce opoznienie 4us w celu zapewnienia opoznienia rownego 10us dla jednej iteracji petli
		NOP
		NOP
		NOP
		NOP
;Zwiekszenie DPTR o jeden
		INC DPTR
;Przeslanie do akumulatora starszego bajtu DPTR-a
		MOV A,DPH
;Wykonanie sumy logicznej miedzy akumulatorenm a mlodszym bajtem DPTR w celu przygotwania akumulatora do sprawdzenia czy rejestr DPTR ulegl wyzerowaniu
		ORL A,DPL
;Sprawdzenie niezerowego stanu rejestru DPTR poprzez weryfikacje stanu akumulatora ustalonego poprzednia instrukcja sumy logicznej
;Wykonanie skoku do kolejnej iteracji petli jesli akumulator jest rozny od zera (oparciu o powyzsze instrukcji rowniez jesli DPTR jest rozny od zera)
		JNZ WAIT_U
;Powrot z procedury		
		RET
;Definicja funkcji otrzymujacej argumenty przez segment danych
_LONG_PARAM_LIST:
;Umieszczenie w R7 (wyjsciu funkcji w przypadku zwracania wartosci jednobajtowej) najmlodszego bajtu drugiego argumentu podanego jako argument wejsciowy	
		MOV R7,?_LONG_PARAM_LIST?BYTE+7
		RET

		END
