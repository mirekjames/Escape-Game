#include <Arduboy2.h>
#include <Tinyfont.h>
#include "escapeGame.h"
#include "images.h"

extern const char* const string_table[] PROGMEM;
extern uint8_t view;
extern char currentText[20];
extern byte inventory;
extern bool pupFed;
extern bool doorOneOpened;
extern bool checkInv(uint8_t needed);
extern bool gameWon;
extern int getInvSlot(uint8_t slot);
extern void checkCombo();
extern uint8_t zoom;

//class for moving the cursor
class Cursor{

  public:
  uint8_t xPos = 54;
  uint8_t yPos = 27;
  
  const int cursorSpeed = 2;
  
  void getInput(Arduboy2 *myArduboy){
    if(myArduboy->pressed(LEFT_BUTTON) && xPos > 1){
      xPos -= cursorSpeed;
      if(xPos <= 1){
        xPos = 1;
      }
    }
    if(myArduboy->pressed(RIGHT_BUTTON) && xPos < 106){
      xPos += cursorSpeed;
      if(xPos > 106){
        xPos = 106;
      }
    }
    if(myArduboy->pressed(UP_BUTTON) && yPos > 1){
      yPos -= cursorSpeed;
      if(yPos < 1){
        yPos = 1;
      }
    }
    if(myArduboy->pressed(DOWN_BUTTON) && yPos < 52){
      yPos += cursorSpeed;
      if(yPos > 52){
        yPos = 52;
      }
    }
  }

  void draw(Arduboy2 *myArduboy){
    myArduboy->fillCircle(xPos, yPos, 1, BLACK);
  }
};

/*for clickable scene items
  Parameters:
    xPos
    yPos
    Width
    Height
    Shape
    isVisible
    textNumber
    usedText
    image
    toZoom
    itemNeeded
    itemGiven

  Subclass: Arrow
*/
class Clickable{

  public:
  uint8_t xPos;
  uint8_t yPos;
  uint8_t width;
  uint8_t height;
  uint8_t shape;
  bool isVisible;
  uint8_t textNumber;
  uint8_t usedText;
  uint8_t *image;
  uint8_t itemNeeded;
  uint8_t itemGiven;
  uint8_t toZoom;
  bool isUsed;
  
  Clickable(uint8_t tempX, uint8_t tempY, uint8_t tempWidth, uint8_t tempHeight, uint8_t tempShape, bool tempVis, uint8_t tempText, uint8_t tempUsed, const uint8_t *tempImage, uint8_t tempZoom, uint8_t tempNeeded, uint8_t tempGiven){
    xPos = tempX;
    yPos = tempY;
    width = tempWidth;
    height = tempHeight;
    shape = tempShape;
    isVisible = tempVis;
    textNumber = tempText;
    image = tempImage;
    itemNeeded = tempNeeded;
    itemGiven = tempGiven;
    usedText = tempUsed;
    toZoom = tempZoom;
    isUsed = false;
  }

  void getInput(Arduboy2 *myArduboy, Cursor *cursor){
    //check to see if user clicks
    
    //measures from top-LEFT corner
    if(shape == RECTANGLE){
      if(cursor->xPos > xPos &&
      cursor->xPos <= xPos + width &&
      cursor->yPos > yPos &&
      cursor->yPos <= yPos + height){
        clicked();
      }
    }
    //measures from center
    if(shape == CIRCLE){
      if(cursor->xPos < xPos + width &&
      cursor->xPos > xPos - width &&
      cursor->yPos < yPos + width &&
      cursor->yPos > yPos - width){
        clicked();
      }
    }
    
    if(shape == TRIANGLE){
      if(cursor->xPos < xPos + width &&
      cursor->xPos > xPos - width &&
      cursor->yPos > yPos + height){
        clicked();
      }
    }
  }
  
  void draw(Arduboy2 *myArduboy){
    if(image == 0){
      if(shape == CIRCLE){
        myArduboy->drawCircle(xPos, yPos, width, BLACK);
      } else if (shape == RECTANGLE){
        myArduboy->drawRect(xPos, yPos, width, height, BLACK);
      }
    } else {
      myArduboy->drawBitmap(xPos, yPos, image, width, height, BLACK); 
    }
  }
  
