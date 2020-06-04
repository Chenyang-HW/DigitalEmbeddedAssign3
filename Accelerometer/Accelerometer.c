/*
******************************************************************
                    Accelerometer Applications(c)
@file   Accelerometer.c
@author Liu Chenyang, 16206550
@date   2020.03.31
@brief  Accelerometer ADXL326 configuration
********************************************************************
*/
// INCLUDES---------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include "typedefs.h"   // type definitions
#include "SPImaster.h"  // functions from the hardware team

// TYPEDEFS---------------------------------------------------------
#define CS_LOW()                      //Chip select pin
#define CS_UP()
#define NONE 0
#define DISP 1
#define ACL                 2        //ADXL326 Slave ID
#define TIMES               10       //Sample rate

// ADXL326 Communication Commands*/
#define regWrite            0x0A
#define regRead             0x0B
#define regFIFO             0x0D

// Registers*/
#define DEVID_AD            0x00
#define XDATA               0x08    //X-axis acceleration data
#define YDATA               0x09
#define ZDATA               0x0A
#define STATUS              0x0B
#define Xaccel_L            0x0E    //X-axis Register(LSM)
#define Xaccel_H            0x0F    //X-axis Register(MSB)
#define Yaccel_L            0x10
#define Yaccel_H            0x11
#define Zaccel_L            0x12
#define Zaccel_H            0x13
#define SOFT_RESET          0x1F
#define THRESH_ACT_L        0x20    //8 bits THRESH_ACT_L
#define THRESH_ACT_H        0x21    //3 bits THRESH_ACT_H
#define TIME_ACT            0x22    //Activity time
#define THRESH_INACT_L      0x23    //
#define THRESH_INACT_H      0x24    //
#define TIME_INACT_L        0x25    //
#define TIME_INACT_H        0x26    //
#define ACT_INACT_CTL       0x27
#define FIFO_CONTROL        0x28
#define FIFO_SAMPLES        0x29
#define INTMAP1             0x2A    //INTMAP1
#define INTMAP2             0x2B    //
#define FILTER_CTL          0x2C    //FILTER_CTL
#define POWER_CTL           0x2D    //POWER_CTL
#define SELF_TEST           0x2E    //SELF_TEST

/*
 *@brief delay
 *@32MHz MCU clock it takes 32 "nop"s for 1 us delay
 */
void Delay_ms(int16 microSecs)
{
    while(microSecs--)
        {
            for(int i=0; i<32; i++)
                asm("nop");
        }
}

void Delay(void)
{
     for(int i=0; i<8; i++)
        asm("nop");
}

/*
 *@brief auto-wake mode
 */
void ADXLInit(void)
{
    //ADXLregWrite(FILTER_CTL,0x0A);             //INAT 2, output data rate 50HZ

    /*Configure and enable Loop mode to detect operation*/
    ADXLregWrite(ACT_INACT_CTL,0x3F);

    /*Active threshold value is 250 codes: write 0xFA to register
     *THRESH_ACT_L; write 0x00 to THRESH_ACT_H
     */
    ADXLregWrite(THRESH_ACT_L,0xFA);
    ADXLregWrite(THRESH_ACT_H,0x00);

    /*activity timer */
    ADXLregWrite(TIME_ACT,50);

    /*Inactive threshold value is 150 codes*/
    ADXLregWrite(THRESH_INACT_L,0x96);
    ADXLregWrite(THRESH_INACT_H,0x00);

    /*Inactivity timer to 30 samples: write 30 decimal (0x1E) to it*/
    ADXLregWrite(TIME_INACT_L,0x1E);
    ADXLregWrite(TIME_INACT_H,0x00);

    ADXLregWrite(FILTER_CTL,0x01);    //ODR 25Hz
    /*Map the AWAKE bit to INT2*/
    ADXLregWrite(INTMAP2,0x2B);

    /*Operate measurement*/
    ADXLregWrite(POWER_CTL,0x0A);

    /*Clear statues of ACT and INAT bits by reading statues*/
    ADXLregWrite(STATUS,0x40);
}

/* Function to read from a register in the ADXL362.
   The argument is the number of the register to read.
   The return value is the value read from the register. */
uint8 ADXLregRead (uint8 regNumber)
{
    //printf("Trying to read register %02x\n", regNumber);
    uint8 value;

    CS_LOW();
    Delay();
    SPIbyte(regRead);   //send regRead commend to master
    value=SPIbyte(regNumber); //receive reg-value from reg-address
    SPIbyte(0x00);
    Delay();
    CS_UP();
    return value;
}

