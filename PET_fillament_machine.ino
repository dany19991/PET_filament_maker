#include <Arduino.h>
#include <AccelStepper.h>
#include <LiquidCrystal.h>

//LCD pin to Arduino
const int pin_RS = 8; 
const int pin_EN = 9; 
const int pin_d4 = 4; 
const int pin_d5 = 5; 
const int pin_d6 = 6; 
const int pin_d7 = 7; 
const int pin_BL = 10; 
LiquidCrystal lcd( pin_RS,  pin_EN,  pin_d4,  pin_d5,  pin_d6,  pin_d7); //instanta LCD 
#define A_PIN 1 //analog A1
#define VOLTAGE 5

// stepper driver wires/pins setup
#define DIR 11
#define STEP 12

AccelStepper stepper(1,STEP,DIR); //instanta STEPPER

  float speed=-1000;
  int direction =1;

  int debounce_right = 0;
  int debounce_left = 0;
  int debounce_up = 0;
  int debounce_down = 0;
  int debounce_switch = 0;

// relay setup
#define SWITCH_PIN 2
#define RELAY_PIN 3

#define STATE_POWERON 1
#define STATE_SET_ISR 2
#define STATE_RUN 3
#define STATE_FINISHED 4
#define STATE_SETUP 5

int current_state = STATE_POWERON;

void setup() {
  stepper.setMaxSpeed(4000);
  lcd.begin(16, 2);

  pinMode(SWITCH_PIN, INPUT_PULLUP); //use interrupt in the future? 
  pinMode(RELAY_PIN,OUTPUT);

  digitalWrite(RELAY_PIN, HIGH);
}

void statePoweron(){
  int x; 
  x = analogRead(0);
  if( x < 60) { //right button
    if(debounceRight()){
      lcd.clear();
      current_state = STATE_RUN;
      
      lcd.setCursor(0, 0);
      lcd.print("state:running");
      lcd.setCursor(0, 1);
      lcd.print("speed:");
      lcd.print(speed);

      return;
    }
  }else if(x < 200){ //up button
    if(debounceUp()){
      lcd.clear();
      //change speed 
      speed+=200;
    }
  }else if(x < 400){ //down button
    if(debounceDown()){
      lcd.clear();
      speed-=200;
    }
  }else if(x < 600){ //left button
    
  }else if(x < 800){ //select button
    //switch direction ? disable interrupt ?
  }else { //default 
    //do nothing 
  }
  stepper.stop();
  lcd.setCursor(0, 0);
  lcd.print("state startup");
  lcd.setCursor(0, 1);
  lcd.print("speed:");
  lcd.print(speed);
}

void stateRunning(){
  int x;
  x = analogRead(0);
  if( x < 60) { //right button
    
  }else if(x < 200){ //up button
    if(debounceUp()) {
      lcd.clear();
      speed+=100; 
      stepper.setSpeed(speed);
      lcd.clear();
      lcd.print(speed);

    }
  }else if(x < 400){ //down button
    if(debounceDown()){
      lcd.clear();
      speed-=100;
      stepper.setSpeed(speed);
      lcd.clear();
      lcd.print(speed);
    }
  }else if(x < 600){ //left button
    
  }else if(x < 800){ //select button
    
  }else { //default 
    //do nothing 
  }

  if (digitalRead(SWITCH_PIN)){ //switch is "open"/ it's read value is 0
    if( debounceSwitch()){
      current_state = STATE_FINISHED;
      digitalWrite(RELAY_PIN, LOW);
      return;
    }
  }  
  stepper.runSpeed();
}

void stateFinished(){
  int x; 
  x = analogRead(0);
  if( x < 60) { //right button
    if(debounceRight()){
      lcd.clear();
      //current_state = STATE_RUN;
    }
  }else if(x < 200){ //up button
    if(debounceUp()){
      lcd.clear();
      //change speed 
      speed+=200;
    }
    //speed up ? 
  }else if(x < 400){ //down button
    if(debounceDown()){
      lcd.clear();
      speed-=200;
    }
  }else if(x < 600){ //left button
    if(debounceLeft()){
      lcd.clear();
      current_state = STATE_POWERON;
      digitalWrite(RELAY_PIN, HIGH);
      return;
    }
  }else if(x < 800){ //select button
    //switch direction ? disable interrupt ?
  }else { //default 
    //do nothing 
  }
  stepper.stop();
  lcd.setCursor(0, 0);
  lcd.print("state:finished");
  lcd.setCursor(0, 1);
  lcd.print("speed:");
  lcd.print(speed);
}

void loop() {
      switch(current_state){
        case STATE_POWERON:
          statePoweron();
          break;
        case STATE_RUN:
          stateRunning();
           break;
        case STATE_FINISHED:
          stateFinished();
          break;
        default:
          //do nothing
          break;
      }
}


int debounceUp(){
  if (debounce_up <200) {
    debounce_up++;
    return 0;
  } else {
    debounce_up = 0;
    return 1;
  }
}

int debounceRight(){
  if (debounce_up <200){
    debounce_up++;
    return 0;
  } else {
    debounce_up = 0;
    return 1;
  }
}

int debounceDown(){
  if (debounce_down <200){
    debounce_down++;
    return 0;
  } else {
    debounce_down = 0;
    return 1;
  }
}

int debounceLeft(){
  if (debounce_left <200){
    debounce_left++;
    return 0;
  } else {
    debounce_left = 0;
    return 1;
  }
}

int debounceSwitch(){
  if(debounce_switch < 200){
    debounce_switch++;
    return 0;
  }else {
    debounce_switch = 0;
    return 1;
  }
}

String getCurrentState(){ //not used
  if(current_state == STATE_POWERON )
  return "state:PowerOn";
  else if (current_state == STATE_RUN)
  return "state:Running";
  else if(current_state == STATE_FINISHED)
  return "state:Finished";
}
