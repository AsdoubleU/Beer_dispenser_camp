/*��� ���� ����*/
#include <avr/io.h>
#include <avr/interrupt.h>
#define F_CPU 16000000UL
#include <string.h>
#include <util/delay.h>
#include <stdlib.h> // ���� ���� rand() ����ϱ� ����
#include <stdio.h>
#include "LCD.h"

//#include <SoftwareSerial.h>
//#include <Adafruit_NeoPixel.h>

#define ADC_AREF_CH0 0
#define ADC_PRESCALE_128 (1<<ADPS0)|(1<<ADPS1)|(1<<ADPS2) // 16 MHz/128 = 16 kHz sampling rate
#define ADC_ENABLE (1<<ADEN)
#define ADC_INTERRUPT_ENABLE (1<<ADIE)
#define ADC_FREE_RUN (1<<ADFR)
#define ADC_START (1<<ADSC)

/*��ũ�� ���� ���� ����*/

#define BAUD_38400 25
#define UART0_RX_INT_ENABLE (UCSR0B|=(1<<RXCIE0))

/*��ư�� ���� ISR(INT0), Ÿ�̸ӿ� ���� ISR(TIMER0)*/
#define Use_IE0_Interrupt (EIMSK |=(1<<INT0))
#define IE0_Detect_Falling (EICRA = (1<<ISC01))
#define IE0_Detect_Rising (EICRA = (1<<ISC01)|(1<<ISC00))

#define TIMER0_ENABLE TIMSK |= (1<<TOIE0)
#define TIMER0_PRESCALE TCCR0 |= (1<<CS02)

#define fridge 6


//int led[6] = { 22, 24, 26, 28, 30, 32}; // 22: PORTF = 0x01, 24: 0x02, 26: 0x04, 28: 0x08, 30: 0x10, 32: 0x20, 34: 0x40

/*���� ���� ����*/
char RByte[10]={}; // USART �Է� �޾��ִ� �迭
char empty[10]={}; // USART �Է� ����ִ� �迭?
unsigned int count = 0; // TIMER0 ISR ���� ����


/*�Լ� ���� ����*/
void initialize_USART(void);
void initialize_Interrupt(void);
void initialize_LCD(void);
void initialize_LED(void);
void ClearArray(char []);

volatile unsigned int ad_val=0;
unsigned int time1 = 0;
unsigned char duty[] = {10,35,60,120,180,250};
unsigned char flag =5;

/*ISR ��ƾ ���� - TIMER0*/
ISR(TIMER0_OVF_vect){
	time1++;
	if (time1 % 500 ==0 )flag ++;
	if (flag%6 ==0 ) flag =0;
	TCNT0 = 6;
	count++;
}

ISR(ADC_vect)
{
	ad_val = ADC; // ���׷����ޱ�
	ad_val = ad_val%6;

}

ISR(INT0_vect)
{
	char line[20];
	LCDOUT();
	sprintf(line,"%d",ad_val);
	Command(LINE2);
	LCD_String(line);
	if(ad_val ==0) PORTA  = 0x20;
	else if (ad_val ==1) PORTA  = 0x40;
	else if (ad_val ==2) PORTD = 0x20;
	else if (ad_val ==3) PORTD  = 0x40;
	else if (ad_val ==4) PORTD = 0x80;
	else PORTD = 0x10;
	_delay_ms(1000.0);
	
	while(1){
		OCR0 = duty[flag];
		

		
		PORTA = 0x20;
		count++;
		if ((PIND & 0x01)==0x01) { _delay_ms(100.); break;}
		_delay_ms(100);
		
		
		PORTA = 0x40;
		count++;
		if ((PIND & 0x01)==0x01) { _delay_ms(100.); break;}
		_delay_ms(100);
		
		
		PORTA = PORTA & 0xcf;
		PORTD = 0x02;
		count++;
		if ((PIND & 0x01)==0x01) { _delay_ms(100.); break;}
		_delay_ms(100);
		
		
		PORTD = 0x04;
		count++;
		if ((PIND & 0x01)==0x01) { _delay_ms(100.); break;}
		_delay_ms(100);
		
		
		PORTD =  0x08;
		count++;
		if ((PIND & 0x01)==0x01) { _delay_ms(100.); break;}
		_delay_ms(100);
		
		
		PORTD = 0x10;
		count++;
		if ((PIND & 0x01)==0x01) { _delay_ms(100.); break;}
		_delay_ms(100.0);
		
		if (count%6  == 0) count=0;
	}
	
	// led 6�� ������� ���ư���
	
	PORTA = PORTA & 0xcf;
	PORTD = PORTD & 0xe0;
	
	if (ad_val == count)
	{
		// ���� �Ҹ�
		PORTA |= 0x02;
		_delay_ms(300);
		PORTA |= 0x04;
		_delay_ms(400);
		PORTA = PORTA^0x02;
		PORTA = PORTA^0x04;
	}
	else {
		PORTA |= 0x08;
		_delay_ms(500.);
		PORTA |= 0x10;
		_delay_ms(500.);
		PORTA = PORTA^(0x08);
		PORTA = PORTA^(0x10);
		// ����
	}
	initialize_LCD();
}

