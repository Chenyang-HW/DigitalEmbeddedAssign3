/* Example program for Accelerometer Applications assignment.
   You need to include this file, SPImaster.h and typedefs.h
   in your project.
   You need to tell your linker where to find the library file
   with the SPI master functions: libSPImaster.a
   In CodeBlocks, right-click on the project, select Build Options...
   On the Linker Settings tab, click Add, browse to find the file
   and add it - you can keep it as a relative path.
   Then on the Search Directories path, add the folder where the
   file is located - a relative path should be fine.
   Then you should be able to compile and run this program,
   and get results - they will not be sensible results until you
   put sensible code in the three functions that you are to write. */

#include <stdio.h>
#include <stdlib.h>
#include "typedefs.h"   // type definitions
#include "SPImaster.h"  // functions from the hardware team

#define SOFT_RESET          0x1F
#define THRESH_ACT_L        0x20
#define THRESH_ACT_H        0x21
#define TIME_ACT            0x22
#define THRESH_INACT_L      0x23
#define THRESH_INACT_H      0x24
#define TIME_INACT_L        0x25
#define TIME_INACT_H        0x26
#define ACT_INACT_CTL       0x27
#define FIFO_CONTROL        0x28
#define FIFO_SAMPLES        0x29
#define INTMAP1             0x2A
#define INTMAP2             0x2B
#define FILTER_CTL          0x2C
#define STATUS              0x0B
#define POWER_CTL           0x2D

#define NONE 0
#define DISP 1
#define ACL  2

#define ADXL345_AVER_TIME 10

void Delay_ms(int16 microSecs)
{
    while(microSecs--)
        {
            for(int i=0; i<32; i++) asm("nop");
        }
}

void Delay_5us0(void)

{
     for(int i=0; i<5; i++) asm("nop");
}


/* Function to read from a register in the ADXL362.
   The argument is the number of the register to read.
   The return value is the value read from the register. */
uint8 ADXLregRead ( uint8 regNumber )
{
    uint8 value;
    printf("Trying to read register %02x\n", regNumber);
    SPIselect(NONE);
    Delay_ms(5);
    SPIbyte(0x0A);
    SPIbyte(regNumber);
    SPIselect(NONE);
    SPIbyte(0x0B);
   value=SPIbyte(regNumber);
     Delay_ms(5);
    SPIselect(ACL);
    return value;
}

/* Function to write to a register in the ADXL362.
   Arguments are the register number and the value to write. */
void ADXLregWrite ( uint8 regNumber, uint8 value )
{
    printf("Trying to change register %2x to value %2x\n",
           regNumber, value);
    SPIselect(NONE);
    Delay_ms(5);
    SPIbyte(0x0A);
    SPIbyte(regNumber);
    SPIbyte(value);
    Delay_ms(5);
    SPIbyte(ACL);
}

void ADCLInit(void)//configure
{
    ADXLregWrite(SOFT_RESET,0x52);	//software reset
    Delay_ms(200);
    ADXLregWrite(THRESH_ACT_L,0xFA);
    ADXLregWrite(THRESH_ACT_H,0x00);
    ADXLregWrite(TIME_ACT,30);
    ADXLregWrite(THRESH_INACT_L,0x96);
    ADXLregWrite(THRESH_INACT_H,0x00);
    ADXLregWrite(TIME_INACT_L,0x1E);
    ADXLregWrite(TIME_INACT_H,0x00);
    ADXLregWrite(ACT_INACT_CTL,0x3F);
    ADXLregWrite(FIFO_CONTROL,0x00);
    ADXLregWrite(FIFO_SAMPLES,0x80);
    ADXLregWrite(INTMAP1,0x84);
    ADXLregWrite(INTMAP2,0x40);
    ADXLregWrite(FILTER_CTL,0x10);
    ADXLregRead(STATUS);
    ADXLregWrite(POWER_CTL,0x2D);
    Delay_ms(200);
    ADXLregRead(0x00);
    ADXLregWrite(POWER_CTL,0x00);
}


/* Function to read 3 acceleration values from the ADXL362.
   The arguments are pointers to variables that hold the
   acceleration values. This function will change
   the values of those three variables. */
   //10 samples/rate


