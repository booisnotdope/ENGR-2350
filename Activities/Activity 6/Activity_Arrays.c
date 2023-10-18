//** ENGR-2350 Activity 6.4.1
//** Name: Ryan So
//** RIN: 662042337

#include <stdio.h>
#include <stdint.h>

uint32_t array1[] = {1,2,3,4,5,6,7,8,9,10};
uint32_t array2[10];

int main(){
    uint8_t i,j;
    for(i = 0; i < 10; i++){
        array2[i] = rand()%20;
        printf("Element %u is in array1 and array2: \t%u and %u\r\n", i, array1[i], array2[i]);
    }
    printf("\r\n");
    printf("\t");
    for(i = 1; i <= 10; i++){
        printf("%u\t",i);
    }
    printf("\r\n");
    for(i=1;i<=10;i++){
        printf("%u\t",i);
        for(j=1;j<=10;j++){
            printf("%u\t", i*j);
        }
        printf("\r\n");
    }
    return 0;
}
