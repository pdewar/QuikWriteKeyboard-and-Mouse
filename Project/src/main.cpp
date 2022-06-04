#include <Arduino.h>
#include <BleCombo.h>
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_BusIO_Register.h>
#include "Adafruit_GFX.h"
#include "Adafruit_ILI9341.h"
#include <Fonts/FreeMono18pt7b.h>
#include "SymbolMono18pt7b.h"       // custom fonts for special functions in layout
#include <ezButton.h>

const int SHORT_PRESS_TIME = 1000;  
const int LONG_PRESS_TIME  = 1000;  
ezButton button_3(21);              // create ezButton object that attach to pin 21;
ezButton button_2(17);              // create ezButton object that attach to pin 17;
ezButton button_1(16);              // create ezButton object that attach to pin 16;
ezButton buttonPIN(0);              // create ezButton object that attach to pin 0;
unsigned long pressedTime  = 0;
unsigned long releasedTime = 0;

String combine[4];
uint8_t combine2[4];


// For the ESP-WROVER_KIT, these are the default.
#define TFT_MISO 19
#define TFT_MOSI 23
#define TFT_CLK  18
#define TFT_CS   15  // Chip select control pin
#define TFT_DC    2  // Data Command control pin
#define TFT_RST   4  // Reset pin (could connect to RST pin)
#define TFT_LED   5  // GPIO not managed by library
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_MOSI, TFT_CLK, TFT_RST, TFT_MISO);

const int VRxPIN = 32;
const int VRyPIN = 33;

typedef struct special {
  int16_t xpos; 
  uint16_t ypos; 
  uint16_t character; 
  uint8_t func;
} special;

String lay1[32] = {"a","m","q","s","k","box","back","up","p","f","n","l","x","h","e","c","u","t","y","v","w","o","g","z","down",
                   "forward","circle","j","r","i","d","b"};

String lay2[32] = {"1","/","=","{","(","box","back","up",")","}","2","*","%","3","5","7","4","6","8","<","9",",","[","-","down",
                   "forward","circle",">","0",".","]","+"};

String lay3[32] = {"?","/","\\","{","`","box","back","up","^","}","!","*","%","'",";","_","\"",":","&","|","~",",","[","@","down",
                   "forward","circle","$","+",".","]","#"};
                  
String  lay1cap[32] = {"A","M","Q","S","K","BOX","BACK","UP","P","F","N","L","X","H","E","C","U","T","Y","V","W","O","G","Z","DOWN",
                   "FORWARD","CIRCLE","J","R","I","D","B"};

String *laycontainer[4]; //Stores pointers to each layout of characters

special *speclaycontainer[3]; //Stores pointer to each layout of special functions

int move = -1;
int enter = 0;
int leave = 0;
int VRx = 0;
int VRy = 0;
int xState = 0;
int yState = 0;

int charPosition;
boolean board_select;
boolean mouse_select;
unsigned long startMillis;
unsigned long currentMillis;
const unsigned long period = 20;
boolean stoploop = false;         //Stops the loop function when true
String currentFunc = "keyboard";  //defaut functionality name
String *currentLay;               //pointer to layouts
int layNum;
special specialChars1[6];
special specialChars2[6];
special specialChars3[6];
special *currentspecLay;
int layNum2;
String held;
int JoyX; //hold x value to remove stick drift
int JoyY; //hold y value to remove stick drift
boolean track = true;


void drawSymbol(uint16_t x, uint16_t y, uint8_t c, uint16_t color, uint16_t bg, uint8_t Size){
  if( (c>=32) && (c<=126) ){        //If it's 33-126 then use standard mono 18 font
    tft.setFont(&FreeMono18pt7b);
  } else {
    tft.setFont(&SymbolMono18pt7b); //Otherwise use special symbol font
    if (c>126) {                    //Remap anything above 126 to be in the range 32 and upwards
      c-=(127-32);
    }
  }
  tft.drawChar(x,y,c,color,bg,Size);
}

