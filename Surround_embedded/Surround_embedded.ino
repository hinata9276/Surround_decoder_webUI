/*
  LiquidCrystal Library - Hello World

 Demonstrates the use a 16x2 LCD display.  The LiquidCrystal
 library works with all LCD displays that are compatible with the
 Hitachi HD44780 driver. There are many of them out there, and you
 can usually tell them by the 16-pin interface.

 This sketch prints "Hello World!" to the LCD
 and shows the time.

  The circuit:
 * LCD RS pin to digital pin 12
 * LCD Enable pin to digital pin 11
 * LCD D4 pin to digital pin 5
 * LCD D5 pin to digital pin 4
 * LCD D6 pin to digital pin 3
 * LCD D7 pin to digital pin 2
 * LCD R/W pin to ground
 * LCD VSS pin to ground
 * LCD VCC pin to 5V
 * 10K resistor:
 * ends to +5V and ground
 * wiper to LCD VO pin (pin 3)

 Library originally added 18 Apr 2008
 by David A. Mellis
 library modified 5 Jul 2009
 by Limor Fried (http://www.ladyada.net)
 example added 9 Jul 2009
 by Tom Igoe
 modified 22 Nov 2010
 by Tom Igoe

 This example code is in the public domain.

 http://www.arduino.cc/en/Tutorial/LiquidCrystal
 */


// include the library code:
#include <LiquidCrystal.h>
#include <SPI.h>
#include <EEPROM.h>

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

int incomingByte = 0;
short sel=1;
short page=0;
short level1=1;
short level2=1;
short level3=1;
short pressed;

int leftvol=0;
int rightvol=0;
int centrevol=0;
int rearvol=0;
short leftN,rightN,centreN,rearN;
short mute;
short mode;
short autobalance;
short rear_stereo;
short rear_delay;
unsigned short centreM;
int lcd_contrast;

const int slave1 = 10;
const int slave2 = 9;
const int slave3 = 8;
const int DM1 = 30;const int DM2 = 31;const int DM3 = 32;
const int DM4 = 33;const int CM1 = 34;const int CM2 = 35;
const int Csw = 36;const int Bsw = 37;const int Asw = 38;
short DM1x,DM2x,DM3x,DM4x,CM1x,CM2x,Cswx,Bswx,Aswx;
short leftlock,rightlock,centrelock,rearlock;
const int LEFT = 0;
const int RIGHT = 1;
const int CENTRE = 0;
const int REARLEFT = 1;
const int REARRIGHT = 0;
const int CONTRAST = 1;

short delta;
unsigned long pre_milli;

void setup() {
  // set up the LCD's number of columns and rows:
  lcd.begin(20, 4);
  pinMode(A0,INPUT);
  pinMode(A1,INPUT);
  pinMode(A2,INPUT);
  pinMode(A3,INPUT);
  pinMode(A4,INPUT);
  pinMode(slave1,OUTPUT);
  pinMode(slave2,OUTPUT);
  pinMode(slave3,OUTPUT);
  pinMode(DM1,OUTPUT);pinMode(DM2,OUTPUT);pinMode(DM3,OUTPUT);
  pinMode(DM4,OUTPUT);pinMode(CM1,OUTPUT);pinMode(CM2,OUTPUT);
  pinMode(Csw,OUTPUT);pinMode(Bsw,OUTPUT);pinMode(Asw,OUTPUT);
  
  digitalWrite(slave1,HIGH);
  digitalWrite(slave2,HIGH);
  digitalWrite(slave3,HIGH);
  SPI.begin();

  DM1x=EEPROM.read(0);DM2x=EEPROM.read(1);DM3x=EEPROM.read(2);
  DM4x=EEPROM.read(3);CM1x=EEPROM.read(4);CM2x=EEPROM.read(5);
  Cswx=EEPROM.read(6);Bswx=EEPROM.read(7);Aswx=EEPROM.read(8);

  autobalance=DM3x;rear_stereo=Bswx;rear_delay=Cswx;
  centreM=(CM1x<<1)|CM2x;
  mode=DM1x;

  tone(48,37900);
  
  leftvol=100-potRead(slave1,LEFT);
  rightvol=100-potRead(slave1,RIGHT);
  centrevol=100-potRead(slave2,CENTRE);
  lcd_contrast=potRead(slave2,CONTRAST);
  rearvol=100-potRead(slave3,REARLEFT);
  setting();
  lcd.setCursor(0, 1);
  lcd.print("Loading");
  delay(200);
  lcd.setCursor(0, 2);
  for(int i=0;i<19;i++){
     lcd.print("\xff");
     delay(30); 
  }
  lcd.clear();
  printmain();
}

