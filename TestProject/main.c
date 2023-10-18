
#include "engr2350_msp432.h"

int main(void)
{
    volatile uint32_t i;
    uint8_t input;

    SysInit();

    printf("\n\n\n\n\n\n");
    printf("**************************\r\n");
    printf("Test Project for ENGR-2350\r\n");
    printf("**************************\r\n");
    printf("LED1 on the MSP-EXP432P401R will toggle at a fixed rate.\r\n");
    printf("Press R, G, or B to toggle LED2's color channels.\r\n");
    printf("--------------------------\r\n");

    // Set P1.0 to output direction (LED1)
    GPIO_setAsOutputPin(
        GPIO_PORT_P1,
        GPIO_PIN0
        );
    // Set P2.0,.1,.2 to output direction (LED2)
    GPIO_setAsOutputPin(
        GPIO_PORT_P2,
        GPIO_PIN0|GPIO_PIN1|GPIO_PIN2
        );
    // Turn LED2 off
    GPIO_setOutputLowOnPin(
        GPIO_PORT_P2,
        GPIO_PIN0|GPIO_PIN1|GPIO_PIN2
        );

    while(1)
    {
        // Toggle P1.0 output
        GPIO_toggleOutputOnPin(
            GPIO_PORT_P1,
			GPIO_PIN0
			);
        if(GPIO_getInputPinValue(
                GPIO_PORT_P1,
                GPIO_PIN0
                )){

            printf("LED1 is: ON\r\n");
        }else
        {
            printf("LED1 is: OFF\r\n");
        }

        input = getchar_nw();
        if(input == 'r' || input == 'R'){
            GPIO_toggleOutputOnPin(
                GPIO_PORT_P2,
                GPIO_PIN0
                );
            printf("LED2 RED Channel Toggled\r\n");
        }else if(input == 'g' || input == 'G'){
            GPIO_toggleOutputOnPin(
                GPIO_PORT_P2,
                GPIO_PIN1
                );
            printf("LED2 GREEN Channel Toggled\r\n");
        }else if(input == 'b' || input == 'B'){
            GPIO_toggleOutputOnPin(
                GPIO_PORT_P2,
                GPIO_PIN2
                );
            printf("LED2 BLUE Channel Toggled\r\n");
        }


        // Delay
        for(i=100000; i>0; i--);
    }
}