// Initialize arrays of of special characters 
void specialCharacterSetup(){
  special square = { .xpos = 83, .ypos = 22, .character = MY_SQUARE, .func = KEY_RETURN};
  specialChars1[0] = square;
  special back = { .xpos = 109, .ypos = 22, .character = MY_BACKWARD, .func = KEY_LEFT_ARROW};
  specialChars1[1] = back;
  special up = { .xpos = 137, .ypos = 23, .character = MY_UPWARD, .func = KEY_UP_ARROW};
  specialChars1[2] = up;
  special down = { .xpos = 83, .ypos = 323, .character = MY_DOWNWARD, .func = KEY_DOWN_ARROW};
  specialChars1[3] = down;
  special fwd = { .xpos = 112, .ypos = 323, .character = MY_FORWARD, .func = KEY_RIGHT_ARROW};
  specialChars1[4] = fwd;
  special circle = { .xpos = 137, .ypos = 323, .character = MY_CIRCLE, .func = KEY_BACKSPACE};
  specialChars1[5] = circle; 
  special control = { .xpos = 85, .ypos = 22, .character = MY_CONTROL, .func = KEY_LEFT_CTRL};
  specialChars2[0] = control;
  special caps = { .xpos = 109, .ypos = 22, .character = MY_CAPS, .func = KEY_CAPS_LOCK};
  specialChars2[1] = caps;
  special command = { .xpos = 137, .ypos = 22, .character = MY_COMMAND};
  specialChars2[2] = command;
  special escape = { .xpos = 86, .ypos = 323, .character = MY_ESCAPE, .func = KEY_ESC};
  specialChars2[3] = escape;
  special shift = { .xpos = 110, .ypos = 323, .character = MY_SHIFT, .func = KEY_RIGHT_SHIFT};
  specialChars2[4] = shift;
  special alt = { .xpos = 139, .ypos = 323, .character = MY_ALT, .func = KEY_RIGHT_ALT};
  specialChars2[5] = alt;
  special del = { .xpos = 85, .ypos = 20, .character = MY_DELETE, .func = KEY_DELETE};
  specialChars3[0] = del;
  special ret = { .xpos = 109, .ypos = 22, .character = MY_RETURN, .func = KEY_RETURN};
  specialChars3[1] = ret;
  special space = { .xpos = 137, .ypos = 12, .character = MY_SPACE_BAR, .func = ' '};
  specialChars3[2] = space;
  special pageup = { .xpos = 85, .ypos = 320, .character = MY_PAGE_UP, .func = KEY_PAGE_UP};
  specialChars3[3] = pageup;
  special tab = { .xpos = 110, .ypos = 323, .character = MY_TAB, .func = KEY_TAB};
  specialChars3[4] = tab;
  special pagedown = { .xpos = 135, .ypos = 318, .character = MY_PAGE_DOWN, .func = KEY_PAGE_DOWN};
  specialChars3[5] = pagedown;
}

// Draw special characters within layout on display
void dispSpecialCharacter(special *hold){
  for(int i =0; i<6; i++){
    drawSymbol(hold[i].xpos, hold[i].ypos, hold[i].character, ILI9341_WHITE, 0,1);
  }
}

