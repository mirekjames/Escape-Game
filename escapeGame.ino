//escapeGame
//Escape-the-room game for Arduboy!
//by Mirek Stolee
// 12/3/2018

/*
 Untitled Escape Game
 Try to get out of the room!
 
 Use d-pad to move cursor.
 Press B to interact with objects.
 Press A to cycle through what item you're using.

 Good luck!
*/

#include <Arduboy2.h>
#include "Classes.h"
#include "escapeGame.h"
#include "images.h"

Arduboy2 arduboy;

//text
const char text_0[] PROGMEM = "";
const char text_1[] PROGMEM = "Just a clock.";
const char text_2[] PROGMEM = "Ooh, a key!";
const char text_3[] PROGMEM = "One weird safe.";
const char text_4[] PROGMEM = "Locked. Of course.";
const char text_5[] PROGMEM = "Success!";
const char text_6[] PROGMEM = "It worked!";
const char text_7[] PROGMEM = "I am the best.";
const char text_8[] PROGMEM = "A piranha tank??";
const char text_9[] PROGMEM = "Ouch! It bit me.";
const char text_10[] PROGMEM = "That didn't work.";
const char text_11[] PROGMEM = "A two-pronged key.";
const char text_12[] PROGMEM = "A piranha!";
const char text_13[] PROGMEM = "A net.";
const char text_14[] PROGMEM = "This one's locked.";
const char text_15[] PROGMEM = "Screwdriver?";
const char text_16[] PROGMEM = "It's empty now.";
const char text_17[] PROGMEM = "Don't need another.";
const char text_18[] PROGMEM = "Pup. Looks hungry.";
const char text_19[] PROGMEM = "Munching away.";
const char text_20[] PROGMEM = "A clue?";
const char text_21[] PROGMEM = "A clue????";
const char text_22[] PROGMEM = "Three-pronged key.";
const char text_23[] PROGMEM = "Seriously...?";
const char text_24[] PROGMEM = "Hmm. Avant garde.";
const char text_25[] PROGMEM = "A screwdriver!";
const char text_26[] PROGMEM = "A barrel o' fun!";
const char text_27[] PROGMEM = "A key!";
const char text_28[] PROGMEM = "Let's see inside!";
const char text_29[] PROGMEM = "The final key!";
const char* const string_table[] PROGMEM = {text_0, text_1, text_2, text_3, text_4, text_5, text_6, text_7, text_8, text_9, text_10, 
                                            text_11, text_12, text_13, text_14, text_15, text_16, text_17, text_18, text_19, text_20,
                                            text_21, text_22, text_23, text_24, text_25, text_26, text_27, text_28, text_29};

//inventory
InventoryItem *keyInv = new InventoryItem(KEY, keyImg, 11);
InventoryItem *piranhaInv = new InventoryItem(PIRANHA, piranhaImg, 12);
InventoryItem *netInv = new InventoryItem(NET, netImg, 13);
InventoryItem *keyTwoInv = new InventoryItem(KEYTWO, keyTwoImg, 29);
InventoryItem *keyThreeInv = new InventoryItem(KEYTHREE, keyThreeImg, 27);
InventoryItem *screwdriverInv = new InventoryItem(SCREWDRIVER, screwdriverImg, 25);
InventoryItem *invItems[] = {keyInv, piranhaInv, netInv, keyTwoInv, keyThreeInv, screwdriverInv};
byte activeSlot = 0;

//interface
Cursor cursor; //cursor
Arrow *leftArrow = new Arrow(4, 27, 3, 3, TRIANGLE, true, 0, 0, LEFT, 0, NOT, NONE, NONE); //RIGHT arrow
Arrow *rightArrow = new Arrow(103, 27, 3, 3, TRIANGLE, true, 0, 0, RIGHT, 0, NOT, NONE, NONE); //LEFT arrow
Arrow *downArrow = new Arrow(54, 47, 3, 3, TRIANGLE, true, 0, 0, DOWN, 0, NOT, NONE, NONE); //DOWN arrow
uint8_t view; //which wall is being looked at
uint8_t zoom; //is zoomed in
char currentText[20]; //stores which text is being shown onscreen
byte inventory = 0; //each bit is an inventory item
byte inventorySlots = 0; //each bit is an inventory slot (only four matter)

//progress flags
bool pupFed;
bool doorOneOpened;
bool gameWon;

//FRONT wall
Clickable *clock = new Clickable(30, 30, 6, 6, CIRCLE, true, 0, 0, 0, CLOCK, NONE, NONE); //clock for testing
Clickable *painting = new Clickable(45, 9, 50, 25, RECTANGLE, true, 24, 0, paintingImg, PAINTING, NONE, NONE);
Clickable *frontWallItems[] = {clock, painting};  //array for objects on the front wall

