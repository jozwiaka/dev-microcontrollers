  .syntax unified
  .set PRESCALER_MAX,50
  .set PERIPH_BASE,0x40000000
  .set AHB1PERIPH_OFFSET, 0x00020000
  .set GPIOG_BASE,PERIPH_BASE+AHB1PERIPH_OFFSET+0x1800
  .set BSRR_OFFSET,0x18



 .global move_square_asm
 .global RGB16Pack
 .global kopiuj_blok_pamieci
 .global kasuj_blok_pamieci

 .global x
 .global y

//
 .global DrawSquare
//

 .section .data
 LEDSTATE_AT_0_AND_PRESCALER_AT_4:
 .word 0,PRESCALER_MAX

 dx_asm:
 .word 2
 dy_asm:
 .word 2


 .section .text

//r0 - adres �r�d�a
//r1 - adres przeznaczenia
//r2 - liczba s��w do przekopiowania (wielokrotno�c 8)

kopiuj_blok_pamieci:
	  LSR   r2,r2,#3
	  PUSH {r4-r10}
kopiuj_osiem_slow:
      LDM    r0!, {r3-r10}
      STM    r1!, {r3-r10}
      SUBS   r2, r2, #1
      BNE    kopiuj_osiem_slow
      POP {r4-r10}
      MOV PC,LR


//r0 - adres kasowanego obszaru
//r1 - liczba s��w do skasowania (wielokrotno�c 8)

kasuj_blok_pamieci:
	  LSR   r1,r1,#3
	  PUSH {r4-r9}
	  LDR r2,=fill+4
	  LDM   r2, {r3-r9}
	  LDR r2,[r2]
wypelnij_osiem_slow:
      STM    r0!, {r2-r9}
      SUBS   r1, r1, #1
      BNE    wypelnij_osiem_slow
      POP {r4-r9}
      MOV PC,LR
fill:
	.word 0,0,0,0,0,0,0,0

//R0 - skladowa Blue
//R1 - skladowa Green
//R2 - skladowa Red

//R0 - wyjscie 16bit RGB

RGB16Pack:
	AND r0,r0,#0x1f
	BFI r0,r1,#5,#6
	BFI r0,r2,#11,#5
	MOV PC,LR



move_square_asm:

//skladowa x
 ldr r0,=x
 ldr r2,=dx_asm
 ldr r3,[r2]
 ldr r1,[r0]
 add r1,r1,r3
 str r1,[r0]
 cmp r1, #1
 ble neg_x
 cmp r1, #240-34
 blt no_neg_x
 neg_x:
 rsb r3, r3, #0
 str r3,[r2]
 no_neg_x:

//skladowa y
 ldr r0,=y
 ldr r3,[r2,#4]
 ldr r1,[r0]
 add r1,r1,r3
 str r1,[r0]
 cmp r1, #1
 ble neg_y
 cmp r1, #320-34
 blt no_neg_y
 neg_y:
 rsb r3, r3, #0
 str r3,[r2,#4]
 no_neg_y:
 mov pc,lr

//R0 - lX
//R1 - lY
//R2 - kolor
//R4 - black
//R5 - x
//R6 - y
//R9 - adres poczatku
DrawSquare:
mov r4,#0
mov r5,#0
mov r6,#0
mov r7,#10
ldr r9,=0xD0100000
loop:
cmp r5,r0
bhi black
cmp r6,r1
bhi black
b white

white:
strh r2,[r9],#2
b continue
black:
strh r4,[r9],#2
b continue

continue:
add r5,r5,#1
cmp r5,#240
bne dontIncrementY
add r6,r6,#1
mov r5,#0
dontIncrementY:
mul r8,r5,r6
cmp r8,#240*320
bne loop
mov pc,lr
