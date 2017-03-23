#include "dmp.h"

#include "nrf24le1.h"
#include <hal_w2_isr.h>
#include "hal_delay.h"
#include "stdint.h"
#include "reg24le1.h" //Definiï¿½ï¿½es de muitos endereï¿½os de registradores.
#include "stdbool.h" //Booleanos
#include "API.h"
#include "nRF-SPIComands.h"


//Subendere?os usados no sistema
#define MY_SUB_ADDR 0x01
#define OTHER_SUB_ADDR 0x02

//Sinais utilizados na comunicacao via RF
#define Sinal_request_data 0x0A
#define Sinal_LEDS 0x0B

uint8_t xdata packet_motion6[12]; //xac,yac,zac,xgy,ygy,zgy

//Definicoes dos botoes e leds
#define	PIN32 //m�dulo com 32 pinos
#ifdef 	PIN32
//Pushbuttons
sbit S1  = P0^2;    // 1/0=no/press
sbit S2  = P1^4;    // 1/0=no/press
//LEDS
sbit LEDVM = P0^3; // 1/0=light/dark
#endif

void luzes_iniciais(void);
void enviar_motion6(void);//Joga no buffer do radio e despacha

void iniciarIO(void){
    //*************************** Init GPIO Pins
    P0DIR = 0xF7;   // 1111 0111 - 1/0 = In/Out - Output: P0.3
    P1DIR = 0xFF;   // Tudo input
    P2DIR = 0xFF;
    P0CON = 0x00;  	// All general I/O
    //PQ p1com?
    P1CON |= 0x53;  	// All general I/O
    P2CON = 0x00;  	// All general I/O
}
void iniciarRF(void){
    // Radio + SPI setup
    RFCE = 0;       // Radio chip enable low
    RFCKEN = 1;     // Radio clk enable
    RF = 1;
    rf_init();
    RX_Mode();
}
void setup() {
    iniciarIO(); //IO
    iniciarRF(); //RF
    hal_w2_configure_master(HAL_W2_100KHZ); //I2C
    EA=1; luzes_iniciais(); //\Enable All interrupts, e pisca luzes
    mpu_initialize(); //inicia dispositivo
    LEDVM = !mpu_testConnection(); //se deu errado acende led
    //XXX, testar get and setters de offset
}

void main(void) {
    setup();
    while(1){
        if(!S1 && LEDVM==0){ //se foi apertado o sinal e o led esta desativado
            getMotion6_packet(packet_motion6);
            enviar_motion6();
            delay_ms(100);
            while(!S1);
            delay_ms(100);
        }
        if(!S2){
            LEDVM = !LEDVM;
            delay_ms(100);
            while(!S2);
            delay_ms(100);
        }
        if(newPayload){
            //verifica se o sinal eh direficionado para mim
			if(rx_buf[0] == MY_SUB_ADDR){
				 switch(rx_buf[1]){
					case Sinal_request_data:
                        getMotion6_packet(packet_motion6);
                        enviar_motion6();
						break;
					case Sinal_LEDS:
						LEDVM = !LEDVM;
						break;
				}
			}
			sta = 0;
     		newPayload = 0;
        }
    }
}
void luzes_iniciais(void){
        LEDVM = 1;
        delay_ms(1000);
        LEDVM = 0;
        delay_ms(1000);
        LEDVM = 1;
        delay_ms(1000);
        LEDVM = 0;
}

void enviar_motion6(void){
	unsigned int i;
    tx_buf[0] = MY_SUB_ADDR;
    for(i=1;i<13;i++){
        tx_buf[i] = packet_motion6[i-1];
    }
    //enviando e retornando ao padrao:
    TX_Mode_NOACK(13);
    RX_Mode();
}

//interrupção o I2C
void I2C_IRQ (void) interrupt INTERRUPT_SERIAL{

	I2C_IRQ_handler();
}
