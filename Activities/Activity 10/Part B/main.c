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
#include <math.h>

// Add function prototypes here, as needed.
void GPIO_Init();
void Timer_Init();
void Encoder_ISR();

// Add global variables here, as needed.
Timer_A_ContinuousModeConfig timer_config;
Timer_A_CaptureModeConfig CCR0;

uint32_t enc_total;
int32_t enc_counts_track;
int32_t enc_counts;
uint8_t enc_flag;

uint16_t capture_value;

int main (void) /** Main Function **/
{  
    // Add local variables here, as needed.

    // We always call the "SysInit()" first to set up the microcontroller
    // for how we are going to use it.
    SysInit();
    GPIO_Init();
    Timer_Init();
    uint8_t radius = 35;
    // Place initialization code (or run-once) code here

    while(1)
    {  
        // Place code that runs continuously in here
        if (enc_flag) {
            enc_flag = 0;
            uint16_t distance = (1.0/360) * enc_total * (2 * M_PI * radius);
            uint16_t speed_rpm = (1.0/360) * (24000000.0 / enc_counts) * 60;
            uint16_t speed_mm = (2 * M_PI / 360.0) * (24000000.0 / enc_counts) * radius;
            printf("%5u mm\t%5u rpm\t%5u mm/s\r\n",distance,speed_rpm,speed_mm);
        }
    }   
}   

// Add function declarations here as needed
void GPIO_Init(){
    GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P10, GPIO_PIN4, GPIO_PRIMARY_MODULE_FUNCTION);
}

void Timer_Init(){
    timer_config.clockSource = TIMER_A_CLOCKSOURCE_SMCLK;    
    timer_config.clockSourceDivider = TIMER_A_CLOCKSOURCE_DIVIDER_1;
    timer_config.timerInterruptEnable_TAIE = TIMER_A_TAIE_INTERRUPT_ENABLE;
    timer_config.timerClear = TIMER_A_DO_CLEAR;
    Timer_A_configureContinuousMode(TIMER_A3_BASE, &timer_config);

    CCR0.captureRegister = TIMER_A_CAPTURECOMPARE_REGISTER_0;
    CCR0.captureMode = TIMER_A_CAPTUREMODE_RISING_EDGE;
    CCR0.captureInputSelect = TIMER_A_CAPTURE_INPUTSELECT_CCIxA;
    CCR0.synchronizeCaptureSource = TIMER_A_CAPTURE_SYNCHRONOUS;
    CCR0.captureInterruptEnable = TIMER_A_CAPTURECOMPARE_INTERRUPT_ENABLE;
    Timer_A_initCapture(TIMER_A3_BASE, &CCR0);

    Timer_A_registerInterrupt(TIMER_A3_BASE, TIMER_A_CCRX_AND_OVERFLOW_INTERRUPT, Encoder_ISR);
    Timer_A_registerInterrupt(TIMER_A3_BASE, TIMER_A_CCR0_INTERRUPT, Encoder_ISR);

    Timer_A_startCounter(TIMER_A3_BASE, TIMER_A_CONTINUOUS_MODE);
}

void Encoder_ISR(){
    if(Timer_A_getEnabledInterruptStatus(TIMER_A3_BASE) == TIMER_A_INTERRUPT_PENDING){
        Timer_A_clearInterruptFlag(TIMER_A3_BASE);
        enc_counts_track += 65536;
    }
    if(Timer_A_getCaptureCompareEnabledInterruptStatus(TIMER_A3_BASE, TIMER_A_CAPTURECOMPARE_REGISTER_0) == TIMER_A_CAPTURECOMPARE_INTERRUPT_FLAG){
        Timer_A_clearCaptureCompareInterrupt(TIMER_A3_BASE, TIMER_A_CAPTURECOMPARE_REGISTER_0);
        capture_value = Timer_A_getCaptureCompareCount(TIMER_A3_BASE, TIMER_A_CAPTURECOMPARE_REGISTER_0);
        enc_total++;
        enc_counts = enc_counts_track + capture_value;
        enc_counts_track = -capture_value;
        enc_flag = 1;
    }
}
// Add interrupt functions last so they are easy to find
