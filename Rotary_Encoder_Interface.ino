/*
Rotary Encoder Interface with LCD
begin 2023-02-01

가장 첫 열은 커서

한 메뉴 안에서
커서는 stepCount%LCD_ROW
step 카운트번째 메뉴부터 LCD_LOW만큼 반복해서 화면 표시 하면 됨
그럼 스크롤링 됨
step 카운트 사용 후 초기화

switch와 노드에 커맨드 넘버 저장해서 기능 구현
*/

#include "src/RotaryEN.h"
RotaryEN rotary(4,20);

#include "src/LiquidCrystal_I2C.h"
#define LCD_ROW 4
LiquidCrystal_I2C lcd(0x27,20,LCD_ROW);

//트리 자료형
class Node {
    public:
    String name;
    Node** child; //하위 자식노드 배열 저장
    uint8_t length; //하위 자식노드 갯수
    uint8_t command;

    Node(String Name = "", uint8_t childSize = 0, uint8_t Command = 0) {
        name = Name;
        length = childSize;
        child = NULL;
        command = Command;
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
    menu_A1 = 1,
    menu_A2,
    menu_A3,

    menu_B1,
    menu_B2,

    menu_C1,
    menu_C2,

    menu_D1,
    menu_D2,

    menu_E1,
    menu_E2,
} order;

//UI 내용 할당
Node* root =  new Node("",5);
Node* menuA = new Node("A",3);
Node* menuB = new Node("B",2);
Node* menuC = new Node("C",2);
Node* menuD = new Node("D",2);
Node* menuE = new Node("E",2);
//root->child = new Node*[5]{menuA, menuB, menuC, menuD, menuE};

Node* menuA1 = new Node("A1", 0, menu_A1);
Node* menuA2 = new Node("A2", 0, menu_A2);
Node* menuA3 = new Node("A3", 0, menu_A3);
//menuA->child = new Node*[3]{menuA1, menuA2, menuA3};

Node* menuB1 = new Node("B1", 0, menu_B1);
Node* menuB2 = new Node("B2", 0, menu_B2);
//menuB->child = new Node*[2]{menuB1, menuB2};

Node* menuC1 = new Node("C1", 0, menu_C1);
Node* menuC2 = new Node("C2", 0, menu_C2);
//menuC->child = new Node*[2]{menuC1, menuC2};

Node* menuD1 = new Node("D1", 0, menu_D1);
Node* menuD2 = new Node("D2", 0, menu_D2);
//menuD->child = new Node*[2]{menuD1, menuD2};

Node* menuE1 = new Node("E1", 0, menu_E1);
Node* menuE2 = new Node("E2", 0, menu_E2);
//menuE->child = new Node*[2]{menuE1, menuE2};


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
    static uint16_t stepCount = 0;
    stepCount += rotary.step();

    //커서 표시
    lcd.setCursor(0,stepCount%LCD_ROW);
    lcd.print(">");

    static Node* node = root;
    uint8_t len = node->length;

    //메뉴 lcd표시
    lcd.clear();
    for(int i=0; i<LCD_ROW; i++) {
        lcd.setCursor(1,(stepCount+i)%LCD_ROW);
        lcd.print(node->child[(stepCount+i)%len]->name);
    }

    if(rotary.pressed()) {
        if(node->child[stepCount%len]->command) { //i=0일때가 커서 있는 지점
            order = node->child[stepCount%len]->command;
            stepCount = 0;
        }
        else {
            node = node->child[stepCount%len];
            stepCount = 0;
        }
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
        node = root;
        break;

        case menu_A2:
        Serial.print("func");
        Serial.println(order);
        
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("func");
        lcd.print(order);
        node = root;
        break;

        case menu_A3:
        Serial.print("func");
        Serial.println(order);
        
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("func");
        lcd.print(order);
        node = root;
        break;

        case menu_B1:
        Serial.print("func");
        Serial.println(order);
        
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("func");
        lcd.print(order);
        node = root;
        break;

        case menu_B2:
        Serial.print("func");
        Serial.println(order);
        
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("func");
        lcd.print(order);
        node = root;
        break;

        case menu_C1:
        Serial.print("func");
        Serial.println(order);
        
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("func");
        lcd.print(order);
        node = root;
        break;

        case menu_C2:
        Serial.print("func");
        Serial.println(order);
        
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("func");
        lcd.print(order);
        node = root;
        break;

        case menu_D1:
        Serial.print("func");
        Serial.println(order);
        
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("func");
        lcd.print(order);
        node = root;
        break;

        case menu_D2:
        Serial.print("func");
        Serial.println(order);
        
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("func");
        lcd.print(order);
        node = root;
        break;

        case menu_E1:
        Serial.print("func");
        Serial.println(order);
        
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("func");
        lcd.print(order);
        node = root;
        break;

        case menu_E2:
        Serial.print("func");
        Serial.println(order);
        
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("func");
        lcd.print(order);
        node = root;
        break;
    }
    
}