void loop() {
level1=readinput(3);
if(level1==0){                                                //display main
  printmain();
  if(millis()%60000<=100){
      EEPROM.write(0,DM1x);EEPROM.write(1,DM2x);EEPROM.write(2,DM3x);
      EEPROM.write(3,DM4x);EEPROM.write(4,CM1x);EEPROM.write(5,CM2x);
      EEPROM.write(6,Cswx);EEPROM.write(7,Bswx);EEPROM.write(8,Aswx);
  }
}

else if(level1==1){                                           //display channel volume
  sel=level2;
  while(1){
      printCV();
      level2=readinput(5);
      if(delta!=0){
        switch(3*page+sel){
          case 1 :  if(leftlock==1){adjustallvol();}
                    else{
                      leftvol+=delta;
                      if(leftvol>=100){leftvol=100;}
                      else if(leftvol<=0){leftvol=0;}
                      potWrite(slave1,LEFT,100-leftvol);
                      }
                    break;
          case 2 :  if(rightlock==1){adjustallvol();}
                    else{
                      rightvol+=delta;
                      if(rightvol>=100){rightvol=100;}
                      else if(rightvol<=0){rightvol=0;}
                      potWrite(slave1,RIGHT,100-rightvol);
                    }
                    break;
          case 3 :  if(centrelock==1){adjustallvol();}
                    else{
                      centrevol+=delta;
                      if(centrevol>=100){centrevol=100;}
                      else if(centrevol<=0){centrevol=0;}
                      potWrite(slave2,CENTRE,100-centrevol);
                    }
                    break;
          case 4 :  if(rearlock==1){adjustallvol();}
                    else{
                      rearvol+=delta;
                      if(rearvol>=100){rearvol=100;}
                      else if(rearvol<=0){rearvol=0;}
                      potWrite(slave3,REARLEFT,100-rearvol);
                      potWrite(slave3,REARRIGHT,100-rearvol);
                    }
                    break;
          default : break;
        }
      }
      if(level2==1){
        if(pressed!=0){leftlock=abs(leftlock-1);}
        }
      if(level2==2){
        if(pressed!=0){rightlock=abs(rightlock-1);}
        }
      if(level2==3){
        if(pressed!=0){centrelock=abs(centrelock-1);}
        }
      if(level2==4){
        if(pressed!=0){rearlock=abs(rearlock-1);}
        }
      if(level2==5){
        level2=1;
        page=0;
        sel=level1;
        potWrite(slave1,LEFT+2,100-leftvol);
        delay(5);
        potWrite(slave1,RIGHT+2,100-rightvol);
        delay(5);
        potWrite(slave2,CENTRE+2,100-centrevol);
        delay(5);
        potWrite(slave3,REARLEFT+2,100-rearvol);
        delay(5);
        potWrite(slave3,REARRIGHT+2,100-rearvol);
        break;}
  }
  }
else if(level1==2){                                           //display mode
  sel=level2;
  while(1){
    printmode();
    level2=readinput(4);
///////////////////Stereo Bypass//////////////////////
    if(level2==1){                                            //display stereo bypass
      sel=level3;
      while(1){
        printSB(); 
        DM1x=0;DM2x=0;DM3x=autobalance;DM4x=0;
        Bswx=rear_stereo;Aswx=Bswx;mode=0;
        setting();
        level3=readinput(3);
        if(delta!=0){
        switch(3*page+sel){
          case 1 :  autobalance=abs(autobalance-1);
                    break;
          case 2 :  rear_stereo=abs(rear_stereo-1);
                    break;  
         default : break;
            }
          }
        if(level3==3){
          level3=1;
          page=0;
          sel=level2;
          break;}
      }
    }
//////////////Dolby Pro-Logic///////////////////
    else if(level2==2){                                       //display Dolby Pro Logic
      sel=level3;
      while(1){
        printDPL();
        DM1x=1;DM2x=1;DM3x=autobalance;DM4x=1;
        CM1x=(centreM&2)>>1;CM2x=centreM&1;
        Cswx=rear_delay;Bswx=0;Aswx=0;mode=1;
        setting();
        level3=readinput(4);
        if(delta!=0){
        switch(3*page+sel){
          case 1 :  autobalance=abs(autobalance-1);
                    break;
          case 2 :  rear_delay=abs(rear_delay-1);
                    break; 
          case 3 :  centreM=(centreM+delta)%4;
                    break;     
         default : break;
            }
          }
        if(level3==4){
          level3=1;
          page=0;
          sel=level2;
          break;}
      }
    }
    else if(level2==4){                                       //back
        level2=1;
        page=0;
        sel=level1;
        break;}
        
    if(3*page+sel==3){                                        //mute
      if(delta!=0){
        mute=abs(mute-1);
        setting();
        }
      }
    }
  }
else if(level1==3){                                           //display others
  sel=level2;
  while(1){
    printothers();
    level2=readinput(6);
    switch(level2){
      case 1 : pre_milli=millis();
               leftN=1;
               break;
      case 2 : pre_milli=millis();
               rightN=1;
               break;
      case 3 : pre_milli=millis();
               centreN=1;
               break;
      case 4 : pre_milli=millis();
               rearN=1;
               break;
      default : break;
    }
    if(millis()-pre_milli>2000){leftN=0;rightN=0;centreN=0;rearN=0;}
    setting();
    if(3*page+sel==5){
      if(delta!=0){
        lcd_contrast+=delta;
        if(lcd_contrast>=100){lcd_contrast=100;}
        else if(lcd_contrast<=0){lcd_contrast=0;}
        potWrite(slave2,CONTRAST,lcd_contrast);
        }
      }
      
    if(level2==6){
        level2=1;
        page=0;
        sel=level1;
        potWrite(slave2,CONTRAST+2,lcd_contrast);
        break;}
    }
  }
}

