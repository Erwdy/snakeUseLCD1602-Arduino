#include <LiquidCrystal.h>

#define bPinNum 4

LiquidCrystal lcd(12,2,7,8,9,10);
typedef struct SnakeNode {
    int x, y;
    struct SnakeNode *n;
    struct SnakeNode *p;
};

int bW=3,bS=4,bA=5,bD=6;

int bPin[]={6,5,4,3};

unsigned long lastLCDTime = 0;
const int LCD_INTERVAL = 9000;

byte validPositions[20*16][2];
int validCount = 0;
int snakeLength;

SnakeNode *snake;
SnakeNode *tail;
byte apple[2];
byte screen[4][2][8];
bool snakeMoved=false;

void setSnakeLength(){
  snakeLength=0;
  SnakeNode *t=snake;
  while(t!=NULL){
    snakeLength++;
    t=t->n;
  }
}
void initValidPositions() {
    validCount = 0;
    for(int x=0; x<20; ++x) {
        for(int y=0; y<16; ++y) {
            bool isOccupied = false;
            SnakeNode* tmp = snake;
            while(tmp) {
                if(tmp->x == x && tmp->y == y) {
                    isOccupied = true;
                    break;
                }
                tmp = tmp->n;
            }
            if(!isOccupied) {
                validPositions[validCount][0] = x;
                validPositions[validCount][1] = y;
                validCount++;
            }
        }
    }
}
void spawnApple(){
  initValidPositions();
  int t=random(0,validCount);
  apple[0]=validPositions[t][0];
  apple[1]=validPositions[t][1];
}
void setScreen(byte x,byte y,bool isLighten){
  if(isLighten){
  screen[x/5][y/8][y%8]|=(1<<(4-x%5));
  }else{
    screen[x/5][y/8][y%8]&=~(byte)(1<<(4-x%5));
  }
}

void renderSnake(){
  SnakeNode *tmp=snake;
  while(tmp!=NULL){
    setScreen(tmp->x,tmp->y,true);
    tmp=tmp->n;
  }
}
void render(){
  for (int i = 0; i < 4; ++i) {
    for (int j = 0; j < 2; ++j) {
        for (int k = 0; k < 8; ++k) {
            screen[i][j][k] = 0;
        }
    }
  }
  setScreen(apple[0],apple[1],true);
  renderSnake();
}
void setup() {
  for(int i=0; i<bPinNum; i++){
    pinMode(bPin[i], INPUT);
  }
  pinMode(0,OUTPUT);
  lcd.begin(16, 2);
  snake=new SnakeNode{10, 8, nullptr,nullptr};
  SnakeNode *mid=new SnakeNode{11, 8, nullptr,nullptr};
  tail=new SnakeNode{12, 8, nullptr,nullptr};
  snake->n=mid;
  tail->p=mid;
  
  mid->p=snake;
  mid->n=tail;
  snakeMoved=true;
  
  spawnApple();
  setSnakeLength();
  //if(snake->n==NULL){
    //digitalWrite(0,HIGH);
  //}
}
void checkIsGO(){
  byte x=snake->x;
  byte y=snake->y;
  SnakeNode *tmp=snake->n;
  while(tmp!=NULL){
    if(tmp->x==x&&tmp->y==y){
      lcd.clear();
      lcd.print("Game Over.");
      delay(9999999999);
    }
    tmp=tmp->n;
  }
}
void snakeMove(bool isX,bool isPlus){
  byte posWillGoTo[2];
  if(isX){
    byte tmp=snake->x;
    if(isPlus){
      if(tmp==19){
        tmp=0;
      }else{
      tmp+=1;
      }
    }else{
      if(tmp==0){
      tmp=19;
      }else{
      tmp-=1;
      }
    }
    posWillGoTo[0]=tmp;
    posWillGoTo[1]=snake->y;
  }else{
    byte tmp=snake->y;
    if(isPlus){
      if(tmp==15){
        tmp=0;
      }else{
      tmp+=1;
      }
    }else{
      if(tmp==0){
        tmp=15;
      }else{
      tmp-=1;
      }
    }
    posWillGoTo[1]=tmp;
    posWillGoTo[0]=snake->x;
  }
  if(posWillGoTo[0]!=apple[0]||posWillGoTo[1]!=apple[1]){
  //if no ate apple
  tail->x=posWillGoTo[0];
  tail->y=posWillGoTo[1];
  snake->p=tail;
  SnakeNode *tailAfter=tail->p;
  tailAfter->n=nullptr;
  tail->p=nullptr;
  tail->n=snake;
  snake=tail;
  tail=tailAfter;
  checkIsGO();
  }else{
  //if ate apple
    SnakeNode *newHead=new SnakeNode{posWillGoTo[0],posWillGoTo[1],snake,nullptr};
    snake->p=newHead;
    snake=newHead;
    spawnApple();
    setSnakeLength();
  }
  snakeMoved=true;
}
void loop() {
  unsigned long currentTime = millis();
  if (currentTime - lastLCDTime >= LCD_INTERVAL||snakeMoved) {
    snakeMoved=false;
  	render();
  	lcd.clear();
    lcd.setCursor(0, 1);
    lcd.print(snakeLength);
  	for(byte i=0;i<4;i++){
  	  for(byte j=0;j<2;j++){
      	lcd.createChar(i+(j<<2), screen[i][j]);
      	lcd.setCursor(i+6, j);
      	lcd.write(i+(j<<2));
      }
  	}
    lastLCDTime = currentTime;
  }
  for(int i=0;i<bPinNum;i++){
    int tmp = digitalRead(bPin[i]);
    if(tmp==HIGH){
        lcd.setCursor(0, 0);
        lcd.print(bPin[i]);
      delay(50);
      if(digitalRead(bPin[i])==HIGH){
        while(digitalRead(bPin[i])==HIGH){
          delay(10);
        }
        if(bPin[i]==bW){
    		snakeMove(false,false);
  		}else if(bPin[i]==bS){
    		snakeMove(false,true);
  		}else if(bPin[i]==bA){
    		snakeMove(true,false);
  		}else if(bPin[i]==bD){
    		snakeMove(true,true);
  		}
      }
      break;
    }
  }
  delay(50);
}