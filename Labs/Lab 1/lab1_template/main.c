// Lab 1
// ENGR-2350
// Name: Ryan So, Jack
// RIN: 662042337,

#include "engr2350_msp432.h"
#include "lab1lib.h"
#include <stdlib.h>

void GPIOInit();
void TestIO();
void ControlSystem();

uint8_t LEDFL = 0; // Two variables to store the state of
uint8_t LEDFR = 0; // the front left/right LEDs (on-car)
uint8_t toggle = 0;

int main(void)
{

    SysInit(); // Basic car initialization
    init_Sequence(); // Initializes the Lab1Lib Driver
    GPIOInit();

    printf("\r\n\n"
           "***********\r\n"
           "Lab 1 Start\r\n"
           "***********\r\n");

    while(1){
//        TestIO(); // Used in Part A to test the IO
        ControlSystem(); // Used in Part B to implement the desired functionality
    }
}

void GPIOInit(){
    // Add initializations of inputs and outputs
    //Inputs P3.5, P5.6, P4.0, P4.2, P4.3, P4.5, P4.6, P4.7
    //slide switch input
    GPIO_setAsInputPin(GPIO_PORT_P3, GPIO_PIN5);
    //pushbutton input
    GPIO_setAsInputPin(GPIO_PORT_P5, GPIO_PIN6);
    //bumper inputs
    GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P4, GPIO_PIN0 | GPIO_PIN2 | GPIO_PIN3 | GPIO_PIN5 | GPIO_PIN6 | GPIO_PIN7);

    //Outputs P3.6, P3.7, P5.4, P5.5, P6.0, P6.1, P8.0, 8.5
    //Turns on/off right and left motor respectively.
    GPIO_setAsOutputPin(GPIO_PORT_P3, GPIO_PIN6 | GPIO_PIN7);
    //Connects to direction of left and right motor respectively.
    GPIO_setAsOutputPin(GPIO_PORT_P5, GPIO_PIN4 | GPIO_PIN5);
    //Connects to BiLED
    GPIO_setAsOutputPin(GPIO_PORT_P6, GPIO_PIN0 | GPIO_PIN1);
    //Connects to LEDFL and LEDFR respectively.
    GPIO_setAsOutputPin(GPIO_PORT_P8, GPIO_PIN0 | GPIO_PIN5);
}

void TestIO(){
    // Add printf statement(s) for testing inputs

    // Example code for testing outputs
    while(1){
        uint8_t cmd = getchar();
        if(cmd == 'a'){
            // Turn LEDL On
            GPIO_setOutputHighOnPin(GPIO_PORT_P8, GPIO_PIN0);
            printf("LEDL is on.\n\r");

        }else if(cmd == 'z'){
            // Turn LEDL Off
            GPIO_setOutputLowOnPin(GPIO_PORT_P8,GPIO_PIN0);
            printf("LEDL is off.\n\r");

        }else if(cmd == 's'){
            //Turn LEDR On
            GPIO_setOutputHighOnPin(GPIO_PORT_P8, GPIO_PIN5);
            printf("LEDR is on.\n\r");

        } else if (cmd == 'x'){
            //Turn LEDR Off
            GPIO_setOutputLowOnPin(GPIO_PORT_P8,GPIO_PIN5);
            printf("LEDR is off.\n\r");

        } else if (cmd == 'q'){
            //Turn BiLED Red
            GPIO_setOutputLowOnPin(GPIO_PORT_P6,GPIO_PIN0);
            GPIO_setOutputHighOnPin(GPIO_PORT_P6, GPIO_PIN1);
            printf("Turn BiLED Red.\n\r");

        } else if (cmd == 'w'){
            //Turn BiLED Off
            GPIO_setOutputLowOnPin(GPIO_PORT_P6,GPIO_PIN0 | GPIO_PIN1);
            printf("Turn BiLED off.\n\r");

        } else if (cmd == 'e'){
            //Turn BiLED Green
            GPIO_setOutputLowOnPin(GPIO_PORT_P6,GPIO_PIN1);
            GPIO_setOutputHighOnPin(GPIO_PORT_P6, GPIO_PIN0);
            printf("Turn BiLED Green.\n\r");
        }
    }
}