void printmain(){
  lcd.setCursor(0, 0);
  lcd.print("Main");
  lcd.setCursor(1, 1);
  lcd.print("Channel Volume");
  lcd.setCursor(1, 2);
  lcd.print("Mode");
  lcd.setCursor(1, 3);
  lcd.print("Others");
  lcd.setCursor(0, sel);
  lcd.print("\x7e");
}

void printCV(){
  lcd.setCursor(0, 0);
  lcd.print("Main>Channel volume");
  if(page==0){
    lcd.setCursor(1, 1);lcd.print("Left");//leftvol=potRead(slave1,LEFT);
    lcd.setCursor(10, 1);lcd.print(leftvol);lcd.print("%   ");
    lcd.setCursor(1, 2);lcd.print("Right");//rightvol=potRead(slave1,RIGHT);
    lcd.setCursor(10, 2);lcd.print(rightvol);lcd.print("%   ");
    lcd.setCursor(1, 3);lcd.print("Centre");//centrevol=potRead(slave2,CENTRE);
    lcd.setCursor(10, 3);lcd.print(centrevol);lcd.print("%   ");
    if(leftlock==1){lcd.setCursor(19, 1);lcd.print("\xef");}
    else{lcd.setCursor(19, 1);lcd.print(" ");}
    if(rightlock==1){lcd.setCursor(19, 2);lcd.print("\xef");}
    else{lcd.setCursor(19, 2);lcd.print(" ");}
    if(centrelock==1){lcd.setCursor(19, 3);lcd.print("\xef");}
    else{lcd.setCursor(19, 3);lcd.print(" ");}
    }
  else if(page==1){
    lcd.setCursor(1, 1);lcd.print("Rear");//rearvol=potRead(slave3,REARLEFT);
    lcd.setCursor(10, 1);lcd.print(rearvol);lcd.print("%   ");
    lcd.setCursor(1, 2);lcd.print("Back");
    if(rearlock==1){lcd.setCursor(19, 1);lcd.print("\xef");}
    else{lcd.setCursor(19, 1);lcd.print(" ");}
    }
  lcd.setCursor(0, sel);lcd.print("\x7e");
}

