/******************************************************************************

                            Online C Compiler.
                Code, Compile, Run and Debug C program online.
Write your code in this editor and press "Run" button to compile and execute it.

*******************************************************************************/

#include <stdio.h>
#include <stdint.h>

// Activity: Number Systems
// ENGR-2350
// Name: Ryan So
// RIN: 662042337

int main()
{
    uint8_t a;
    int8_t b;
    uint32_t c;
    printf("\r\n\n\n\n\n");
    printf("****************************\r\n");
    printf("**Activity: Number Systems**\r\n");
    printf("****************************\r\n");
    printf("\n\n");
    
    a = 0x00;
    b = 0x00;
    c = 0x00;
    printf("Printing 0x00:\na is %u, b is %d, and c is %u.\n\n",a, b, c);
    
    a = 0b00000000;
    b = 0b00000000;
    c = 0b00000000;
    printf("Printing 0b00000000:\na is %u, b is %d, and c is %u.\n\n",a, b, c);
    
    a = 0x10;
    b = 0x10;
    c = 0x10;
    printf("Printing 0x10:\na is %u, b is %d, and c is %u.\n\n",a, b, c);
    
    a = 0x0C;
    b = 0x0C;
    c = 0x0C;
    printf("Printing 0x0C:\na is %u, b is %d, and c is %u.\n\n",a, b, c);
    
    a = 0x70;
    b = 0x70;
    c = 0x70;
    printf("Printing 0x70:\na is %u, b is %d, and c is %u.\n\n",a, b, c);
    
    a = 0xFF;
    b = 0xFF;
    c = 0xFF;
    printf("Printing 0xFF:\na is %u, b is %d, and c is %u.\n\n",a, b, c);
    
    a = 0b11111111;
    b = 0b11111111;
    c = 0b11111111;
    printf("Printing 0b11111111:\na is %u, b is %d, and c is %u.\n\n",a, b, c);
    
    a = 0x0100;
    b = 0x0100;
    c = 0x0100;
    printf("Printing 0x0100:\na is %u, b is %d, and c is %u.\n\n",a, b, c);
    
    a = 0xFFFF;
    b = 0xFFFF;
    c = 0xFFFF;
    printf("Printing 0xFFFF:\na is %u, b is %d, and c is %u.\n\n",a, b, c);
    
    c = 662042337;
    printf("%u\n", c);
    printf("Ryan So");
    printf("\n\n--End of Program--\n\n");
    
    return 0;
}
