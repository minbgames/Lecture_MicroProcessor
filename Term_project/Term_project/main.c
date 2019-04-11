#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "lcd.h"
#include "keypad.h"

/**************** Segment define 정의 ****************/

#define N_BUF 5
#define N_SEGMENT 4
#define CONT_MASK ((1<<PB3)|(1<<PB2)|(1<<PB1)|(1<<PB0))
#define OUTPUT_VALUE (1<<PB0)


/********************함수 선언**********************/

void msec_delay(int n); // delay 함수 선언
void lcd_seg_velocity(short value); //lcd와 segment에 표시할 숫자 만드는 함수 선언
void ISeg7DispNum(unsigned short num, unsigned short radix);

/****************Segment 문자 만들기*************/

static unsigned char SegTable[17]
={0x3f, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F, 0x77, 0x7C, 0x58, 0x5E, 0x79, 0x71, 0x00};
//Segment 문자 1, 2, 3, 4, 5, 6, 7, 8, 9, A, B, c, d, e, F
static unsigned char cnumber[N_BUF] = {16,16,16,16,16}; //Segment 16으로 모두 초기화

/*****************Dot Matrix 문자 만들기***********/

unsigned char Dot_Basic_Driving[4][8] = { //초보운전 글자 만들기
	{0xE7,0x00,0xDB,0xBD,0x7E,0xE7,0xE7,0x00}, //초
	{0x7E,0x7E,0x00,0x7E,0x00,0xE7,0xE7,0x00}, //보
	{0xE7,0xDB,0xE7,0xFF,0x00,0xE7,0x7F,0x00}, //운
	{0x06,0xDE,0xA8,0x76,0xFE,0x7F,0x7F,0x00}  //전
};

/************LCD에 사용할 문자 5개 만들기************/

char font1[8]= {0x01, 0x01, 0x09, 0x15, 0x15, 0x15, 0x01, 0x01}; //시
char font2[8]= {0x1F, 0x10, 0x1F, 0x04, 0x1F, 0x04, 0x0A, 0x04}; //동
char font3[8]= {0x0E, 0x04, 0x0A, 0x1F, 0x04, 0x04, 0x0A, 0x04}; //중
char font4[8]= {0x04, 0x04, 0x0A, 0x0A, 0x11, 0x11, 0x00, 0x1F}; //스
char font5[8]= {0x1B, 0x13, 0x1F, 0x13, 0x1B, 0x00, 0x1F, 0x1F}; //템

/*****************전역 변수 설정********************/
unsigned short power=0;
unsigned short emergency_on_off_test=0;
unsigned short on_off_test=0;
unsigned short present_state=0;

char present_velocity[4];
char velocity_unit[6];
char emergency_ment[5];
char start_ment[5];
char end_ment[8];

/************오버플로우 타이머2 인터럽트 사용*********/
ISR(TIMER2_OVF_vect)
{
	static short type = 0;
	static short index = 0;
	static short emergency_state = 0;
	static int one_sec_change = 0;
	static int seg_index = 0;

	/*************전원이 켜졌을 때*************/
	if(present_state){
		/*************Segment 부분***************/
		PORTB = ( PORTB & ~CONT_MASK)|(~(OUTPUT_VALUE<<seg_index)&CONT_MASK);
		PORTE = ~SegTable[cnumber[seg_index]];
		seg_index++;
		if(seg_index == N_SEGMENT) seg_index=0;
		//Segment 출력

		/*****Dot matrix & 비상등 부분**********/
		if(++index==8) index=0;
		PORTD = 0x01 << index;
		PORTA = Dot_Basic_Driving[type][index];
		//type은 글자, index는 돌아가며 출력

		one_sec_change++;
		if(one_sec_change == 1000) //1sec
		{
			one_sec_change = 0;
			if(++type==4) type=0; //1초 마다 "초보운전" 글자가 바뀜
			
			if(emergency_on_off_test){ // 비상등 사용시 1초마다 번갈아가면서 on off
				if(emergency_state){
					PORTB=PORTB | 0x60; // 1<<PB6 1<<PB5 끔
					LcdMove(0,6);
					LcdPutchar(0x20);
					LcdMove(0,9);
					LcdPutchar(0x20);
					//공백으로 덮어씌우기
					emergency_state=0;
				}
				else{
					PORTB=PORTB & 0x9F; // 0<<PB6 0<<PB5 킴
					LcdMove(0,6);
					LcdPuts(emergency_ment);
					// <-  -> 출력
					emergency_state=1;
				}
			}
		}
	}
	/***************전원이 꺼졌을 때****************/
	else{
		index=0; // 전원을 끄면서 0으로 초기화
		type=0;  // 전원을 끄면서 0으로 초기화
		PORTA = 0xFF; //Dot matrix 전원 끄기
		PORTE = 0xFF; //Segment 전원 끄기
	}
	TCNT2 = 6; //1msec 

}

