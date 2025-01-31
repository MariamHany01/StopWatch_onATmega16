/*
 * project2.c
 *
 *  Created on: 17 Sep 2023
 *      Author: maria
 */
#include<avr/io.h>
#include<avr/interrupt.h>
#include <util/delay.h>
/*
 * Ftimer=Fcpu/N=1MG/1024=977HZ
 * Ttimer=1/Ftimer=1ms
 * Tcmpare=cmp_val*Ttimer=1000*1ms=1sec ->CMP_val=1000
 */

//variables to control what will be displayed in each 7_segment
unsigned char sec_units=0;
unsigned char sec_tenth=0;
unsigned char min_units=0;
unsigned char min_tenth=0;
unsigned char hr_units =0;
unsigned char hr_tenth=0;

void Timer1_CTC_Init(void){
	TCNT1=24;//start counting from

	OCR1A=1000;//set the compare value

	TIMSK |=(1<<OCIE1A);//enable module interrupt with the CTC mood

	TCCR1B =(1<<WGM12)|(1<<CS12)|(1<<CS10);//set the timer to CTC mood and the N=1024
}
void INT0_Falling(void ){
	DDRD &=~(1<<2);//set pin 2 as input(push button->pull up)
	PORTD |=(1<<2);//enable internal pull up

	MCUCR |=(1<<ISC01);//set INT0 with falling edge

	GICR |=(1<<INT0);//enable INT1
}

void INT1_Rising(void){
	DDRD &=~(1<<3);//set pin 3 as input(push button->pull down)

	 MCUCR |=(1<<ISC11)|(1<<ISC10);//enable INT1 with rising edge

	 GICR |=(1<<INT1);//enable INT1
}
void INT2_Falling(void){
	DDRB &=~(1<<2);//set pin 2 as input(push button->pull up)

	PORTB |=(1<<2);//enable internal pull up

	MCUCSR &=~(1<< ISC2);//set INT2 with falling edge

	GICR |=(1<< INT2);//enable INT2
}
int main (){

	//Initializations
	DDRA |=0x3F;//the first 6 pins in port A are output(enable of the 7_segments)
	PORTA &=0xC0;//all 7_segments are initially off

	DDRC |=0x0F ;//first 4 pins in port C are output(decoder is connected on)
	PORTC &=0XF0;//start from 0;

	SREG |=(1<<7);//enable the I_bit

	Timer1_CTC_Init();//stop watch

	INT0_Falling();//reset the timer

	INT1_Rising();//pause the timer

	INT2_Falling();//resume the timer

	while(1){
		//display the segments

		PORTA &=0xC0;//all 7_segments are  off
		PORTC &=0XF0;//clear the decoder
		PORTC |=sec_units;//insert value
		PORTA |=(1<<5);//first 7_segment is ON

		_delay_ms(5);

		PORTA &=0xC0;//all 7_segments are  off
		PORTC &=0XF0;//clear the decoder
		PORTC |=sec_tenth;//insert the value
		PORTA |=(1<<4);//2nd 7_segment in ON

		_delay_ms(5);

		PORTA &=0xC0;//all 7_segments are  off
		PORTC &=0XF0;//clear the decoder
		PORTC |=min_units;//insert the value
		PORTA |=(1<<3);//3nd 7_segment in ON

		_delay_ms(5);

		PORTA &=0xC0;//all 7_segments are off
		PORTC &=0XF0;//clear the decoder
		PORTC |=min_tenth;//insert the value
		PORTA |=(1<<2);//4th 7_segment in ON

		_delay_ms(5);

		PORTA &=0xC0;//all 7_segments are  off
		PORTC &=0XF0;//clear the decoder
		PORTC |=hr_units;//insert the value
		PORTA |=(1<<1);//5th 7_segment in ON

		_delay_ms(5);

		PORTA &=0xC0;//all 7_segments are off
		PORTC &=0XF0;//clear the decoder
		PORTC |=hr_tenth;//insert the value
		PORTA |=(1<<0);//6th 7_segment in ON

		_delay_ms(5);
	}
}
ISR(TIMER1_COMPA_vect){

	sec_units++;
	if(sec_units==10){
		sec_units=0;
		sec_tenth++;
		if(sec_tenth==6){
				sec_tenth=0;
				sec_units=0;
				min_units++;
				if(min_units==10){
						min_units=0;
						min_tenth++;
						if(min_tenth==6){
							min_tenth=0;
							min_units=0;
							hr_units++;
							if(hr_units==10){
								hr_units=0;
								hr_tenth++;
								if((hr_tenth==2)&&(hr_units==4)){
									sec_units=0;
									sec_tenth=0;
									min_units=0;
									min_tenth=0;
									hr_units=0;
									hr_tenth=0;
								}
							}
						}
			        }
	            }
         	}
}
ISR(INT0_vect){
	 sec_units=0;
	 sec_tenth=0;
	 min_units=0;
	 min_tenth=0;
	 hr_units =0;
	 hr_tenth=0;
}
ISR(INT1_vect){
	TCCR1B=0;//stop the timer
}

ISR (INT2_vect){
	TCCR1B =(1<<WGM12)|(1<<CS12)|(1<<CS10);//start again the timer
}


