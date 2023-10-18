/**********************************************************************/
//** ENGR-2350 F23 Lab 2
//** Names: Ryan, Jack
//** Section: 02
//** Side: A/B
//** Seat#: 15
/**********************************************************************/

#include "engr2350_msp432.h"
#include <stdlib.h>

// Add function prototypes here, as needed.
void GPIO_Init();
void Timer_Init();
int8_t readBumpers();
void setRGB(int8_t color);
uint8_t checkGuess(int8_t *sol,int8_t *gss,int8_t *res);
void printResult(int8_t *gss,int8_t *res);
void Timer_ISR();

void create_sequence();
void reset_user_sequence();

void half_second_delay(int8_t color);

void Update_Timer();
void Reset_Timer();
// Add global variables here, as needed.
Timer_A_UpModeConfig timer;
uint8_t time[4];
int8_t pb1;
uint16_t counter1 = 0;
uint8_t counter2 = 0;
uint8_t flag = 0;

uint8_t game_started = 0;
uint8_t number_of_games = 0;
uint8_t number_of_guesses = 0;

int8_t index = 0;

int8_t gen_sequence[4];
int8_t user_sequence[4];
int8_t result[4];


int main (void){
    SysInit();
    Timer_Init();
    GPIO_Init();

    // Place initialization code (or run-once) code here
    printf("\r\nWelcome to Colordle!\r\nPress Push Button to Start\r\n");

    while(1){
        // Place code that runs continuously in here
        pb1=GPIO_getInputPinValue(GPIO_PORT_P5, GPIO_PIN6);

        //if game hasn't started yet
        if(!game_started){
            //start the game if pb1 is pressed
            if (pb1){
                printf("New Game:\r\n");
                game_started = 1;
                number_of_games++;
                create_sequence();
                reset_user_sequence();
            }
            // if this is the first game, bumper buttons can be used
            else {
                if (number_of_games == 0) setRGB(readBumpers());
            }
        } 
        //if the game has started
        else {
            //start a personal timer
            Update_Timer();
            //check if the push button is pressed
            if(pb1){
                half_second_delay(6);
                reset_user_sequence();
            } 
            //read user input
            else {
                if(index < 4){
                    int8_t bumper_read = readBumpers();
                    if(bumper_read != -1){
                        half_second_delay(bumper_read);
                        user_sequence[index] = bumper_read;
                        index++;
                    }
                } 
                //check the guess
                else {
                    index = 0;
                    counter1 = 0;
                    int8_t i = 0;
                    uint8_t Ncorrect = checkGuess(gen_sequence, user_sequence, result);
                    while(i < 4){
                        half_second_delay(result[i]);
                        half_second_delay(-1);
                        i++;
                    }
                    printResult(user_sequence, result);
                    if (Ncorrect == 4){
                        game_started = 0;
                        number_of_guesses = 0;
                        printf("WIN! Total Timer: %2u:%02u:%02u.%u\r\n",time[3],time[2],time[1],time[0]);
                        Reset_Timer();
                        while(!pb1){
                            half_second_delay(1);
                        }
                    }
                    else {
                        number_of_guesses++;                        
                        if(number_of_guesses >=5){
                            printf("Too many guesses. Try again\r\n");
                            game_started = 0;
                            number_of_guesses = 0;
                        }
                    }
                }
            }
        }
    }
}

void GPIO_Init(){
    // Complete for Part B
    //Set input for push button
    GPIO_setAsInputPin(GPIO_PORT_P5, GPIO_PIN6);

    //Set inputs for all the bumpers.
    GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P4, GPIO_PIN0 | GPIO_PIN2 | GPIO_PIN3 | GPIO_PIN5 | GPIO_PIN6 | GPIO_PIN7);

    //Set the outputs of LP_RGB_RLED LP_RGB_GLED and LP_RGB_BLED
    GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_PIN0 | GPIO_PIN1 | GPIO_PIN2);
}

void Timer_Init(){
    // Complete for Part B. Also add interrupt function
    timer.clockSource = TIMER_A_CLOCKSOURCE_SMCLK;
    timer.clockSourceDivider = TIMER_A_CLOCKSOURCE_DIVIDER_64;
    timer.timerPeriod = 37500;
    timer.timerInterruptEnable_TAIE = TIMER_A_TAIE_INTERRUPT_ENABLE;

    Timer_A_registerInterrupt(TIMER_A1_BASE,TIMER_A_CCRX_AND_OVERFLOW_INTERRUPT,Timer_ISR);
    Timer_A_configureUpMode(TIMER_A1_BASE, &timer);
    Timer_A_startCounter(TIMER_A1_BASE, TIMER_A_UP_MODE);
}

