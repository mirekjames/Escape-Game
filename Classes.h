#include <Arduboy2.h>
#include <Tinyfont.h>
#include "escapeGame.h"
#include "images.h"

extern const char* const string_table[] PROGMEM;
extern uint8_t view;
extern char currentText[30];
extern byte inventory;

//class for moving the cursor
class Cursor{

  public:
  uint8_t xPos = 64;
  uint8_t yPos = 32;
  
  const int cursorSpeed = 2;
  
  void getInput(Arduboy2 *myArduboy){
    if(myArduboy->pressed(LEFT_BUTTON) && xPos > 1){
      xPos -= cursorSpeed;
      if(xPos < 1){
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

//for clickable scene items
class Clickable{

  public:
  uint8_t xPos;
  uint8_t yPos;
  uint8_t width;
  uint8_t height;
  uint8_t shape;
  bool isVisible;
  uint8_t textNumber;
  uint8_t *image;

  Clickable(uint8_t tempX, uint8_t tempY, uint8_t tempWidth, uint8_t tempHeight, uint8_t tempShape, bool tempVis, uint8_t tempText, uint8_t *tempImage){
    xPos = tempX;
    yPos = tempY;
    width = tempWidth;
    height = tempHeight;
    shape = tempShape;
    isVisible = tempVis;
    textNumber = tempText;
    image = tempImage;
  }

  void getInput(Arduboy2 *myArduboy, Cursor *cursor){
    //check to see if user clicks
    
    //measures from top-left corner
    if(shape == rectangle){
      if(cursor->xPos > xPos &&
      cursor->xPos <= xPos + width &&
      cursor->yPos > yPos &&
      cursor->yPos <= yPos + height){
        clicked();
      }
    }
    //measures from center
    if(shape == circle){
      if(cursor->xPos < yPos + width &&
      cursor->xPos > xPos - width &&
      cursor->yPos < yPos + width &&
      cursor->yPos > yPos - width){
        clicked();
      }
    }
  }

  void draw(Arduboy2 *myArduboy){
    if(image == 0){
      if(shape == circle){
        myArduboy->drawCircle(xPos, yPos, width, BLACK);
      } else if (shape == rectangle){
        myArduboy->drawRect(xPos, yPos, width, height, BLACK);
      }
    } else {
      myArduboy->drawBitmap(xPos, yPos, image, 16, 16, BLACK); 
    }
  }

  void clicked(){
    strcpy_P(currentText, (char*)pgm_read_word(&(string_table[textNumber])));
    afterClick();
  }

  virtual void afterClick(){
    return;
  }

  
};

//class for navigation arrows extends clickable
class Arrow: public Clickable{

  //direction the arrow is pointing
  uint8_t pointing;
  
  public:
  
  //constructor
  Arrow(uint8_t tempX, uint8_t tempY, uint8_t tempWidth, uint8_t tempHeight, uint8_t tempShape, bool tempVis, uint8_t tempText, uint8_t tempPointing, uint8_t tempImage) 
  : Clickable (tempX, tempY, tempWidth, tempHeight, tempShape, tempVis, tempText, tempImage){
    pointing = tempPointing;
  }
  
  void getInput(Arduboy2 *myArduboy, Cursor *cursor){
    //check if clicked
    if(pointing == left){
      if(cursor->xPos < xPos &&
      cursor->yPos < yPos + height &&
      cursor->yPos > yPos - height){
        clicked();
      }
    }
    if(pointing == right){
      if(cursor->xPos > xPos &&
      cursor->yPos < yPos + height &&
      cursor->yPos > yPos - height){
        clicked();
      }
    }
  }
    
  void draw(Arduboy2 *myArduboy){
    if(pointing == left){
      myArduboy->fillTriangle(xPos, yPos + height, xPos - width, yPos, xPos, yPos - height, BLACK);
    }
    if(pointing == right){
      myArduboy->fillTriangle(xPos, yPos + height, xPos + width, yPos, xPos, yPos - height, BLACK);
    }
    if(pointing == up){
      return;
    }
    if(pointing == down){
      return;
    }
  }

  void clicked(){
    if(pointing == left){
      if(view == frontWall){
        view = leftWall;
      } else {
        view--;
      }
    }
    if(pointing == right){
      if(view == leftWall){
        view = frontWall;
      } else {
        view++;
      }
    }
  }
};


class Item : public Clickable{
  
  byte mask;
  
  public:
  Item(uint8_t tempX, uint8_t tempY, uint8_t tempWidth, uint8_t tempHeight, uint8_t tempShape, bool tempVis, uint8_t tempText, uint8_t *tempImage, byte tempName)
  : Clickable(tempX, tempY, tempWidth, tempHeight, tempShape, tempVis, tempText, tempImage){
    if (tempName == key){
      mask = 1;
    } else if (tempName == piranha){
      mask = 2;
    }
  }
  
  void afterClick(){
    isVisible = false;
    inventory += mask;
  }
};