//RIGHT wall
Clickable *topDrawer = new Clickable (20, 17, 16, 16, RECTANGLE, true, 14, 16, drawerImg, NOT, KEYTHREE, NET);
Clickable *bottomDrawer = new Clickable (20, 32, 16, 16, RECTANGLE, true, 15, 16, drawerImg, NOT, NONE, SCREWDRIVER);
Clickable *doorOne = new Clickable(65, 16, 16, 32, RECTANGLE, true, 4, 0, doorImg, NOT, KEY, NONE);
Clickable *doorTwo = new Clickable(65, 16, 16, 36, RECTANGLE, false, 23, 0, doorTwoImg, NOT, KEYTWO, NONE);
Clickable *rightWallItems[] = {doorOne, topDrawer, bottomDrawer, doorTwo};

//BACK wall
Clickable *safe = new Clickable(55, 16, 32, 32, RECTANGLE, true, 3, 16, safeImg, SAFE, NONE, NONE); 
Clickable *pup = new Clickable(6, 32, 32, 16, RECTANGLE, true, 18, 19, pupImg, NOT, PIRANHA, NONE);
Clickable *wallClue = new Clickable(16, 40, 3, 3, CIRCLE, false, 20, 20, 0, WALLCLUE, NONE, NONE);
Clickable *backWallItems[] = {safe, pup, wallClue};

//LEFT wall
Clickable *piranhaClick = new Clickable(23, 19, 16, 16, RECTANGLE, true, 9, 17, piranhaImg, NOT, NET, PIRANHA);
Clickable *piranhaTank = new Clickable(20, 19, 32, 32, RECTANGLE, true, 8, 17, piranhaTankEmpty, NOT, NONE, NONE);
Clickable *barrel = new Clickable(60, 28, 15, 20, RECTANGLE, true, 26, 0, barrelImg, BARREL, NONE, NONE);
Clickable *leftWallItems[] = {piranhaTank, piranhaClick, barrel};

//SAFE zoom
Clickable *safeZoom = new Clickable(0, 0, 108, 54, RECTANGLE, true, 0, 0, safeZoomImg, NOT, NONE, NONE);
SafeLock *circleLock = new SafeLock(42, 19, 8, 8, CIRCLE, 8);
SafeLock *squareLock = new SafeLock(60, 12, 15, 15, RECTANGLE, 3);
SafeLock *triLock = new SafeLock(54, 43, 7, 15, TRIANGLE, 5);
SafeLock *safeLocks[] = {circleLock, squareLock, triLock}; 

//WALLCLUE zoom
Clickable *wallClueZoom = new Clickable(30, 30, 16, 16, CIRCLE, true, 21, 21, wallClueZoomImg, NOT, NONE, NONE);

//PAINTING zoom
Clickable *paintingZoom = new Clickable(0, 0, 108, 54, RECTANGLE, true, 24, 0, paintingZoomImg, NOT, SCREWDRIVER, NONE);

//BARREL zoom
Clickable *barrelZoom = new Clickable(0, 0, 108, 54, RECTANGLE, true, 0, 0, barrelZoomImg, NOT, NONE, NONE);
Clickable *barrelClue = new Clickable(40, 15, 16, 16, RECTANGLE, true, 20, 0, barrelClueImg, NOT, NONE, NONE);
Clickable *keyThree = new Clickable(54, 27, 16, 16, RECTANGLE, true, 27, 0, keyThreeImg, NOT, NONE, KEYTHREE);

//CLOCK zoom
Clickable *clockZoom = new Clickable(0, 0, 108, 54, RECTANGLE, true, 0, 0, clockZoomImg, NOT, NONE, NONE);
Clickable *key = new Clickable(49, 26, 20, 8, RECTANGLE, true, 2, 0, keyClickImg, NOT, NONE, KEY);

void setup() {
  Serial.begin(9600);
  arduboy.begin();
  arduboy.setFrameRate(30);
  view = FRONTWALL;
  zoom = NOT;
}

void loop() {
  //handle frames
  if(!arduboy.nextFrame()){
    return;
  }
  arduboy.clear();
  if(!gameWon){
    getInput();
    draw();
  } else {
    arduboy.setCursor(1, 26);
    arduboy.print("You did it!\nHope it wasn't too...\n ...ARDUous.");
    arduboy.display();
  }
}

//GETINPUT FUNCTIONS
void getInput(){
  arduboy.pollButtons(); //check for buttons

  if(arduboy.justPressed(A_BUTTON)){
    if(inventory != 0){
      selectInv();
    }
  }
  
  if(arduboy.justPressed(B_BUTTON)){
    memset(currentText, 0, sizeof(currentText)); //delete old text
    if(zoom == NOT){
      getRoomInput();
    } else {
      getZoomInput();
    }
  }
  
  //update inventory variables
  for(uint8_t i = 0; i < 6; i++){
    invItems[i]->updateInv();
  }
  updateVisibility();
  //cursor
  cursor.getInput(&arduboy);
}


