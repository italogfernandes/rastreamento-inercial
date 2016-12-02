#ifndef DMP_H
/* Se a biblioteca mpu.h não for definida, defina-a.
Verificar é preciso para que não haja varias chamadas da
mesma biblioteca. */
#define DMP_H

#include <hal_w2_isr.h>
#include "stdint.h"
#include "stdbool.h"
#include "mpu6050_reg.h"
#include "stdlib.h"//malloc e free
#include <string.h> //memcmp 
#define MPU_endereco MPU6050_DEFAULT_ADDRESS

uint8_t xdata buffer[14]; //usado em testConnection e getIntStatus
uint8_t xdata *dmpPacketBuffer;
uint16_t xdata dmpPacketSize;
uint8_t xdata malloc_memory_pool[500];

void mpu_8051_malloc_setup(){
    init_mempool (&malloc_memory_pool, sizeof(malloc_memory_pool));
}
void setClockSource(uint8_t source) {
    i2c_mpu_writeBits(MPU_endereco, MPU6050_RA_PWR_MGMT_1, MPU6050_PWR1_CLKSEL_BIT, MPU6050_PWR1_CLKSEL_LENGTH, source);
}
void setFullScaleGyroRange(uint8_t range) {
    i2c_mpu_writeBits(MPU_endereco, MPU6050_RA_GYRO_CONFIG, MPU6050_GCONFIG_FS_SEL_BIT, MPU6050_GCONFIG_FS_SEL_LENGTH, range);
}
void setFullScaleAccelRange(uint8_t range) {
    i2c_mpu_writeBits(MPU_endereco, MPU6050_RA_ACCEL_CONFIG, MPU6050_ACONFIG_AFS_SEL_BIT, MPU6050_ACONFIG_AFS_SEL_LENGTH, range);
}
void setSleepEnabled(bool enabled) {
    i2c_mpu_writeBit(MPU_endereco, MPU6050_RA_PWR_MGMT_1, MPU6050_PWR1_SLEEP_BIT, enabled);
}
void mpu_initialize(void){
  setClockSource(MPU6050_CLOCK_PLL_XGYRO);
  setFullScaleGyroRange(MPU6050_GYRO_FS_250);
  setFullScaleAccelRange(MPU6050_ACCEL_FS_2);
  setSleepEnabled(false);
}
bool mpu_testConnection(void){
    i2c_mpu_readBits(MPU_endereco,MPU6050_RA_WHO_AM_I, MPU6050_WHO_AM_I_BIT, MPU6050_WHO_AM_I_LENGTH, buffer);
    return buffer[0] == 0x34;
}
void getMotion6_packet(uint8_t *packet6){
    i2c_mpu_readBytes(MPU_endereco, 0x3B, 14, buffer);
    packet6[0] = buffer[0]; //Xac_H
    packet6[1] = buffer[1]; //Xac_L
    packet6[2] = buffer[2]; //Yac_H
    packet6[3] = buffer[3]; //Yac_L
    packet6[4] = buffer[4]; //Zac_H
    packet6[5] = buffer[5]; //Zac_L
    packet6[6] = buffer[8]; //Xgy_H
    packet6[7] = buffer[9]; //Xgy_L
    packet6[8] = buffer[10]; //Ygy_H
    packet6[9] = buffer[11]; //Ygy_L
    packet6[10] = buffer[12]; //Zgy_H
    packet6[11] = buffer[13]; //Zgy_L
}
void setXAccelOffset(int16_t xdata offset) large {
    i2c_mpu_writeWord(MPU_endereco,  MPU6050_RA_XA_OFFS_H, offset);
}
void setYAccelOffset(int16_t xdata offset) large {
    i2c_mpu_writeWord(MPU_endereco, MPU6050_RA_YA_OFFS_H, offset);
}
void setZAccelOffset(int16_t xdata offset) large {
    i2c_mpu_writeWord(MPU_endereco, MPU6050_RA_ZA_OFFS_H, offset);
}
void setXGyroOffset(int16_t xdata offset) large {
    i2c_mpu_writeWord(MPU_endereco, MPU6050_RA_XG_OFFS_USRH, offset);
}
void setYGyroOffset(int16_t xdata offset) large {
    i2c_mpu_writeWord(MPU_endereco, MPU6050_RA_YG_OFFS_USRH, offset);
}
void setZGyroOffset(int16_t xdata offset) large {
    i2c_mpu_writeWord(MPU_endereco, MPU6050_RA_ZG_OFFS_USRH, offset);
}

