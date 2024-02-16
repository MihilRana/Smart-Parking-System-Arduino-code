#include <ArduinoJson.h>
#include <ArduinoJson.hpp>
#include <ESP8266WiFi.h>
#include <FirebaseArduino.h>
#include <Servo.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define FIREBASE_HOST "##########"
#define FIREBASE_AUTH "##########"
#define WIFI_SSID "##########"
#define WIFI_PASSWORD "##########"

#define ir_slot1 D5
#define ir_slot2 D6
#define ir_slot3 D7
#define ir_slot4 D8
#define ir_entrance D3
#define ir_exit D4

LiquidCrystal_I2C lcd(0x27,16,2);  
/*for lcd i2c connect SDA to D2 and SCL to D1*/

Servo myservo;
int openAngle = 90;
int closeAngle = 0;

int S1 = 0, S2 = 0, S3 = 0, S4 = 0;
int flag1 = 0, flag2 = 0;
int slot = 4;

void setup() {
  Serial.begin(9600);
  
  pinMode(ir_slot1, INPUT);
  pinMode(ir_slot2, INPUT);
  pinMode(ir_slot3, INPUT);
  pinMode(ir_slot4, INPUT);
  pinMode(ir_entrance, INPUT);
  pinMode(ir_exit, INPUT);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH); // Initialize Firebase within setup

  myservo.attach(3); //change pin
  myservo.write(90);

  lcd.init();
  lcd.backlight();
  lcd.begin(16, 2);

  Read_Sensor();
 
  int total = S1 + S2 + S3 + S4;
  slot = slot - total;
}

void loop() {
  Read_Sensor();
  
  //lcd display
  lcd.setCursor(0, 0);
  if(S1 == 1){
    lcd.print("S1:Full ");
  }else{
    lcd.print("S1:Empty");
  }

  lcd.setCursor(8, 0);
  if(S2 == 1){
    lcd.print("S2:Full ");
  }else{
    lcd.print("S2:Empty");
  }

  lcd.setCursor(0, 1);
  if(S3 == 1){
    lcd.print("S3:Full ");
  }else{
    lcd.print("S3:Empty");
  }

  lcd.setCursor(8, 1);
  if(S4 == 1){
    lcd.print("S4:Full ");
  }else{
    lcd.print("S4:Empty");
  }

  //gate commands and display
  if (digitalRead(ir_entrance) == 0 && flag1 == 0) {
    if (slot > 0) {
      flag1 = 1;
      if (flag2 == 0) {
        myservo.write(180);
        slot = slot - 1;
      }
    } else {
      lcd.setCursor(0, 0);
      lcd.print(" Sorry Parking Full ");
      delay(1500);
    }
  }
 
  if (digitalRead(ir_exit) == 0 && flag2 == 0) {
    flag2 = 1;
    if (flag1 == 0) {
      myservo.write(180);
      slot = slot + 1;
    }
  }
 
  if (flag1 == 1 && flag2 == 1) {
    delay(1000);
    myservo.write(90);
    flag1 = 0, flag2 = 0;
  }

  delay(1000);
}

//IR sensor read
void Read_Sensor() {
  S1 = 0, S2 = 0, S3 = 0, S4 = 0;
 
  if (digitalRead(ir_slot1) == 0) {
    S1 = 1;
    int sensorV1 = ir_slot1;
    Firebase.setString("irSensorValue/Value1", String(sensorV1));
    Serial.print("IR sensor value1: ");
    Serial.println(sensorV1);
  }

  if (digitalRead(ir_slot2) == 0) {
    S2 = 1;
    int sensorV2 = ir_slot2;
    Firebase.setString("irSensorValue/Value2", String(sensorV2));
    Serial.print("IR sensor value2: ");
    Serial.println(sensorV2);
  }

  if (digitalRead(ir_slot3) == 0) {
    S3 = 1;
    int sensorV3 = ir_slot3;
    Firebase.setString("irSensorValue/Value3", String(sensorV3));
    Serial.print("IR sensor value3: ");
    Serial.println(sensorV3);
  }

  if (digitalRead(ir_slot4) == 0) {
    S4 = 1;
    int sensorV4 = ir_slot4;
    Firebase.setString("irSensorValue/Value4", String(sensorV4));
    Serial.print("IR sensor value4: ");
    Serial.println(sensorV4);
  }

  delay(500);
}

//servor functions
void openBarrier() {
  myservo.write(openAngle);
  delay(1000); 
}

void closeBarrier() {
  myservo.write(closeAngle);
  delay(1000); 
}
