/*
Rotary Encoder Interface with LCD
begin 2023-02-01

다른 프로젝트의 일부분이 될 수 있는 로터리 인코더 인터페이스
메뉴 항목이 많고, 자주 메뉴바가 뜨는 경우 이렇게 코딩하면 메모리 파편화 문제가 있을 수 있음
이경우 클래스 배열로 선언하거나, 정적으로 선언하는 것을 고려

다만 클래스 배열인 경우, 크게 잡으면 램이 2KB밖에 되지않는 아두이노 우노에서는
동적할당에 실패하여 버그가 발생할 수 있음, 메뉴바 단위, ex A B C D E 로 할당하면 적당할 듯
그렇다고 정적으로 선언하자니, 메뉴바가 뜨는 시간은 전체 실행시간에서 얼마 되지 않는데,
불필요하게 메모리만 차지함

문자열이 크기를 많이 차지하니, 문자열 부분이라도 따로 배열로 만들어 PROGMEM으로 선언해두고,
노드에는 주소값을 저장한 뒤, 실행할 때 불러오면 어느정도 해결될 듯
*/

//#define DEBUG

#ifndef constrain
#define constrain(amt,low,high) ((amt)<(low)?(low):((amt)>(high)?(high):(amt)))
#endif

#include "src/RotaryEN.h"

#include "src/LiquidCrystal_I2C.h"
#define LCD_ROW 4
LiquidCrystal_I2C lcd(0x27,20,LCD_ROW);

//트리 자료형
class Node {
    public:
    String name;
    Node* pri; //이전 노드
    Node** child; //하위 자식노드 배열 저장
    uint8_t length; //하위 자식노드 갯수
    uint8_t command;

    Node(String Name = "", uint8_t childSize = 0, Node* Pri = NULL, uint8_t Command = 0) {
        name = Name;
        length = childSize;
        child = NULL;
        command = Command;
        pri = Pri;
    };

    ~Node() {
      if(child) {
        delete [] child;
        child = NULL;
      }
    }
};

#define BACK 1
enum Order {
    menu_A1 = 2,
    menu_A2,

    menu_B1,

    menu_C1,

    menu_D1,

    menu_E1,
} order;

void setup() {
    #ifdef DEBUG
    Serial.begin(9600);
    #endif
    lcd.init();
    lcd.backlight();
}

