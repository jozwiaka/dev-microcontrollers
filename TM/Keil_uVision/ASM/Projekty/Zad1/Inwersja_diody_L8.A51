
;Dyrektywa tworzaca segment PRGSEG pamieci programu (CODE) zarzadzany przez linkera. Oznaczenia typow pamieci: BIT, CODE, DATA, IDATA, XDATA
PRGSEG  SEGMENT CODE

;Dyrektywa upubliczniajaca (udostepniajaca na zewnatrz pliku - modulu) symbol 
	PUBLIC  INWERSJAP1_6
;Dyrektywa sprowadzajaca do przestrzeni modulu zewnetrzny symbol ZEZWOLENIE_L8	
	EXTRN DATA (ZEZWOLENIE_L8)
;Wykorzystanie zdefiniowanego segmnetu programu PRGSEG		
        RSEG    PRGSEG
;Etykieta rozpoczynajaca procedure INWERSJAP1_6
INWERSJAP1_6:
;Odczyt komorki pamieci mlodszego bajtu zmiennej zewnetrznej  ZEZWOLENIE_L8 i wprowdzenie odczytanej wartosci do akumulatora (rejestru A)
	MOV A,ZEZWOLENIE_L8+1
;Skok warunkowy testujacy akumulator, ktory zawiera informacje o zezwoleniu na inwersje diody
; Jesli akumulator rowna sie zero (brak zgody na innwersje L8), to wowczas wykonaj skok do etykiety BRAK_ZEZWOLENIA. W przeciwnym wypadku skok nie zostanie wykonany.
	JZ BRAK_ZEZWOLENIA
; Inwersja bitu 6 portu P1, ktory steruje dioda L8
	CPL P1.6 
BRAK_ZEZWOLENIA:
;Powrot z procdury (podprogramu) - skok bezwarunkowy pod adres zapisany na stosie
	RET
;Dyrektywa asemblera konczaca kod zrodlowy
	END