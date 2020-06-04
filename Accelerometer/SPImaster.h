#include "typedefs.h"

// define names for SPI slaves
#define NONE 0
#define DISP 1
#define ACL  2

/* Function to activate the slave select signal (active low) for
   a specific slave device. Use argument ACL to select the
   ADXL362. Use argument NONE to de-select all slaves. */
void SPIselect ( uint8 slaveNumber );

/* Function to generate 8 clock pulses and transfer 8 bits on
   MOSI and MISO, MSB first. The argument is the byte to send
   on MOSI. The function will not return until the transfer is
   complete. The return value is the byte received on MISO. */
uint8 SPIbyte ( uint8 byteTX );
