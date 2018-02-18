#include <err.h>
#include <inttypes.h>
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <wiringPi.h>

int main(void){
	if(wiringPiSetupGpio ()  ==  - 1 )  { 
	    printf("Error"); 
	    exit(1);
	  }
	pinMode ( 18 ,  PWM_OUTPUT ); 
	pwmSetMode ( PWM_MODE_MS ); 
	pwmSetClock ( 400 ); 
	pwmSetRange ( 1024 );


	pwmWrite ( 18 ,  115 );
	delay(10000);
	pwmWrite ( 18 ,  25 );
	delay(1000);
}