// Print characters within layout on display
void dispCharacter(String *inp){
  tft.setTextColor(ILI9341_WHITE);
  tft.setFont(NULL);
  tft.setTextSize(2);
  
  //1
  tft.setCursor(2,2);         
  //tft.print(("a"));
  tft.print(inp[0]);
  tft.setCursor(((tft.width()/3)-10)/2 ,2);  
  //tft.print("s");
  tft.print(inp[3]);
  tft.setCursor((tft.width()/3)-10 ,2);  
  //tft.print("k");
  tft.print(inp[4]);

  //2
  //tft.setCursor((tft.width()/3)+10 ,2); 
  //tft.print("u");
  //tft.setCursor(((tft.width()/3)+10)+(((((tft.width()/3)*2)-18)-((tft.width()/3)+10))/2) ,2);
  //tft.print("u");
  //tft.setCursor(((tft.width()/3)*2)-18 ,2);
  //tft.print("u");

  //3
  tft.setCursor(((tft.width()/3)*2)+3 ,2);  
  //tft.print("p");
  tft.print(inp[8]);
  tft.setCursor((((tft.width()/3)*2)+4)+(((tft.width()-12)-(((tft.width()/3)*2)+4))/2) ,2);
  //tft.print("f");
  tft.print(inp[9]);
  tft.setCursor((tft.width()-12) ,2);
  //tft.print("n");
  tft.print(inp[10]);

  //4
  tft.setCursor(tft.width()-12, (2+(((tft.height()/3)-15)-2)/2));  
  //tft.print("i");
  tft.print(inp[11]);
  tft.setCursor(tft.width()-12, (tft.height()/3)-15);
  //tft.print("x");
  tft.print(inp[12]);

  //5
  tft.setCursor(tft.width()-12, (tft.height()/3)+10);  
  //tft.print("u");
  tft.print(inp[16]);
  tft.setCursor(tft.width()-12, ((tft.height()/3)+10)+((((tft.height()/3)*2)-25)-((tft.height()/3)+10))/2);
  //tft.print("t");
  tft.print(inp[17]);
  tft.setCursor(tft.width()-12, ((tft.height()/3)*2)-25);
  //tft.print("y");
  tft.print(inp[18]);

  //6
  tft.setCursor(tft.width()-12, ((tft.height()/3)*2)+1);  
  //tft.print("j");
  tft.print(inp[27]);
  tft.setCursor(tft.width()-12, (((tft.height()/3)*2)+1)+(((tft.height()-18)-(((tft.height()/3)*2)+1))/2));
  //tft.print("r");
  tft.print(inp[28]);

  //7
  tft.setCursor(tft.width()-12, tft.height()-18);
  //tft.print("i");
  tft.print(inp[29]);
  tft.setCursor((((tft.width()/3)*2)+4)+(((tft.width()-8)-(((tft.width()/3)*2)+4))/2) ,tft.height()-18);  
  //tft.print("d");
  tft.print(inp[30]);
  tft.setCursor(((tft.width()/3)*2)+3 ,tft.height()-18);
  //tft.print("b");
  tft.print(inp[31]);

  //8
  //tft.setCursor(((tft.width()/3)*2)-18 ,tft.height()-18);
  //tft.print("u");
  //tft.setCursor(((tft.width()/3)+10)+(((((tft.width()/3)*2)-18)-((tft.width()/3)+10))/2) ,tft.height()-18);
  //tft.print("u");
  //tft.setCursor((tft.width()/3)+10 ,tft.height()-18);  
  //tft.print("u");

  //9
  tft.setCursor((tft.width()/3)-10 ,tft.height()-18);  
  //tft.print("z");
  tft.print(inp[23]);
  tft.setCursor(((tft.width()/3)-10)/2 ,tft.height()-18);  
  //tft.print("g");
  tft.print(inp[22]);
  tft.setCursor(2 ,tft.height()-18);         
  //tft.print(("o"));
  tft.print(inp[21]);

  //10
  tft.setCursor(2, (((tft.height()/3)*2)+1)+(((tft.height()-18)-(((tft.height()/3)*2)+1))/2));
  //tft.print("w");
  tft.print(inp[20]);
  tft.setCursor(2, ((tft.height()/3)*2)+1);  
  //tft.print("v");
  tft.print(inp[19]);

  //11
  tft.setCursor(2, ((tft.height()/3)*2)-25);
  //tft.print("c");
  tft.print(inp[15]);
  tft.setCursor(2, ((tft.height()/3)+10)+((((tft.height()/3)*2)-25)-((tft.height()/3)+10))/2);
  //tft.print("e");
  tft.print(inp[14]);
  tft.setCursor(2, (tft.height()/3)+10);  
  //tft.print("h");
  tft.print(inp[13]);
            
  //12
  tft.setCursor(2, (tft.height()/3)-15);
  //tft.print("m");
  tft.print(inp[1]);
  tft.setCursor(2, (2+(((tft.height()/3)-15)-2)/2));
  //tft.print("q");
  tft.print(inp[2]);

}

// Display joystick movement
void trackMove(String direction){
  if(track){
    if (direction == "UPLEFT"){
    tft.fillTriangle(70,84,48,101,80,115,ILI9341_WHITE);}     //UPPERLEFT
    if (direction == "LOWRIGHT"){
    tft.fillTriangle(170,234,192,217,160,203,ILI9341_WHITE);} //LOWERRIGHT
    if (direction == "UPRIGHT"){
    tft.fillTriangle(170,84,192,101,160,115,ILI9341_WHITE);}  //UPPERRIGHT
    if (direction == "LOWLEFT"){
    tft.fillTriangle(70,234,48,217,80,203,ILI9341_WHITE);}    //LOWERLEFT
    if (direction == "LEFT"){
    tft.fillTriangle(30,170,30,150,50,160,ILI9341_WHITE);}    //LEFT
    if (direction == "RIGHT"){
    tft.fillTriangle(210,170,210,150,190,160,ILI9341_WHITE);} //RIGHT
    if (direction == "UP"){
    tft.fillTriangle(110,60,130,60,120,80,ILI9341_WHITE);}    //UP
    if (direction == "DOWN"){
    tft.fillTriangle(110,258,130,258,120,238,ILI9341_WHITE);} //DOWN
  }
}

