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
void Compare_Init();
void Capture_Init();
void Interrupts_Init();
void CCR_ISR();

// Add global variables here, as needed.
Timer_A_ContinuousModeConfig timer_config_cont;
Timer_A_UpModeConfig timer_config_PMW;
Timer_A_CompareModeConfig CCR_three, CCR_four;
Timer_A_CaptureModeConfig CCR_zero, CCR_one;

uint16_t CCR_three_compareValue = 100, CCR_four_compareValue = 100;

const uint8_t radius = 35;

/* Variables for left motor */
uint32_t enc_total_L; //total left wheel encoder 
int32_t enc_counts_track_L;
int32_t enc_counts_L;
uint8_t enc_flag_L;                             
uint16_t capture_value_L;
int32_t wheel_speed_sum_L;
int32_t wheel_speed_counts_L;

/* Variables for right motor*/
uint32_t enc_total_R;
int32_t enc_counts_track_R;
int32_t enc_counts_R;
uint8_t enc_flag_R;                             
uint16_t capture_value_R;
int32_t wheel_speed_sum_R;
int32_t wheel_speed_counts_R;

int32_t setpoint = 65000;

int main (void) /** Main Function **/
{  
    // Add local variables here, as needed.

    // We always call the "SysInit()" first to set up the microcontroller
    // for how we are going to use it.
    SysInit();
    GPIO_Init();
    Timer_Init();
    Compare_Init();
    Capture_Init();
    Interrupts_Init();

    // Place initialization code (or run-once) code here
    printf("Program Started\r\n");
    while(1)
    {  
        // Place code that runs continuously in here
    }   
}   

