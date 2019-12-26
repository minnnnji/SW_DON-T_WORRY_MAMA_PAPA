#define trig1 6
#define echo1 5
#define trig2 11
#define echo2 12
#define red 8
#define blue 9
#define sw1 13
#define buzz 10
#define sensorPin 2
#include<SoftwareSerial.h>
SoftwareSerial bluetooth(4,3);
int freq = 150;
int save =32;
int redState = LOW;
int blueState = LOW;
int whichLED = 0;  //  0 = red LED, 1 = blue LED
boolean freqFlag = true;
boolean bReady=false;
unsigned long previousMillis = 0;
unsigned long switch_previousMillis = 0;
const long interval = 50;  // 50밀리초 주기로 동작
const long switch_interval = 500;  // 500밀리초 주기로 LED 바뀜
int check = 0;
char cmd;
void setup(){
  Serial.begin(9600); 
  bluetooth.begin(9600);
  pinMode(trig1,OUTPUT);
  pinMode(echo1,INPUT);
  pinMode(trig2,OUTPUT);
  pinMode(echo2,INPUT);
  pinMode(red,OUTPUT);
  pinMode(blue,OUTPUT);
  pinMode(sw1,INPUT_PULLUP);
  pinMode(buzz,OUTPUT);
  pinMode(sensorPin,INPUT);
}
void distance1_check(){
  digitalWrite(trig1,LOW);
  delayMicroseconds(2);
  digitalWrite(trig1,HIGH);
  delayMicroseconds(2);
  digitalWrite(trig1,LOW); 
}
void distance2_check(){
  digitalWrite(trig2,LOW);
  delayMicroseconds(2);
  digitalWrite(trig2,HIGH);
  delayMicroseconds(2);
  digitalWrite(trig2,LOW);
  
  }
  void loop(){
  byte state = digitalRead(sensorPin);
  distance1_check();
  int distance1 = pulseIn(echo1,HIGH)*17/1000;
  delay(500);
  distance2_check();
  int distance2 = pulseIn(echo2,HIGH)*17/1000;
  Serial.print(distance1);
  Serial.println(" :cm");
  Serial.print(distance2);
  Serial.println(" :cm");
  Serial.println("------------------------------------");
  if(bluetooth.available())
  {
    cmd=bluetooth.read();
    switch(cmd)
    {
      case'1':
      bReady=true;
      check = 0;
      break;
      case'2':
      bReady=false;
      break;
    }
  }
  delay(100);
 if(bReady){
    cmd=bluetooth.read();
    if(state == 1||cmd =='3'){
     bluetooth.write('a');
     Serial.println("Somebody is in this area!");
     while(digitalRead(sw1)==HIGH){
        tone(buzz,freq,10);
        if(freqFlag == true) freq += 2;
        if(freq >= 1800) freqFlag = false;
        if(freqFlag == false) freq -= 2;
        if(freq <= 150) freqFlag = true;
        //여기까지 부저 사이렌 소리(freq를 150 ~ 1800까지 왔다갔다 거리면서 소리냄)
        
        unsigned long currentMillis = millis();
        if(currentMillis - previousMillis >= interval) {
          previousMillis = currentMillis;
   
          if (whichLED == 0 && redState == LOW) redState = HIGH;
          else redState = LOW;
   
          if (whichLED == 1 && blueState == LOW) blueState = HIGH;
          else blueState = LOW;
        }
 
        if(currentMillis - switch_previousMillis >= switch_interval) {
          switch_previousMillis = currentMillis;
   
          if (whichLED == 0) whichLED = 1;
          else whichLED = 0;
        }
        //여기까지 경광등
      digitalWrite(red, redState);
      digitalWrite(blue, blueState);
      }
      noTone(buzz);
      cmd = '2';
      bluetooth.write('b');
      digitalWrite(red,LOW);
      digitalWrite(blue,LOW);
      digitalWrite(sw1,HIGH);
      delay(5000); //스위치를 누른 후 2초뒤 다시 loop실행
      
    }
    else if((distance1 <= save || distance2 <= save)){
      delay(500);
      distance1_check();
      distance1 = pulseIn(echo1,HIGH)*17/1000;
      delay(500);
      distance2_check();
      distance2 = pulseIn(echo2,HIGH)*17/1000;
      if((distance1 <= save || distance2 <= save)&&check==0){
     bluetooth.write('e');
     Serial.println("Something is in this area!");
     check=1;
      }
    }
    else{
    Serial.println("No one!");
    digitalWrite(red,LOW);
    digitalWrite(blue,LOW);
   }
 }
  if(!bReady)
  {
    if(((distance1 <= save || distance2 <= save) && state == 1) || (state == 1)){
     bluetooth.write('c');
    }
    else{
     bluetooth.write('d');
    }
  }
}