// Erase displayed joystick movement
void resetDir(){
  if(track){
  tft.fillTriangle(70,84,48,101,80,115,ILI9341_BLACK);      //UPPERLEFT
  tft.fillTriangle(170,234,192,217,160,203,ILI9341_BLACK);  //LOWERRIGHT
  tft.fillTriangle(170,84,192,101,160,115,ILI9341_BLACK);   //UPPERRIGHT
  tft.fillTriangle(70,234,48,217,80,203,ILI9341_BLACK);     //LOWERLEFT
  tft.fillTriangle(30,170,30,150,50,160,ILI9341_BLACK);     //LEFT
  tft.fillTriangle(210,170,210,150,190,160,ILI9341_BLACK);  //RIGHT
  tft.fillTriangle(110,60,130,60,120,80,ILI9341_BLACK);     //UP
  tft.fillTriangle(110,258,130,258,120,238,ILI9341_BLACK);  //DOWN
  }
}

// Erase characters displayed from layout 
void resetLay(){
  tft.fillRect(1, 2, 80, 104, ILI9341_BLACK);
  tft.fillRect((tft.width()/3)+8 ,2, 66, 18, ILI9341_BLACK);
  tft.fillRect(((tft.width()/3)*2)+2 ,2, 77, 104, ILI9341_BLACK);
  tft.fillRect(tft.width()-12, (tft.height()/3)+10, 11, 88, ILI9341_BLACK);
  tft.fillRect(((tft.width()/3)*2)+2, ((tft.height()/3)*2)+1, 77, 104, ILI9341_BLACK);
  tft.fillRect((tft.width()/3)+8 ,tft.height()-20, 65, 18, ILI9341_BLACK);
  tft.fillRect(1 ,((tft.height()/3)*2)+1, 80, 106, ILI9341_BLACK);
  tft.fillRect(1, (tft.height()/3)+10, 11, 88, ILI9341_BLACK);
}

// Draw lines on the layout 
void intlay(){

  tft.fillScreen(ILI9341_BLACK);
  yield();

  tft.drawRect(0, 0, tft.width(), tft.height(), ILI9341_WHITE);

  tft.drawLine(tft.width()/3, 0, (tft.width()/3)+((tft.width()/2)/4), (tft.height()/3)-5, ILI9341_WHITE);
   

  delay(1);

  tft.drawLine((tft.width()/3)*2, 0, (tft.width()-(tft.width()/3))-((tft.width()/2)/4), (tft.height()/3)-5, ILI9341_WHITE);
   

  delay(1);

  tft.drawLine(tft.width(), tft.height()/3, ((tft.width()/3)*2)+20, (tft.height()/3)+((tft.height()/2)/4), ILI9341_WHITE);
    

  delay(1);

  tft.drawLine(tft.width(), (tft.height()/3)*2, ((tft.width()/3)*2)+20, (tft.height()-(tft.height()/3))-((tft.height()/2)/4), ILI9341_WHITE);
    
  
  delay(1);

  tft.drawLine((tft.width()/3)*2, tft.height(), (tft.width()-(tft.width()/3))-((tft.width()/2)/4), ((tft.height()/3)*2)+5, ILI9341_WHITE);
    

  delay(1);

  tft.drawLine((tft.width()/3), tft.height(), (tft.width()/3)+((tft.width()/2)/4), ((tft.height()/3)*2)+5, ILI9341_WHITE);
  

  delay(1);

  tft.drawLine(0, (tft.height()/3)*2, (tft.width()/3)-20, (tft.height()-(tft.height()/3))-((tft.height()/2)/4), ILI9341_WHITE);
  

  delay(1);

  tft.drawLine(0, tft.height()/3, (tft.width()/3)-20, (tft.height()/3)+((tft.height()/2)/4), ILI9341_WHITE);
    

  delay(1);

}