/***********************메인 함수*********************/
int main()
{
	/*****************변수 설정*****************/
	static volatile unsigned char duty = 5;
	unsigned char key;

	unsigned short right_motion=0;
	unsigned short left_motion=0;
	unsigned short emergency_motion=0;
	unsigned short headlight_motion=0;
	unsigned short break_motion=0;
	unsigned short original_velocity=0;
	unsigned short reduced_velocity=0;

	/*****************포트 설정*****************/
	PORTB = 0xFF;
	PORTD = 0x00;
	PORTA = 0x00;
	PORTE = 0xFF;
	//전부 초기값은 꺼진 상태(PB5~7은 1로 설정해야 꺼짐)
	// PORTE도 0이면 켜지기 때문에 1로 입력
	DDRB = 0xFF;
	DDRA = 0xFF;
	DDRD = 0xFF;
	DDRE = 0xFF;
	//B, D, A전부 출력으로 사용
	
	/**************인터럽트 설정****************/
	TCCR0=(1<<WGM01)|(1<<WGM00)|(2<<COM00); //PWM 모드 설정
	TCCR0 |=(0x02<<CS00); //분주비 1024
	//PWM 사용하기 위한 설정

	TCCR2 = 0x00; // Normal 모드는 기본값이 0
	TCCR2 |=(3 << CS20); //분주비 64
	TIMSK = (1 << TOIE2); // 타이머2 오버플로우 인터럽트 사용
	TCNT2 = 6; //1msec[256-1*16*1000/64]
	//Normal mode 타이머2 인터럽트 사용하기 위한 설정

	sei();

	/****************초기화 함수*****************/

	KeyInit(); //Keypad 초기화 함수
	LcdInit(); //LCD 초기화 함수

	/****************글자 만들기*****************/

	LcdNewchar(0, font1);
	LcdNewchar(1, font2);
	LcdNewchar(2, font3);
	LcdNewchar(3, font4);
	LcdNewchar(4, font5);
	// 순서대로 시, 동, 중, 스, 템 글자 등록

	start_ment[0]= 8;
	start_ment[1]= 1;
	start_ment[2]= ' ';
	start_ment[3]= 2;
	start_ment[4]= '\0';
	//시동중

	end_ment[0]= 8;
	end_ment[1]= 3;
	end_ment[2]= 4;
	end_ment[3]= ' ';
	end_ment[4]= 'o';
	end_ment[5]= 'f';
	end_ment[6]= 'f';
	end_ment[7]= '\0';
	// 시스템 off

	emergency_ment[0]=0x7F;
	emergency_ment[1]=' ';
	emergency_ment[2]=' ';
	emergency_ment[3]=0x7E;
	emergency_ment[4]='\0';
	// <-  ->

	velocity_unit[0]=' ';
	velocity_unit[1]='k';
	velocity_unit[2]='m';
	velocity_unit[3]='/';
	velocity_unit[4]='h';
	velocity_unit[5]='\0';
	// km/h

	for (int i=0; i<3; i++)
	{
		present_velocity[i]='0';
	}
	present_velocity[3]='\0';
	// 000속도숫자표시

	/***************시스템 시작****************/
	while(1){
		/*********키 입력 부분*********/
		key = KeyInput(); // 입력된 키 변수에 저장
		switch(key){
			case SW3: //시동off
				power=0;
				break;
			case SW4: //좌회전
				left_motion = 1;
				break;
			case SW6: //우회전
				right_motion = 1;
				break;
			case SW7: //비상등
				emergency_motion = 1;
				break;
			case SW11: //전조등
				headlight_motion = 1;
				break;
			default:
				break;
		}

		key = KeypadInput_Press(); //엑셀과 브레이크만 키를 연속적으로 받음
		switch(key){
			case SW1: //엑셀
				duty +=5;
				if(duty >250) duty=250;
				break;
			case SW9: //브레이크
				duty -=10;
				if(duty <10) duty=10;
				break_motion=1;
				break;
			default: // 마찰력 고려 아무 것도 입력하지 않았을 때 서서히 속도가 줄어듬
				duty -=1;
				if(duty <15) duty=15;
				break;
		}

		/*********시동 off중*********/
		if(present_state == 1 && power == 0){ //켜진 상태에서 power = 0 이 되면 꺼짐
			present_state = 0; // 시스템 off
			duty=5;
			OCR0 = duty; 
			//DC motor 멈춤

			PORTB = 0xF0;
			//LED와 Segment 끔

			PORTD = 0x00;
			PORTA = 0x00;
			//Dot Matrix 끔
			
			emergency_on_off_test=0; //비상등 off
			on_off_test=0; //전조등 off

			LcdCommand(ALLCLR);
			LcdMove(0,5);
			LcdPuts(end_ment);
			//초기화 후 "시스템 off" 출력

			for (int i=0; i<3; i++) //1초 마다 깜빡임(다른 키 입력되지 않음)
			{
				msec_delay(1000);
				LcdCommand(DISP_OFF);
				msec_delay(1000);
				LcdCommand(DISP_ON);
			}

			msec_delay(1000);
			LcdCommand(ALLCLR);
			LcdCommand(DISP_OFF);
			//LCD화면 off
			
		}

		/**********시동 중*********/
		if(present_state == 0 && power == 1){ //꺼진 상태에서 power = 1 이 되면 켜짐
			LcdMove(0,6);
			LcdPuts(start_ment);
			//"시동중" 출력

			for (int i=0; i<3; i++) //1초 마다 깜빡임(다른 키 입력되지 않음)
			{
				msec_delay(1000);
				LcdCommand(DISP_OFF);
				msec_delay(1000);
				LcdCommand(DISP_ON);
			}
			msec_delay(1000);
			LcdCommand(ALLCLR);
			//화면 초기화
			LcdMove(1,7);
			LcdPuts(velocity_unit);
			//LCD km/h 표시
			present_state = 1;//시스템 on
		}

		/********시동 완료 상태*******/
		if(present_state){ 
			OCR0 = duty; //duty값에 따라 비교일치 인터럽트
			lcd_seg_velocity(duty); //LCD와 Segment에 현재 속도와 RPM을 저장

			LcdMove(1,4);
			LcdPuts(present_velocity); //현재 속도 표시
			//LCD 속도 표시

			/**********전조등 작동 시*********/
			if(headlight_motion){
				if(on_off_test){ //on_off_test 0과 1이 번갈아가며 바뀜
					PORTB = PORTB | 0x80; // 1<<PB7 끔
					on_off_test=0;
					LcdMove(0,15);
					LcdPutchar(0x20);
					//공백 출력
				}
				else{
					PORTB = PORTB & 0x7F; // 0<<PB7 킴
					on_off_test=1;
					LcdMove(0,15);
					LcdPutchar(0xD6);
					 //문자 출력

			    }
				headlight_motion=0;
			}

			/**********브레이크 작동 시*********/
			if(break_motion){
				emergency_on_off_test=0;
				
				LcdMove(0,6);
				LcdPuts(emergency_ment);
				// "<-  ->" 출력

				PORTB=PORTB & 0x9F; // 0<<PB6 0<<PB5 킴
				break_motion=0;
			}
			else{
				if(!emergency_on_off_test){
					LcdMove(0,6);
					LcdPutchar(0x20); // 공백 출력
					LcdMove(0,9);
					LcdPutchar(0x20); // 공백 출력
					PORTB=PORTB | 0x60;  // 1<<PB6 1<<PB5 끔
				}
			}

			/**********비상등 작동 시*********/
			if(emergency_motion){
				if(emergency_on_off_test){
					emergency_on_off_test=0;

					LcdMove(0,6);
					LcdPutchar(0x20); // 공백 출력
					LcdMove(0,9);
					LcdPutchar(0x20); // 공백 출력

					PORTB=PORTB | 0x60; // 1<<PB6 1<<PB5 끔
				}
				else{
					emergency_on_off_test=1;
				}
				emergency_motion=0;
			}

			/**********우회전 작동 시*********/
			if(right_motion){
				
				emergency_on_off_test=0; //비상등 off

				LcdMove(0,6);
				LcdPutchar(0x20);
				LcdMove(0,9);
				LcdPutchar(0x20);
				 // 공백 출력 "<-  ->" 지우기

				PORTB=PORTB | 0x40; // 1<<PB6 끔

				original_velocity=duty; //현재 속도 저장
				reduced_velocity=duty*0.8; //80% 만큼 감속된 속도 저장

				/*------ 선회운동을 하기 전 감속 ------*/
				while(duty>=reduced_velocity){ //감속된 속도 만큼 duty값 감소
					OCR0 = duty; //duty값에 따라 비교일치 인터럽트
					lcd_seg_velocity(duty); //LCD와 Segment에 현재 속도와 RPM을 저장
					
					LcdMove(1,4);
					LcdPuts(present_velocity);
					//현재 속도 출력
					msec_delay(20);
					duty--;
				}
				duty=reduced_velocity;

				/*------ 선회운동 중 ------*/
				for (int i=0; i<5; i++)
				{
					LcdMove(0,9);
					LcdPutchar(0x7E);
					//"->" 출력
					PORTB = PORTB & 0xBF;// 0<<PB6 킴
					msec_delay(1000);

					LcdMove(0,9);
					LcdPutchar(0x20);
					// 공백 출력
					PORTB = PORTB | 0x40;// 1<<PB6 끔
					msec_delay(1000);
				}

				/*------ 선회운동을 한 후 원래속도로 가속 ------*/
				while(duty<=original_velocity){//원래 속도 까지 duty값 증가
					OCR0 = duty; //duty값에 따라 비교일치 인터럽트
					lcd_seg_velocity(duty); //LCD와 Segment에 현재 속도와 RPM을 저장
					
					LcdMove(1,4);
					LcdPuts(present_velocity);
					//현재 속도 출력
					msec_delay(20);
					duty++;
				}
				duty=original_velocity;

				right_motion=0;
			}

			/**********자회전 작동 시*********/
			if(left_motion){
				emergency_on_off_test=0;
				
				LcdMove(0,6);
				LcdPutchar(0x20);
				LcdMove(0,9);
				LcdPutchar(0x20);
				// 공백 출력 "<-  ->" 지우기

				PORTB=PORTB | 0x20; // 1<<PB5 끔
				
				original_velocity=duty; //현재 속도 저장
				reduced_velocity=duty*0.8; //80% 만큼 감속된 속도 저장

				/*------ 선회운동을 하기 전 감속 ------*/
				while(duty>=reduced_velocity){ //감속된 속도 만큼 duty값 감소
					OCR0 = duty; //duty값에 따라 비교일치 인터럽트
					lcd_seg_velocity(duty); //LCD와 Segment에 현재 속도와 RPM을 저장
					
					LcdMove(1,4);
					LcdPuts(present_velocity);
					//현재 속도 출력
					msec_delay(20);
					duty--;
				}
				duty=reduced_velocity;

				/*------ 선회운동 중 ------*/
				for (int i=0; i<5; i++)
				{
					LcdMove(0,6);
					LcdPutchar(0x7F);
					//"<-" 출력
					PORTB = PORTB & 0xDF; // 0<<PB5 킴
					msec_delay(1000);

					LcdMove(0,6);
					LcdPutchar(0x20);
					// 공백 출력
					PORTB = PORTB | 0x20; // 1<<PB5 끔
					msec_delay(1000);
				}

				/*------ 선회운동을 한 후 원래속도로 가속 ------*/
				while(duty<=original_velocity){//원래 속도 까지 duty값 증가
					OCR0 = duty; //duty값에 따라 비교일치 인터럽트
					lcd_seg_velocity(duty); //LCD와 Segment에 현재 속도와 RPM을 저장
					
					LcdMove(1,4);
					LcdPuts(present_velocity);
					//현재 속도 출력
					msec_delay(20);
					duty++;
				}
				duty=original_velocity;

				left_motion = 0;
			}

		}	

		/********시동이 꺼졌을 때 SW3키만 받을 수 있음********/
		while(!present_state){
			
			 key= KeyInput(); //입력된 key 저장
			switch(key){
				case SW3: //시동on
					power=1;
					break;
				default:
					break;
			}
			if(power) break;
		}

	}

}