int8_t readBumpers(){
    // Complete for Part B
        // Check the state of each bumper
        __delay_cycles(240e3);
        if (GPIO_getInputPinValue(GPIO_PORT_P4, GPIO_PIN0)== 0x00) {
            while(GPIO_getInputPinValue(GPIO_PORT_P4, GPIO_PIN0)== 0x00){}
            // Set the corresponding color
            __delay_cycles(240e3);
            return 0;
        } else if (GPIO_getInputPinValue(GPIO_PORT_P4, GPIO_PIN2)== 0x00) {
            while(GPIO_getInputPinValue(GPIO_PORT_P4, GPIO_PIN2)== 0x00){}
            __delay_cycles(240e3);
            return(1);
        } else if (GPIO_getInputPinValue(GPIO_PORT_P4, GPIO_PIN3)== 0x00) {
            while(GPIO_getInputPinValue(GPIO_PORT_P4, GPIO_PIN3)== 0x00){}
            __delay_cycles(240e3);
            return(2);
        } else if (GPIO_getInputPinValue(GPIO_PORT_P4, GPIO_PIN5)== 0x00) {
            while(GPIO_getInputPinValue(GPIO_PORT_P4, GPIO_PIN5)== 0x00){}
            __delay_cycles(240e3);
            return(3);
        } else if (GPIO_getInputPinValue(GPIO_PORT_P4, GPIO_PIN6)== 0x00) {
            while(GPIO_getInputPinValue(GPIO_PORT_P4, GPIO_PIN6)== 0x00){}
            __delay_cycles(240e3);
            return(4);
        } else if (GPIO_getInputPinValue(GPIO_PORT_P4, GPIO_PIN7)== 0x00) {
            while(GPIO_getInputPinValue(GPIO_PORT_P4, GPIO_PIN7)== 0x00){}
            __delay_cycles(240e3);
            return(5);

    }
    return(-1);
}

void setRGB(int8_t color){
    // Complete for Part B
    // Set all color channels to LOW (turn off all colors)
   if(color ==-1){
   GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN0 | GPIO_PIN1 | GPIO_PIN2);
   }
   // Use if statements to set the requested color
   if (color == 0) {
       // Turn on the Red channel, leave others off
       GPIO_setOutputHighOnPin(GPIO_PORT_P2, GPIO_PIN0);
       GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN1);
       GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN2);
   }
   else if (color == 1) {
       // Turn on the Green channel, leave others off
       GPIO_setOutputHighOnPin(GPIO_PORT_P2, GPIO_PIN1);
       GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN0);
       GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN2);
   }
   else if (color == 2) {
       // Turn on the Blue channel, leave others off
       GPIO_setOutputHighOnPin(GPIO_PORT_P2, GPIO_PIN2);
       GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN0);
       GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN1);
   }
   else if (color == 3) {
       // Turn on both Red and Green channels (mixes to create Yellow), leave Blue off
       GPIO_setOutputHighOnPin(GPIO_PORT_P2, GPIO_PIN0);
       GPIO_setOutputHighOnPin(GPIO_PORT_P2, GPIO_PIN1);
       GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN2);
   }
   else if (color == 4) {
       // Turn on both Red and Blue channels (mixes to create Magenta), leave Green off
       GPIO_setOutputHighOnPin(GPIO_PORT_P2, GPIO_PIN0);
       GPIO_setOutputHighOnPin(GPIO_PORT_P2, GPIO_PIN2);
       GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN1);
   }
   else if (color == 5) {
       // Turn on both Green and Blue channels (mixes to create Cyan), leave Red off
       GPIO_setOutputHighOnPin(GPIO_PORT_P2, GPIO_PIN1);
       GPIO_setOutputHighOnPin(GPIO_PORT_P2, GPIO_PIN2);
       GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN0);
   }
   else if (color == 6){
        // Turn on all channels (mixes to white)
        GPIO_setOutputHighOnPin(GPIO_PORT_P2, GPIO_PIN1);
        GPIO_setOutputHighOnPin(GPIO_PORT_P2, GPIO_PIN2);
        GPIO_setOutputHighOnPin(GPIO_PORT_P2, GPIO_PIN0);
   }
}


/**
 * checkGuess is used to check the player's guess against the solution
 * and produce the associated correct positions, incorrect positions, and
 * incorrect colors.
 *
 *  !!! WARNING !!! All of these inputs are expected to be pointers. Arrays are
 *                  technically pointers already! They should not have an & in
 *                  front of them when passed into the function.
 * Input Parameters:
 *      int8_t * sol: A 4-element array that stores the game solution (input)
 *      int8_t * gss: A 4-element array that stores the player's guess (input)
 *      int8_t * res: A 4-element array that stores the guess correctness result
 *                    This array is really an output of this function. It is
 *                    modified within the function, with the changes persistent
 *                    after the function is complete.
 *                    This array will only have the values of:
 *                      0: Red - Incorrect color
 *                      1: Green - Correct color and position
 *                      3: Yellow - Correct color, incorrect position
 * Outputs:
 *      uint8_t - the number of correct positions. This may be used to determine
 *                is the guess was correct.
 */