void ADXLreadAccel(int16 *Xaccel, int16 *Yaccel, int16 *Zaccel){
    uint8 BUF[6];
    uint8 i;
    SPIselect(NONE);
    SPIbyte(0x0B);
    SPIbyte(0x0F);
    SPIbyte(0x0A);
   /* BUF[0]=ADXLregRead(0x0E);
    BUF[1]=ADXLregRead(0x0F);

    BUF[2]=ADXLregRead(0x10);
    BUF[3]=ADXLregRead(0x11);

    BUF[4]=ADXLregRead(0x12);
    BUF[5]=ADXLregRead(0x13);*/
    for(i=0; i<6;i++){
        if(i==5)BUF[1]=SPIbyte(0);
        else BUF[1]=SPIbyte(1);
    }
SPIselect(ACL);
    *Xaccel=(((int16)BUF[1] << 8)|BUF[0])-65536;
    *Yaccel=(((int16)BUF[3] << 8)|BUF[2])-65536;
    *Zaccel=(((int16)BUF[5] << 8)|BUF[4])-65536;

     printf("RAcceleration values X %d Y %d Z %d\n", *Xaccel, *Yaccel, *Zaccel);

}

void ADXLgetAccel ( int16 *Xaccel, int16 *Yaccel, int16 *Zaccel )
{
    printf("Getting acceleration values...\n");
    /*int16 tempx=0, tempy=0, tempz=0;
    uint8 i;
    for(i=0;i<10;i++){
        ADXLreadAccel(Xaccel,Yaccel,Zaccel);
        Delay_ms(10);
        tempx+=(int16)*Xaccel;
        tempy+=(int16)*Yaccel;
        tempz+=(int16)*Zaccel;
    }
    *Xaccel = tempx/10;
    *Yaccel = tempy/10;
    *Zaccel = tempz/10;*/

ADXLreadAccel(Xaccel,Yaccel,Zaccel);
*Xaccel=(*Xaccel*0.244)/1000;
    printf("GAcceleration values X %fG Y %d Z %d\n", *Xaccel, *Yaccel, *Zaccel);

}

void ADXLreadAverage(int16 *Xaccel, int16 *Yaccel, int16 *Zaccel){
    uint8 i;
    int16 tx,ty,tz;
    *Xaccel=0;*Yaccel=0;*Zaccel=0;

    if(ADXL345_AVER_TIME){
    for(i=0;i<ADXL345_AVER_TIME;i++){
          ADXLreadAccel(&tx, &ty, &tz);
          *Xaccel+=tx;
          *Yaccel+=ty;
          *Zaccel+=tz;
          Delay_ms(5);
    }
    *Xaccel/=ADXL345_AVER_TIME;
    *Yaccel/=ADXL345_AVER_TIME;
    *Zaccel/=ADXL345_AVER_TIME;
}
    printf("RAx = %d, y = %d ,z = %d \r\n",*Xaccel,*Yaccel,*Zaccel);
  //  ADXLregWrite(0x1E,*Xaccel);
    //ADXLregWrite(0x1F,*Yaccel);
    //ADXLregWrite(0x20,*Zaccel);
}


/* main function to call the other functions and test them */
int main()
{
    uint8 regNum, regVal;         // register number and value
    int16 Xaccel, Yaccel, Zaccel; // acceleration components

    ADCLInit();

    /*printf("\nReading registers\n");
    for (regNum = 0x0E; regNum < 0x15; regNum++)
    {
        regVal = ADXLregRead( regNum );
        printf("Read from register %02x, got value %02x\n", regNum, regVal);
    }*/

   /* printf("\nWriting registers\n");
    for (regNum = 0x0E; regNum < 0x13; regNum++)
    {
        regVal += 22;   // some number
        ADXLregWrite(regNum, regVal);
        printf("Wrote to register %02x, value %02x\n", regNum, regVal);
        regVal = ADXLregRead( regNum );
        printf("Read back value %02x\n", regVal);
   }*/

    printf("\nReading acceleration\n");
    //ADXLgetAccel(&Xaccel, &Yaccel, &Zaccel);
    ADXLgetAccel(&Xaccel, &Yaccel, &Zaccel);
    printf("Acceleration values X %d Y %d Z %d\n", Xaccel, Yaccel, Zaccel);

     printf("\nReading registers\n");
    for (regNum = 0x0E; regNum < 0x15; regNum++)
    {
        regVal = ADXLregRead( regNum );
        printf("Read from register %02x, got value %02x\n", regNum, regVal);
    }
    return 0;
}
