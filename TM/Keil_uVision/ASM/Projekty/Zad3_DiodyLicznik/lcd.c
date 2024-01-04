#include <reg52.h>
#include <intrins.h>

unsigned char xdata ctrlw _at_ 0x8010;
unsigned char xdata ctrlr _at_ 0x8012; 
unsigned char xdata datw _at_ 0x8011;
unsigned char xdata datr _at_ 0x8013; 
								
#define adres_linijka1  0
#define adres_linijka2  0x40
#define loop_iteration_cycles 1
#define op_15msek (15000/loop_iteration_cycles)
#define op_4_2msek (4200/loop_iteration_cycles)
#define op_120usek (120/loop_iteration_cycles)

void opz(unsigned short int x){
while(--x!=0); _nop_();
}
void busylcd(){
while(ctrlr&0x80)  _nop_();
		}
void adrlcd(unsigned char adres){
ctrlw=adres|0x80;
busylcd();
							}
void disptext(unsigned char xdata *wsk){
unsigned char data i;
adrlcd(adres_linijka1);
for(i=0;i<16;i++){
	datw=*wsk;
	wsk++;
	busylcd();
				}
adrlcd(adres_linijka2);
for(i=0;i<16;i++){
	datw=*wsk;
	wsk++;
	busylcd();
				}
								}
//*********************
void prglcd(){
opz(op_15msek);
ctrlw=0x38;
opz(op_4_2msek);
ctrlw=0x38;
opz(op_120usek);
ctrlw=0x38;
busylcd();
ctrlw=0x38;
busylcd();
ctrlw=0x0c;
busylcd();
ctrlw=0x01;
busylcd();
ctrlw=0x06;
busylcd();

		}