int16_t getXAccelOffset() {
    i2c_mpu_readBytes(MPU_endereco, MPU6050_RA_XA_OFFS_H, 2, buffer);
    return (((int16_t)buffer[0]) << 8) | buffer[1];
}
int16_t getYAccelOffset() {
    i2c_mpu_readBytes(MPU_endereco, MPU6050_RA_YA_OFFS_H, 2, buffer);
    return (((int16_t)buffer[0]) << 8) | buffer[1];
}
int16_t getZAccelOffset() {
    i2c_mpu_readBytes(MPU_endereco, MPU6050_RA_ZA_OFFS_H, 2, buffer);
    return (((int16_t)buffer[0]) << 8) | buffer[1];
}
int16_t getXGyroOffset() {
    i2c_mpu_readBytes(MPU_endereco, MPU6050_RA_XG_OFFS_USRH, 2, buffer);
    return (((int16_t)buffer[0]) << 8) | buffer[1];
}
int16_t getYGyroOffset() {
    i2c_mpu_readBytes(MPU_endereco, MPU6050_RA_YG_OFFS_USRH, 2, buffer);
    return (((int16_t)buffer[0]) << 8) | buffer[1];
}
int16_t getZGyroOffset() {
    i2c_mpu_readBytes(MPU_endereco, MPU6050_RA_ZG_OFFS_USRH, 2, buffer);
    return (((int16_t)buffer[0]) << 8) | buffer[1];
}

void setMemoryBank(uint8_t xdata bank, bool xdata prefetchEnabled, bool xdata userBank) large {
    bank &= 0x1F;
    if (userBank) bank |= 0x20;
    if (prefetchEnabled) bank |= 0x40;
    i2c_mpu_writeByte(MPU_endereco, MPU6050_RA_BANK_SEL, bank);
}

uint16_t getFIFOCount() {
    i2c_mpu_readBytes(MPU_endereco, MPU6050_RA_FIFO_COUNTH, 2, buffer);
    return (((uint16_t)buffer[0]) << 8) | buffer[1];
}

uint8_t getIntStatus() {
    i2c_mpu_readByte(MPU_endereco, MPU6050_RA_INT_STATUS, buffer);
    return buffer[0];
}

void setDMPEnabled(bool xdata enabled) large {
    i2c_mpu_writeBit(MPU_endereco, MPU6050_RA_USER_CTRL, MPU6050_USERCTRL_DMP_EN_BIT, enabled);
}

void resetFIFO() {
    i2c_mpu_writeBit(MPU_endereco, MPU6050_RA_USER_CTRL, MPU6050_USERCTRL_FIFO_RESET_BIT, true);
}

uint16_t dmpGetFIFOPacketSize() {
    return dmpPacketSize;
}

void getFIFOBytes(uint8_t *data_ptr, uint8_t data_len) {
    if(data_len > 0){
        i2c_mpu_readBytes(MPU_endereco, MPU6050_RA_FIFO_R_W, data_len, data_ptr);
    } else {
        *data_ptr = 0;
    }
}
void setMemoryStartAddress(uint8_t xdata address) large {
    i2c_mpu_writeByte(MPU_endereco, MPU6050_RA_MEM_START_ADDR, address);
}

