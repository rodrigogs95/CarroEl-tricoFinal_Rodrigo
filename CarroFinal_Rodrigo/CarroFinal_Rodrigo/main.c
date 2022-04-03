/*
 * CarroFinal_Rodrigo.c
 *
 * Author : Rodrigo Guimarães da Silva
 * Matrícula: 119210855
 */ 


#define F_CPU 16000000UL
#define  BAUD 9600
#define MYUBRR  F_CPU/16/BAUD-1
#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/delay.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <avr/eeprom.h>
#include <util/delay.h>
#include "SSD1306.h"
#include "Font5x8.h"

uint32_t farol = 0, dimenroda = 50, distan = 0, RPM = 0, Time_ = 0, VeloMed = 0, auxiliar = 0, ajudar_distan =0, reconhecendo_ADC=0, Pwm_Signal=0;
uint32_t T_BordaSubida, T_Delta, battery_PWM=0, battery=0, Temperatura_PWM=0, Temperatura=0,aux_temperatura=0, c=0,auxiliar_ler=0;
char matriz_eeprom[2];

ISR(INT0_vect)
{
	dimenroda++;
	eeprom_write_byte(0,dimenroda);
}

ISR(INT1_vect)
{
	dimenroda--;
 
	if ( dimenroda < 45){
  
		dimenroda = 45;
  
	}
	eeprom_write_byte(0,dimenroda);
}
 
 ISR(TIMER0_COMPA_vect)
 {
	  static float auxiliarNov;
	  Time_++;
	  if((Time_ % 1000) == 0)
	  {
		   auxiliarNov = (auxiliar/2);
		   RPM = (auxiliarNov*60);
		   auxiliar = 0;
   
		   ajudar_distan++;
	  }
 }
 
 ISR(PCINT2_vect)
 {
	auxiliar++;
 }
 

 
  ISR(ADC_vect)
  {
	  
	if(c==0){
		reconhecendo_ADC = ADC;
		if (ADC==1023)
		{
			Pwm_Signal = 255;
		}
		else
		{
			Pwm_Signal = (ADC/4);
		}
		  
		if(T_Delta <= 299 && VeloMed >21){
			  
			Pwm_Signal= 90;
		}
	}
	if(c==1){
		 
		if (ADC==1023)
		{
			battery_PWM = 255;
			battery=100;
		}
		else
		{
			battery_PWM= (ADC/4);
			battery=(battery_PWM*0.392);
		}
		 
	}

	if(c==2){
		if (ADC==1023)
		{
			Temperatura_PWM = 255;
			 
			Temperatura=200;
		}
		else
		{
			Temperatura_PWM= (ADC/4);
			 
			Temperatura= ((Temperatura_PWM-105)/0.37);
		}
		 
	}
 }
 ISR(TIMER1_CAPT_vect){
	 
	 if(TCCR1B &(1<<ICES1))
	 
	 T_BordaSubida=ICR1;
	 
	 
	 else
	 
	 T_Delta=((ICR1-T_BordaSubida)*16)/58;
	 
	 TCCR1B^=(1<<ICES1);
 }
  void USART_Init(unsigned int ubrr){
	  
	  UBRR0H = (unsigned char)(ubrr>>8);
	  UBRR0L=(unsigned char)ubrr;
	  UCSR0B =  (1<<RXCIE0) | ( 1<<RXEN0) | (1<<TXEN0);
	  UCSR0C =(1<<USBS0)|(3<<UCSZ00);

  }
  void USART_Transmit(unsigned char data){
	  
	  while(!( UCSR0A & (1<< UDRE0)));
	  UDR0 = data;
  }
  unsigned char USART_Receive (void){
	  
	  while(!(UCSR0A & (1<<RXC0)));
	  return UDR0;
	  
	  
  }
  ISR(USART_RX_vect){
	  
	  char variavel;
	  variavel = UDR0;
	  int aux_eeprom=0;
	  
	  sprintf(matriz_eeprom,"%u",aux_temperatura);
	  if(variavel=='l'){
		  eeprom_write_byte(3,0);
	  }
	  
	  if(variavel=='d'){
		  
		  if (aux_temperatura <= 9)
		  {
			  aux_eeprom = 1;
		  }
		  else if (aux_temperatura <= 100 ){
			 aux_eeprom = 2;
		  }
		  else {
			 aux_eeprom = 3;
		  }
		  
		  for (int i=0;i<aux_eeprom;i++)
		  {
			  USART_Transmit(matriz_eeprom[i]);
		  }
		  
	  }

	  if(variavel=='f') {
		  PORTD |= 0b10000000;  // Ligo o Farol através do monitor serial
	  }
	  if(variavel=='n') {
		  PORTD &= 0b01111111; // Desligo o Farol
	  }
	  
	  if(variavel=='s') {
		  PORTC ^= 0b00001000; // Ligo e Desligo o Som da ambulância (or exclusivo)
	  }
  }
 
 
 
 
 int main(void)
 {
	DDRB =  0b11111110; 
	PORTB = 0b00000001;
	DDRC =  0b11111000; 
	PORTC = 0b11110000;
	DDRD  = 0b10000001;
	PORTD = 0b01101101;
  
	TCCR0A = 0b00000010;
	TCCR0B = 0b00000011;
	OCR0A = 249;
	TIMSK0 = 0b00000010;
	TCCR1B= (1<<ICES1)|(1<<CS12);
	TIMSK1 = 1<<ICIE1;
  
	EICRA = 0b00001010;
	EIMSK = 0b00000011;
  
	PCICR = 0b00000100;
	PCMSK2= 0b00010000;
  
	ADMUX = 0b11000000;
	ADCSRA = 0b11101111;
	ADCSRB = 0x00;
  
	DIDR0 = 0b00111000;
	TCCR0A = 0b10100011;
	TCCR0B = 0b00000011;
	
	USART_Init(MYUBRR);
	
	sei();
  
	GLCD_Setup();
	GLCD_SetFont(Font5x8,5,8, GLCD_Overwrite);
	GLCD_InvertScreen();
  
  
  while (1)
  {
	  
	VeloMed = 2*3.14*(dimenroda/2)/100000*RPM*60;
	distan = ajudar_distan*VeloMed/3600;
	OCR0A = Pwm_Signal;
   
	dimenroda = eeprom_read_byte(0);
   
	if((Time_ % 2) == 0)
	{
		GLCD_Clear();
   
		ADMUX = 0b11000000;
		c=0;
		_delay_ms(80);
   
   
		ADMUX = 0b11000001;
  
		c=1;
		_delay_ms(80);
   
		GLCD_GotoXY(115,5);
		GLCD_PrintString("%");
		GLCD_GotoXY(95,5);
		GLCD_PrintInteger(battery);
   
   
   
		ADMUX = 0b11000010;
		c=2;
		_delay_ms(80);
   
		GLCD_GotoXY(2,50);
		GLCD_PrintString("Temperatura:");
		GLCD_GotoXY(75,50);
		GLCD_PrintInteger(Temperatura);
		GLCD_GotoXY(90,50);
		GLCD_PrintString("C");
   
		aux_temperatura= eeprom_read_byte(3);
   
		if (Temperatura>aux_temperatura)
		{
			eeprom_write_byte(3,Temperatura);
		}
   
		GLCD_GotoXY(2,5);
		GLCD_PrintString("Sonar:");
		GLCD_GotoXY(40,5);
		GLCD_PrintInteger(T_Delta);
   
		GLCD_GotoXY(2,20);
		GLCD_PrintString("Diamentro:");
		GLCD_GotoXY(65,20);
		GLCD_PrintInteger( dimenroda);
   
		GLCD_GotoXY(2,35);
		GLCD_PrintString("RPM:");
		GLCD_GotoXY(32,35);
		GLCD_PrintInteger(RPM);
   
		GLCD_DrawRectangle(90,28,125,42,GLCD_Black);
		GLCD_GotoXY(95,32);
		GLCD_PrintInteger(distan);
		GLCD_GotoXY(110,32);
		GLCD_PrintString("Km");
   
		GLCD_Render();
	   
		PORTC ^= 0b01000000; // Inverto o estado de PC6 a cada 2 segundos
	}
      
	PORTB &= 0b00000001;
	PORTB |= 0b11000000;
	PORTB |=((((VeloMed/1)%10)*2) & 0b00011110);
   
	_delay_ms(4);
   
	PORTB &= 0b00000001;
	PORTB |= 0b10100000;
	PORTB |= ((((VeloMed/10)%10)*2) & 0b00011110);
   
	_delay_ms(4);
   
	PORTB &= 0b00000001;
	PORTB |= 0b01100000;
	PORTB |= ((((VeloMed/100)%10)*2) & 0b00011110);
   
	_delay_ms(4);
	}
 }

