// ENGR-2350 Template Project
#include "engr2350_msp432.h"

// Add function prototypes here, as needed.
void Timer_Init();
void GPIO_Init();
void Encoder_ISR();

// Add global variables here, as needed.
Timer_A_UpModeConfig timer;
Timer_A_ContinuousModeConfig timer2;
Timer_A_CompareModeConfig timer_left;
Timer_A_CompareModeConfig timer_right;
Timer_A_CaptureModeConfig timer_capturel;
Timer_A_CaptureModeConfig timer_capturer;
uint32_t enc_counts_left;
uint32_t enc_counts_track_left;
uint32_t enc_total_left;
//uint8_t enc_flag_left;
uint32_t enc_counts_right;
uint32_t enc_counts_track_right;
uint32_t enc_total_right;
//uint8_t enc_flag_right;
uint8_t BMP0;
uint16_t enc_event_count;
uint32_t array[250];


int main (void) /** Main Function **/
{
    // Add local variables here, as needed.

    // We always call the "SysInit()" first to set up the microcontroller
    // for how we are going to use it.
    SysInit();
    Timer_Init();
    GPIO_Init();

    // Place initialization code (or run-once) code here
    GPIO_setOutputLowOnPin(GPIO_PORT_P3,GPIO_PIN6|GPIO_PIN7);

    while(1)
    // Place code that runs continuously in here
    {
    if (GPIO_getInputPinValue(GPIO_PORT_P4,GPIO_PIN0) == 0){
            printf("enter");
            __delay_cycles(24e6);
            enc_event_count = 0;
            GPIO_setOutputHighOnPin(GPIO_PORT_P3,GPIO_PIN6|GPIO_PIN7);
            while (enc_event_count <= 250){
                printf("%u\r\n", enc_event_count);
            }
            GPIO_setOutputLowOnPin(GPIO_PORT_P3,GPIO_PIN6|GPIO_PIN7);
            __delay_cycles(12e6);
            uint16_t i;
            printf("Sample \t Value \r\n");
            for (i = 0; i <= 250; i++){
                //printf("%u \t %u \r\n", i, array[i]);
                printf("%u\r\n",array[i]);
            }
        }
    }
}
// Add function declarations here as needed

void Timer_Init(){
    //Timer
    timer.clockSource = TIMER_A_CLOCKSOURCE_SMCLK;
    timer.clockSourceDivider = TIMER_A_CLOCKSOURCE_DIVIDER_1;
    timer.timerPeriod = 800;
    timer.timerClear = TIMER_A_DO_CLEAR;
    timer.timerInterruptEnable_TAIE = TIMER_A_TAIE_INTERRUPT_ENABLE;
    Timer_A_configureUpMode(TIMER_A0_BASE,&timer);
    Timer_A_configureContinuousMode(TIMER_A3_BASE,&timer2);

    //Right Wheel
    timer_right.compareRegister = TIMER_A_CAPTURECOMPARE_REGISTER_4;
    timer_right.compareInterruptEnable = TIMER_A_CAPTURECOMPARE_INTERRUPT_ENABLE;
    timer_right.compareOutputMode = TIMER_A_OUTPUTMODE_RESET_SET;
    timer_right.compareValue = 80;
    Timer_A_initCompare(TIMER_A0_BASE,&timer_right);

    //Left Wheel
    timer_left.compareRegister = TIMER_A_CAPTURECOMPARE_REGISTER_3;
    timer_left.compareInterruptEnable = TIMER_A_CAPTURECOMPARE_INTERRUPT_ENABLE;
    timer_left.compareOutputMode = TIMER_A_OUTPUTMODE_RESET_SET;
    timer_left.compareValue = 80;
    Timer_A_initCompare(TIMER_A0_BASE,&timer_left);

    //Second Timer
    timer2.clockSource = TIMER_A_CLOCKSOURCE_SMCLK;
    timer2.clockSourceDivider = TIMER_A_CLOCKSOURCE_DIVIDER_1;
    timer2.timerInterruptEnable_TAIE = TIMER_A_TAIE_INTERRUPT_ENABLE;
    Timer_A_configureContinuousMode(TIMER_A3_BASE,&timer2);

    //Right Capture
    timer_capturer.captureRegister = TIMER_A_CAPTURECOMPARE_REGISTER_1;
    timer_capturer.captureMode = TIMER_A_CAPTUREMODE_RISING_EDGE;
    timer_capturer.captureInputSelect = TIMER_A_CAPTURE_INPUTSELECT_CCIxA;
    timer_capturer.synchronizeCaptureSource = TIMER_A_CAPTURE_SYNCHRONOUS;
    timer_capturer.captureInterruptEnable = TIMER_A_CAPTURECOMPARE_INTERRUPT_ENABLE;
    Timer_A_initCapture(TIMER_A3_BASE,&timer_capturer);

    //Left Capture
    timer_capturel.captureRegister = TIMER_A_CAPTURECOMPARE_REGISTER_0;
    timer_capturel.captureMode = TIMER_A_CAPTUREMODE_RISING_EDGE;
    timer_capturel.captureInputSelect = TIMER_A_CAPTURE_INPUTSELECT_CCIxA;
    timer_capturel.synchronizeCaptureSource = TIMER_A_CAPTURE_SYNCHRONOUS;
    timer_capturel.captureInterruptEnable = TIMER_A_CAPTURECOMPARE_INTERRUPT_ENABLE;
    Timer_A_initCapture(TIMER_A3_BASE,&timer_capturel);

    //Register Interrupts
    Timer_A_registerInterrupt(TIMER_A3_BASE,TIMER_A_CCR0_INTERRUPT,Encoder_ISR);
    Timer_A_registerInterrupt(TIMER_A3_BASE,TIMER_A_CCRX_AND_OVERFLOW_INTERRUPT,Encoder_ISR);

    //Start Timers
    Timer_A_startCounter(TIMER_A0_BASE,TIMER_A_UP_MODE);
    Timer_A_startCounter(TIMER_A3_BASE,TIMER_A_CONTINUOUS_MODE);
}


