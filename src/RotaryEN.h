/*
Rotary Encoder
로터리 인코더의 회전 방향과 스탭, 회전 각도, 각속도를 계산할 수 있음
참고 https://elecs.tistory.com/181
참고하기 전에 내가 설계했을 때는 인터럽트 INT0, INT1 두개 썻는데, 이상하게 작동할 때 있음
*/

//Macro
#ifndef cbi //clear bit
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif

#ifndef sbi //set bit
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif

#ifndef ROTARY
#define ROTARY
#include <avr/io.h>
#include <avr/interrupt.h>
#include "Arduino.h"

void INIT_INT0();   //s1핀의 RISING

//버튼 입력
void INIT_PCINT(); //D포트 0~7, B포트 8~13, C포트 14~19
void END_PCINT();
inline void button(); //버튼 입력시 인터럽트 처리 함수

class RotaryEN {
    protected:
    uint8_t _step; //로터리 인코더 스탭

    public:
    RotaryEN(); //방향만 필요한 경우
    RotaryEN(uint8_t button); //방향+버튼 입력
    RotaryEN(uint8_t button, uint8_t step); //몇도 회전했는지 까지 필요한 경우
    ~RotaryEN();

    uint8_t pressed(); //버튼이 눌러졌으면 1
    int16_t step(); //로터리 인코더 회전 스탭 반환
    inline float rottation(uint8_t step); //로터리 인코더 회전 각도 변환(단위: 도)
    inline float velocity(uint8_t step);

};
#endif