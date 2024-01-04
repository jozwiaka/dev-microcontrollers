org 0
	
	setb p1.6
loop:
	mov dptr,#8008h//umieszczam adres komórki pamieci w dtpr odpowiedzialny za odczyt przyciski
	movx a,@dptr //odczyt z tej komorki pamieci zawartosc tej komorki;
	//akumulutor
	sjmp loop
	
end