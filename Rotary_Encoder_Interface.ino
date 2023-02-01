/*
Rotary Encoder Interface with LCD
begin 2023-02-01

가장 첫 열은 커서 있음

한 메뉴 안에서
커서는 step 카운트를 LCD_ROW로 나눈 나머지 연산을 이용하면 자동으로 되고
step 카운트번째 메뉴부터 LCD_LOW만큼 반복해서 화면 표시 하면 됨
그럼 스크롤링 됨
step 카운트 사용 후 초기화

비트마스크 진리표 만들어서 switch() 해서
0번 비트 1이면 메인메뉴 활성화
1번 비트 1이면 메뉴 A 활성화
...

각 case에서 메뉴 정보를 담은 배열 주소를 전달하고
*/

#include "src/RotaryEN.h"
RotaryEN rotary(4,20);

#include "LiquidCrystal_I2C.h"
#define LCD_ROW 4
LiquidCrystal_I2C lcd(0x27,20,LCD_ROW);

#include "UI.h"

const String menuMain[] = {"menuA", "menuB", "menuC", "menuD", "menuE"};
const String menuA[] =    {"menuA1", "menuA2", "menuA3", "Back"};
const String menuB[] =    {"1", "2", "Back"};
const String menuC[] =    {"1", "2", "Back"};
const String menuD[] =    {"1", "2", "Back"};
const String menuE[] =    {"1", "2", "Back"};

uint8_t index;

void setup() {
    lcd.init();
    lcd.backlight();
}

void loop() {
    while(1)  {
        static uint16_t stepCount = 0;
        stepCount += rotary.step();

        static String *buffer = &menuMain;

        //커서 표시
        lcd.setCursor(0,stepCount%LCD_ROW);
        lcd.write(0b10100100);
        
        //메뉴 lcd표시
        uint16_t temp = stepCount;
        for(int i=0; i<LCD_ROW; i++) {
            lcd.setCursor(1,i);
            lcd.print(buffer[temp]);
            temp %= buffer.length();
        }

        if(rotary.pressed()) {
            index = stepCount; //만약 인덱스가 0이면 menuA의 주소가 buffer로 전달되어야 함, 이게 문제
            stepCount = 0;
        }
    }

    //최종 메뉴에서 기능은 enum{} state;과 switch(state) 구문으로 구현 
    
}