//Finds character in layouts - keeps track of postion in layout array and character
String findChar(int in, String *arr ,special *hold) 
{
  // switch on character
  switch (in) {
  case 10:
	  charPosition = 0;
    return arr[0];
  case 14:
    charPosition = 1;
    return arr[1];
  case 17:
    charPosition = 2;
    return arr[2];
  case 12:
    charPosition = 3;
    return arr[3];
  case 13:
    charPosition = 4;
    return arr[4];
  case 21:
    charPosition = 0;
    Keyboard.write(hold[0].func);
    return"";
    //return arr[5];
  case 20:
    charPosition = 1;
    Keyboard.write(hold[1].func);
    return"";
    //return arr[6];
  case 23:
    charPosition = 2;
    Keyboard.write(hold[2].func);
    return"";
    //return arr[7];
  case 31:
    charPosition = 8;
    return arr[8];
  case 32:
    charPosition = 9;
    return arr[9];
  case 30:
    charPosition = 10;
    return arr[10];
  case 36:
    charPosition = 11;
    return arr[11];
  case 39:
    charPosition = 12;
    return arr[12];
  case 41:
    charPosition = 13;
    return arr[13];
  case 40:
    charPosition = 14;
    return arr[14];
  case 47:
    charPosition = 15;
    return arr[15];
  case 63:
    charPosition = 16;
    return arr[16];
  case 60:
    charPosition = 17;
    return arr[17];
  case 69:
    charPosition = 18;
    return arr[18];
  case 71:
    charPosition = 19;
    return arr[19];
  case 74:
    charPosition = 20;
    return arr[20];
  case 70:
    charPosition = 21;
    return arr[21];
  case 78:
    charPosition = 22;
    return arr[22];
  case 79:
    charPosition = 23;
    return arr[23];
  case 87:
    charPosition = 3;
    Keyboard.write(hold[3].func);
    return"";
    //return arr[24];
  case 80:
    charPosition = 4;
    Keyboard.write(hold[4].func);
    return"";
    //return arr[25];
  case 89:
    charPosition = 5;
    Keyboard.write(hold[5].func);
    return"";
    //return arr[26];
  case 93:
    charPosition = 27;
    return arr[27];
  case 96:
    charPosition = 28;
    return arr[28];
  case 90:
    charPosition = 29;
    return arr[29];
  case 98:
    charPosition = 30;
    return arr[30];
  case 97:
    charPosition = 31;
    return arr[31];
  default:
	//Serial.println("error unknown input");
	return "";
  }
}

//switches functionality
void swfunc(){
    if (currentFunc == "keyboard") {
      currentFunc = "mouse";
      board_select = false;
      mouse_select = true;
    }
    else {
      currentFunc = "keyboard";
      board_select = true;
      mouse_select = false; 
    }
    Serial.println(currentFunc); 
}

//switches the layout 
void swlay(){
    if (layNum2 == (sizeof(speclaycontainer)/sizeof(speclaycontainer[0]))) {
      layNum2 = 1;
      currentspecLay = speclaycontainer[0];
    }
    else {
      layNum2++;
      currentspecLay = speclaycontainer[layNum2-1];
    }
    Serial.println(layNum2);

    if (layNum == (sizeof(laycontainer)/sizeof(laycontainer[0]))) {
      layNum = 1;
      currentLay = laycontainer[0];
      resetLay();
      dispCharacter(currentLay);
      dispSpecialCharacter(currentspecLay);
    }
    else {
      layNum++;
      currentLay = laycontainer[layNum-1];
      resetLay();
      dispCharacter(currentLay);
      dispSpecialCharacter(currentspecLay);
    }
    Serial.println(layNum);
}

