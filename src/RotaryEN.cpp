#include "RotaryEN.h"

/*
인터럽트 내에서 방향을 파악한 뒤, count++ or count--
*/
volatile int8_t count = 0; //s1 핀의 RISING 카운트
volatile unsigned long pri; //각속도 측정용 이전 시간 저장
volatile unsigned long time; //걸린 시간

void INIT_INT0() {
  cbi(DDRB,PD2); //2번핀 입력으로
  sbi(EIMSK,INT0); //INT0 인터럽트 활성화
  EICRA ^= 0x03; //RISING 에서 활성화
  sei(); //set interupt, 인터럽트 전역적으로 활성화
};

ISR(INT0_vect) {
    if(digitalRead(3)) {
        count++;
        time = millis() - pri;
        pri = millis();
    }
};

void INIT_INT1() {
  cbi(DDRB,PD3); //3번핀 입력으로
  sbi(EIMSK,INT1); //INT1 인터럽트 활성화
  EICRA ^= 0x0c; //RISING 에서 활성화
  sei(); //set interupt, 인터럽트 전역적으로 활성화
};

ISR(INT1_vect) {
    if(digitalRead(2)) {
        count--;
        time = millis() - pri;
        pri - millis();
    }
};

volatile uint8_t buttonLV = 0;

void INIT_PCINT(uint8_t pin) {
    if(pin < 8) {
        //D포트(0~7) PCINT16~23
        cbi(DDRD,pin);
        sbi(PCICR,PCIE2); //PCINT2 인터럽트 활성화
        sbi(PCMSK2,(pin+16)); //해당 핀의 PCINT2 인터럽트 활성화
    }
    else if(pin < 14) {
        //B포트(8~13) PCINT0~7
        cbi(DDRB,(pin-8));
        sbi(PCMSK0,PCIE0); //PCINT0 인터럽트 활성화
        sbi(PCMSK0,(pin-8)); //해당 핀의 PCINT0 인터럽트 활성화
    }
    else if(pin < 20) {
        //C포트(14~19) PCINT14~19
        cbi(DDRC,(pin-14));
        sbi(PCMSK1,PCIE1); //PCINT1 인터럽트 활성화
        sbi(PCMSK1,(pin-6)); //해당 핀의 PCINT1 인터럽트 활성화
    }
    else return;
    sei();
};

inline void button() {
    buttonLV = 1;
};

ISR(PCINT0_vect) {
  button();
};

ISR(PCINT1_vect) {
  button();
};

ISR(PCINT2_vect) {
  button();
};

RotaryEN::RotaryEN() {
    pri = millis();
    INIT_INT0();
    INIT_INT1();
}

RotaryEN::RotaryEN(uint8_t button) {
    pri = millis();
    INIT_INT0();
    INIT_INT1();
    INIT_PCINT(button);
    buttonPin = button;
}

RotaryEN::RotaryEN(uint8_t button, uint8_t step) {
    pri = millis();
    INIT_INT0();
    INIT_INT1();
    INIT_PCINT(button);
    buttonPin = button;
    _step = step;
}

uint8_t RotaryEN::pressed() {
    if(buttonLV) {buttonLV = 0; return 1;}
    else         return 0;
}

int8_t RotaryEN::step() {
    int8_t temp = count;
    count = 0;
    return temp;
}

inline float RotaryEN::rottation(uint8_t pulse) {
    return pulse*(360.0/_step);
}

inline float RotaryEN::velocity(uint8_t pulse) {
    return this->rottation(pulse)/time;
}