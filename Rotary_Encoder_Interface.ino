/*
Rotary Encoder Interface with LCD
begin 2023-02-01

cursor = 0
>A 0123          0123 
 B
 C
 D

cursor =1
 A                0123
>B 1230
 C
 D

cursor =2
 A                0123
 B
>C 2301
 D

cursor =3
 A                0123
 B
 C
>D 3012

cursor =4
 B                1234
 C
 D
>E 4123

cursor =5 나머지 연산 %len

>A 0123
 B
 C
 D

 cursor 4->3이면
 B                1234
 C
>D
 E

 자료 커서와 LCD 커서를 만든 뒤
 자료커서는 나머지 연산으로 순환
 LCD 커서는 constrain

 LCD커서로부터 위아래로 움직이면서 화면채우기

 값 복사 도중 인터럽트가 일어나면, 버그가 생길 수 있음
 그래서 인코더를 너무 빨리 돌리지 않도록 조심
 이 경우 천천히 조금 돌려주면 해결됨
*/
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
        delete[] child;
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

    menu_E1,s
} order;

void setup() {
    Serial.begin(9600);
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
        uint16_t cursor = 0; //자료 선택커서
        uint16_t LCDcursor = 0; //LCD 화면 커서

        while(1) {
            uint8_t len = node->length;
            long count = rotary.step();
            if(count > 128) count = 127;
            if(count < -128) count = -127;
            
            uint8_t num = min(LCD_ROW,len);
            LCDcursor += count;
            LCDcursor = constrain(LCDcursor,0, num-1);

            cursor += count;
            cursor %= len;
            if(LCDcursor > cursor) LCDcursor = cursor;
            //자료 커서가 마지막 E에 있다가 다음으로 넘어가면 자료 커서는 0번을 가르키는데 LCD 커서는 여전히 마지막 줄을 가르키니 초기화 필요
            //그리고 로터리 엔코더를 빨리 돌리면, LCD 커서는 마지막 줄을 가르키는데, 자료 커서는 0,1,2(num 보다 작은 값)을 가르킬 수 있고, 이 경우 문제됨
            //LCD 커서는 항상 자료 커서보다 작거나 같아야 함

            lcd.clear();
            //LCD 커서 표시
            lcd.setCursor(0,LCDcursor);
            lcd.print(">");
    
            //아래로 진행해서 화면 채우기
            for(int i=0; i<LCD_ROW-LCDcursor; i++) {
                if(LCDcursor+i < LCD_ROW && cursor+i < len) {
                    lcd.setCursor(1,LCDcursor+i);
                    lcd.print(node->child[cursor+i]->name);
                }
            }

            //위로 진행해서 화면 채우기
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
        Serial.print("func");
        Serial.println(order);

        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("func");
        lcd.print(order);
        order = 0;
        delay(2000);
        break;

        case menu_A2:
        Serial.print("func");
        Serial.println(order);
        
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("func");
        lcd.print(order);
        order = 0;
        delay(2000);
        break;

        case menu_B1:
        Serial.print("func");
        Serial.println(order);
        
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("func");
        lcd.print(order);
        order = 0;
        delay(2000);
        break;

        case menu_C1:
        Serial.print("func");
        Serial.println(order);
        
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("func");
        lcd.print(order);
        order = 0;
        delay(2000);
        break;

        case menu_D1:
        Serial.print("func");
        Serial.println(order);
        
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("func");
        lcd.print(order);
        order = 0;
        delay(2000);
        break;

        case menu_E1:
        Serial.print("func");
        Serial.println(order);
        
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("func");
        lcd.print(order);
        order = 0;
        delay(2000);
        break;
    }
}