/*ISR ��ƾ ���� - USART*/
ISR(USART0_RX_vect){
	unsigned char index = 0; // USART ISR ���� ����
	RByte[index] = UDR0;
	index++;
	
	if(strncmp(RByte, "2", 1) == 0){
		/*LED ��� �κ�*/
		PORTA |= 0x01; // 1�� LED ON, 1���� �ǵ帲
		/*USART ���� �κ�*/
		index=0;
		for(int i =0; i<8;i++)
		RByte[i]=empty[i];
		/*LCD ǥ�� �κ�*/
		Command(LINE2);
		//Command(CLEAR);
		LCD_String("   FridgeOn");
		/*2�� �Ŀ� LCD �ʱ� ȭ������ ���ƿ���*/
		_delay_ms(2000);
		Command(CLEAR); Command(HOME); initialize_LCD();
		ClearArray(RByte); ClearArray(empty);
		index = 0;
	}
	
	else if(strncmp(RByte, "3", 1) == 0){
		PORTA = PORTA & 0xFE; // 1�� LED OFF, 1���� �ǵ帲
		index=0;
		for(int i =0; i<8;i++)
		RByte[i]=empty[i];
		Command(LINE2);
		//Command(CLEAR);
		LCD_String("   FridgeOff");
		_delay_ms(2000);
		Command(CLEAR); Command(HOME); initialize_LCD();
		ClearArray(RByte); ClearArray(empty);
		index = 0;
	}
	/*����:���� = 3:7 ������ ���ߴ� �κ�*/
	else if(strncmp(RByte, "4", 1) == 0){ // LED ���� �ð� ����
		Command(CLEAR);
		//Command(CLEAR);
		Command(HOME); LCD_String(" SOJU:3 BEER:7");
		Command(LINE2); LCD_String("    4.LED2,3");
		PORTA |= 0x06; // 2��, 3�� LED ON
		index=0;
		for(int i =0; i<8;i++)
		RByte[i]=empty[i];
		
		/*�Ϻη� LCD�� �ణ�� ȿ���� �־����*/
		
		_delay_ms(1000);
		Command(CLEAR); Command(HOME); LCD_String(" SOJU:2 BEER:6");
		_delay_ms(1000);
		Command(CLEAR); Command(HOME); LCD_String(" SOJU:1 BEER:5");
		_delay_ms(1000);
		PORTA = PORTA^0x02; //2�� LED OFF
		Command(CLEAR); Command(HOME); LCD_String(" SOJU:0 BEER:4");
		_delay_ms(1000);
		Command(CLEAR); Command(HOME); LCD_String(" SOJU:0 BEER:3");
		_delay_ms(1000);
		Command(CLEAR); Command(HOME); LCD_String(" SOJU:0 BEER:2");
		_delay_ms(1000);
		Command(CLEAR); Command(HOME); LCD_String(" SOJU:0 BEER:1");
		_delay_ms(1000);
		Command(CLEAR); Command(HOME); LCD_String(" SOJU:0 BEER:0");
		Command(LINE2); LCD_String("    COMPLETE!");
		PORTA = PORTA^0x04; //3�� LED OFF
		_delay_ms(1000);
		Command(CLEAR); Command(HOME); initialize_LCD();
		ClearArray(RByte); ClearArray(empty);
		index = 0;
	}
}

int main(void){
	initialize_LED();
	initialize_LCD();
	initialize_USART();
	initialize_Interrupt();
	ADMUX = 0;
	ADCSRA |= ADC_PRESCALE_128;
	ADCSRA |= ADC_ENABLE ;
	ADCSRA |= ADC_INTERRUPT_ENABLE;
	ADCSRA |= ADC_FREE_RUN;
	ADCSRA |= ADC_START;
	sei();
	
	while(1);
}

void initialize_USART(void){			// USART ���� �ʱ�ȭ
	UBRR0L = BAUD_38400;
	UCSR0B |= (1 << RXEN0 ) |(1<<TXEN0);
	UCSR0C |= (0<<USBS0);
	UCSR0C |= (1 << UCSZ00 ) |(1<<UCSZ01);
	UCSR0B |= (1<<RXCIE0);
}

void initialize_Interrupt(void){			// ��ư INT0�� Ÿ�̸� TIMER0 ���� �ʱ�ȭ
	Use_IE0_Interrupt;
	IE0_Detect_Falling;
	IE0_Detect_Rising;
	TIMER0_ENABLE;
	TIMER0_PRESCALE;
	sei();
}

void initialize_LCD(void){			// LCD ���� �ʱ�ȭ
	LCDOUT();
	Command(CLEAR);
	Command(HOME);
	char firstLINE[] = " SOJU BEER GAME ";
	LCD_String(firstLINE);
	char secondLINE[] = "         ";
	Command(LINE2);
	LCD_String(secondLINE);
}

void initialize_LED(void){			// LED ���� �ʱ�ȭ
	DDRA = 0xff;
}

void ClearArray(char Array[]){			// ���ڿ� �ʱ�ȭ
	for (int i = 0; i<10;i++)
	{
		*(Array+i) = 0;
	}
}