void printmode(){
  lcd.setCursor(0, 0);
  lcd.print("Main>Mode");
  if(page==0){
    lcd.setCursor(1, 1);lcd.print("Stereo Bypass");
    lcd.setCursor(1, 2);lcd.print("Dolby Pro-Logic");
    lcd.setCursor(1, 3);lcd.print("Mute");
    lcd.setCursor(10, 3);
    if(mute==1){lcd.print("ON  ");}
    else{lcd.print("OFF  ");}
    if(mode==0){
      lcd.setCursor(19, 1);
      lcd.print("*");
      lcd.setCursor(19, 2);
      lcd.print(" ");
    }
    else{
      lcd.setCursor(19, 1);
      lcd.print(" ");
      lcd.setCursor(19, 2);
      lcd.print("*");
    }
  }
  else if(page==1){
    lcd.setCursor(1, 1);lcd.print("Back");
    lcd.setCursor(19, 1);
    lcd.print(" ");
    lcd.setCursor(19, 2);
    lcd.print(" ");
    }
  lcd.setCursor(0, sel);lcd.print("\x7e");
}

void printSB(){
  lcd.setCursor(0, 0);
  lcd.print("Mode>Stereo Bypass");
  
  lcd.setCursor(1, 1);
  lcd.print("Autobalance");
  lcd.setCursor(15, 1);
    if(autobalance==1){lcd.print("ON  ");}
    else{lcd.print("OFF  ");}
    
  lcd.setCursor(1, 2);
  lcd.print("Rear stereo");
  lcd.setCursor(15, 2);
    if(rear_stereo==1){lcd.print("ON  ");}
    else{lcd.print("OFF  ");}
    
  lcd.setCursor(1, 3);lcd.print("Back");
  lcd.setCursor(0, sel);lcd.print("\x7e");
  }

void printDPL(){
  lcd.setCursor(0, 0);
  lcd.print("Mode>Dolby Pro-Logic");
  if(page==0){
    lcd.setCursor(1, 1);
    lcd.print("Autobalance");
    lcd.setCursor(15, 1);
    if(autobalance==1){lcd.print("ON  ");}
    else{lcd.print("OFF  ");}
    
    lcd.setCursor(1, 2);
    lcd.print("Rear delay");
    lcd.setCursor(15, 2);
    if(rear_delay==1){lcd.print("ON  ");}
    else{lcd.print("OFF  ");}

    lcd.setCursor(1, 3);
    lcd.print("Centre");
    lcd.setCursor(10, 3);
    if(centreM==1){lcd.print("Wideband  ");}
    else if(centreM==2){lcd.print("Phantom  ");}
    else if(centreM==3){lcd.print("Normal  ");}
    else{lcd.print("OFF      ");}
    }
  else if(page==1){
    lcd.setCursor(1, 1);lcd.print("Back");
    }
  lcd.setCursor(0, sel);lcd.print("\x7e");
}

void printothers(){
   lcd.setCursor(0, 0);
    lcd.print("Main>Others");
    if(page==0){
    lcd.setCursor(1, 1);
    lcd.print("Left noise");
    lcd.setCursor(15, 1);
    if(leftN==1){lcd.print("ON  ");}
    else{lcd.print("OFF  ");}
    
    lcd.setCursor(1, 2);
    lcd.print("Right noise");
    lcd.setCursor(15, 2);
    if(rightN==1){lcd.print("ON  ");}
    else{lcd.print("OFF  ");}

    lcd.setCursor(1, 3);
    lcd.print("Centre noise");
    lcd.setCursor(15, 3);
    if(centreN==1){lcd.print("ON  ");}
    else{lcd.print("OFF  ");}
    }
  else if(page==1){
    lcd.setCursor(1, 1);
    lcd.print("Rear noise");
    lcd.setCursor(15, 1);
    if(rearN==1){lcd.print("ON  ");}
    else{lcd.print("OFF  ");}

    lcd.setCursor(1, 2);lcd.print("LCD contrast");//lcd_contrast=potRead(slave3,CONTRAST);
    lcd.setCursor(15, 2);lcd.print(lcd_contrast);lcd.print("%  ");
    lcd.setCursor(1, 3);lcd.print("Back");
    }
  lcd.setCursor(0, sel);lcd.print("\x7e");}

void clearline(int line){
  lcd.setCursor(0, line);
  lcd.print("                    ");
}

void cleararrow(){
  lcd.setCursor(0, 1);
  lcd.print(" ");
  lcd.setCursor(0, 2);
  lcd.print(" ");
  lcd.setCursor(0, 3);
  lcd.print(" ");
}

