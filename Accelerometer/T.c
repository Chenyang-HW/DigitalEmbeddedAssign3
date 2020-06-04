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

void ADCLInit(void)
{
    ADXLregWrite(SOFT_RESET,0x52);	//software reset
    ADXLregWrite(THRESH_ACT_L,0x1F);
    ADXLregWrite(THRESH_ACT_H,0x00);
    ADXLregWrite(TIME_ACT,50);
    ADXLregWrite(THRESH_INACT_L,0x09);
    ADXLregWrite(THRESH_INACT_H,0x00);
    ADXLregWrite(TIME_INACT_L,0x01);
    ADXLregWrite(TIME_INACT_H,0x00);
    ADXLregWrite(ACT_INACT_CTL,0x03);
    ADXLregWrite(FIFO_CONTROL,0x00);
    ADXLregWrite(FIFO_SAMPLES,0x80);
    ADXLregWrite(INTMAP1,0x84);
    ADXLregWrite(INTMAP2,0x90);
    ADXLregWrite(FILTER_CTL,0x82);
    //ADXLregRead(STATUS);
    ADXLregWrite(POWER_CTL,0x02);
    ADXLregRead(0x00);

}
/* Function to read from a register in the ADXL362.
   The argument is the number of the register to read.
   The return value is the value read from the register. */
uint8 ADXLregRead ( uint8 regNumber )
{
    printf("Trying to read register %02x\n", regNumber);
    return regNumber;  // stupid return value for testing
}

/* Function to write to a register in the ADXL362.
   Arguments are the register number and the value to write. */
void ADXLregWrite ( uint8 regNumber, uint8 value )
{
    printf("Trying to change register %2x to value %2x\n",
           regNumber, value);
}

/* Function to read 3 acceleration values from the ADXL362.
   The arguments are pointers to variables that hold the
   acceleration values. This function will change
   the values of those three variables. */
void ADXLgetAccel ( int16 *Xaccel, int16 *Yaccel, int16 *Zaccel )
{
    printf("Getting acceleration values...\n");
    *Xaccel = (int16) 5;
    *Yaccel = (int16) 65530;
    *Zaccel = (int16) 1002;
}

/* main function to call the other functions and test them */
int main()
{
    uint8 regNum, regVal;         // register number and value
    int16 Xaccel, Yaccel, Zaccel; // acceleration components

    printf("\nReading registers\n");
    for (regNum = 0; regNum < 11; regNum++)
    {
        regVal = ADXLregRead( regNum );
        printf("Read from register %02x, got value %02x\n", regNum, regVal);
    }

    printf("\nWriting registers\n");
    for (regNum = 0x20; regNum < 0x23; regNum++)
    {
        regVal += 22;   // some number
        ADXLregWrite(regNum, regVal);
        printf("Wrote to register %02x, value %02x\n", regNum, regVal);
        regVal = ADXLregRead( regNum );
        printf("Read back value %02x\n", regVal);
   }

    printf("\nReading acceleration\n");
    ADXLgetAccel(&Xaccel, &Yaccel, &Zaccel);
    printf("Acceleration values X %d Y %d Z %d\n", Xaccel, Yaccel, Zaccel);

    return 0;
}
