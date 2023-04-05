// include the library code:
#include <LiquidCrystal.h>
#include <Wire.h>
#include <EEPROM.h>

char* text[]={"Gain", "Volume", "Bass", "Midrange", "Treble", "Attenuate", "Back"};
int gain,vol,bass,mid,tre,LA,RA;
short overlap=0;
short highlight;
short direct,select,pos;

//program flag
short selected;
short saved;

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

void setup() {
  // put your setup code here, to run once:
lcd.begin(20, 4);
Wire.begin();
print_main();
delay(1000);
load_data();
gain_set(gain);
volume_set(vol);
bass_set(bass);
midrange_set(mid);
treble_set(tre);
left_att_set(LA);
right_att_set(RA); 
}

void loop() {
  // put your main code here, to run repeatedly:
  print_function();
  read_encoder();
  //show cursor position
  if (direct==1){
    if (highlight<=0){
      highlight=0;
    }
    else{
      highlight++;
    }
  }
  else if (direct==-1){
    if (highlight>=3){
      highlight=3;
    }
    else{
      highlight--;
    }
  }
  else{}

  //enable selection
  if (select==1){
  
  
}

void print_main(){
  lcd.setCursor(4, 0);
  lcd.print("Welcome to");
  lcd.setCursor(2, 1);
  lcd.print("Digital Equaliser");
  lcd.setCursor(4, 2);
  lcd.print("designed by");
  lcd.setCursor(5, 3);
  lcd.print("Sunny Lee");
  }

void load_data(){
  
}

void save_data(){
  
}

short read_encoder(){
  
}

void print_function(){
  lcd.setCursor(1, 0);
  lcd.print(text[overlap]);
  lcd.setCursor(1, 1);
  lcd.print(text[overlap+1]);
  lcd.setCursor(1, 2);
  lcd.print(text[overlap+2]);
  lcd.setCursor(1, 3);
  lcd.print(text[overlap+3]);
  lcd.setCursor(0,highlight);
  lcd.print("\x7e");
}
void gain_set(int data){//in dB
  data = abs(data);
  Wire.beginTransmission(0x44); // transmit to device
  // device address is specified in datasheet
  Wire.write(0x11);            // sends instruction byte
  Wire.write(data);             // sends potentiometer value byte
  Wire.endTransmission(); 
}

void volume_set(int data){//in dB
  data = abs(data);
  Wire.beginTransmission(0x44); // transmit to device
  // device address is specified in datasheet
  Wire.write(0x12);            // sends instruction byte
  Wire.write(data);             // sends potentiometer value byte
  Wire.endTransmission(); 
}

void bass_set(int data){//in dB
  if (data<=0){
    data = 7-abs(data/2);}
  else{
    data = 15-data/2;}
  Wire.beginTransmission(0x44); // transmit to device
  // device address is specified in datasheet
  Wire.write(0x13);            // sends instruction byte
  Wire.write(data);             // sends potentiometer value byte
  Wire.endTransmission(); 
}

void midrange_set(int data){//in dB
  if (data<=0){
    data = 7-abs(data/2);}
  else{
    data = 15-data/2;}
  Wire.beginTransmission(0x44); // transmit to device
  // device address is specified in datasheet
  Wire.write(0x14);            // sends instruction byte
  Wire.write(data);             // sends potentiometer value byte
  Wire.endTransmission(); 
}

void treble_set(int data){//in dB
  if (data<=0){
    data = 7-abs(data/2);}
  else{
    data = 15-data/2;}
  Wire.beginTransmission(0x44); // transmit to device
  // device address is specified in datasheet
  Wire.write(0x15);            // sends instruction byte
  Wire.write(data);             // sends potentiometer value byte
  Wire.endTransmission(); 
}

void left_att_set(int data){//in dB
  data = abs(data);
  Wire.beginTransmission(0x44); // transmit to device
  // device address is specified in datasheet
  Wire.write(0x17);            // sends instruction byte
  Wire.write(data);             // sends potentiometer value byte
  Wire.endTransmission(); 
}

void right_att_set(int data){//in dB
  data = abs(data);
  Wire.beginTransmission(0x44); // transmit to device
  // device address is specified in datasheet
  Wire.write(0x16);            // sends instruction byte
  Wire.write(data);             // sends potentiometer value byte
  Wire.endTransmission(); 
}