  void clicked(){
    if(toZoom != NOT){ //if this is a zoom item
      zoom = toZoom; //zoom in!
      return;
    }
    
    if(!isUsed){
      bool hasItem = checkInv(itemNeeded);
      if(hasItem && itemNeeded != NONE){ //if player needs an item and they are using the item
        strcpy_P(currentText, (char*)pgm_read_word(&(string_table[random(5, 8)]))); //random success text
        if(itemGiven != 0){
          addItem(itemGiven);
        }
        afterUse();
        isUsed = true;
      } else { //player is using wrong item / doesn't need an item
        strcpy_P(currentText, (char*)pgm_read_word(&(string_table[textNumber])));
        if(itemGiven != 0 && itemNeeded == NONE){ //if they can just get the item
          addItem(itemGiven);
          afterUse();
          isUsed = true;
        }
      }
    } else {
      strcpy_P(currentText, (char*)pgm_read_word(&(string_table[usedText])));
    }
  }
  
  void addItem(uint8_t item){
    byte mask;
    if (item == KEY){
      mask = 1 << 0; //1
    } else if (item == PIRANHA){
      mask = 1 << 1; //2
    } else if (item == NET){
      mask = 1 << 2; //4
    } else if (item == KEYTWO){
      mask = 1 << 3; //8
    } else if (item == KEYTHREE){
      mask = 1 << 4; // 16
    } else if (item == SCREWDRIVER){
      mask = 1 << 5; // 32
    }
    inventory += mask;
  }

  void afterUse(){
    if(textNumber == 24){ //paintingZoom
      image = paintingZoomImgTwo;
    }
    if(textNumber == 27 || textNumber == 2 || textNumber == 9){ //keyThree or key or piranha
      isVisible = false;
    }
    if(textNumber == 18){ //pup
      xPos += 16; //move
      pupFed = true;
    }
    if(textNumber == 4){ //door 1
      isVisible = false;
      doorOneOpened = true;
    }
    if(textNumber == 23){ //door 2
      gameWon = true;
    }
  }
};

//class for navigation arrows extends clickable
class Arrow: public Clickable{

  //direction the arrow is pointing
  uint8_t pointing;
  
  public:
  
  //constructor
  Arrow(uint8_t tempX, uint8_t tempY, uint8_t tempWidth, uint8_t tempHeight, uint8_t tempShape, bool tempVis, uint8_t tempText, uint8_t tempUsed, uint8_t tempPointing, const uint8_t *tempImage, uint8_t tempZoom, uint8_t tempNeeded, uint8_t tempGiven) 
  : Clickable (tempX, tempY, tempWidth, tempHeight, tempShape, tempVis, tempText, tempUsed, tempImage, tempZoom, tempNeeded, tempGiven){
    pointing = tempPointing;
  }
  
  void getInput(Arduboy2 *myArduboy, Cursor *cursor){
    //check if clicked
    if(pointing == LEFT){
      if(cursor->xPos < xPos + 2 &&
      cursor->yPos < yPos + height &&
      cursor->yPos > yPos - height){
        clicked();
      }
    }
    if(pointing == RIGHT){
      if(cursor->xPos > xPos - 2 &&
      cursor->yPos < yPos + height &&
      cursor->yPos > yPos - height){
        clicked();
      }
    }
    if (pointing == DOWN){
      if(cursor->xPos < xPos + width &&
      cursor->xPos > xPos - width &&
      cursor->yPos > yPos - 2){
        clicked();
      }
    }
  }
    
  void draw(Arduboy2 *myArduboy){
    if(pointing == LEFT){
      myArduboy->fillTriangle(xPos, yPos + height, xPos - width, yPos, xPos, yPos - height, BLACK);
    }
    if(pointing == RIGHT){
      myArduboy->fillTriangle(xPos, yPos + height, xPos + width, yPos, xPos, yPos - height, BLACK);
    }
    if(pointing == UP){
      return;
    }
    if(pointing == DOWN){
      //black if not looking in the barrel, white if you are
      if(zoom != BARREL){
        myArduboy->fillTriangle(xPos, yPos + height, xPos - width, yPos, xPos + width, yPos, BLACK);
      } else {
        myArduboy->fillTriangle(xPos, yPos + height, xPos - width, yPos, xPos + width, yPos, WHITE);
      }
    }
  }

  void clicked(){
    if(pointing == LEFT){
      if(view == FRONTWALL){
        view = LEFTWALL;
      } else {
        view--;
      }
    }
    if(pointing == RIGHT){
      if(view == LEFTWALL){
        view = FRONTWALL;
      } else {
        view++;
      }
    }
    if(pointing == DOWN){
      zoom = NOT;
    }
  }
};

