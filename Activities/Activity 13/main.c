/**********************************************************************/
//** ENGR-2350 Template Project
//** Name: XXXX
//** RIN: XXXX
//** This is the base project for several activities and labs throughout
//** the course.  The outline provided below isn't necessarily *required*
//** by a C program; however, this format is required within ENGR-2350
//** to ease debugging/grading by the staff.
/**********************************************************************/

// We'll always add this include statement. This basically takes the
// code contained within the "engr_2350_msp432.h" file and adds it here.
#include "engr2350_msp432.h"

// Add function prototypes here, as needed.
void GPIOInit();
void I2CInit();
void I2C_writeData( uint32_t moduleInstance , uint8_t PeriphAddress , uint8_t StartReg , uint8_t * data , uint8_t len );
void I2C_readData( uint32_t moduleInstance , uint8_t PeriphAddress , uint8_t StartReg , uint8_t * data , uint8_t len );
uint16_t readCompass();
uint16_t readRanger();

// Add global variables here, as needed.
eUSCI_I2C_MasterConfig I2C;
uint16_t heading;
uint16_t range;

int main (void) /** Main Function **/
{  
    // Add local variables here, as needed.

    // We always call the "SysInit()" first to set up the microcontroller
    // for how we are going to use it.
    SysInit();
    GPIOInit();
    I2CInit();
    // Place initialization code (or run-once) code here
    printf("Running\r\n");

    while(1)
    {  
        // Place code that runs continuously in here
//        printf("Compass: %u\r\n", readCompass());
         printf("Compass: %4u\tRanger: %4u\r\n",readCompass(),readRanger());
    }   
}   

// Add function declarations here as needed
void GPIOInit(){
    GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P1, GPIO_PIN7, GPIO_PRIMARY_MODULE_FUNCTION);
    GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P1, GPIO_PIN6, GPIO_PRIMARY_MODULE_FUNCTION);

}
void I2CInit(){
    I2C.selectClockSource = EUSCI_B_I2C_CLOCKSOURCE_SMCLK;
    I2C.i2cClk = 24000000;
    I2C.dataRate = EUSCI_B_I2C_SET_DATA_RATE_100KBPS;
    I2C.byteCounterThreshold = 0;
    I2C_initMaster(EUSCI_B0_BASE, &I2C);
    I2C_enableModule(EUSCI_B0_BASE);
}
void I2C_writeData(uint32_t moduleInstance
                  ,uint8_t PeriphAddress
                  ,uint8_t StartReg
                  ,uint8_t *data
                  ,uint8_t len){
    I2C_setSlaveAddress(moduleInstance,PeriphAddress); // Set the peripheral address
    I2C_setMode(moduleInstance,EUSCI_B_I2C_TRANSMIT_MODE); // Indicate a write operation

    I2C_masterSendMultiByteStart(moduleInstance,StartReg); // Start the communication.
                // This function does three things. It sends the START signal,
                // sends the address, and then sends the start register.

    // This code loops through all of the bytes to send.
    uint8_t ctr;
    for(ctr = 0;ctr<len;ctr++){
        I2C_masterSendMultiByteNext(moduleInstance,data[ctr]);
    }
    // Once all bytes are sent, the I2C transaction is stopped by sending the STOP signal
    I2C_masterSendMultiByteStop(moduleInstance);

    __delay_cycles(200); // A short delay to avoid starting another I2C transaction too quickly
}

void I2C_readData(uint32_t moduleInstance
                 ,uint8_t PeriphAddress
                 ,uint8_t StartReg
                 ,uint8_t *data
                 ,uint8_t len){
    // First write the start register to the peripheral device. This can be
    // done by using the I2C_writeData function with a length of 0.
    I2C_writeData(moduleInstance,PeriphAddress,StartReg,0,0);

    Interrupt_disableMaster(); //  Disable all interrupts to prevent timing issues

    // Then do read transaction...
    I2C_setSlaveAddress(moduleInstance,PeriphAddress); // Set the peripheral address
    I2C_setMode(moduleInstance,EUSCI_B_I2C_RECEIVE_MODE); // Indicate a read operation
    I2C_masterReceiveStart(moduleInstance); // Start the communication. This function
                // doe two things: It first sends the START signal and
                // then sends the peripheral address. Once started, the eUSCI
                // will automatically fetch bytes from the peripheral until
                // a STOP signal is requested to be sent.

    // This code loops through 1 less than all bytes to receive
    uint8_t ctr;
    for(ctr = 0;ctr<(len-1);ctr++){
        uint32_t tout_tmp = 10000;
        while(!(EUSCI_B_CMSIS(moduleInstance)->IFG & EUSCI_B_IFG_RXIFG0) && --tout_tmp); // Wait for a data byte to become available
        if(tout_tmp){
            data[ctr] = I2C_masterReceiveMultiByteNext(moduleInstance); // read and store received byte
        }else{
            data[ctr] = 0xFF;
        }
    }
    // Prior to receiving the final byte, request the STOP signal such that the
    // communication will halt after the byte is received.
    data[ctr] = I2C_masterReceiveMultiByteFinish(moduleInstance); // send STOP, read and store received byte

    Interrupt_enableMaster(); // Re-enable interrupts

    __delay_cycles(200); // A short delay to avoid starting another I2C transaction too quickly
}
uint16_t readCompass(){
    __delay_cycles(2.4e6);
    uint8_t array[2];
    uint8_t result;
    I2C_readData(EUSCI_B0_BASE, 0x60, 2, array, 2);
    result = (array[0] << 8) + array[1];
    heading = result;
    return result;
}
uint16_t readRanger(){
    __delay_cycles(2.4e6);
    uint8_t array[2];
    uint8_t result; 
    I2C_readData(EUSCI_B0_BASE, 0x70, 2, array, 2);
    result = (array[0] << 8) + array[1];
    uint8_t temp = 0x51;
    I2C_writeData(EUSCI_B0_BASE, 0x70, 0, &temp, 1);
    return result;
}
// Add interrupt functions last so they are easy to find
