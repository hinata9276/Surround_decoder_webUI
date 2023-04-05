/*
  Digital Pot Control

  This example controls an Analog Devices AD5206 digital potentiometer.
  The AD5206 has 6 potentiometer channels. Each channel's pins are labeled
  A - connect this to voltage
  W - this is the pot's wiper, which changes when you set it
  B - connect this to ground.

 The AD5206 is SPI-compatible,and to command it, you send two bytes,
 one with the channel number (0 - 5) and one with the resistance value for the
 channel (0 - 255).

 The circuit:
  * All A pins  of AD5206 connected to +5V
  * All B pins of AD5206 connected to ground
  * An LED and a 220-ohm resisor in series connected from each W pin to ground
  * CS - to digital pin 10  (SS pin)
  * SDI - to digital pin 11 (MOSI pin)
  * CLK - to digital pin 13 (SCK pin)

 created 10 Aug 2010
 by Tom Igoe

 Thanks to Heather Dewey-Hagborg for the original tutorial, 2005

*/


// inslude the SPI library:
#include <SPI.h>


// set pin 10 as the slave select for the digital pot:
const int slave1 = 10;
const int INCR = 8;
const int DECR = 9;
const int LED = 13;
const int LEFT = 0;
const int RIGHT = 1;
int state;
int volL=50;
int volR=50;

void setup() {
  // set the slaveSelectPin as an output:
  pinMode (slave1, OUTPUT);
  pinMode (INCR,INPUT);
  pinMode (DECR,INPUT);
  pinMode (LED,OUTPUT);
  // initialize SPI:
  SPI.begin();
}

void loop() {
  if(digitalRead(INCR)==HIGH){
    volL+=1;
    volR+=1;
    if(volL>=100){volL=100;}
    if(volR>=100){volR=100;}
    potWrite(slave1,LEFT,volL);
    potWrite(slave1,RIGHT,volR);
    delay(300);
    while(digitalRead(INCR)==HIGH){
    volL+=1;
    volR+=1;
    if(volL>=100){volL=100;}
    if(volR>=100){volR=100;state=!state;digitalWrite(LED,state);}
    potWrite(slave1,LEFT,volL);
    potWrite(slave1,RIGHT,volR);
    delay(30); 
    }
  }
  else if(digitalRead(DECR)==HIGH){
    volL-=1;
    volR-=1;
    if(volL<=0){volL=0;}
    if(volR<=0){volR=0;}
    potWrite(slave1,LEFT,volL);
    potWrite(slave1,RIGHT,volR);
    delay(300);
    while(digitalRead(DECR)==HIGH){
    volL-=1;
    volR-=1;
    if(volL<=0){volL=0;}
    if(volR<=0){volR=0;state=!state;digitalWrite(LED,state);}
    potWrite(slave1,LEFT,volL);
    potWrite(slave1,RIGHT,volR);
    delay(30); 
  }
  }
  digitalWrite(LED,LOW);
}

void potWrite(unsigned int chipSelect, unsigned int channel, unsigned int percent){
  if(percent<=100 or percent>=0){
    if(channel<=3 or channel>=0){
      unsigned int val,data;
      val = (percent<<8)/100;
      data = (channel<<12)|val;
      digitalWrite(chipSelect, LOW);
      SPI.transfer16(data);
      digitalWrite(chipSelect, HIGH);}
  }
}

int potRead(unsigned int chipSelect, unsigned int channel){
    if(channel<=3 or channel>=0){
      unsigned int val,data,percent;
      data = (channel<<12)|0x0C00;
      digitalWrite(chipSelect, LOW);
      val=SPI.transfer16(data);
      digitalWrite(chipSelect, HIGH);
      val = val & 0x01FF;
      percent = (val*100)>>8;
      return percent;
    }
}

