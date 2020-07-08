/*
 * Author: Zachary Anderson (zacharyjcanderson@gmail.com)
 * Date: August 10, 2019
 * Purpose: A new revision of the Zen Maker Lab MP3 software
 *    which is compatible with arduino.
 */
 
#include "Arduino.h"
#include "SoftwareSerial.h"
#include "DFRobotDFPlayerMini.h"

SoftwareSerial mySoftwareSerial(7, 6); // RX, TX
DFRobotDFPlayerMini myDFPlayer;
void printDetail(uint8_t type, int value);
void buttonEventHandler();
void buttonFlush();
void playerEventHandler();
byte button_status = 0x0;
bool event_triggered = false;
bool player_play = false;
int player_song = 0;

//ACTIVITY---ASSIGN BUTTONS TO PINS D2-5
int play_button;
int next_button;
int vol_up_button;
int vol_down_button;
//END

void setup() {

  //ACTIVITY---BUTTON INITIALIZATION
  //END
  
  mySoftwareSerial.begin(9600);
  Serial.begin(115200);
  
  
  Serial.println();
  Serial.println(F("DFRobot DFPlayer Mini Demo"));
  Serial.println(F("Initializing DFPlayer ... (May take 3~5 seconds)"));
  
  if (!myDFPlayer.begin(mySoftwareSerial)) {  //Use softwareSerial to communicate with mp3.
    Serial.println(F("Unable to begin:"));
    Serial.println(F("1.Please recheck the connection!"));
    Serial.println(F("2.Please insert memory card!"));
    while(true){
      delay(0); // Code to compatible with ESP8266 watch dog.
    }
  }
  Serial.println(F("DFPlayer Mini online."));
  
  myDFPlayer.volume(30);  //Set volume value. From 0 to 30
}

void loop() {
  static unsigned long timer = millis();

  buttonEventHandler(); //Grabs the state of buttons
  
  //Update Player and Flush Buttons every 300ms
  if (millis() - timer > 300) {
    timer = millis();
    
    if(!event_triggered){
      playerEventHandler(); //Uses state of buttons to do something
    }
    
    if(event_triggered && digitalRead(play_button) && digitalRead(next_button) && digitalRead(vol_up_button) && digitalRead(vol_down_button)){
      event_triggered = false;
      buttonFlush();
      }
    }

  if (myDFPlayer.available()) {
    printDetail(myDFPlayer.readType(), myDFPlayer.read()); //Print the detail message from DFPlayer to handle different errors and states.
  }
}

void buttonEventHandler(){
    if(!digitalRead(play_button)){
        button_status = (button_status | 0b00000001);
      }
    if(!digitalRead(next_button)){
        button_status = (button_status | 0b00000010);
      }
    if(!digitalRead(vol_up_button)){
        button_status = (button_status | 0b00000100);
      }
    if(!digitalRead(vol_down_button)){
        button_status = (button_status | 0b00001000);
      }
}

void buttonFlush(){
    button_status = 0x0;
  }

void playerEventHandler(){
      event_triggered = true;
      if(button_status & 0b00000001){
        if(!player_play){
          Serial.println(F("Play Event"));
          player_play = true;
          myDFPlayer.start();
        }else{
          Serial.println(F("Pause Event"));
          player_play = false;
          myDFPlayer.pause();
          }
      }else if(button_status & 0b00000010){
        Serial.println(F("Next Event"));
        myDFPlayer.next();
      }else if(button_status & 0b00000100){
        if(button_status & 0b00001000){
          Serial.println(F("Restart Event"));
          myDFPlayer.play(1);
        }else{
          Serial.println(F("Volume Up Event"));
          myDFPlayer.volumeUp();
        }
      }else if(button_status & 0b00001000){
        Serial.println(F("Volume Down Event"));
        myDFPlayer.volumeDown();
      }else{
        Serial.println(F("No Event"));
        event_triggered = false;
      }
  }

void printDetail(uint8_t type, int value){
  switch (type) {
    case TimeOut:
      Serial.println(F("Time Out!"));
      break;
    case WrongStack:
      Serial.println(F("Stack Wrong!"));
      break;
    case DFPlayerCardInserted:
      Serial.println(F("Card Inserted!"));
      break;
    case DFPlayerCardRemoved:
      Serial.println(F("Card Removed!"));
      break;
    case DFPlayerCardOnline:
      Serial.println(F("Card Online!"));
      break;
    case DFPlayerUSBInserted:
      Serial.println("USB Inserted!");
      break;
    case DFPlayerUSBRemoved:
      Serial.println("USB Removed!");
      break;
    case DFPlayerPlayFinished:
      Serial.print(F("Number:"));
      Serial.print(value);
      Serial.println(F(" Play Finished!"));
      break;
    case DFPlayerError:
      Serial.print(F("DFPlayerError:"));
      switch (value) {
        case Busy:
          Serial.println(F("Card not found"));
          break;
        case Sleeping:
          Serial.println(F("Sleeping"));
          break;
        case SerialWrongStack:
          Serial.println(F("Get Wrong Stack"));
          break;
        case CheckSumNotMatch:
          Serial.println(F("Check Sum Not Match"));
          break;
        case FileIndexOut:
          Serial.println(F("File Index Out of Bound"));
          break;
        case FileMismatch:
          Serial.println(F("Cannot Find File"));
          break;
        case Advertise:
          Serial.println(F("In Advertise"));
          break;
        default:
          break;
      }
      break;
    default:
      break;
  }
  
}