//BUG: pq nao da pra colocar dentro da função?
uint8_t xdata chunkSize_wmb;
uint8_t xdata *verifyBuffer_wmb;
uint8_t xdata *progBuffer_wmb=0;
uint16_t xdata i_wmb;
uint8_t xdata j_wmb;
bool writeMemoryBlock(uint8_t xdata *data_ptr, uint16_t xdata dataSize, uint8_t xdata bank, uint8_t xdata address, bool xdata verify, bool xdata useProgMem) large {
    setMemoryBank(bank,false,false);
    setMemoryStartAddress(address);
    if (verify) verifyBuffer_wmb = (uint8_t *)malloc(MPU6050_DMP_MEMORY_CHUNK_SIZE);
    if (useProgMem) progBuffer_wmb = (uint8_t *)malloc(MPU6050_DMP_MEMORY_CHUNK_SIZE);
    for (i_wmb = 0; i_wmb < dataSize;) {
        // determine correct chunk size according to bank position and data size
        chunkSize_wmb = MPU6050_DMP_MEMORY_CHUNK_SIZE;

        // make sure we don't go past the data size
        if (i_wmb + chunkSize_wmb > dataSize) chunkSize_wmb = dataSize - i_wmb;

        // make sure this chunk doesn't go past the bank boundary (256 bytes)
        if (chunkSize_wmb > 256 - address) chunkSize_wmb = 256 - address;
        
        if (useProgMem) {
            // write the chunk of data as specified
            for (j_wmb = 0; j_wmb < chunkSize_wmb; j_wmb++) progBuffer_wmb[j_wmb] = pgm_read_byte(data_ptr + i_wmb + j_wmb);
        } else {
            // write the chunk of data as specified
            progBuffer_wmb = (uint8_t *)data_ptr + i_wmb;
        }

        i2c_mpu_writeBytes(MPU_endereco, MPU6050_RA_MEM_R_W, chunkSize_wmb, progBuffer_wmb);

        // verify data if needed
        if (verify && verifyBuffer_wmb) {
            setMemoryBank(bank,false,false);
            setMemoryStartAddress(address);
            i2c_mpu_readBytes(MPU_endereco, MPU6050_RA_MEM_R_W, chunkSize_wmb, verifyBuffer_wmb);
            if (memcmp(progBuffer_wmb, verifyBuffer_wmb, chunkSize_wmb) != 0) {
                free(verifyBuffer_wmb);
                if (useProgMem) free(progBuffer_wmb);
                return false; // uh oh.
            }
        }

        // increase byte index by [chunkSize_wmb]
        i_wmb += chunkSize_wmb;

        // uint8_t automatically wraps to 0 at 256
        address += chunkSize_wmb;

        // if we aren't done, update bank (if necessary) and address
        if (i_wmb < dataSize) {
            if (address == 0) bank++;
            setMemoryBank(bank,false,false);//BUG:XXX:TODO: estou usando false e false
            setMemoryStartAddress(address);
        }
    }
    if (verify) free(verifyBuffer_wmb);
    if (useProgMem) free(progBuffer_wmb);
    return true;
}

uint8_t xdata *progBuffer_wdcs = 0;
uint8_t xdata success_wdcs, special_wdcs;
uint16_t xdata i_wdcs, j_wdcs;
uint8_t xdata bank_wdcs, offset_wdcs, length_wdcs;
bool writeDMPConfigurationSet(uint8_t xdata *data_ptr, uint16_t xdata dataSize, bool xdata useProgMem) large {
    if (useProgMem) {
        progBuffer_wdcs = (uint8_t *)malloc(8); // assume 8-byte blocks, realloc later if necessary
    }

    // config set data is a long string of blocks with the following structure:
    // [bank_wdcs] [offset_wdcs] [length_wdcs] [byte[0], byte[1], ..., byte[length_wdcs]]
    
    for (i_wdcs = 0; i_wdcs < dataSize;) {
        if (useProgMem) {
            bank_wdcs = pgm_read_byte(data_ptr + i_wdcs++);
            offset_wdcs = pgm_read_byte(data_ptr + i_wdcs++);
            length_wdcs = pgm_read_byte(data_ptr + i_wdcs++);
        } else {
            bank_wdcs = data_ptr[i_wdcs++];
            offset_wdcs = data_ptr[i_wdcs++];
            length_wdcs = data_ptr[i_wdcs++];
        }

        // write data or perform special action
        if (length_wdcs > 0) {
            // regular block of data to write
            if (useProgMem) {
                if (sizeof(progBuffer_wdcs) < length_wdcs) progBuffer_wdcs = (uint8_t *)realloc(progBuffer_wdcs, length_wdcs);
                for (j_wdcs = 0; j_wdcs < length_wdcs; j_wdcs++) progBuffer_wdcs[j_wdcs] = pgm_read_byte(data_ptr + i_wdcs + j_wdcs);
            } else {
                progBuffer_wdcs = (uint8_t *)data_ptr + i_wdcs;
            }
            success_wdcs = writeMemoryBlock(progBuffer_wdcs, length_wdcs, bank_wdcs, offset_wdcs, true,false);
            i_wdcs += length_wdcs;
        } else {
            // special instruction
            // NOTE: this kind of behavior (what and when to do certain things)
            // is totally undocumented. This code is in here based on observed
            // behavior only, and exactly why (or even whether) it has to be here
            // is anybody's guess for now.
            if (useProgMem) {
                special_wdcs = pgm_read_byte(data_ptr + i_wdcs++);
            } else {
                special_wdcs = data_ptr[i_wdcs++];
            }
            /*Serial.print("Special command code ");
            Serial.print(special, HEX);
            Serial.println(" found...");*/
            if (special_wdcs == 0x01) {
                // enable DMP-related interrupts
                
                //setIntZeroMotionEnabled(true);
                //setIntFIFOBufferOverflowEnabled(true);
                //setIntDMPEnabled(true);
                i2c_mpu_writeByte(MPU_endereco, MPU6050_RA_INT_ENABLE, 0x32);  // single operation

                success_wdcs = true;
            } else {
                // unknown special command
                success_wdcs = false;
            }
        }
        
        if (!success_wdcs) {
            if (useProgMem) free(progBuffer_wdcs);
            return false; // uh oh
        }
    }
    if (useProgMem) free(progBuffer_wdcs);
    return true;
}