void loop() {
    while(1) {
        RotaryEN rotary(4,20); //로터리 인코더가 메뉴 화면에서만 작동해야 함

        //UI 내용 할당
        Node root("",5);
        Node menuA("A",3,&root);
        Node menuB("B",2,&root);
        Node menuC("C",2,&root);
        Node menuD("D",2,&root);
        Node menuE("E",2,&root);
        root.child =  new Node*[5]{&menuA, &menuB, &menuC, &menuD, &menuE};

        Node menuA1("A1",0,&menuA,menu_A1);
        Node menuA2("A2",0,&menuA,menu_A2);
        Node menuA3("Back",0,&menuA,BACK);
        menuA.child = new Node*[3]{&menuA1, &menuA2, &menuA3};

        Node menuB1("B1",0,&menuB,menu_B1);
        Node menuB2("Back",0,&menuB,BACK);
        menuB.child = new Node*[2]{&menuB1, &menuB2};

        Node menuC1("C1",0,&menuC,menu_C1);
        Node menuC2("Back",0,&menuC,BACK);
        menuC.child = new Node*[2]{&menuC1, &menuC2};
        
        Node menuD1("D1",0,&menuD,menu_D1);
        Node menuD2("Back",0,&menuD,BACK);
        menuD.child = new Node*[2]{&menuD1, &menuD2};

        Node menuE1("E1",0,&menuD,menu_E1);
        Node menuE2("Back",0,&menuD,BACK);
        menuE.child = new Node*[2]{&menuE1, &menuE2};

        Node* node = &root;
        int16_t cursor = 0; //자료 선택커서
        int16_t LCDcursor = 0; //LCD 화면 커서

        while(1) {
            uint8_t len = node->length;
            int16_t count = rotary.step();
            
            uint8_t num = min(LCD_ROW,len);
            LCDcursor += count;
            LCDcursor = constrain(LCDcursor,0, num-1);

            cursor += count;
            if(cursor < 0) cursor = 0;
            else           cursor %= len;
            if(LCDcursor > cursor) LCDcursor = cursor;
            //자료 커서가 마지막 E에 있다가 다음으로 넘어가면 자료 커서는 0번을 가르키는데 LCD 커서는 여전히 마지막 줄을 가르키니 초기화 필요
            //그리고 로터리 엔코더를 빨리 돌리면, LCD 커서는 마지막 줄을 가르키는데, 자료 커서는 0,1,2(num 보다 작은 값)을 가르킬 수 있고, 이 경우 문제됨
            //LCD 커서는 항상 자료 커서보다 작거나 같아야 함

            #ifdef DEBUG
            Serial.print(cursor);
            Serial.print(", ");
            Serial.print(LCDcursor);
            Serial.print(", ");
            Serial.print(count);
            Serial.println("");
            #endif

            lcd.clear();
            //LCD 커서 표시
            lcd.setCursor(0,LCDcursor);
            lcd.print(">");
    
            //위로 진행해서 화면 채우기
            for(int i=0; i<LCD_ROW-LCDcursor; i++) {
                if(LCDcursor+i < LCD_ROW && cursor+i < len) {
                    lcd.setCursor(1,LCDcursor+i);
                    lcd.print(node->child[cursor+i]->name);
                }
            }

            //아래로 진행해서 화면 채우기
            for(int i=1; i<=LCDcursor; i++) {
                if(0 <= LCDcursor-i && 0 <= cursor-i) {
                    lcd.setCursor(1,LCDcursor-i);
                    lcd.print(node->child[cursor-i]->name);
                }
            }
    
            if(rotary.pressed()) {
                if(node->child[cursor]->command) {
                    order = node->child[cursor]->command;
                    cursor = 0;
                    LCDcursor = 0;
                    if(order == BACK) {order = 0; node = node->pri;}
                    else             {break;}
                }
                else {
                    node = node->child[cursor];
                    cursor = 0;
                    LCDcursor = 0;
                }
            }
            else delay(500); //너무 빨리 clear와 쓰기가 반복되면 화면이 제대로 안보임 
        }
        break;
    }
    
    //선택한 메뉴의 기능실행
    switch(order) {
        default:
        break;

        case menu_A1:
        #ifdef DEBUG
        Serial.print("func");
        Serial.println(order-1);
        #endif

        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("func");
        lcd.print(order-1);
        order = 0;
        delay(2000);
        break;

        case menu_A2:
        #ifdef DEBUG
        Serial.print("func");
        Serial.println(order-1);
        #endif

        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("func");
        lcd.print(order-1);
        order = 0;
        delay(2000);
        break;

        case menu_B1:
        #ifdef DEBUG
        Serial.print("func");
        Serial.println(order-1);
        #endif

        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("func");
        lcd.print(order-1);
        order = 0;
        delay(2000);
        break;

        case menu_C1:
        #ifdef DEBUG
        Serial.print("func");
        Serial.println(order-1);
        #endif

        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("func");
        lcd.print(order-1);
        order = 0;
        delay(2000);
        break;

        case menu_D1:
        #ifdef DEBUG
        Serial.print("func");
        Serial.println(order-1);
        #endif

        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("func");
        lcd.print(order-1);
        order = 0;
        delay(2000);
        break;

        case menu_E1:
        #ifdef DEBUG
        Serial.print("func");
        Serial.println(order-1);
        #endif

        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("func");
        lcd.print(order-1);
        order = 0;
        delay(2000);
        break;
    }
}
