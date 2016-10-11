#include "nrf24le1.h"
#include <hal_w2_isr.h>
#include <stdint.h>
#include <hal_delay.h>


//Pushbuttons
sbit S1  = P0^2;    // 1/0=no/press
sbit S2  = P1^4;    // 1/0=no/press
//LEDS
sbit LED1 = P0^3; // 1/0=light/dark
sbit LED2 = P0^6; // 1/0=light/dark


void delay_timer0(void);
void delay_ms(unsigned int x);
void luzes_iniciais(void);


uint8_t data_string[5] = {97,98,99,100,101};
void main()
{
	
	
	//Indicate startup process by flashing the LED1 

//*************************** Init GPIO Pins
  // Set up GPIO
    P0DIR = 0xB7;   // 1011 0111 - 1/0 = In/Out - Output: P0.3 e P0.6
    P1DIR = 0xFF;   // Tudo input
    P2DIR = 0xFF;
    P0CON = 0x00;  	// All general I/O
    P1CON = 0x00;  	// All general I/O
    P2CON = 0x00;  	// All general I/O
	P3DIR = 0xFF;
	luzes_iniciais();
//*************************** Init I2C
  	hal_w2_configure_master(HAL_W2_400KHZ);
	EA= 1;
	
	while(1)	
	{
	   	if(!S1){
			hal_w2_write(0x68, data_string, 5);
			delay_ms(100); //evita ruidos
			while(!S1); //espera soltar o botao
			delay_ms(100);
		}
		if(!S2){
			LED1 = !LED1; LED2 = !LED2;
			delay_ms(100); //evita ruidos
			while(!S2); //espera soltar o botao
			delay_ms(100);
		}

	}
}
void delay_ms(unsigned int x)
{
    unsigned int i,j;
    i=0;
    for(i=0;i<x;i++)
    {
       j=508;
           ;
       while(j--);
    }
}

void luzes_iniciais(void){
        LED1 = 1; LED2 = 0;
        delay_ms(1000);
        LED1 = 0; LED2 = 1;
        delay_ms(1000);
        LED1 = 1; LED2 = 1;
        delay_ms(1000);
        LED1 = 0; LED2 = 0;
}


void I2C_IRQ (void) interrupt INTERRUPT_SERIAL
{

	I2C_IRQ_handler();
}