uint8_t checkGuess(int8_t *sol,int8_t *gss,int8_t *res){
    uint8_t PAT_LEN = 4; // The pattern length
    uint8_t _i,_j; // Loop variables. underscores added to avoid conflict with possible globals.
    uint8_t matched[4]; // Array to store if a color in the answer has been matched yet or not
    for(_i=0;_i<PAT_LEN;_i++){ // set default values of arrays
        res[_i] = 0; // Answer is incorrect (RED)
        matched[_i] = 0; // Guess position is not used yet
    }
    uint8_t Ncorrect = 0; // Number of positions correct.
    // Fist loop through and find corrects
    for(_i=0;_i<PAT_LEN;_i++){
        if(sol[_i] == gss[_i]){ // If the guess and answer match...
            Ncorrect++; // Increment number of correct guesses
            res[_i] = 1; // 1 for green
            matched[_i] = 1; // 1 for used (can't compare this position again)
        }
    }
    // Now check for correct color, incorrect position
    for(_i=0;_i<PAT_LEN;_i++){ // Loop through guess positions
        if(res[_i] == 1) continue; // If this position is marked correct, skip it
        for(_j=0;_j<PAT_LEN;_j++){ // Loop through answer positions, looking for the same color
         // if(i==j) continue; // If checking the same position, skip. This isn't necessary as it would correspond
                               // the correct case and would be skipped by the "checked" array anyway
            if(matched[_j]) continue; // If this answer color is already taken by a correct or close, skip it
            if(gss[_i] == sol[_j]){ // If the colors are the same (correct color, incorrect position)
                res[_i] = 3; // 3 for yellow
                matched[_j] = 1; // 1 for used (can't compare this position again)
            }
        }
    }
    return Ncorrect; // return number of correct positions
}

/*
 * printResult will take the players guess and the checked result and print them
 * in the necessary format on the terminal. The colors in the player's guess will be
 * printed first, using the first letter of each color. Afterwards the result of
 * the guess is printed using the characters:
 *              $ - correct color and position (Green result)
 *              O - correct color, incorrect position (Yellow result)
 *              X - incorrect color (Red result)
 *
 *  !!! WARNING !!! Both of these inputs are expected to be pointers. Arrays are
 *                  technically pointers already! They should not have an & in
 *                  front of them when passed into the function.
 * Input Parameters:
 *      int8_t * gss: A 4-element array that stores the player's guess (input)
 *      int8_t * res: A 4-element array that stores the guess correctness result
 */
void printResult(int8_t *gss,int8_t *res){
    uint8_t PAT_LEN=4;
    uint8_t i = 0; // loop variable
    for(i=0;i<PAT_LEN;i++){
        switch(gss[i]){
        case -1: putchar('-'); break;
        case 0: putchar('R'); break;
        case 1: putchar('G'); break;
        case 2: putchar('B'); break;
        case 3: putchar('Y'); break;
        case 4: putchar('M'); break;
        case 5: putchar('C'); break;
        }
    }
    putchar(' '); // put a space in
    for(i=0;i<PAT_LEN;i++){
        switch(res[i]){
        case 0: putchar('X'); break;
        case 3: putchar('O'); break;
        case 1: putchar('$'); break;
        }
    }
    putchar('\r');putchar('\n');
}

void Update_Timer(){
    time[0]++;  // Increment hundredths of seconds
    if(time[0] == 100){  // If a whole second has passed...
        time[0] = 0;    // Reset tenths of seconds
        time[1]++;      // And increment seconds
        if(time[1] == 60){ // If a minute has passed...
            time[1] = 0;   // Reset seconds
            time[2]++;     // Increment minutes
            if(time[2] == 60){  // and so on...
                time[2] = 0;
                time[3]++;
                if(time[3] == 24){
                    time[3] = 0;
                }
            }
        }
    }
}

void Reset_Timer(){
    time[0] = 0;
    time[1] = 0;
    time[2] = 0;
    time[3] = 0;
}

void half_second_delay(int8_t color){
    setRGB(color);
    flag = 1;
    while(flag == 1);
    setRGB(-1);
}

void create_sequence(){
    int8_t i = 0;
    while(i < 4){
        gen_sequence[i] = rand()%6;
        i++;
    }
    while(counter1 < 20) setRGB(6);
    setRGB(-1);
}

void reset_user_sequence(){
    int8_t i = 0;
    while(i < 4){
        user_sequence[i] = -1;
        i++;
    }
    index = 0;
}

// Add interrupt functions last so they are easy to find

void Timer_ISR(){
    Timer_A_clearInterruptFlag(TIMER_A1_BASE);
    //if game hasn't started don't do anything
    if(!game_started) return;

    // count for 30s
    if (counter1 < 300 && !flag){
        counter1++;
    } else if (counter1 >= 300 && !flag) {
        printf("Timeout!\r\n");
        reset_user_sequence();
        index = 4;
        number_of_guesses++;
        counter1 = 0;
    }

    //for clearing sequence with push button
    if(flag){
        counter2++;
    }
    if (counter2 >= 5){
        flag = 0;
        counter2 = 0;
    }
}