uint8_t xdata chunkSize_rmb;
uint16_t xdata i_rmb;
void readMemoryBlock(uint8_t xdata *data_ptr, uint16_t xdata dataSize, uint8_t xdata bank, uint8_t xdata address) large {
    setMemoryBank(bank,false,false);
    setMemoryStartAddress(address);
        for (i_rmb=0; i_rmb < dataSize;) {
        // determine correct chunk size according to bank position and data_ptr size
        chunkSize_rmb = MPU6050_DMP_MEMORY_CHUNK_SIZE;

        // make sure we don't go past the data_ptr size
        if (i_rmb + chunkSize_rmb > dataSize) chunkSize_rmb = dataSize - i_rmb;

        // make sure this chunk doesn't go past the bank boundary (256 bytes)
        if (chunkSize_rmb > 256 - address) chunkSize_rmb = 256 - address;

        // read the chunk of data_ptr as specified
        i2c_mpu_readBytes(MPU_endereco, MPU6050_RA_MEM_R_W, chunkSize_rmb, data_ptr + i_rmb);
        
        // increase byte index by [chunkSize_rmb]
        i_rmb += chunkSize_rmb;

        // uint8_t automatically wraps to 0 at 256
        address += chunkSize_rmb;

        // if we aren't done, update bank (if necessary) and address
        if (i_rmb < dataSize) {
            if (address == 0) bank++;
            setMemoryBank(bank,false,false);
            setMemoryStartAddress(address);
        }
    }
}
bool writeProgDMPConfigurationSet(uint8_t xdata *data_ptr, uint16_t xdata dataSize) large {
    return writeDMPConfigurationSet(data_ptr, dataSize, true);
}
bool writeProgMemoryBlock(uint8_t xdata *data_ptr, uint16_t xdata dataSize, uint8_t xdata bank, uint8_t xdata address, bool xdata verify) large {
    return writeMemoryBlock(data_ptr, dataSize, bank, address, verify, true);
}
void i2c_mpu_reset() {
    i2c_mpu_writeBit(MPU_endereco, MPU6050_RA_PWR_MGMT_1, MPU6050_PWR1_DEVICE_RESET_BIT, true);
}
void setSlaveAddress(uint8_t num, uint8_t address) {
    if (num > 3) return;
    i2c_mpu_writeByte(MPU_endereco, MPU6050_RA_I2C_SLV0_ADDR + num*3, address);
}
void setI2CMasterModeEnabled(bool enabled) {
    i2c_mpu_writeBit(MPU_endereco, MPU6050_RA_USER_CTRL, MPU6050_USERCTRL_I2C_MST_EN_BIT, enabled);
}
void resetI2CMaster() {
    i2c_mpu_writeBit(MPU_endereco, MPU6050_RA_USER_CTRL, MPU6050_USERCTRL_I2C_MST_RESET_BIT, true);
}
void setIntEnabled(uint8_t enabled) {
    i2c_mpu_writeByte(MPU_endereco, MPU6050_RA_INT_ENABLE, enabled);
}
void setRate(uint8_t rate) {
    i2c_mpu_writeByte(MPU_endereco, MPU6050_RA_SMPLRT_DIV, rate);
}
void setExternalFrameSync(uint8_t sync) {
    i2c_mpu_writeBits(MPU_endereco, MPU6050_RA_CONFIG, MPU6050_CFG_EXT_SYNC_SET_BIT, MPU6050_CFG_EXT_SYNC_SET_LENGTH, sync);
}
void setDLPFMode(uint8_t mode) {
    i2c_mpu_writeBits(MPU_endereco, MPU6050_RA_CONFIG, MPU6050_CFG_DLPF_CFG_BIT, MPU6050_CFG_DLPF_CFG_LENGTH, mode);
}
// DMP_CFG_1 register

