#include "RotaryEN.h"

/*
인터럽트 내에서 방향을 파악한 뒤, count++ or count--
s1 A상
s2 B상
*/
volatile int16_t count = 0; //s1 핀의 RISING 카운트
volatile unsigned long pri; //각속도 측정용 이전 시간 저장
volatile unsigned long time; //걸린 시간

void INIT_INT0() {
  cbi(DDRB,PD2); //2번핀 입력으로
  cbi(DDRB,PD3); //3번핀 입력으로
  sbi(EIMSK,INT0); //INT0 인터럽트 활성화
  EICRA |= 0x03; //RISING 에서 활성화
  sei(); //set interupt, 인터럽트 전역적으로 활성화
};

ISR(INT0_vect) {
    int8_t state = digitalRead(3);
    if(state) count--;
    else      count++;
    time = millis() - pri;
    pri = millis();
};

volatile uint8_t buttonLV = 0;
volatile unsigned long t = 0;
uint8_t pin;

void INIT_PCINT() {
    if(pin < 8) {
        //D포트(0~7) PCINT16~23
        cbi(DDRD,pin);
        sbi(PCICR,PCIE2); //PCINT2 인터럽트 활성화
        sbi(PCMSK2,pin); //해당 핀의 PCINT2 인터럽트 활성화
    }
    else if(pin < 14) {
        //B포트(8~13) PCINT0~7
        cbi(DDRB,(pin-8));
        sbi(PCICR,PCIE0); //PCINT0 인터럽트 활성화
        sbi(PCMSK0,(pin-8)); //해당 핀의 PCINT0 인터럽트 활성화
    }
    else if(pin < 20) {
        //C포트(14~19) PCINT14~19
        cbi(DDRC,(pin-14));
        sbi(PCICR,PCIE1); //PCINT1 인터럽트 활성화
        sbi(PCMSK1,(pin-14)); //해당 핀의 PCINT1 인터럽트 활성화
    }
    else return;
    sei();
};

void END_PCINT() {
    if(pin < 8) {
        //D포트(0~7) PCINT16~23
        cbi(PCICR,PCIE2); //PCINT2 인터럽트 비활성화
        cbi(PCMSK2,pin); //해당 핀의 PCINT2 인터럽트 비활성화
    }
    else if(pin < 14) {
        //B포트(8~13) PCINT0~7
        cbi(PCICR,PCIE0); //PCINT0 인터럽트 비활성화
        cbi(PCMSK0,(pin-8)); //해당 핀의 PCINT0 인터럽트 비활성화
    }
    else if(pin < 20) {
        //C포트(14~19) PCINT14~19
        cbi(PCICR,PCIE1); //PCINT1 인터럽트 활성화
        cbi(PCMSK1,(pin-14)); //해당 핀의 PCINT1 인터럽트 비활성화
    }
    else return;
}

inline void button() {
    if(millis()-t > 500 && digitalRead(pin)) {
        buttonLV = 1;
    }
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
}

RotaryEN::RotaryEN(uint8_t button) {
    pri = millis();
    pin = button;
    INIT_INT0();
    INIT_PCINT();
}

RotaryEN::RotaryEN(uint8_t button, uint8_t step) {
    pri = millis();
    pin = button;
    INIT_INT0();
    INIT_PCINT();
   
    _step = step;
}

RotaryEN::~RotaryEN() {
    cbi(EIMSK,INT0); //INT0 인터럽트 비활성화
    END_PCINT();
    count = 0;
    pri = 0;
    time = 0;
    buttonLV = 0;
    t = 0;
    pin = 0;
}

uint8_t RotaryEN::pressed() {
    if(buttonLV) {buttonLV = 0; return 1;}
    else         return 0;
}

int16_t RotaryEN::step() {
    int16_t temp = count;
    count = 0;
    return temp;
}

inline float RotaryEN::rottation(uint8_t pulse) {
    return pulse*(360.0/_step);
}

inline float RotaryEN::velocity(uint8_t pulse) {
    return this->rottation(pulse)/time;
}
