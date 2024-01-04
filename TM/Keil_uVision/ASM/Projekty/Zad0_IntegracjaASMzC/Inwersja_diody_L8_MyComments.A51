PRGSEG  SEGMENT CODE 
;1. tworzy segment typu CODE i nazywa go PRGSEG
;2. tworzymy go po to by linker wiedzial, gdzie umiescic intrukcje
	PUBLIC  INWERSJAP1_6 
    ;1. INWERSJAP1_6() //main.c
    ;2. upubliczniam identyfikator, który jest adresem
	EXTRN DATA (ZEZWOLENIE_L8)
    ;1. data int ZEZWOLENIE_L8 = 1 z main.c
    ;2. ZEZWOLENIE_L8 = symbol + adres
        RSEG    PRGSEG 
        ;1. i1-4 zostaną umieszczone w PRGSEG
INWERSJAP1_6: ;etykieta, która jest adresem
	MOV A,ZEZWOLENIE_L8+1;i1
    ;1. MOV przeznaczenie, źródło // A - Akumulator = przeznaczenie
        ;-źródło:
            ;a) natychmastowy = #
            ;b) bezpośredni = //bez kratki - dotyczy dostępu do pamięci
        ;- MOV przenosi źródło do przeznaczenia
    ;2. data int ZEZWOLENIE_L8 = 2 bajty (16 bitów)
    ;3. Big endian = najbardziej znaczący bajt jest jako pierwszy
        ;np. 0x4A3B pod adresem 100
        ;100 - 4A - Starszy Bajt
        ;101 -3B - Mlodszy Bajt
    ;//Little endian
    ;4. Pod adresem ZEZWOLENIE_L8 znajduje się Starszy Bajt tej zmiennej
    ;5. Do akumulatora chcemy umieścić młodszy bajt Z_L8, więc Z_L8+1 = Mlodszy Bajt
    ;6. int data ZEZWOLENIE_L8 = b2b1 = 1 = 0x0001
        ; Adres 0x12 - b2=0x00
        ; Adres 0x13 - b1=0x01
    ;7. A = ZEZWOLENIE_L8+1 = b1 = 0x01 = 1
    ;8. Przyklad:
        ;a) MOV A, #1 -> A=1
        ;b) MOV A, 1 -> A=Zawartosc komorki pamieci o adresie 1
	JZ BRAK_ZEZWOLENIA ;i2
    ;1. Wykonuje skok do linijki poniżej, gdy A==0
    ;2. Teraz skok jest A=1, więc skok nie wystąpi
	CPL P1.6 ;i3
    ;1. CPL = Negacja bitu o adresie podanym bezpośrednio
    ;2. P1.6 zostało zamienione na adres 0x90.6
    ;3. Zaneguje 6. bit portu 1., do którego mam podłączoną diodę L_8
    ;4. Dioda zacznie świecić
BRAK_ZEZWOLENIA:
	RET ;i4
    ;1. Powrócę do miejsca wykonania procedury
	END