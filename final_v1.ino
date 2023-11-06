#define BLYNK_TEMPLATE_ID 
#define BLYNK_TEMPLATE_NAME "HomeIOT"
#define BLYNK_AUTH_TOKEN 
//#define BLYNK_PRINT Serial

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <LiquidCrystal_I2C.h>
#include <Servo.h>


LiquidCrystal_I2C lcd(0x27, 16, 2);
Servo myservo;

#define IR_PIN D6
#define TEMPER_PIN A0
#define SERVO_PIN D3

BlynkTimer timer;
int doorPresentState = 0;
float prev_temp, current_temp;
int masterDoor = 0;

void myTimerEvent(){
  prev_temp = analogRead(TEMPER_PIN);
  prev_temp = prev_temp * 330;
  prev_temp = prev_temp / 1024.0;
  delay(500);
  current_temp = analogRead(TEMPER_PIN);
  current_temp = current_temp * 330;
  current_temp = current_temp / 1024.0;
  Serial.println(current_temp);
  Serial.println("\n");
  Blynk.virtualWrite(V3, current_temp);
  if(current_temp >= 32 && prev_temp >= 32){
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("WARNING!! Temperature");
    lcd.setCursor(0,1);
    lcd.print("Level Exceeded!!");
  }
}

BLYNK_WRITE(V4){
  Serial.println("MasterCalled!!");
  masterDoor = param.asInt();
  myservo.write(masterDoor * 180);
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Welcome Sam!");
  lcd.setCursor(0,1);
  lcd.print("It is ");
  lcd.setCursor(6,1);
  lcd.print(current_temp);
}

BLYNK_WRITE(V2){
  int serverState = param.asInt();
  if(serverState == 1 && masterDoor == 0){
    myservo.write(180);
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Welcome Sam!");
    lcd.setCursor(0,1);
    lcd.print("It is ");
    lcd.setCursor(6,1);
    lcd.print(current_temp);
    int iRPres, iRNext;
    for(int lol=0;lol <= 10;lol++){
      iRPres = digitalRead(IR_PIN);
      delay(1500);
      iRNext = digitalRead(IR_PIN);
      Serial.println(" ");
      Serial.println(iRPres);
      Serial.println(" ");
      Serial.println(iRNext);
      if(iRPres == iRNext && iRPres == 0){
        doorPresentState = 0;
        myservo.write(0);
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Gate has been");
        lcd.setCursor(0,1);
        lcd.print("closed autom.");
        Blynk.virtualWrite(V2, doorPresentState);
        break;
      }    
    }
    doorPresentState = 0;
    myservo.write(0);
    Blynk.virtualWrite(V2, doorPresentState);
  }
}

void setup(){
  pinMode(IR_PIN, INPUT);
  myservo.attach(SERVO_PIN);
  myservo.write(0);
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0,0);
  lcd.clear();
  lcd.print("Connecting...");
  Serial.begin(9600);
  pinMode(A0, INPUT);
  Blynk.begin(BLYNK_AUTH_TOKEN, "Penguin", "not12345678");
  lcd.clear();
  lcd.print("Connected!");
  delay(750);
  timer.setInterval(750L, myTimerEvent);
}

void loop(){
  Blynk.run();
  timer.run();
}