void adjustallvol(){
  if(leftlock==1){
    leftvol+=delta;
    if(leftvol>=100){leftvol=100;}
    else if(leftvol<=0){leftvol=0;}
    potWrite(slave1,LEFT,100-leftvol);
    }
  if(rightlock==1){
    rightvol+=delta;
    if(rightvol>=100){rightvol=100;}
    else if(rightvol<=0){rightvol=0;}
    potWrite(slave1,RIGHT,100-rightvol);
    }
  if(centrelock==1){
    centrevol+=delta;
    if(centrevol>=100){centrevol=100;}
    else if(centrevol<=0){centrevol=0;}
    potWrite(slave2,CENTRE,100-centrevol);
    }
  if(rearlock==1){
    rearvol+=delta;
    if(rearvol>=100){rearvol=100;}
    else if(rearvol<=0){rearvol=0;}
    potWrite(slave3,REARLEFT,100-rearvol);
    potWrite(slave3,REARRIGHT,100-rearvol);
    }                  
}

void setting(){
  if(mute==1){
    digitalWrite(DM1,LOW);digitalWrite(DM2,LOW);digitalWrite(DM3,LOW);
    digitalWrite(DM4,HIGH);digitalWrite(CM1,HIGH);digitalWrite(CM2,HIGH);
    digitalWrite(Bsw,LOW);digitalWrite(Asw,LOW);
  }
  else if(leftN==1){
    digitalWrite(DM1,LOW);digitalWrite(DM2,HIGH);digitalWrite(DM3,LOW);
    digitalWrite(DM4,LOW);digitalWrite(CM1,HIGH);digitalWrite(CM2,HIGH);
    digitalWrite(Bsw,LOW);digitalWrite(Asw,LOW);
  }
  else if(rightN==1){
    digitalWrite(DM1,LOW);digitalWrite(DM2,HIGH);digitalWrite(DM3,HIGH);
    digitalWrite(DM4,LOW);digitalWrite(CM1,HIGH);digitalWrite(CM2,HIGH);
    digitalWrite(Bsw,LOW);digitalWrite(Asw,LOW);
  }
  else if(centreN==1){
    digitalWrite(DM1,LOW);digitalWrite(DM2,HIGH);digitalWrite(DM3,LOW);
    digitalWrite(DM4,HIGH);digitalWrite(CM1,HIGH);digitalWrite(CM2,HIGH);
    digitalWrite(Bsw,LOW);digitalWrite(Asw,LOW);
  }
  else if(rearN==1){
    digitalWrite(DM1,LOW);digitalWrite(DM2,HIGH);digitalWrite(DM3,HIGH);
    digitalWrite(DM4,HIGH);digitalWrite(CM1,HIGH);digitalWrite(CM2,HIGH);
    digitalWrite(Bsw,LOW);digitalWrite(Asw,LOW);
  }
  else{
    digitalWrite(DM1,DM1x);digitalWrite(DM2,DM2x);digitalWrite(DM3,DM3x);
    digitalWrite(DM4,DM4x);digitalWrite(CM1,CM1x);digitalWrite(CM2,CM2x);
    digitalWrite(Csw,Cswx);digitalWrite(Bsw,Bswx);digitalWrite(Asw,Aswx);
  }
}

int readinput(int option){
  if(pressed>=0 && pressed<4){delay(300);}
  else{delay(20);}
  int level=0;
  delta=0;
      if(digitalRead(A2) == HIGH){
          pressed++;
          cleararrow();
          if(3*page+sel>=option){}
          else{sel++;}
          if(sel>3){
            sel=1;page++;clearline(1);clearline(2);clearline(3);
          }
          lcd.setCursor(0, sel);
          lcd.print("\x7e");
          //break;
      }
      else if(digitalRead(A0) == HIGH){
          pressed++;
          cleararrow();
          sel--;
          if(sel<1){
            if(page>0){
              sel=3;page--;clearline(1);clearline(2);clearline(3);
            }
            else{
              sel=1;}
          }
          lcd.setCursor(0, sel);
          lcd.print("\x7e");
          //break;
      }
      else if(digitalRead(A4) == HIGH){
          pressed++;
          lcd.clear();
          level = 3*page+sel;
          //break;
      } 
      else if(digitalRead(A3) == HIGH){
          pressed++;
          delta=-1;
          //break;
      }
      else if(digitalRead(A1) == HIGH){
          pressed++;
          delta=1;
          //break;
      }      
      else{pressed=0;}
  return level;
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