/*************lcd와 segment 숫자 표시 함수 정의************/
void lcd_seg_velocity(short value){
	unsigned short number = 0;
	value/=2;
	if(value<=7) value=0;
	//value값이 7이하면 그냥 0으로 출력(10만 되도 모터가 돌지 않음)
	
	ISeg7DispNum(70*value,10); //Segment rpm값 저장
	//일반적으로 가솔린 승용차의 경우 8~9000rpm으로 맞춤
	
	while(1){
		present_velocity[2-number]='0'+value%10;
		//입력된 숫자의 각 자리수를 반대로 배열에 저장
		value/=10;
		if(!value){ 
			for (int i=number+1; i<=2; i++) //계산된 나머지 자리에 0을 입력해줌
			{
				present_velocity[2-i]='0';
			}
			break; //value값이 0 이라면 반복문을 종료
		}
		number++;
	}
}
	
/*******************딜레이 함수 정의********************/
void msec_delay(int n)
{
	for(; n>0; n--)
	_delay_ms(1);
}

/************Segment RPM값 저장 함수 정의***************/
void ISeg7DispNum(unsigned short num, unsigned short radix)
{
	int j;

	TIMSK &= ~(1<<TOIE2); //타이머2 오버플로우 인터럽트 금지
	for (j = 1; j < N_BUF; j++) cnumber[j] =16;

	j=0;
	do{ // 입력된 숫자를 cnumber에 저장
		cnumber[j++]=num%radix;
		num/=radix;
	}while(num); //num이 0이면 while문을 멈춤


	TIMSK |= (1<<TOIE2); //타이머2 오버플로우 인터럽트 허용
}