uint8_t getDMPConfig1() {
    i2c_mpu_readByte(MPU_endereco, MPU6050_RA_DMP_CFG_1, buffer);
    return buffer[0];
}
void setDMPConfig1(uint8_t config) {
    i2c_mpu_writeByte(MPU_endereco, MPU6050_RA_DMP_CFG_1, config);
}

// DMP_CFG_2 register

uint8_t getDMPConfig2() {
    i2c_mpu_readByte(MPU_endereco, MPU6050_RA_DMP_CFG_2, buffer);
    return buffer[0];
}
void setDMPConfig2(uint8_t config) {
    i2c_mpu_writeByte(MPU_endereco, MPU6050_RA_DMP_CFG_2, config);
}
uint8_t getOTPBankValid() {
    i2c_mpu_readBit(MPU_endereco, MPU6050_RA_XG_OFFS_TC, MPU6050_TC_OTP_BNK_VLD_BIT, buffer);
    return buffer[0];
}
void setOTPBankValid(bool enabled) {
    i2c_mpu_writeBit(MPU_endereco, MPU6050_RA_XG_OFFS_TC, MPU6050_TC_OTP_BNK_VLD_BIT, enabled);
}
int8_t getXGyroOffsetTC() {
    i2c_mpu_readBits(MPU_endereco, MPU6050_RA_XG_OFFS_TC, MPU6050_TC_OFFSET_BIT, MPU6050_TC_OFFSET_LENGTH, buffer);
    return buffer[0];
}
void setXGyroOffsetTC(int8_t offset) {
    i2c_mpu_writeBits(MPU_endereco, MPU6050_RA_XG_OFFS_TC, MPU6050_TC_OFFSET_BIT, MPU6050_TC_OFFSET_LENGTH, offset);
}

// YG_OFFS_TC register

int8_t getYGyroOffsetTC() {
    i2c_mpu_readBits(MPU_endereco, MPU6050_RA_YG_OFFS_TC, MPU6050_TC_OFFSET_BIT, MPU6050_TC_OFFSET_LENGTH, buffer);
    return buffer[0];
}
void setYGyroOffsetTC(int8_t offset) {
    i2c_mpu_writeBits(MPU_endereco, MPU6050_RA_YG_OFFS_TC, MPU6050_TC_OFFSET_BIT, MPU6050_TC_OFFSET_LENGTH, offset);
}

// ZG_OFFS_TC register

int8_t getZGyroOffsetTC() {
    i2c_mpu_readBits(MPU_endereco, MPU6050_RA_ZG_OFFS_TC, MPU6050_TC_OFFSET_BIT, MPU6050_TC_OFFSET_LENGTH, buffer);
    return buffer[0];
}
void setZGyroOffsetTC(int8_t offset) {
    i2c_mpu_writeBits(MPU_endereco, MPU6050_RA_ZG_OFFS_TC, MPU6050_TC_OFFSET_BIT, MPU6050_TC_OFFSET_LENGTH, offset);
}

uint8_t getMotionDetectionThreshold() {
    i2c_mpu_readByte(MPU_endereco, MPU6050_RA_MOT_THR, buffer);
    return buffer[0];
}
void setMotionDetectionThreshold(uint8_t threshold) {
    i2c_mpu_writeByte(MPU_endereco, MPU6050_RA_MOT_THR, threshold);
}
uint8_t getZeroMotionDetectionThreshold() {
    i2c_mpu_readByte(MPU_endereco, MPU6050_RA_ZRMOT_THR, buffer);
    return buffer[0];
}

void setZeroMotionDetectionThreshold(uint8_t threshold) {
    i2c_mpu_writeByte(MPU_endereco, MPU6050_RA_ZRMOT_THR, threshold);
}
uint8_t getMotionDetectionDuration() {
    i2c_mpu_readByte(MPU_endereco, MPU6050_RA_MOT_DUR, buffer);
    return buffer[0];
}

void setMotionDetectionDuration(uint8_t duration) {
    i2c_mpu_writeByte(MPU_endereco, MPU6050_RA_MOT_DUR, duration);
}
 
uint8_t getZeroMotionDetectionDuration() {
    i2c_mpu_readByte(MPU_endereco, MPU6050_RA_ZRMOT_DUR, buffer);
    return buffer[0];
}