// Add function declarations here as needed
void GPIO_Init(){
    GPIO_setAsOutputPin(GPIO_PORT_P3, GPIO_PIN6 | GPIO_PIN7); // Enable    P3.7: left motor P3.6 right motor
    GPIO_setOutputHighOnPin(GPIO_PORT_P3, GPIO_PIN6 | GPIO_PIN7);
    GPIO_setAsOutputPin(GPIO_PORT_P5, GPIO_PIN4 | GPIO_PIN5); // Direction P5.4: left motor P5.5 right motor
    GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P2,  /* Speed     P2.7: left motor P2.6 right motor*/
                                               GPIO_PIN6 | GPIO_PIN7, 
                                               GPIO_PRIMARY_MODULE_FUNCTION);                                                              
    GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P10, /*Encoder    P10.4 left wheel P10.5 right wheel*/
                                               GPIO_PIN4 | GPIO_PIN5, 
                                               GPIO_PRIMARY_MODULE_FUNCTION);
}
void Timer_Init(){
    //enable a clock for PMW
    timer_config_PMW.clockSource = TIMER_A_CLOCKSOURCE_SMCLK;
    timer_config_PMW.clockSourceDivider = TIMER_A_CLOCKSOURCE_DIVIDER_1;
    timer_config_PMW.timerPeriod = 800;
    timer_config_PMW.timerClear = TIMER_A_DO_CLEAR;
    Timer_A_configureUpMode(TIMER_A0_BASE, &timer_config_PMW);

    //enable continuous clock
    timer_config_cont.clockSource = TIMER_A_CLOCKSOURCE_SMCLK;
    timer_config_cont.clockSourceDivider = TIMER_A_CLOCKSOURCE_DIVIDER_1;
    timer_config_cont.timerInterruptEnable_TAIE = TIMER_A_TAIE_INTERRUPT_ENABLE;
    timer_config_cont.timerClear = TIMER_A_DO_CLEAR;
    Timer_A_configureContinuousMode(TIMER_A3_BASE, &timer_config_cont);
}
void Compare_Init(){
    //left motor
    CCR_three.compareRegister = TIMER_A_CAPTURECOMPARE_REGISTER_3;
    CCR_three.compareInterruptEnable = TIMER_A_CAPTURECOMPARE_INTERRUPT_DISABLE;
    CCR_three.compareOutputMode = TIMER_A_OUTPUTMODE_RESET_SET;
    CCR_three.compareValue = CCR_three_compareValue;
    Timer_A_initCompare(TIMER_A3_BASE, &CCR_three);

    //right motor
    CCR_four.compareRegister = TIMER_A_CAPTURECOMPARE_REGISTER_4;
    CCR_four.compareInterruptEnable = TIMER_A_CAPTURECOMPARE_INTERRUPT_DISABLE;
    CCR_four.compareOutputMode = TIMER_A_OUTPUTMODE_RESET_SET;
    CCR_four.compareValue = CCR_four_compareValue;
    Timer_A_initCompare(TIMER_A3_BASE, &CCR_four);
}
void Capture_Init(){
    //right motor
    CCR_zero.captureRegister = TIMER_A_CAPTURECOMPARE_REGISTER_0;
    CCR_zero.captureMode = TIMER_A_CAPTUREMODE_RISING_EDGE;
    CCR_zero.captureInputSelect = TIMER_A_CAPTURE_INPUTSELECT_CCIxA;
    CCR_zero.synchronizeCaptureSource = TIMER_A_CAPTURE_SYNCHRONOUS;
    CCR_zero.captureInterruptEnable = TIMER_A_CAPTURECOMPARE_INTERRUPT_ENABLE;
    Timer_A_initCapture(TIMER_A3_BASE, &CCR_zero);

    //left motor
    CCR_one.captureRegister = TIMER_A_CAPTURECOMPARE_REGISTER_1;
    CCR_one.captureMode = TIMER_A_CAPTUREMODE_RISING_EDGE;
    CCR_one.captureInputSelect = TIMER_A_CAPTURE_INPUTSELECT_CCIxA;
    CCR_one.synchronizeCaptureSource = TIMER_A_CAPTURE_SYNCHRONOUS;
    CCR_one.captureInterruptEnable = TIMER_A_CAPTURECOMPARE_INTERRUPT_ENABLE;
    Timer_A_initCapture(TIMER_A3_BASE, &CCR_one);
}
void Interrupts_Init(){
    Timer_A_registerInterrupt(TIMER_A3_BASE, TIMER_A_CCR0_INTERRUPT, CCR_ISR);
    Timer_A_registerInterrupt(TIMER_A3_BASE, TIMER_A_CCRX_AND_OVERFLOW_INTERRUPT, CCR_ISR);

    Timer_A_startCounter(TIMER_A3_BASE, TIMER_A_CONTINUOUS_MODE);
    Timer_A_startCounter(TIMER_A0_BASE, TIMER_A_UP_MODE);
}
// Add interrupt functions last so they are easy to find
void CCR_ISR() {
    if (Timer_A_getEnabledInterruptStatus(TIMER_A3_BASE) == TIMER_A_INTERRUPT_PENDING) {
        Timer_A_clearInterruptFlag(TIMER_A3_BASE);
        enc_counts_track_L += 65536;
        enc_counts_track_R += 65536;
    }
    if (Timer_A_getCaptureCompareEnabledInterruptStatus(TIMER_A3_BASE, TIMER_A_CAPTURECOMPARE_REGISTER_0) & TIMER_A_CAPTURECOMPARE_INTERRUPT_FLAG) {
        Timer_A_clearCaptureCompareInterrupt(TIMER_A3_BASE, TIMER_A_CAPTURECOMPARE_REGISTER_0);
        enc_total_L++;
        capture_value_L = Timer_A_getCaptureCompareCount(TIMER_A3_BASE, TIMER_A_CAPTURECOMPARE_REGISTER_0);
        enc_counts_L = enc_counts_track_L + capture_value_L;
        enc_counts_track_L = -capture_value_L;
        enc_flag_L = 1;
        wheel_speed_sum_L += enc_counts_L;
        wheel_speed_counts_L++;
        if (wheel_speed_counts_L == 6) {
            if (wheel_speed_sum_L / 6 > setpoint) {
                CCR_three_compareValue++;
            }
            if (wheel_speed_sum_L / 6 < setpoint) {
                CCR_three_compareValue--;
            }
            Timer_A_setCompareValue(TIMER_A0_BASE,
                                    TIMER_A_CAPTURECOMPARE_REGISTER_3,
                                    CCR_three_compareValue);
            wheel_speed_sum_L = 0;
            wheel_speed_counts_L = 0;
            printf("Right: %u, Left: %u\r\n", CCR_three_compareValue, CCR_four_compareValue);
        }
    }
    if (Timer_A_getCaptureCompareEnabledInterruptStatus(TIMER_A3_BASE, TIMER_A_CAPTURECOMPARE_REGISTER_1) & TIMER_A_CAPTURECOMPARE_INTERRUPT_FLAG) {
        Timer_A_clearCaptureCompareInterrupt(TIMER_A3_BASE, TIMER_A_CAPTURECOMPARE_REGISTER_1);
        enc_total_R++;
        capture_value_R = Timer_A_getCaptureCompareCount(TIMER_A3_BASE, TIMER_A_CAPTURECOMPARE_REGISTER_1);
        enc_counts_R = enc_counts_track_R + capture_value_R;
        enc_counts_track_R = -capture_value_R;
        enc_flag_R = 1;
        wheel_speed_sum_R += enc_counts_R;
        wheel_speed_counts_R++;
        if (wheel_speed_counts_R == 6) {
            if (wheel_speed_sum_R / 6 > setpoint) {
                CCR_four_compareValue++;
            }
            if (wheel_speed_sum_R / 6 < setpoint) {
                CCR_four_compareValue--;
            }
            Timer_A_setCompareValue(TIMER_A0_BASE,
                                    TIMER_A_CAPTURECOMPARE_REGISTER_4,
                                    CCR_four_compareValue);
            wheel_speed_sum_R = 0;
            wheel_speed_counts_R = 0;
            printf("Right: %u, Left: %u\r\n", CCR_three_compareValue, CCR_four_compareValue);
        }
    }
}
