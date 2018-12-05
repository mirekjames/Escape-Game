//escapeGame
//Escape-the-room game for Arduboy!
//by Mirek Stolee
// 12/3/2018

#include <Arduboy2.h>
#include "Classes.h"
#include "escapeGame.h"
#include "images.h"

//add arduboy and ardbitmap
Arduboy2 arduboy;

//text
const char text_0[] PROGMEM = "";
const char text_1[] PROGMEM = "It's a clock.";
const char text_2[] PROGMEM = "Cool, a key.";
const char text_3[] PROGMEM = "Whoa, a safe!";
const char text_4[] PROGMEM = "";
const char* const string_table[] PROGMEM = {text_0, text_1, text_2, text_3, text_4};

//interface
Cursor cursor; //cursor
Arrow *leftArrow = new Arrow(4, 27, 3, 3, triangle, true, 0, left, 0); //right arrow
Arrow *rightArrow = new Arrow(103, 27, 3, 3, triangle, true, 0, right, 0); //left arrow
uint8_t view; //which wall is being looked at
bool zoom; //is zoomed in
char currentText[30]; //stores which text is being shown onscreen
byte inventory = 0;

//front wall
Clickable *clock = new Clickable(30, 30, 6, 6, circle, true, 1, 0); //clock for testing
Item *keyClick = new Item(50, 30, 16, 16, rectangle, true, 2, keyImg, key); //key for testing
Clickable *frontWallItems[2] = {clock, keyClick};  //array for objects on the front wall

//right wall


//back wall
Clickable *safe = new Clickable(45, 38, 10, 10, rectangle, true, 3, 0); //safe for testing
Clickable *backWallItems[1] = {safe};

//left wall



void setup() {
  Serial.begin(9600);
  arduboy.begin();
  arduboy.setFrameRate(30);
  view = frontWall;
  zoom = false;
}

void loop() {
  //handle frames
  if(!arduboy.nextFrame()){
    return;
  }
  arduboy.clear();
  getInput();
  draw();
}

void getInput(){
  arduboy.pollButtons(); //check for buttons

  if(arduboy.justPressed(B_BUTTON)){
    memset(currentText, 0, sizeof(currentText)); //delete old text
    if(view == frontWall){
      for(uint8_t i = 0; i < 2; i++){
        if(frontWallItems[i]->isVisible == true){
          frontWallItems[i]->getInput(&arduboy, &cursor);
        }
      }
    }
    if(view == backWall){
      for(uint8_t i = 0; i < 1; i++){
        if(backWallItems[i]->isVisible == true){
          backWallItems[i]->getInput(&arduboy, &cursor);
        }
      }
    }
    leftArrow->getInput(&arduboy, &cursor);
    rightArrow->getInput(&arduboy, &cursor);
  }

  //cursor
  cursor.getInput(&arduboy);
}

void draw(){
  arduboy.fillRect(0, 0, 108, 54); //draw viewport
  if(!zoom){
    //draw room
    arduboy.drawRect(6, 6, 96, 42, BLACK);
    arduboy.drawLine(0, 0, 6, 6, BLACK);
    arduboy.drawLine(0, 54, 6, 48, BLACK);
    arduboy.drawLine(108, 0, 102, 6, BLACK);
    arduboy.drawLine(108, 54, 102, 48, BLACK); 
    
    if(view == frontWall){
      for(uint8_t i = 0; i < 2; i++){
        if(frontWallItems[i]->isVisible == true){
          frontWallItems[i]->draw(&arduboy);
        }
      }
    }    

    if(view == backWall){
      for(uint8_t i = 0; i < 1; i++){
        if(backWallItems[i]->isVisible == true){
          backWallItems[i]->draw(&arduboy);
        }
      }
    }

    leftArrow->draw(&arduboy);
    rightArrow->draw(&arduboy);
  }

  //print currentText
  arduboy.setCursor(1, 54); 
  arduboy.print(currentText);
  cursor.draw(&arduboy);

  arduboy.display();
}
