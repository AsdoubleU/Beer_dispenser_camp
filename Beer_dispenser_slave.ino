
#include<SoftwareSerial.h>
#include <Adafruit_NeoPixel.h>

int TxPin = 10;
int RxPin = 11;
int neo = 38;
int redColor;
int greenColor;
int blueColor;
SoftwareSerial BTSerial(TxPin, RxPin);
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(30, neo, NEO_GRB + NEO_KHZ800);

#define fridge 6

int led[] = { 22, 24, 26, 28, 30, 32};

void setup() {
  randomSeed(analogRead(A0));
  pinMode(fridge, OUTPUT);
  pinMode(7, OUTPUT);
  pinMode(34,OUTPUT);
  pinMode(36,INPUT);
  for (int i = 2; i < 6; i++) {
    pinMode(i, OUTPUT);
  }
  for(int i=0;i<6;i++){
    pinMode(led[i],OUTPUT);
  }
  BTSerial.begin(300);
  Serial.begin(9600);
  pixels.begin();
  pixels.setBrightness(255);
}

void loop() {

  redColor = 255;
  greenColor = 255;
  blueColor = 255;

  for (int i=0; i < neo; i++){
    pixels.setPixelColor(i, pixels.Color(redColor, greenColor, blueColor));
  }
  pixels.show();

  if(BTSerial.available()) {
  
  if (BTSerial.read() == '2') {
    digitalWrite(fridge, HIGH);
    Serial.println(2);
    delay(100);
  }

  else if (BTSerial.read() == '3') {
    digitalWrite(fridge, LOW);
    Serial.println(3);
    delay(100);
  }

  else if (BTSerial.read() == '4') { //소맥
    digitalWrite(2,HIGH);
    digitalWrite(3,HIGH);
    delay(3000);
    digitalWrite(2,LOW);
    delay(4000);
    digitalWrite(3,LOW);
  }

  else if (BTSerial.read() == '0') {
    digitalWrite(random(4) + 2, HIGH);
    digitalWrite(random(4) + 2, HIGH);
    delay(1000);
    for (int i = 2; i < 6; i++) {
      digitalWrite(i, LOW);
    }
  }

 else if (BTSerial.read() == '9') {
  Serial.println(9);
  }

  else if (BTSerial.read() == '6') {

    delay(1000);
    digitalWrite(34,HIGH);
    
    int num = random(6);
    int count = 0;

    digitalWrite(led[num], HIGH);
    
    while (digitalRead(36) == LOW) {
      for (int i = 0; i < 6; i++) {
        digitalWrite(led[i], HIGH);
        delay(100);
        if (i != num) {
          digitalWrite(led[i], LOW);
        }
        delay(100);
        count = i;
      }
    }

    digitalWrite(34,LOW);
    delay(100);
    digitalWrite(34,HIGH);
    delay(100);
    digitalWrite(34,LOW);
    delay(100);
    digitalWrite(34,HIGH);
    delay(100);
    digitalWrite(34,LOW);
    
    
    if (count == num) {
    for (int p = 0; p < 7; p++) {
      digitalWrite(led[count], HIGH);
      delay(50);
      digitalWrite(led[count], LOW);
      delay(50);
    }
     
  }
  else {
    for (int p = 0; p < 7; p++) {
      digitalWrite(led[count], HIGH);
      digitalWrite(led[num], HIGH);
      delay(50);
      digitalWrite(led[count], LOW);
      digitalWrite(led[num], LOW);
      delay(50);
    }
  }
  int alchole = random(4);
  digitalWrite(alchole+2, HIGH);

  digitalWrite(led[1], HIGH);
  digitalWrite(led[2], HIGH);
  digitalWrite(led[3], HIGH);
  digitalWrite(led[4], HIGH);
  digitalWrite(led[5], HIGH);
  digitalWrite(led[6], HIGH);
  delay(1000);

  digitalWrite(alchole+2, LOW);
    delay(1000);

    digitalWrite(led[1], LOW);
    digitalWrite(led[2], LOW);
    digitalWrite(led[3], LOW);
    digitalWrite(led[4], LOW);
    digitalWrite(led[5], LOW);
    digitalWrite(led[6], LOW);
    delay(500);
  
}
  }

  else if (digitalRead(36) == HIGH) {

    delay(1000);
    digitalWrite(34,HIGH);//buttonled
    
    int num = random(6);
    int count = 0;

    digitalWrite(led[num], HIGH);
    
    while (digitalRead(36) == LOW) {
      for (int i = 0; i < 6; i++) {
        digitalWrite(led[i], HIGH);
        delay(100);
        if (i != num) {
          digitalWrite(led[i], LOW);
        }
        delay(100);
        count = i;
      }
    }

    digitalWrite(34,LOW);
    delay(100);
    digitalWrite(34,HIGH);
    delay(100);
    digitalWrite(34,LOW);
    delay(100);
    digitalWrite(34,HIGH);
    delay(100);
    digitalWrite(34,LOW);
    
    
    if (count == num) {
     BTSerial.println(2);
    for (int p = 0; p < 7; p++) {
      digitalWrite(led[count], HIGH);
      delay(50);
      digitalWrite(led[count], LOW);
      delay(50);
    }
  }
  else {
    for (int p = 0; p < 7; p++) {
      digitalWrite(led[count], HIGH);
      digitalWrite(led[num], HIGH);
      delay(50);
      digitalWrite(led[count], LOW);
      digitalWrite(led[num], LOW);
      delay(50);
    }
  }
  int alchole = random(4);
  digitalWrite(alchole+2, HIGH);

  digitalWrite(led[1], HIGH);
  digitalWrite(led[2], HIGH);
  digitalWrite(led[3], HIGH);
  digitalWrite(led[4], HIGH);
  digitalWrite(led[5], HIGH);
  digitalWrite(led[6], HIGH);
  delay(1000);

  digitalWrite(alchole+2, LOW);
    delay(1000);

    digitalWrite(led[1], LOW);
    digitalWrite(led[2], LOW);
    digitalWrite(led[3], LOW);
    digitalWrite(led[4], LOW);
    digitalWrite(led[5], LOW);
    digitalWrite(led[6], LOW);
    delay(500);
  
}
}
  