//for items in inventory
class InventoryItem {
  public:
  uint8_t name;
  uint8_t *image;
  uint8_t yPos;
  uint8_t slot = 0;
  uint8_t textNumber;
  bool isGotten;
  bool isSelected;
  bool isUsed;

  InventoryItem(uint8_t tempName, uint8_t *tempImage, uint8_t tempText){
    name = tempName;
    image = tempImage;
    textNumber = tempText;
    isGotten = false;
    isSelected = false;
    isUsed = false;
  }

  void draw(Arduboy2 *myArduboy){
    if(isSelected){
      myArduboy->fillRect(110, 16 * (slot - 1), 16, 16, WHITE);
      myArduboy->drawBitmap(110, 16 * (slot - 1), image, 16, 16, BLACK);
    } else {
      myArduboy->drawBitmap(110, 16 * (slot - 1), image, 16, 16, WHITE);
    }
  }

  void updateInv(){
    if(name == KEY && (inventory & 1 << 0) == 1 << 0 && !isGotten && !isUsed){
      isGotten = true;
      slot = getInvSlot(slot);
    }
    if(name == PIRANHA && (inventory & 1 << 1) == 1 << 1 && !isGotten && !isUsed){
      isGotten = true;
      slot = getInvSlot(slot);
    }
    if(name == NET && (inventory & 1 << 2) == 1 << 2 && !isGotten && !isUsed){
      isGotten = true;
      slot = getInvSlot(slot);
    }
    if(name == KEYTWO && (inventory & 1 << 3) == 1 << 3 && !isGotten && !isUsed){
      isGotten = true;
      slot = getInvSlot(slot);
    }
    if(name == KEYTHREE && (inventory & 1 << 4) == 1 << 4 && !isGotten && !isUsed){
      isGotten = true;
      slot = getInvSlot(slot);
    }
    if(name == SCREWDRIVER && (inventory & 1 << 5) == 1 << 5 && !isGotten && !isUsed){
      isGotten = true;
      slot = getInvSlot(slot);
    }
  }
};

//for locks on safe
class SafeLock{

  uint8_t xPos;
  uint8_t yPos;
  uint8_t width;
  uint8_t height;
  uint8_t shape;
  
  public:
  uint8_t number;
  uint8_t combo;


  SafeLock(uint8_t tempX, uint8_t tempY, uint8_t tempWidth, uint8_t tempHeight, uint8_t tempShape, uint8_t tempCombo){
      xPos = tempX;
      yPos = tempY;
      width = tempWidth;
      height = tempHeight;
      shape = tempShape;
      combo = tempCombo;
      number = 0;
  }
  
  void getInput(Arduboy2 *myArduboy, Cursor *cursor){
    //check to see if user clicks
    
    //measures from top-LEFT corner
    if(shape == RECTANGLE){
      if(cursor->xPos > xPos &&
      cursor->xPos <= xPos + width &&
      cursor->yPos > yPos &&
      cursor->yPos <= yPos + height){
        increment();
      }
    }
    
    //measures from center
    if(shape == CIRCLE){
      if(cursor->xPos < xPos + width &&
      cursor->xPos > xPos - width &&
      cursor->yPos < yPos + width &&
      cursor->yPos > yPos - width){
        increment();
      }
    }
    
    if(shape == TRIANGLE){
      if(cursor->xPos < xPos + width &&
      cursor->xPos > xPos - width &&
      cursor->yPos > yPos - height &&
      cursor->yPos < yPos){
        increment();
      }
    }
  }

  void draw(Arduboy2 *myArduboy){
    if(shape == TRIANGLE){
      myArduboy->drawTriangle(xPos, yPos - height, xPos - width, yPos, xPos + width, yPos, BLACK);
      myArduboy->setCursor(xPos - 2, yPos - 8); 
    } 
    if (shape == CIRCLE){
      myArduboy->drawCircle(xPos, yPos, width, BLACK);
      myArduboy->setCursor(xPos - 2, yPos - 3); 
    } 
    if (shape == RECTANGLE){
      myArduboy->drawRect(xPos, yPos, width, height, BLACK);
      myArduboy->setCursor(xPos + 5, yPos + 3); 
    }
    myArduboy->setTextColor(BLACK);
    myArduboy->setTextBackground(WHITE);
    myArduboy->print(number);
  }

  void increment(){
    number++;
    number < 10 ? number = number : number = 0;
    checkCombo();
  }
};