// keeps track of first zone entered and last zone left
int movement(){
  if (VRx != xState || VRy != yState) {
    if (VRx == 5 && VRy == -5) {              //zone 1 (KEY_UPPPERLEFT_ARROW)
      trackMove("UPLEFT");
      xState = VRx;
      yState = VRy;
      if(enter == 0){
        enter = 1;
      }
      else{ 
        leave = 1;
      }
    }
    else if (VRx == 5 && VRy == 0) {          //zone 2 (KEY_UP_ARROW)
      trackMove("UP");
      xState = VRx;
      yState = VRy;
      if(enter == 0){
        enter = 2;
      }
      else{ 
        leave = 2;
      }
    }
    else if (VRx == 5 && VRy == 5) {          //zone 3 (KEY_UPPERRIGHT_ARROW) 
      trackMove("UPRIGHT");
      xState = VRx;
      yState = VRy;
      if(enter == 0){
        enter = 3;
      }
      else{ 
        leave = 3;
      }
    }
    else if (VRx == 0 && VRy == -5) {         //zone 4 (KEY_LEFT_ARROW)
      trackMove("LEFT");
      xState = VRx;
      yState = VRy;
      if(enter == 0){
        enter = 4;
      }
      else{ 
        leave = 4;
      }
    }
    else if (VRx == 0 && VRy == 0) {          //zone 5 (neutral state)
      xState = 0;
      yState = 0;
      resetDir();
      enter = enter*10;
      int hold = enter+leave;
      enter = 0;
      leave = 0;
      return hold;
    }
    else if (VRx == 0 && VRy == 5) {          //zone 6 (KEY_RIGHT_ARROW)
      trackMove("RIGHT");
      xState = VRx;
      yState = VRy;
      if(enter == 0){
        enter = 6;
      }
      else{ 
        leave = 6;
      }
    }
    else if (VRx == -5 && VRy == -5) {        //zone 7 (KEY_LOWERLEFT_ARROW)
      trackMove("LOWLEFT");
      xState = VRx;
      yState = VRy;
      if(enter == 0){
        enter = 7;
      }
      else{ 
        leave = 7;
      }
    }
    else if (VRx == -5 && VRy == 0) {         //zone 8 (KEY_DOWN_ARROW)
      trackMove("DOWN");
      xState = VRx;
      yState = VRy;
      if(enter == 0){
        enter = 8;
      }
      else{ 
        leave = 8;
      }
    }
    else if (VRx == -5 && VRy == 5) {         //zone 9 (KEY_LOWERRIGHT_ARROW)
      trackMove("LOWRIGHT");
      xState = VRx;
      yState = VRy;
      if(enter == 0){
        enter = 9;
      }
      else{ 
        leave = 9;
      }
    }
  }
  return -1;
}

//Prompt to traverse through layouts
void msg1(){
  tft.setTextColor(ILI9341_WHITE);  
  tft.setTextSize(1);
  tft.setCursor(((tft.width()/3)-10)/2 ,(tft.height()/3)+10);
  tft.println("> Push analog left or right");
  tft.setCursor((((tft.width()/3)-10)/2)+12 ,(tft.height()/3)+10+10);
  tft.print("to traversse layouts");
  tft.setCursor(((tft.width()/3)-10)/2, ((tft.height()/3)+10)+((((tft.height()/3)*2)-25)-((tft.height()/3)+10))/2);
  tft.println("> Push analog down");
  tft.setCursor((((tft.width()/3)-10)/2)+12, (((tft.height()/3)+10)+((((tft.height()/3)*2)-25)-((tft.height()/3)+10))/2)+10);
  tft.print("to select layout");
}
//Prompt to select the character
void msg2(){
  tft.setTextColor(ILI9341_WHITE);  
  tft.setTextSize(1);
  tft.setCursor(((tft.width()/3)-10)/2 ,(tft.height()/3)+10);
  tft.println("> Select the character by");
  tft.setCursor((((tft.width()/3)-10)/2)+12 ,(tft.height()/3)+10+10);
  tft.print("doing the sequence");
  tft.setCursor(((tft.width()/3)-10)/2, ((tft.height()/3)+10)+((((tft.height()/3)*2)-25)-((tft.height()/3)+10))/2);
  tft.println("> After sequence push");
  tft.setCursor((((tft.width()/3)-10)/2)+12, (((tft.height()/3)+10)+((((tft.height()/3)*2)-25)-((tft.height()/3)+10))/2)+10);
  tft.print("analog down to proceed");
}

// Clear display area for prompts when switching characters bewteen layouts  
void resetMsg(){
  tft.fillRect(((tft.width()/3)-10)/2,(tft.height()/3)+10,175,60,ILI9341_BLACK);
}