void getRoomInput(){
    if(view == FRONTWALL){
      for(uint8_t i = 0; i < 2; i++){
        if(frontWallItems[i]->isVisible == true){
          frontWallItems[i]->getInput(&arduboy, &cursor);
        }
      }
    }
    if(view == BACKWALL){
      for(uint8_t i = 0; i < 3; i++){
        if(backWallItems[i]->isVisible == true){
          backWallItems[i]->getInput(&arduboy, &cursor);
        }
      }
    }
    
    if(view == RIGHTWALL){
      for(uint8_t i = 0; i < 4; i++){
        if(rightWallItems[i]->isVisible == true){
          rightWallItems[i]->getInput(&arduboy, &cursor);
        }
      }
    }
    
    if(view == LEFTWALL){
      for(uint8_t i = 0; i < 3; i++){
        if(leftWallItems[i]->isVisible == true){
          leftWallItems[i]->getInput(&arduboy, &cursor);
        }
      }
    } 
  leftArrow->getInput(&arduboy, &cursor);
  rightArrow->getInput(&arduboy, &cursor);
}

void getZoomInput(){
  if (zoom == SAFE) {
    downArrow->getInput(&arduboy, &cursor);
    safeZoom->getInput(&arduboy, &cursor);
    for(uint8_t i = 0; i < 3; i++){
      safeLocks[i]->getInput(&arduboy, &cursor);
    }
  } else if (zoom == WALLCLUE){
    downArrow->getInput(&arduboy, &cursor);
    wallClueZoom->getInput(&arduboy, &cursor);
  } else if (zoom == PAINTING){
    downArrow->getInput(&arduboy, &cursor);
    paintingZoom->getInput(&arduboy, &cursor);
  } else if (zoom == BARREL){
    downArrow->getInput(&arduboy, &cursor);
    barrelClue->getInput(&arduboy, &cursor);
    if(keyThree->isVisible){
      keyThree->getInput(&arduboy, &cursor);
    }
  } else if (zoom == CLOCK){
    downArrow->getInput(&arduboy, &cursor);
    clockZoom->getInput(&arduboy, &cursor);
    if(key->isVisible){
      key->getInput(&arduboy, &cursor);
    }
  }
}

//DRAW FUNCTIONS
void draw(){
  arduboy.fillRect(0, 0, 108, 54); //draw viewport
  
  if(zoom == NOT){
    drawRoom();
  } else {
    drawZoom();
  }

  if(inventory != 0){
    drawInventory();
  }

  //print currentText
  arduboy.setCursor(1, 54); 
  arduboy.setTextColor(WHITE);
  arduboy.setTextBackground(BLACK);
  arduboy.print(currentText);
  cursor.draw(&arduboy);

  arduboy.display();
}

void drawRoom(){
  arduboy.drawRect(6, 6, 96, 42, BLACK);
  arduboy.drawLine(0, 0, 6, 6, BLACK);
  arduboy.drawLine(0, 54, 6, 48, BLACK);
  arduboy.drawLine(108, 0, 102, 6, BLACK);
  arduboy.drawLine(108, 54, 102, 48, BLACK); 
  
  if(view == FRONTWALL){
    for(uint8_t i = 0; i < 2; i++){
      if(frontWallItems[i]->isVisible == true){
        frontWallItems[i]->draw(&arduboy);
      }
    }
  }    

  if(view == BACKWALL){
    for(uint8_t i = 0; i < 3; i++){
      if(backWallItems[i]->isVisible == true){
        backWallItems[i]->draw(&arduboy);
      }
    }
  }

  if(view == RIGHTWALL){
    for(uint8_t i = 0; i < 4; i++){
      if(rightWallItems[i]->isVisible == true){
        rightWallItems[i]->draw(&arduboy);
      }
    }
  }
  
  if(view == LEFTWALL){
    for(uint8_t i = 0; i < 3; i++){
      if(leftWallItems[i]->isVisible == true){
        leftWallItems[i]->draw(&arduboy);
      }
    }
  }

  leftArrow->draw(&arduboy);
  rightArrow->draw(&arduboy);
}

void drawInventory(){
  for(uint8_t i = 0; i < 6; i++){
    if(invItems[i]->isGotten == true && invItems[i]->isUsed == false){
      invItems[i]->draw(&arduboy);
    }
  }
}