void setZeroMotionDetectionDuration(uint8_t duration) {
    i2c_mpu_writeByte(MPU_endereco, MPU6050_RA_ZRMOT_DUR, duration);
}
bool getFIFOEnabled() {
    i2c_mpu_readBit(MPU_endereco, MPU6050_RA_USER_CTRL, MPU6050_USERCTRL_FIFO_EN_BIT, buffer);
    return buffer[0];
}

void setFIFOEnabled(bool enabled) {
    i2c_mpu_writeBit(MPU_endereco, MPU6050_RA_USER_CTRL, MPU6050_USERCTRL_FIFO_EN_BIT, enabled);
}
void resetDMP() {
    i2c_mpu_writeBit(MPU_endereco, MPU6050_RA_USER_CTRL, MPU6050_USERCTRL_DMP_RESET_BIT, true);
}
uint8_t dmpInitialize() {
    // reset device
    //DEBUG_PRINTLN(F("\n\nResetting MPU6050..."));
    i2c_mpu_reset();
    delay_ms(30); // wait after reset

    // enable sleep mode and wake cycle
    /*Serial.println(F("Enabling sleep mode..."));
    setSleepEnabled(true);
    Serial.println(F("Enabling wake cycle..."));
    setWakeCycleEnabled(true);*/

    // disable sleep mode
    //DEBUG_PRINTLN(F("Disabling sleep mode..."));
    setSleepEnabled(false);
    // get MPU hardware revision
    //DEBUG_PRINTLN(F("Selecting user bank 16..."));
    setMemoryBank(0x10, true, true);
    //DEBUG_PRINTLN(F("Selecting memory byte 6..."));
    setMemoryStartAddress(0x06);
    //DEBUG_PRINTLN(F("Checking hardware revision..."));
    //DEBUG_PRINT(F("Revision @ user[16][6] = "));
    //DEBUG_PRINTLNF(readMemoryByte(), HEX);
    //DEBUG_PRINTLN(F("Resetting memory bank selection to 0..."));
    setMemoryBank(0, false, false);

    // check OTP bank valid
    //DEBUG_PRINTLN(F("Reading OTP bank valid flag..."));
    //DEBUG_PRINT(F("OTP bank is "));
   // DEBUG_PRINTLN(getOTPBankValid() ? F("valid!") : F("invalid!"));

    // get X/Y/Z gyro offsets
    //DEBUG_PRINTLN(F("Reading gyro offset TC values..."));
    int8_t xgOffsetTC = getXGyroOffsetTC();
    int8_t ygOffsetTC = getYGyroOffsetTC();
    int8_t zgOffsetTC = getZGyroOffsetTC();
    //DEBUG_PRINT(F("X gyro offset = "));
    //DEBUG_PRINTLN(xgOffsetTC);
    //DEBUG_PRINT(F("Y gyro offset = "));
    //DEBUG_PRINTLN(ygOffsetTC);
    //DEBUG_PRINT(F("Z gyro offset = "));
    //DEBUG_PRINTLN(zgOffsetTC);

    // setup weird slave stuff (?)
    //DEBUG_PRINTLN(F("Setting slave 0 address to 0x7F..."));
    setSlaveAddress(0, 0x7F);
    //DEBUG_PRINTLN(F("Disabling I2C Master mode..."));
    setI2CMasterModeEnabled(false);
    //DEBUG_PRINTLN(F("Setting slave 0 address to 0x68 (self)..."));
    setSlaveAddress(0, 0x68);
    ///DEBUG_PRINTLN(F("Resetting I2C Master control..."));
    resetI2CMaster();
    delay_ms(20);

    // load DMP code into memory banks
    //DEBUG_PRINT(F("Writing DMP code to MPU memory banks ("));
    //DEBUG_PRINT(MPU6050_DMP_CODE_SIZE);
    //DEBUG_PRINTLN(F(" bytes)"));
    if (writeProgMemoryBlock(dmpMemory, MPU6050_DMP_CODE_SIZE)) {
        //DEBUG_PRINTLN(F("Success! DMP code written and verified."));

        // write DMP configuration
        //DEBUG_PRINT(F("Writing DMP configuration to MPU memory banks ("));
        //DEBUG_PRINT(MPU6050_DMP_CONFIG_SIZE);
        //DEBUG_PRINTLN(F(" bytes in config def)"));
        if (writeProgDMPConfigurationSet(dmpConfig, MPU6050_DMP_CONFIG_SIZE)) {
            //DEBUG_PRINTLN(F("Success! DMP configuration written and verified."));

            //DEBUG_PRINTLN(F("Setting clock source to Z Gyro..."));
            setClockSource(MPU6050_CLOCK_PLL_ZGYRO);

            //DEBUG_PRINTLN(F("Setting DMP and FIFO_OFLOW interrupts enabled..."));
            setIntEnabled(0x12);

            //DEBUG_PRINTLN(F("Setting sample rate to 200Hz..."));
            setRate(4); // 1khz / (1 + 4) = 200 Hz

            //DEBUG_PRINTLN(F("Setting external frame sync to TEMP_OUT_L[0]..."));
            setExternalFrameSync(MPU6050_EXT_SYNC_TEMP_OUT_L);

            //DEBUG_PRINTLN(F("Setting DLPF bandwidth to 42Hz..."));
            setDLPFMode(MPU6050_DLPF_BW_42);

            //DEBUG_PRINTLN(F("Setting gyro sensitivity to +/- 2000 deg/sec..."));
            setFullScaleGyroRange(MPU6050_GYRO_FS_2000);

            //DEBUG_PRINTLN(F("Setting DMP configuration bytes (function unknown)..."));
            setDMPConfig1(0x03);
            setDMPConfig2(0x00);

            //DEBUG_PRINTLN(F("Clearing OTP Bank flag..."));
            setOTPBankValid(false);

            //DEBUG_PRINTLN(F("Setting X/Y/Z gyro offset TCs to previous values..."));
            setXGyroOffsetTC(xgOffsetTC);
            setYGyroOffsetTC(ygOffsetTC);
            setZGyroOffsetTC(zgOffsetTC);

            //DEBUG_PRINTLN(F("Setting X/Y/Z gyro user offsets to zero..."));
            //setXGyroOffset(0);
            //setYGyroOffset(0);
            //setZGyroOffset(0);

            //DEBUG_PRINTLN(F("Writing final memory update 1/7 (function unknown)..."));
            uint8_t dmpUpdate[16], j;
            uint16_t pos = 0;
            for (j = 0; j < 4 || j < dmpUpdate[2] + 3; j++, pos++) dmpUpdate[j] = pgm_read_byte(&dmpUpdates[pos]);
            writeMemoryBlock(dmpUpdate + 3, dmpUpdate[2], dmpUpdate[0], dmpUpdate[1]);

            //DEBUG_PRINTLN(F("Writing final memory update 2/7 (function unknown)..."));
            for (j = 0; j < 4 || j < dmpUpdate[2] + 3; j++, pos++) dmpUpdate[j] = pgm_read_byte(&dmpUpdates[pos]);
            writeMemoryBlock(dmpUpdate + 3, dmpUpdate[2], dmpUpdate[0], dmpUpdate[1]);

            //DEBUG_PRINTLN(F("Resetting FIFO..."));
            resetFIFO();

            //DEBUG_PRINTLN(F("Reading FIFO count..."));
            uint16_t fifoCount = getFIFOCount();
            uint8_t fifoBuffer[128];

            //DEBUG_PRINT(F("Current FIFO count="));
            //DEBUG_PRINTLN(fifoCount);
            getFIFOBytes(fifoBuffer, fifoCount);

            //DEBUG_PRINTLN(F("Setting motion detection threshold to 2..."));
            setMotionDetectionThreshold(2);

            //DEBUG_PRINTLN(F("Setting zero-motion detection threshold to 156..."));
            setZeroMotionDetectionThreshold(156);

            //DEBUG_PRINTLN(F("Setting motion detection duration to 80..."));
            setMotionDetectionDuration(80);

            //DEBUG_PRINTLN(F("Setting zero-motion detection duration to 0..."));
            setZeroMotionDetectionDuration(0);

            //DEBUG_PRINTLN(F("Resetting FIFO..."));
            resetFIFO();

            //DEBUG_PRINTLN(F("Enabling FIFO..."));
            setFIFOEnabled(true);

            //DEBUG_PRINTLN(F("Enabling DMP..."));
            setDMPEnabled(true);

            DEBUG_PRINTLN(F("Resetting DMP..."));
            resetDMP();

            //DEBUG_PRINTLN(F("Writing final memory update 3/7 (function unknown)..."));
            for (j = 0; j < 4 || j < dmpUpdate[2] + 3; j++, pos++) dmpUpdate[j] = pgm_read_byte(&dmpUpdates[pos]);
            writeMemoryBlock(dmpUpdate + 3, dmpUpdate[2], dmpUpdate[0], dmpUpdate[1]);

            //DEBUG_PRINTLN(F("Writing final memory update 4/7 (function unknown)..."));
            for (j = 0; j < 4 || j < dmpUpdate[2] + 3; j++, pos++) dmpUpdate[j] = pgm_read_byte(&dmpUpdates[pos]);
            writeMemoryBlock(dmpUpdate + 3, dmpUpdate[2], dmpUpdate[0], dmpUpdate[1]);

            //DEBUG_PRINTLN(F("Writing final memory update 5/7 (function unknown)..."));
            for (j = 0; j < 4 || j < dmpUpdate[2] + 3; j++, pos++) dmpUpdate[j] = pgm_read_byte(&dmpUpdates[pos]);
            writeMemoryBlock(dmpUpdate + 3, dmpUpdate[2], dmpUpdate[0], dmpUpdate[1]);

            //DEBUG_PRINTLN(F("Waiting for FIFO count > 2..."));
            while ((fifoCount = getFIFOCount()) < 3);

            //DEBUG_PRINT(F("Current FIFO count="));
            //DEBUG_PRINTLN(fifoCount);
            //DEBUG_PRINTLN(F("Reading FIFO data..."));
            getFIFOBytes(fifoBuffer, fifoCount);

            //DEBUG_PRINTLN(F("Reading interrupt status..."));

            //DEBUG_PRINT(F("Current interrupt status="));
            //DEBUG_PRINTLNF(getIntStatus(), HEX);

            //DEBUG_PRINTLN(F("Reading final memory update 6/7 (function unknown)..."));
            for (j = 0; j < 4 || j < dmpUpdate[2] + 3; j++, pos++) dmpUpdate[j] = pgm_read_byte(&dmpUpdates[pos]);
            readMemoryBlock(dmpUpdate + 3, dmpUpdate[2], dmpUpdate[0], dmpUpdate[1]);

            //DEBUG_PRINTLN(F("Waiting for FIFO count > 2..."));
            while ((fifoCount = getFIFOCount()) < 3);

            //DEBUG_PRINT(F("Current FIFO count="));
            //DEBUG_PRINTLN(fifoCount);

            //DEBUG_PRINTLN(F("Reading FIFO data..."));
            getFIFOBytes(fifoBuffer, fifoCount);

            //DEBUG_PRINTLN(F("Reading interrupt status..."));

            //DEBUG_PRINT(F("Current interrupt status="));
            //DEBUG_PRINTLNF(getIntStatus(), HEX);

            //DEBUG_PRINTLN(F("Writing final memory update 7/7 (function unknown)..."));
            for (j = 0; j < 4 || j < dmpUpdate[2] + 3; j++, pos++) dmpUpdate[j] = pgm_read_byte(&dmpUpdates[pos]);
            writeMemoryBlock(dmpUpdate + 3, dmpUpdate[2], dmpUpdate[0], dmpUpdate[1]);

            //DEBUG_PRINTLN(F("DMP is good to go! Finally."));

            //DEBUG_PRINTLN(F("Disabling DMP (you turn it on later)..."));
            setDMPEnabled(false);

            //DEBUG_PRINTLN(F("Setting up internal 42-byte (default) DMP packet buffer..."));
            dmpPacketSize = 42;
            /*if ((dmpPacketBuffer = (uint8_t *)malloc(42)) == 0) {
                return 3; // TODO: proper error code for no memory
            }*/

            //DEBUG_PRINTLN(F("Resetting FIFO and clearing INT status one last time..."));
            resetFIFO();
            getIntStatus();
        } else {
            //DEBUG_PRINTLN(F("ERROR! DMP configuration verification failed."));
            return 2; // configuration block loading failed
        }
    } else {
        //DEBUG_PRINTLN(F("ERROR! DMP code verification failed."));
        return 1; // main binary block loading failed
    }
    return 0; // success
}
uint8_t dmpGetQuaternion_int16(int16_t *data_ptr, const uint8_t* packet) {
    // TODO: accommodate different arrangements of sent data (ONLY default supported now)
    if (packet == 0) packet = dmpPacketBuffer;
    data_ptr[0] = ((packet[0] << 8) | packet[1]);
    data_ptr[1] = ((packet[4] << 8) | packet[5]);
    data_ptr[2] = ((packet[8] << 8) | packet[9]);
    data_ptr[3] = ((packet[12] << 8) | packet[13]);
    return 0;
}
#endif