void ControlSystem(){
    //checks if the slide switch is on or off. On means the
    //pattern is ready.
    if(GPIO_getInputPinValue(GPIO_PORT_P3, GPIO_PIN5) == 0x01){
        //error checking for the sequence. Checks if
        //the sequence is empty or if the car can run.
        if(status_Sequence() == 100){
            uint8_t sequence_start = run_Sequence();
            if(sequence_start == 1){
//                printf("Sequence is empty.\n\r");
//                exit(1);
            } else if (sequence_start == 2){
                printf("GPIO not set up properly.\n\r");
                exit(1);
            } else {
                printf("Sequence has started.\n\r");
                //Turn BiLED Greens
                GPIO_setOutputLowOnPin(GPIO_PORT_P6,GPIO_PIN0);
                GPIO_setOutputHighOnPin(GPIO_PORT_P6, GPIO_PIN1);
                while(status_Sequence() != 100){
                }
                //Turn BiLED Red
                GPIO_setOutputLowOnPin(GPIO_PORT_P6,GPIO_PIN1);
                GPIO_setOutputHighOnPin(GPIO_PORT_P6, GPIO_PIN0);
                clear_Sequence();
            }
        }
    } else {
        //turns off the BiLED and starts adding segments to pattern
        GPIO_setOutputLowOnPin(GPIO_PORT_P6,GPIO_PIN0 | GPIO_PIN1);
        int8_t recorded_Segment = 2;
        //each bumper button represents a different
        __delay_cycles(240e3);
        if(GPIO_getInputPinValue(GPIO_PORT_P4, GPIO_PIN0) == 0x00){
            while(GPIO_getInputPinValue(GPIO_PORT_P4, GPIO_PIN0) == 0x00){}
            //BMP0
            __delay_cycles(240e3);
            recorded_Segment = record_Segment(2);
            printf("Turn right 90 degrees added\n\r");
        } else if(GPIO_getInputPinValue(GPIO_PORT_P4, GPIO_PIN2) == 0x00){
            while(GPIO_getInputPinValue(GPIO_PORT_P4, GPIO_PIN2) == 0x00){}
            //BMP1
            __delay_cycles(240e3);
            recorded_Segment = record_Segment(1);
            printf("Turn right 45 degrees added\n\r");
        } else if(GPIO_getInputPinValue(GPIO_PORT_P4, GPIO_PIN3) == 0x00){
            while(GPIO_getInputPinValue(GPIO_PORT_P4, GPIO_PIN3) == 0x00){}
            //BMP2
            __delay_cycles(240e3);
            recorded_Segment = record_Segment(0);
            printf("Drive forward added\n\r");
        } else if(GPIO_getInputPinValue(GPIO_PORT_P4, GPIO_PIN5) == 0x00){
            while(GPIO_getInputPinValue(GPIO_PORT_P4, GPIO_PIN5) == 0x00){}
            //BMP3
            __delay_cycles(240e3);
            recorded_Segment = record_Segment(127);
            printf("Stop for 1s added\n\r");
        } else if(GPIO_getInputPinValue(GPIO_PORT_P4, GPIO_PIN6) == 0x00){
            while(GPIO_getInputPinValue(GPIO_PORT_P4, GPIO_PIN6) == 0x00){}
            //BMP4
            __delay_cycles(240e3);
            recorded_Segment = record_Segment(-1);
            printf("Turn left 45 degrees added\n\r");
        } else if(GPIO_getInputPinValue(GPIO_PORT_P4, GPIO_PIN7) == 0x00){
            while(GPIO_getInputPinValue(GPIO_PORT_P4, GPIO_PIN7) == 0x00){}
            //BMP5
            __delay_cycles(240e3);
            recorded_Segment = record_Segment(-2);
            printf("Turn left 90 degrees added\n\r");
        } else {
            //if no bumper buttons are pressed and the pushbutton is
            //pressed, delete the most recent segment.
            if (GPIO_getInputPinValue(GPIO_PORT_P5, GPIO_PIN6) == 0x01){
                while(GPIO_getInputPinValue(GPIO_PORT_P5, GPIO_PIN6) == 0x01){}
                __delay_cycles(240e3);
                uint8_t pop_Segment_value = pop_Segment();
                if(pop_Segment_value){
                    printf("No segment to erase\n\r");
                } else {
                    printf("segment deleted.\n\r");
                }
            }
        }
        if (recorded_Segment == 0){
            if (toggle == 0){
                //turn on right turn off left
                GPIO_setOutputHighOnPin(GPIO_PORT_P8, GPIO_PIN5);
                GPIO_setOutputLowOnPin(GPIO_PORT_P8,GPIO_PIN0);
                toggle = 1;
            } else {
                //turn on left turn off right
                GPIO_setOutputHighOnPin(GPIO_PORT_P8, GPIO_PIN0);
                GPIO_setOutputLowOnPin(GPIO_PORT_P8,GPIO_PIN5);
                toggle = 0;
            }
        }
        //delay
        __delay_cycles(240e3);
        //error checking for record_Segement()
        if(recorded_Segment == 1){
            printf("Segment has reached maximum length (50)\n\r");
        } else if (recorded_Segment == -1){
            printf("Value of mv is not in the table\n\r");
        }
    }
}
