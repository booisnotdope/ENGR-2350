/**********************************************************************/
//** ENGR-2350 Template Project
//** Name: Ryan So, Jack Catelotti
//** RIN: XXXX
//** This is the base project for several activities and labs throughout
//** the course.  The outline provided below isn't necessarily *required*
//** by a C program; however, this format is required within ENGR-2350
//** to ease debugging/grading by the staff.
/**********************************************************************/

// We'll always add this include statement. This basically takes the
// code contained within the "engr_2350_msp432.h" file and adds it here.
#include "engr2350_msp432.h"
#include <math.h> //This is needed to use pi as I did not want to use a float and it is shown as M_PI

// Add function prototypes here, as needed.
void GPIO_Init();
void Timer_Init();
void CCR_ISR();
void Drive(uint16_t td);
void Turn(uint16_t td, int8_t l_or_r);

// Add global variables here, as needed.
Timer_A_UpModeConfig timer_config_PWM;
Timer_A_CompareModeConfig CCR3, CCR4;
uint16_t CCR3_compareValue = 200;
uint16_t CCR4_compareValue = 200;

Timer_A_ContinuousModeConfig timer_config_ENC;
Timer_A_CaptureModeConfig CCR0, CCR1;

uint32_t enc_total_L;
int32_t enc_counts_track_L;
int32_t enc_counts_L;
uint8_t enc_flag_L;                             // Calculate counts L motor
uint16_t capture_value_L;
int32_t wheel_speed_sum_L;
int32_t wheel_speed_counts_L;

uint32_t enc_total_R;
int32_t enc_counts_track_R;
int32_t enc_counts_R;
uint8_t enc_flag_R;                             // Calculate counts R motor
uint16_t capture_value_R;
int32_t wheel_speed_sum_R;
int32_t wheel_speed_counts_R;

int32_t setpoint = 65000;
uint8_t rad = 35;
uint16_t wheel;

int main (void) /** Main Function **/
{  
    // Add local variables here, as needed.

    SysInit();

    // Place initialization code (or run-once) code here
    GPIO_Init();
    Timer_Init();

    while(1)
    {  
        // Place code that runs continuously in here

        // code for the r
        Drive(1830);
        Turn(149/2*(M_PI/2),1);
        Drive(845);
        Turn(149/2*(M_PI/2),1);
        Drive(845);
        Turn(149/2*(M_PI/2),1);
        Drive(845);
        Turn(152/2*(M_PI/2+M_PI/4),0);
        Drive(1300);
        break;

          // code for the i
        /*Drive(860);
        Turn(149/2*M_PI,1);
        Drive(430);
        Turn(63/2*M_PI,1);
        Drive(1820);
        Turn(149/2*(M_PI/2),0);
        Drive(430);
        Turn(147/2*M_PI,1);
        Drive(860);
        break;*/
    }
}

// Add function declarations here as needed
void GPIO_Init() {
    GPIO_setAsOutputPin(GPIO_PORT_P5, GPIO_PIN4 | GPIO_PIN5);
    GPIO_setOutputLowOnPin(GPIO_PORT_P5, GPIO_PIN4 | GPIO_PIN5);

    GPIO_setAsOutputPin(GPIO_PORT_P3, GPIO_PIN7 | GPIO_PIN6);
    GPIO_setOutputLowOnPin(GPIO_PORT_P3, GPIO_PIN7 | GPIO_PIN6);

    GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P2,
                                                GPIO_PIN7 | GPIO_PIN6,
                                                GPIO_PRIMARY_MODULE_FUNCTION);
    GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P10,                   // Encoders
                                               GPIO_PIN4 | GPIO_PIN5,GPIO_PRIMARY_MODULE_FUNCTION);
}

