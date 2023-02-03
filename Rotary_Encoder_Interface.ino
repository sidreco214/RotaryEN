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
*/
#ifndef constrain
#define constrain(amt,low,high) ((amt)<(low)?(low):((amt)>(high)?(high):(amt)))
#endif

#include "src/RotaryEN.h"
RotaryEN rotary(4,20);

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
      //전역으로 설정되었기에 호출될 일이 없지만 혹시 모르니
      //노드별로 포인터가 남아 있으니, 노드 포인터를 모아둔 배열은 그냥 해제해도 상관없음
      if(child) {
        delete[] child;
        child = NULL;
      }
    }
};

enum Order {
    back = 1,
    menu_A1,
    menu_A2,

    menu_B1,

    menu_C1,

    menu_D1,

    menu_E1,s
} order;

//UI 내용 할당
Node* root =  new Node("",5);
Node* menuA = new Node("A",3,root);
Node* menuB = new Node("B",2,root);
Node* menuC = new Node("C",2,root);
Node* menuD = new Node("D",2,root);
Node* menuE = new Node("E",2,root);

Node* menuA1 = new Node("A1",0,menuA,menu_A1);
Node* menuA2 = new Node("A2",0,menuA,menu_A2);
Node* menuA3 = new Node("Back",0,menuA,back);

Node* menuB1 = new Node("B1",0,menuB,menu_B1);
Node* menuB2 = new Node("Back",0,menuB,back);

Node* menuC1 = new Node("C1",0,menuC,menu_C1);
Node* menuC2 = new Node("Back",0,menuC,back);

Node* menuD1 = new Node("D1",0,menuD,menu_D1);
Node* menuD2 = new Node("Back",0,menuD,back);

Node* menuE1 = new Node("E1",0,menuD,menu_E1);
Node* menuE2 = new Node("Back",0,menuD,back);


void setup() {
    root->child =  new Node*[5]{menuA, menuB, menuC, menuD, menuE};
    menuA->child = new Node*[3]{menuA1, menuA2, menuA3};
    menuB->child = new Node*[2]{menuB1, menuB2};
    menuC->child = new Node*[2]{menuC1, menuC2};
    menuD->child = new Node*[2]{menuD1, menuD2};
    menuE->child = new Node*[2]{menuE1, menuE2};

    Serial.begin(9600);
    lcd.init();
    lcd.backlight();
}

void loop() {
    static Node* node = root;
    uint8_t len = node->length;

    static uint8_t cursor = 0; //자료 선택커서
    static uint8_t LCDcursor = 0; //LCD 화면 커서
    int8_t count = rotary.step();
    cursor += count;
    cursor %= len; 
    LCDcursor += count;
    LCDcursor = constrain(LCDcursor,0, min(LCD_ROW,len)-1);
    if(!cursor) LCDcursor = 0; //자료 커서가 마지막 E에 있다가 다음으로 넘어가면 자료 커서는 0번을 가르키는데 LCD 커서는 여전히 3을 가르키니 초기화 필요

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
        }
        else {
            node = node->child[cursor];
            cursor = 0;
            LCDcursor = 0;
        }
    }
    
    //선택한 메뉴의 기능실행
    switch(order) {
        default:
        break;

        case back:
        order = 0;
        node = node->pri;
        break;

        case menu_A1:
        Serial.print("func");
        Serial.println(order);

        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("func");
        lcd.print(order);
        order = 0;
        node = root;
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
        node = root;
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
        node = root;
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
        node = root;
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
        node = root;
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
        node = root;
        delay(2000);
        break;
    }
    delay(700);
}