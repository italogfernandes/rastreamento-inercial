/* Copyright (c) 2009 Nordic Semiconductor. All Rights Reserved.
 *
 * The information contained herein is confidential property of Nordic 
 * Semiconductor ASA.Terms and conditions of usage are described in detail 
 * in NORDIC SEMICONDUCTOR STANDARD SOFTWARE LICENSE AGREEMENT. 
 *
 * Licensees are granted free, non-transferable use of the information. NO
 * WARRENTY of ANY KIND is provided. This heading must NOT be removed from
 * the file.
 *              
 * $LastChangedRevision: 133 $
 */

/** @file
* @brief  Implementation of the SPI HAL module for nRF24LU1+
 */
#include "nrf24lu1p.h"
#include "hal_spi.h"

#define SPI_DATA    0x01
#define SPI_START   0x02
#define SPI_STOP    0x04

void hal_spi_master_init(hal_spi_clkdivider_t ck, hal_spi_mode_t mode, hal_spi_byte_order_t bo)
{
  uint8_t smctl;
  uint8_t temp = mode;  // mode is not used in nRF24LU1
  temp = bo;            // byte_order is not used in nRF24LU1

  I3FR = 1;             // rising edge SPI ready detect
  P0EXP = 0x01;         // Map SPI master on P0
  INTEXP = 0x02;        // Select SPI master on IEX3
  SPIF = 0;             // Clear any pending interrupts
  switch(ck)
  {
    case SPI_CLK_DIV2:
      smctl = 0x11;
      break;

    case SPI_CLK_DIV4:
      smctl = 0x12;
      break;

    case SPI_CLK_DIV8:
      smctl = 0x13;
      break;

    case  SPI_CLK_DIV16:
      smctl = 0x14;
      break;

    case SPI_CLK_DIV32:
      smctl = 0x15;
      break;

    case SPI_CLK_DIV64:
    default:
      smctl = 0x16;
      break;
  }
  SMCTL = smctl;        // Enable SPI master with the specified divide factor
}

uint8_t hal_spi_master_read_write(uint8_t pLoad)
{
  SPIF = 0;             // Clear interrupt request
  SMDAT = pLoad;        // Start the SPI operation by writing the data
  while(SPIF == 0)      // Wait until SPI has finished transmitting
    ;
  return SMDAT;         // Return the the read byte
}
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                         #define INTERRUPT_RFIRQ	9

/*NOTE: Alterei do Original:
*   - Apaguei NRO_SENSOR
*   - Apaguei ADDR_SENSOR[NRO_SENSOR]
*/


#define TX_ADR_WIDTH    5   	// 5 bytes TX(RX) address width
#define TX_PLOAD_WIDTH  2   //usei 2 pois o pacote sera	:
// pacote = [sub_enderešo] [id_do_led_para_mudar]


//Enderešos:
//Definido como enderešo da pipe 0
uint8_t const ADDR_HOST[TX_ADR_WIDTH] = {0xE7,0xE7,0xE7,0xE7,0xE7}; // Define a host adr

uint8_t data rx_buf[TX_PLOAD_WIDTH];
uint8_t data tx_buf[TX_PLOAD_WIDTH];

uint8_t bdata sta;
sbit	RX_DR	= sta^6;
sbit	TX_DS	= sta^5;
sbit	MAX_RT  = sta^4;

bit newPayload = 0;     // Flag to show new Payload from host
uint8_t payloadWidth = 0;

/**************************************************/
uint8_t SPI_RW(uint8_t value)
{
    SPIRDAT = value;			 			 							//spidat

    while(!(SPIRSTAT & 0x02));  							// wait for byte transfer finished

    return SPIRDAT;             							// return SPI read value
}
/**************************************************/
uint8_t SPI_RW_Reg(uint8_t reg, uint8_t value)
{
    uint8_t status;

    RFCSN = 0;                   						// CSN low, init SPI transaction?
    status = SPI_RW(reg);      							// select register
    SPI_RW(value);             							// ..and write value to it..
    RFCSN = 1;                   						// CSN high again  ??rfcon^1

    return(status);            							// return nRF24L01 status byte
}
/**************************************************/
uint8_t SPI_Read(uint8_t reg)
{
    uint8_t reg_val;

    RFCSN = 0;                											// CSN low, initialize SPI communication...
    SPI_RW(reg);            												// Select register to read from..
    reg_val = SPI_RW(0);    												// ..then read registervalue
    RFCSN = 1;                											// CSN high, terminate SPI communication RF

    return(reg_val);        												// return register value
}
/**************************************************/
uint8_t SPI_Read_Buf(uint8_t reg, uint8_t *pBuf, uint8_t bytes)
{
    uint8_t status,byte_ctr;

    RFCSN = 0;                    								// Set CSN low, init SPI tranaction
    status = SPI_RW(reg);       									// Select register to write to and read status byte

    for(byte_ctr=0;byte_ctr<bytes;