void readJoystick(){ 
  int setx = analogRead(VRxPIN);
  int sety = analogRead(VRyPIN);
  if(setx > (JoyX*2)){
    VRx = map(JoyX*2, 0, JoyX*2, -5, 5);
  }
  else{
    VRx = map(setx, 0, JoyX*2, -5, 5);
  }
  if(sety > (JoyY*2)){
    VRy = map(JoyY*2, 0, JoyY*2, -5, 5);
  }
  else{
    VRy = map(sety, 0, JoyY*2, -5, 5);
  }
}

//switches the characters between layouts
void swChar(){
    String firstChar;   //stores first selected character
    String secondChar;  //stores second selected character
    String *firstPos;   //stores layout of first selected character
    String *secondPos;  //stores layout of second selected character
    int position1;      //stores position in selected layout of the first seleced character 
    int position2;      //stores position in selected layout of the second seleced character 
    int num;
    int numberoflayouts = (sizeof(laycontainer)/sizeof(laycontainer[0]));
    stoploop = true;  //pause main loop in loop()
    track = false;    //pause showing jostick movement on display
    boolean done = false;
    tft.fillScreen(ILI9341_BLACK);
    
    // Select the first layout
    num = 1;
    firstPos = laycontainer[0]; 
    dispCharacter(firstPos); // Print first layout on display
    msg1();
    while (!done)
    {
      buttonPIN.loop(); 
      readJoystick();
      int move;
      move = movement();
      if(move == 40 || move == 60){
        if (num == numberoflayouts) {
          num = 1;
          firstPos = laycontainer[0];
          resetLay();
          dispCharacter(firstPos);
        }
        else {
          num++;
          firstPos = laycontainer[num-1];
          resetLay();
          dispCharacter(firstPos);
        }
      }
      if(buttonPIN.isReleased()){ // Select layout
        done = true;
      }
    }
    
    // Select the first character
    resetMsg();
    msg2();
    while (done)
    {
      buttonPIN.loop(); 
      readJoystick();
      int move;
      move = movement();
      if(move != -1){
        Serial.println("key entered = " + findChar(move, firstPos, NULL));
        position1 = charPosition;
        firstChar = firstPos[position1];
      }
      if(buttonPIN.isReleased()){ // Select character
        done = false;
      }
    }

    // Select the second layout
    num = 1;
    secondPos = laycontainer[0]; 
    resetLay();
    dispCharacter(secondPos); // Print first layout on display
    resetMsg();
    msg1();
    while (!done)
    {
      buttonPIN.loop(); 
      readJoystick();
      int move;
      move = movement();
      if(move == 40 || move == 60){
        if (num == numberoflayouts) {
          num = 1;
          secondPos = laycontainer[0];
          resetLay();
          dispCharacter(secondPos);
        }
        else {
          num++;
          secondPos = laycontainer[num-1];
          resetLay();
          dispCharacter(secondPos);
        }
      }
      if(buttonPIN.isReleased()){ // Select layout
        done = true;
      }
    }

    // Select the second character
    resetMsg();
    msg2();
    while (done)
    {
      buttonPIN.loop(); 
      readJoystick();
      int move;
      move = movement();
      if(move != -1){
        position2 = charPosition;
        secondChar = secondPos[position2];
      }
      if(buttonPIN.isReleased()){ // Select character
        done = false;
      }
    }

    // Switch the characters and display the layout
    secondPos[position2] = firstChar;
    firstPos[position1] = secondChar;
    resetLay();
    intlay();
    dispCharacter(currentLay);
    dispSpecialCharacter(currentspecLay);

  track = true; //resume showing jostick movement on display
  stoploop = false; //resume main loop in loop()
}

int set_combination(){
  stoploop = true;
  combine2[0] = 0x00;
  combine2[1] = 0x00;
  combine2[2] = 0x00;
  combine2[3] = 0x00;
  int i = 0;
  int set = 0;

  while(set == 0){
    button_3.loop();      
    button_1.loop(); 

    if(button_3.isReleased()){ //Button press to end set a combination 
      set = 1;
      stoploop = false;
    }

    if(button_1.isReleased()){
      swlay();
    }

    readJoystick();
    int move;
    move = movement();
    if(move != -1){
      if(i <= 3){
        String held = findChar(move, currentLay, currentspecLay);
        if(held != ""){
          combine[i] = currentLay[charPosition];
          i++;
        }
        else{
          combine2[i] = currentspecLay[charPosition].func;
          i++;
        }
      }
      else{
        i=0;
        String held = findChar(move, currentLay, currentspecLay);
        if(held != ""){
          combine[i] = currentLay[charPosition];
          i++;
        }
        else{
          combine2[i] = currentspecLay[charPosition].func;
          i++;
        }
        i++;
      }
    }
  }
  return 0;
}

