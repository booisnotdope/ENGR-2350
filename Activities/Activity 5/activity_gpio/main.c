// **ACTIVITY**: Simple GPIO
// ENGR-2350
// Name: Ryan So
// RIN: 662042337

#include "engr2350_msp432.h"

// Add function prototypes here, as needed.
void GPIOInit();
void GetInputs();
void RegisterLogic();
void DriverLibLogic();

// Add global variables here, as needed.
uint8_t pb1;
uint8_t pb2;
uint8_t ss1;


// Main Function
int main(void)
{
    // We always call the "SysInit()" first to set up the microcontroller
    // for how we are going to use it.
    SysInit();

    // Place initialization code (or run-once) code here
    GPIOInit();

    while(1){
        // Place code that runs continuously in here
        GetInputs();


        // Functions to calculate outputs
        RegisterLogic();
         //DriverLibLogic();



        //// TEST CODE BELOW ////

        // Test print of inputs
        printf("PB1: %u, PB2: %u, SS1: %u\r\n",pb1,pb2,ss1);

        // Code to test the outputs

//        uint32_t count;
        // &= (AND=) sets bits low, |= (OR=) sets bits high,
        // ^= (Exclusive OR=) toggles the value of a bit
//        P6OUT ^= 0x02;  // Replace number to toggle one leg of BiLED1
//        P5OUT ^= 0x02;  // Replace number to toggle LED1
//        for(count=100000; count>0; count--); // This Creates a crude delay
//        P6OUT ^= 0x01;  // Replace number to toggle the other leg of BiLED1
//        for(count=100000; count>0; count--); // This Creates a crude delay


        //// END OF TEST CODE ////
    }
}

// Add function declarations here as needed
void GPIOInit(){
    // Configure inputs and outputs
    P6DIR |= 0x03; // Set P6.0 and P6.1 to output
    P3DIR &= ~0x20; // Set P3.5 to input
    P5DIR |= 0x02;  //Set P5.1 to LED1 output
    P5DIR &= ~0xC0; //Set P5.6 to pb1 input and set P5.7 to pb2 input

    // GPIO_setAsInputPin(GPIO_PORT_3, GPIO_PIN5);
    // GPIO_setAsInputPin(GPIO_PORT_5, GPIO_PIN6 | GPIO_PIN7);
    // GPIO_setAsOutputPin(GPIO_PORT_5, GPIO_PIN1);
    // GPIO_setAsOutputPin(GPIO_PORT_6, GPIO_PIN0 | GPIO_PIN1);

}

void GetInputs(){
    // Read the input values
    ss1 = (P3IN & 0x20) != 0;    // Determine ss1 value.
    pb1 = (P5IN & 0x40) != 0; // Determine pb1 value.
    pb2 = (P5IN & 0x80) != 0; // Determine pb2 value.
    // ss1 = GPIO_getInputPinValue(GPIO_PORT_3,GPIO_PIN5) != 0;
    // pb1 = GPIO_getInputPinValue(GPIO_PORT_5,GPIO_PIN6) != 0;
    // pb2 = GPIO_getInputPinValue(GPIO_PORT_5,GPIO_PIN7) != 0;
}

void RegisterLogic(){
    if( ss1 ){    // Check if slide switch is ON
        if( pb1 && pb2 ){
            // Turn BiLED1 OFF by setting both pins to the same value
            P6OUT |= 0x03;  // This sets both 6.0&6.1 to 1 (xxxxxx11)
            P5OUT |= 0x02; //Turns LED1 on

        }else if( pb1 ){ // **ACTIVITY**: Check if pushbutton 1 is pressed (replace 0)
            // Turn BiLED1 to 1 color by setting both pins to the opposite value
           P6OUT |= 0x02;  // Set P6.1 to 1 (xxxxxx1x)
           P6OUT &= ~0x01; // Then set P6.0 to 0 (xxxxxx10)
           P5OUT &= ~0x02; //Turns LED1 off

        }else if( pb2 ){ // Check if pushbutton 2 is pressed
            // **ACTIVITY**: Turn BiLED1 to the other color by... and also turn LED1 ON
            P6OUT |= 0x01;  // Set P6.0 to 1 (xxxxxxx1)
            P6OUT &= ~0x02; // Then set P6.1 to 0 (xxxxxx01)
            P5OUT |= 0x02; //Turns LED1 on

        }else{
            P6OUT &= ~0x03; //Turns off 6.0 and 6.1
            P5OUT &= ~0x02; //set P5.1 to 0
        }
    }else{
        // Turn everything off
        P6OUT &= ~0x03;  // This sets both 6.0&6.1 to 0 (xxxxxx00)
        P5OUT &= ~0x02; // set pin 5.1 to 0 (xxxxxx0x)
    }
}

void DriverLibLogic(){
    // **ACTIVITY**: Finish the code in this function (look at comments)
    if( ss1 ){    // Check if slide switch is ON
        if( pb1 && pb2 ){
            // Turn BiLED1 OFF by setting both pins to the same value
            GPIO_setOutputLowOnPin(GPIO_PORT_P6,GPIO_PIN0|GPIO_PIN1);
            // **ACTIVITY**: Turn LED1 ON (add command below)
            GPIO_setOutputHighOnPin(GPIO_PORT_P5, GPIO_PIN1);

        }else if( pb1 ){
            // Turn BiLED1 to 1 color by setting both pins to the opposite value
            GPIO_setOutputHighOnPin(GPIO_PORT_P6,GPIO_PIN1);
            GPIO_setOutputLowOnPin(GPIO_PORT_P6,GPIO_PIN0);
            GPIO_setOutputLowOnPin(GPIO_PORT_P5, GPIO_PIN1);

        }else if( pb2 ){
            GPIO_setOutputLowOnPin(GPIO_PORT_P6,GPIO_PIN1);
            GPIO_setOutputHighOnPin(GPIO_PORT_P6,GPIO_PIN0);
            GPIO_setOutputHighOnPin(GPIO_PORT_P5, GPIO_PIN1);


        }else{
            GPIO_setOutputLowOnPin(GPIO_PORT_P6,GPIO_PIN0);
            GPIO_setOutputLowOnPin(GPIO_PORT_P6,GPIO_PIN1);
            GPIO_setOutputLowOnPin(GPIO_PORT_P5,GPIO_PIN1);
        }
    }else{
        GPIO_setOutputLowOnPin(GPIO_PORT_P6,GPIO_PIN0);
        GPIO_setOutputLowOnPin(GPIO_PORT_P6,GPIO_PIN1);
        GPIO_setOutputLowOnPin(GPIO_PORT_P5,GPIO_PIN1);
    }
}

// Add interrupt functions last so they are easy to find