/* Function to write to a register in the ADXL362.
   Arguments are the register number and the value to write. */
void ADXLregWrite (uint8 regNumber, uint8 value)
{
    printf("Trying to change register %2x to value %2x\n",regNumber, value);

    CS_LOW();
    Delay();
    SPIbyte(regWrite);//send regWrite commend to master
    SPIbyte(regNumber);
    SPIbyte(value);//write reg-value to reg-address
    Delay();
    CS_UP();
}

/* Function to read 3 acceleration values from the ADXL362.
   The arguments are pointers to variables that hold the
   acceleration values. This function will change
   the values of those three variables. */
/*
 *@param measure range: 3g
 *       sample rate: 10samples/sec
 */
void ADXLreadAccel(int16 *Xaccel,int16 *Yaccel,int16 *Zaccel)
{
    int16 tempx=0, tempy=0, tempz=0;
    uint8 i;

   for(i=0;i<10;i++){
        ADXLgetAccel(Xaccel,Yaccel,Zaccel);
        Delay_ms(10);
        tempx+=(int16)*Xaccel;
        tempy+=(int16)*Yaccel;
        tempz+=(int16)*Zaccel;
    }
    //*Xaccel = tempx/TIMES;
    *Xaccel = tempx/TIMES;
    *Yaccel = tempy/TIMES;
    *Zaccel = tempz/TIMES;
}
void ADXLgetAccel(int16 *Xaccel,int16 *Yaccel,int16 *Zaccel)
{
    uint8 BUF[6];

    BUF[0]=ADXLregRead(Xaccel_L);//X-axis 8-LSB
    BUF[1]=ADXLregRead(Xaccel_H);//X-axis 8-MSB

    BUF[2]=ADXLregRead(Yaccel_L);
    BUF[3]=ADXLregRead(Yaccel_H);

    BUF[4]=ADXLregRead(Zaccel_L);
    BUF[5]=ADXLregRead(Zaccel_H);

    *Xaccel=(((int16)BUF[1] << 8)|BUF[0]);
    *Yaccel=(((int16)BUF[3] << 8)|BUF[2]);
    *Zaccel=(((int16)BUF[5] << 8)|BUF[4]);
}
/* main function to call the other functions and test them */
int main()
{
   // uint8 regNum,regVal;         // register number and value
    int16 Xaccel,Yaccel,Zaccel;                 // acceleration components

    SPIselect(NONE);                    //initial SPI
    SPIselect(ACL);                     //select the ADXL362
    /*ADXL362 Register Reset*/
    ADXLregWrite(SOFT_RESET,0x52);             //Software reset
    Delay_ms(10);

    ADXLregWrite(STATUS,0x40);
    ADXLregWrite(ACT_INACT_CTL,0x00);
    ADXLregWrite(FIFO_CONTROL,0x00);
    ADXLregWrite(INTMAP2,0x00);                 //interrupt
    ADXLregWrite(FILTER_CTL,0x13);
    ADXLregWrite(POWER_CTL,0x00);               //Standby mode
    ADXLregWrite(SELF_TEST,0x00);

    ADXLInit();
    Delay_ms(10);
   /* printf("\nReading registers\n");
    for (regNum = 0x0E; regNum < 0x2D; regNum++)
    {
        regVal = ADXLregRead( regNum );
        printf("Read from register %02x, got value %02x\n", regNum, regVal);
    }*/

    /*printf("\nWriting registers\n");
    for (regNum = 0x0E; regNum < 0x13; regNum++)
    {
        regVal += 22;   // some number
        ADXLregWrite(regNum, regVal);
        printf("Wrote to register %02x, value %02x\n", regNum, regVal);
        regVal = ADXLregRead( regNum );
        printf("Read back value %02x\n", regVal);
   }*/

    printf("\nReading acceleration\n");
    ADXLreadAccel(&Xaccel, &Yaccel, &Zaccel);
    float valXaccel=Xaccel*1.5;ADXLregWrite(XDATA,valXaccel);
    float valYaccel=Yaccel*1.5;ADXLregWrite(YDATA,valYaccel);
    float valZaccel=Zaccel*1.5;ADXLregWrite(ZDATA,valZaccel);
    printf("Acceleration values X %f, Y %f, Z %f\n",valXaccel,valYaccel,valZaccel);

    return 0;
}
