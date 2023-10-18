//** ENGR-2350 Activity 6.4.2 and 6.4.3
//** Name: Ryan So
//** RIN: 662042337

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

typedef struct homework_t {
    float average;      // The struct has a "field" for each value listed
    float stdev;        // in the "Homework Gradebook" table.
    uint8_t min;        // Note that the fields can be all different types
    uint8_t max;
    uint16_t subs;
}homework_t;

float remove_min_from_avg(homework_t *homework);

//homework 1
homework_t hw1;

//array of homeworks
homework_t hws[5];

int main(){
    //hw1
    hw1.average = 94.2;
    hw1.stdev = 10.8;
    hw1.min = 55;
    hw1.max = 100;
    hw1.subs = 109;
    
    printf("Homework 1 Stats\r\n"
       "    Average: %.2f\r\n"
       "  Std. Dev.: %.2f\r\n"
       "    Minimum: %u\r\n"
       "    Maximum: %u\r\n"
       "Submissions: %u\r\n",
       hw1.average,hw1.stdev,hw1.min,
       hw1.max,hw1.subs);
    printf("New average of hw1: %f\r\n\n", remove_min_from_avg(&hw1));
    
    //hw1
    hws[0].average = 94.2;
    hws[0].stdev = 10.8;
    hws[0].min = 55;
    hws[0].max = 100;
    hws[0].subs = 109;
    //hw2
    hws[1].average = 76.7;
    hws[1].stdev = 12.6;
    hws[1].min = 40;
    hws[1].max = 100;
    hws[1].subs = 106;
    //hw3
    hws[2].average = 84.5;
    hws[2].stdev = 15.9;
    hws[2].min = 25;
    hws[2].max = 100;
    hws[2].subs = 101;
    //hw4
    hws[3].average = 92.1;
    hws[3].stdev = 12.6;
    hws[3].min = 45;
    hws[3].max = 100;
    hws[3].subs = 99;
    //hw5
    hws[4].average = 66.3;
    hws[4].stdev = 27.3;
    hws[4].min = 10;
    hws[4].max = 100;
    hws[4].subs = 99;
    
    //printing homeworks
    for(uint8_t i = 0; i < 5; i++){
        printf("Homework %u Stats\r\n"
       "    Average: %.2f\r\n"
       "  Std. Dev.: %.2f\r\n"
       "    Minimum: %u\r\n"
       "    Maximum: %u\r\n"
       "Submissions: %u\r\n",i + 1,
       hws[i].average,hws[i].stdev,hws[i].min,
       hws[i].max,hws[i].subs);
       printf("New average of hw%u: %f\r\n\n",i, remove_min_from_avg(&hws[i]));
    }
    return 0;
}

float remove_min_from_avg(homework_t *homework){
    float homework_sum = homework->average*homework->subs;
    homework_sum -= homework->min;
    return homework_sum/(homework->subs-1);
}