void drawZoom(){
  if(zoom == SAFE){
    safeZoom->draw(&arduboy);
    for(uint8_t i = 0; i < 3; i++){
      safeLocks[i]->draw(&arduboy);
    }
  } else if (zoom == PAINTING){
    paintingZoom->draw(&arduboy);
  } else if (zoom == WALLCLUE){
    wallClueZoom->draw(&arduboy);
  } else if (zoom == BARREL){
    barrelZoom->draw(&arduboy);
    barrelClue->draw(&arduboy);
    if(keyThree->isVisible == true){
      keyThree->draw(&arduboy);
    }
  } else if (zoom == CLOCK){
    clockZoom->draw(&arduboy);
    if(key->isVisible == true){
      key->draw(&arduboy);
    }
  }
  downArrow->draw(&arduboy);
}

//HELPER FUNCTIONS
void checkCombo(){
  uint8_t correct = 0;
  for(uint8_t i = 0; i < 3; i++){
    if(safeLocks[i]->number == safeLocks[i]->combo){
      correct++;
    }
  }
  if (correct == 3){
    zoom = NOT;
    safe->toZoom = NOT;
    safe->itemGiven = KEYTWO;
    safe->image = safeOpenImg;
    safe->textNumber = 29;
    strcpy_P(currentText, (char*)pgm_read_word(&(string_table[28]))); //safe open text!
  }
}

void updateVisibility(){
  if(pupFed && wallClue->isVisible != true){
    wallClue->isVisible = true;
  }
  if(doorOneOpened && doorTwo->isVisible != true){
    doorTwo->isVisible = true;
  }
}


//called when A button is pressed. Changes which item is selected
void selectInv(){
  byte itemCount = 0;
  
  //generate itemCount byte. If items are two, itemCount should be 00000011. 3 = 00000111, etc.
  for(uint8_t i = 0; i < 6; i++){
    if(invItems[i]->isGotten == true && invItems[i]->isUsed == false){
      itemCount = itemCount << 1;
      itemCount += 1 << 0;
    }
  }
 
  if(itemCount != 0){
    //determine activeSlot
    //if no item is selected
    if(activeSlot == 0){
      activeSlot += 1 << 0; //00000001; set to first slot
    } else {
      activeSlot = activeSlot << 1; //shift to next slot e.g. 00000010
    }
  
    if((activeSlot & itemCount) == 0){ //if activeSlot's 1 bit is too far shifted
      activeSlot = 0;
    }
    
    for (int i = 0; i < 6; i++){
      if(activeSlot < (1 << 2)){ //this is a hack to make items in slots 3 and 4 work. will rework if there's time.
        if((itemCount & activeSlot) == activeSlot && activeSlot == invItems[i]-> slot && invItems[i]->isGotten == true && invItems[i]->isUsed == false){
          invItems[i]->isSelected = true;
          strcpy_P(currentText, (char*)pgm_read_word(&(string_table[invItems[i]->textNumber])));
        } else {
          invItems[i]->isSelected = false;
        }
      } else if (activeSlot == (1 << 2)) {
        if((itemCount & activeSlot) == activeSlot && activeSlot == invItems[i]->slot + 1 && invItems[i]->isGotten == true && invItems[i]->isUsed == false){
          invItems[i]->isSelected = true;
          strcpy_P(currentText, (char*)pgm_read_word(&(string_table[invItems[i]->textNumber])));
        } else {
          invItems[i]->isSelected = false;
        }
      } else if (activeSlot == (1 << 3)) {
        if((itemCount & activeSlot) == activeSlot && activeSlot == invItems[i]->slot + 4 && invItems[i]->isGotten == true && invItems[i]->isUsed == false){
          invItems[i]->isSelected = true;
          strcpy_P(currentText, (char*)pgm_read_word(&(string_table[invItems[i]->textNumber])));
        } else {
          invItems[i]->isSelected = false;
        }
      }
    }
  } 
}

//used when checking to see if player is using the right item
bool checkInv(uint8_t needed){
  for(uint8_t i = 0; i < 6; i++){
    if(invItems[i]->name == needed){
      if(invItems[i]->isSelected && invItems[i]->isGotten){
        invItems[i]->isUsed = true; //remove used item from inventory
        //update slots (shift other items down)
        for(uint8_t j = 0; j < 6; j++){
          if(invItems[j]->isGotten == true && invItems[j]->isUsed == false){
            if(invItems[j]->slot > invItems[i]->slot){
              invItems[j]->slot -= 1;
            }
          }
        }
        activeSlot = 0;
        return true;
      }
    }
  }
  return false;
}

//set slots for items
int getInvSlot(uint8_t slot){
  //count number of items the player has
  uint8_t itemCount = 0;
  for(uint8_t i = 0; i < 6; i++){
    if(invItems[i]->isGotten == true && invItems[i]->isUsed == false){
      itemCount++;
    }
  }
  return itemCount;
}