void Timer_Init() {
    timer_config_PWM.clockSource = TIMER_A_CLOCKSOURCE_SMCLK;
    timer_config_PWM.clockSourceDivider = TIMER_A_CLOCKSOURCE_DIVIDER_1;
    timer_config_PWM.timerPeriod = 800;
    timer_config_PWM.timerClear = TIMER_A_DO_CLEAR;
    Timer_A_configureUpMode(TIMER_A0_BASE, &timer_config_PWM);

    // CCR3: right motor
    CCR3.compareRegister = TIMER_A_CAPTURECOMPARE_REGISTER_3;
    CCR3.compareInterruptEnable = TIMER_A_CAPTURECOMPARE_INTERRUPT_DISABLE;
    CCR3.compareOutputMode = TIMER_A_OUTPUTMODE_RESET_SET;
    CCR3.compareValue = CCR3_compareValue;
    Timer_A_initCompare(TIMER_A0_BASE, &CCR3);

    // CCR0: right motor
    CCR0.captureRegister = TIMER_A_CAPTURECOMPARE_REGISTER_0;
    CCR0.captureMode = TIMER_A_CAPTUREMODE_RISING_EDGE;
    CCR0.captureInputSelect = TIMER_A_CAPTURE_INPUTSELECT_CCIxA;
    CCR0.synchronizeCaptureSource = TIMER_A_CAPTURE_SYNCHRONOUS;
    CCR0.captureInterruptEnable = TIMER_A_CAPTURECOMPARE_INTERRUPT_ENABLE;
    Timer_A_initCapture(TIMER_A3_BASE, &CCR0);

    // CCR4: left motor
    CCR4.compareRegister = TIMER_A_CAPTURECOMPARE_REGISTER_4;
    CCR4.compareInterruptEnable = TIMER_A_CAPTURECOMPARE_INTERRUPT_DISABLE;
    CCR4.compareOutputMode = TIMER_A_OUTPUTMODE_RESET_SET;
    CCR4.compareValue = CCR4_compareValue;
    Timer_A_initCompare(TIMER_A0_BASE, &CCR4);

    // CCR1: left motor
    CCR1.captureRegister = TIMER_A_CAPTURECOMPARE_REGISTER_1;
    CCR1.captureMode = TIMER_A_CAPTUREMODE_RISING_EDGE;
    CCR1.captureInputSelect = TIMER_A_CAPTURE_INPUTSELECT_CCIxA;
    CCR1.synchronizeCaptureSource = TIMER_A_CAPTURE_SYNCHRONOUS;
    CCR1.captureInterruptEnable = TIMER_A_CAPTURECOMPARE_INTERRUPT_ENABLE;
    Timer_A_initCapture(TIMER_A3_BASE, &CCR1);

    // Timer for encoders to detect speed
    timer_config_ENC.clockSource = TIMER_A_CLOCKSOURCE_SMCLK;
    timer_config_ENC.clockSourceDivider = TIMER_A_CLOCKSOURCE_DIVIDER_1;
    timer_config_ENC.timerInterruptEnable_TAIE = TIMER_A_TAIE_INTERRUPT_ENABLE;
    timer_config_ENC.timerClear = TIMER_A_DO_CLEAR;
    Timer_A_configureContinuousMode(TIMER_A3_BASE, &timer_config_ENC);

    Timer_A_registerInterrupt(TIMER_A3_BASE, TIMER_A_CCRX_AND_OVERFLOW_INTERRUPT, CCR_ISR);
    Timer_A_registerInterrupt(TIMER_A3_BASE, TIMER_A_CCR0_INTERRUPT, CCR_ISR);
    Timer_A_startCounter(TIMER_A3_BASE, TIMER_A_CONTINUOUS_MODE);
    Timer_A_startCounter(TIMER_A0_BASE, TIMER_A_UP_MODE);
}

void Drive(uint16_t td) {
    setpoint = 65000;
    GPIO_setOutputHighOnPin(GPIO_PORT_P3, GPIO_PIN7 | GPIO_PIN6);
    enc_total_L = 0;
    enc_total_R = 0;
    wheel = (1.0/360) * enc_total_L * (2 * M_PI * rad);
    while (wheel < td) {
        wheel = (1.0/360) * enc_total_L * (2 * M_PI * rad);
    }
    GPIO_setOutputLowOnPin(GPIO_PORT_P3, GPIO_PIN7 | GPIO_PIN6);
}

void Turn(uint16_t td, int8_t l_or_r) {
    setpoint = 100000;
    GPIO_setOutputHighOnPin(GPIO_PORT_P3, GPIO_PIN7 | GPIO_PIN6);
    enc_total_L = 0;
    enc_total_R = 0;
    wheel = (1.0/360) * enc_total_L * (2 * M_PI * rad);
    if (l_or_r == 0) {   // Left
        GPIO_setOutputLowOnPin(GPIO_PORT_P5, GPIO_PIN5);
        GPIO_setOutputHighOnPin(GPIO_PORT_P5, GPIO_PIN4);
        while (wheel < td) {
            wheel = (1.0/360) * enc_total_L * (2 * M_PI * rad);
        }
    } else if (l_or_r == 1) {    // Right
        GPIO_setOutputLowOnPin(GPIO_PORT_P5, GPIO_PIN4);
        GPIO_setOutputHighOnPin(GPIO_PORT_P5, GPIO_PIN5);
        while (wheel < td) {
            wheel = (1.0/360) * enc_total_R * (2 * M_PI * rad);
        }
    }
    GPIO_setOutputLowOnPin(GPIO_PORT_P5, GPIO_PIN5 | GPIO_PIN4);
    GPIO_setOutputLowOnPin(GPIO_PORT_P3, GPIO_PIN7 | GPIO_PIN6);
}

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
                CCR3_compareValue++;
            }
            if (wheel_speed_sum_L / 6 < setpoint) {
                CCR3_compareValue--;
            }
            Timer_A_setCompareValue(TIMER_A0_BASE,
                                    TIMER_A_CAPTURECOMPARE_REGISTER_3,
                                    CCR3_compareValue);
            wheel_speed_sum_L = 0;
            wheel_speed_counts_L = 0;
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
                CCR4_compareValue++;
            }
            if (wheel_speed_sum_R / 6 < setpoint) {
                CCR4_compareValue--;
            }
            Timer_A_setCompareValue(TIMER_A0_BASE,
                                    TIMER_A_CAPTURECOMPARE_REGISTER_4,
                                    CCR4_compareValue);
            wheel_speed_sum_R = 0;
            wheel_speed_counts_R = 0;
        }
    }
}