void GPIO_Init(){
    GPIO_setAsOutputPin(GPIO_PORT_P3,GPIO_PIN7|GPIO_PIN6); // Enables Motors
    GPIO_setOutputLowOnPin(GPIO_PORT_P3,GPIO_PIN7|GPIO_PIN6); // enabled initially
    GPIO_setAsOutputPin(GPIO_PORT_P5,GPIO_PIN4|GPIO_PIN5); // Directions
    GPIO_setOutputHighOnPin(GPIO_PORT_P5,GPIO_PIN4|GPIO_PIN5); // forward initially

    GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P2,GPIO_PIN7, GPIO_PRIMARY_MODULE_FUNCTION); // Left PWM signal
    GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P2,GPIO_PIN6, GPIO_PRIMARY_MODULE_FUNCTION); // Right PWM signal

    GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P4,GPIO_PIN0); // BMP 0

    GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P10,GPIO_PIN4|GPIO_PIN5, GPIO_PRIMARY_MODULE_FUNCTION); // Wheel Encoder

}

// Add interrupt functions last so they are easy to find

void Encoder_ISR(){
    //enc_event_count++;
    // left
    if (Timer_A_getEnabledInterruptStatus(TIMER_A3_BASE) == TIMER_A_INTERRUPT_PENDING)
    {
        Timer_A_clearInterruptFlag(TIMER_A3_BASE);
        enc_counts_track_left += 65536;
        enc_counts_track_right += 65536;

    }
    else if (Timer_A_getCaptureCompareEnabledInterruptStatus(TIMER_A3_BASE,TIMER_A_CAPTURECOMPARE_REGISTER_0) & TIMER_A_CAPTURECOMPARE_INTERRUPT_FLAG)
    {
        Timer_A_clearCaptureCompareInterrupt(TIMER_A3_BASE,TIMER_A_CAPTURECOMPARE_REGISTER_0);
        enc_counts_left = enc_counts_track_left + Timer_A_getCaptureCompareCount(TIMER_A3_BASE,TIMER_A_CAPTURECOMPARE_REGISTER_0);
        enc_counts_track_left = -Timer_A_getCaptureCompareCount(TIMER_A3_BASE,TIMER_A_CAPTURECOMPARE_REGISTER_0);
        if (enc_event_count <= 250)
        {
            array[enc_event_count] = enc_counts_left;
        }
        enc_event_count++;
    }
    else if (Timer_A_getCaptureCompareEnabledInterruptStatus(TIMER_A3_BASE,TIMER_A_CAPTURECOMPARE_REGISTER_1) & TIMER_A_CAPTURECOMPARE_INTERRUPT_FLAG)
    {
        Timer_A_clearCaptureCompareInterrupt(TIMER_A3_BASE,TIMER_A_CAPTURECOMPARE_REGISTER_1);
        enc_counts_right = enc_counts_track_right + Timer_A_getCaptureCompareCount(TIMER_A3_BASE,TIMER_A_CAPTURECOMPARE_REGISTER_1);
        enc_counts_track_right = -Timer_A_getCaptureCompareCount(TIMER_A3_BASE,TIMER_A_CAPTURECOMPARE_REGISTER_1);
//        if (enc_event_count < 250)
//        {
//            array[enc_event_count] = enc_counts_track_right;
//        }
//        enc_event_count++;
    }
}