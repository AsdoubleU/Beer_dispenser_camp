#include <ESP8266WiFi.h> //와이파이 헤더파일 지정 
#include <Arduino.h>
#include <U8g2lib.h>
#include <Wire.h> //OLED 헤더파일 지정 
#include <DHT.h> //온습도 센서 헤더파일 지정 
#include <SoftwareSerial.h> //TX,RX핀 새로지정 헤더파일 지정 

SoftwareSerial Mega(D2, D8);


int StarwarsDuration[] = {
  8, 8, 8, 2, 2, 8, 8, 8, 2, 4, 8, 8, 8, 2, 4, 8, 8, 8, 4,

  4, 8, 2, 2, 8, 8, 8, 2, 4, 8, 8, 8, 2, 4, 8, 8, 8, 4, 4
};

int noteDurations[] = {
  4, 4, 4, 2
};


#ifdef U8X8_HAVE_HW_I2C
#endif
U8G2_SSD1306_128X64_NONAME_1_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);
#define DHTTYPE DHT11


const char* ssid = "YOUR_SSID";
const char* password = "YOUR_PASSWORD"; // 공유기 id와 비밀번호를 전역포인터로 설정

WiFiServer server(80);
DHT Therm(D5, DHTTYPE);

void setup() {

  pinMode(D7, INPUT);
  Serial.begin(115200);
  u8g2.begin();
  Therm.begin();
  Mega.begin(300);
  delay(10);


  Serial.println();
  Serial.print("Connecting to ");
  Serial.print(ssid);
  u8g2.firstPage();
  do {
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_6x10_mf);
    u8g2.drawStr(0, 10, "Connecting to ");
    u8g2.drawStr(0, 20, ssid);
  } while ( u8g2.nextPage() );


  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(400);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("Connected!");


  server.begin();
  Serial.println("Server started");

  Serial.println(WiFi.localIP());

  u8g2.firstPage();
  do {
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_6x10_mf);
    u8g2.drawStr(34, 10, "Connected!");
    u8g2.drawStr(35, 30, "IP Number");
    u8g2.setCursor(30, 40);
    u8g2.print(WiFi.localIP());
  } while ( u8g2.nextPage() );

}

void loop() {

  /*
  int thisNote = 0;

  if (digitalRead(D7) == HIGH) {
    delay(1000);
    while (digitalRead(D7) == LOW) {
      int noteDuration = 1000 / StarwarsDuration[thisNote];
      tone(D6, Starwars[thisNote], noteDuration);
      int pauseBetweenNotes = noteDuration * 1.30;
      delay(pauseBetweenNotes);
      noTone(D6);
      thisNote++;
      if (thisNote == 40) {
        thisNote = 0;
      }
    }

    delay(100);

    if (Mega.read() == '2') {
        int noteDuration = 1000 / noteDurations[thisNote];
        tone(D6, NOTE_C3, noteDuration);
        int pauseBetweenNotes = noteDuration * 1.30;
        delay(pauseBetweenNotes);
      }
      
     else {
      tone(D6,NOTE_A3,500);
      delay(550);  
      noTone(D6);   
      }
    }
    */

    WiFiClient client = server.available();
    if (!client) {
      return;
    }

    Serial.println("new client");
    while (!client.available()) {
      delay(1);
    }

    String req = client.readStringUntil('\r');
    Serial.println(req);
    client.flush();

    int got = 0;
    float t = Therm.readTemperature();

    if (req.indexOf("/beer") != -1) { // '/beer'라는 값이 서버 아이피 뒷자리면 릴레이 열기
      Mega.println(2);
      Mega.println(2);
      Mega.println(4);
      Mega.print(4);
      got = 1;
    }

    else if (req.indexOf("/random") != -1) {
      Mega.print(2);
      Mega.print(2);
      Mega.print(5);
      Mega.println(5);
      got = 1;
    }

    else if (req.indexOf("/fridgeon") != -1) {
      Mega.print(2);
      Mega.print(2);
      Mega.println(2);
      delay(10);
      got = 1;
    }

    else if (req.indexOf("/fridgeoff") != -1) {
      Mega.print(2);
      Mega.print(2);
      Mega.print(3);
      Mega.println(3);
      delay(10);
      got = 1;
    }

    else if (req.indexOf("/game") != -1) {
      Mega.print(1);
      Mega.print(1);
      Mega.println(1);
      got = 1;
    }

    else if (req.indexOf("/thermo") != -1) {

      u8g2.firstPage();
      do {
        u8g2.clearBuffer();
        u8g2.setFont(u8g2_font_6x10_mf);
        u8g2.drawStr(35, 30, "degree");
        u8g2.setCursor(40, 40);
        u8g2.print(t, 1);
      } while ( u8g2.nextPage() );

      delay(3000);

      u8g2.firstPage();
      do {
        u8g2.clearBuffer();
        u8g2.setFont(u8g2_font_6x10_mf);
        u8g2.drawStr(35, 30, "IP Number");
        u8g2.setCursor(30, 40);
        u8g2.print(WiFi.localIP());
      } while ( u8g2.nextPage() );

    }

    else {
      Serial.println("invalid request");
      Mega.println(9);
      client.stop();
      return;
    }

    client.flush();

    String s1 = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n \r\n\r\nSending to Mega is now ";
    s1 += (got) ? "succeed" : "failed";
    s1 += "\n";

    client.print(s1);
    delay(1);
    Serial.println("Client disonnected");
  }