void write_combination(){
  Keyboard.press(combine2[0]);
  Keyboard.press(combine2[1]);
  Keyboard.press(combine2[2]);
  Keyboard.press(combine2[3]);
  Keyboard.releaseAll();
}

void setup() {

  button_3.setDebounceTime(50);         // set debounce time to 50 milliseconds
  button_2.setDebounceTime(50);  // set debounce time to 50 milliseconds
  button_1.setDebounceTime(50);    // set debounce time to 50 milliseconds
  buttonPIN.setDebounceTime(50);      // set debounce time to 50 milliseconds

  Serial.begin(115200);

  specialCharacterSetup();

  laycontainer[0] = &lay1[0];     //add pointers for each layout to array
  laycontainer[1] = &lay2[0];
  laycontainer[2] = &lay3[0];
  laycontainer[3] = &lay1cap[0];
  currentLay = laycontainer[0];   //set layout 1 as default layout
  layNum = 1;                     //set layout number as 1

  speclaycontainer[0] = &specialChars1[0];
  speclaycontainer[1] = &specialChars2[0];
  speclaycontainer[2] = &specialChars3[0];
  currentspecLay = speclaycontainer[0];
  layNum2 = 1;

  pinMode(VRxPIN, INPUT);
  pinMode(VRyPIN, INPUT);

  JoyX = analogRead(VRxPIN);  // Set stick drift in x 
  JoyY = analogRead(VRyPIN);  // Set stick drift in y

  Serial.println("Starting BLE work!");
  Keyboard.begin();
  Mouse.begin();

  tft.begin();
     
  intlay();
  dispCharacter(currentLay);
  dispSpecialCharacter(currentspecLay);

  board_select = true;   // Select keyboard as default input
}

void loop()
{
  if(Keyboard.isConnected()) {
    if(!stoploop){
      
      buttonPIN.loop(); 
      if(buttonPIN.isReleased()){  // Button press to switch functionality
        swfunc();
      }

      if(mouse_select == true){

        button_2.loop(); 
        button_1.loop(); 
        
        if(button_1.isReleased()){ // Button press right click
          Mouse.click(MOUSE_RIGHT);
        }

        if(button_2.isReleased()){ // Button press left click
          Mouse.click(MOUSE_LEFT);
        }

        currentMillis = millis();  //get the current time
        if (currentMillis - startMillis >= period)  //test whether the period has elapsed
        {
          int xValue = analogRead(VRxPIN);       
          int yValue = analogRead(VRyPIN);
          // xValue = xValue - JoyX;                               // Remove Stick drift on X position
          // yValue = yValue - JoyY;                               // Remove Stick drift on X position
          int mouseX = map( xValue,0, JoyX*2, -25, 25);       // map the X value to a range of movement for the mouse X, Changes input range to include new values
          int mouseY = map( yValue,0, JoyY*2, -25, 25);
          Mouse.move(mouseY, -mouseX, 0);
          startMillis = currentMillis;  //IMPORTANT to save the start time
        }
      }

      if(board_select == true){
        button_3.loop();        
        button_2.loop(); 
        button_1.loop();   

        if(button_3.isPressed())
          pressedTime = millis();

        if(button_3.isReleased()) {
          releasedTime = millis();

          long pressDuration = releasedTime - pressedTime;
          //short press to execute combo
          if( pressDuration < SHORT_PRESS_TIME ){
            //Serial.println("Write");
            write_combination();
          }
          //add combo with long press
          if( pressDuration > LONG_PRESS_TIME ){
            //Serial.println("Set");
            set_combination();
          }
        }

        if(button_1.isReleased()){ // Button press switch layout
          swlay(); 
        }
        
        if(button_2.isReleased()){ // Button press initiate switch character between layout
          swChar();
        }

        readJoystick();
        move = movement();
        if(move != -1){
          String held = findChar(move, currentLay, currentspecLay);
          if(held != ""){
            //printf("key entered = %s\n",held);
            Keyboard.print(held);
          }
        }
        move = -1;
      }
    }
  }
}