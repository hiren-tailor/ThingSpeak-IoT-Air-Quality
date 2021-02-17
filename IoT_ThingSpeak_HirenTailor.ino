#include <ESP8266WiFi.h>
#include <LiquidCrystal.h>  // include the library code:

// initialize the library by associating any needed LCD interface pin
// with the arduino pin number it is connected to
const int rs = D5, en = D4, d4 = D3, d5 = D2, d6 = D1, d7 = D0;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

const int aqsensor = A0;  //output of mq135 connected to A0 pin of ESP-8266

int gled = D6;  //green led connected to pin 6
int rled = D7;  //red led connected to pin 7
int buz = D8;   //buzzer led connected to pin 8

String apiKey = "XXXXXXXXXXXXXXXX";     //  Enter your Write API key here
const char *ssid =  "XXXXX";     // Enter your WiFi Name
const char *pass =  "XXXXXXXX"; // Enter your WiFi Password
const char* server = "api.thingspeak.com";
WiFiClient client;

void setup() {
  // put your setup code here, to run once:
  pinMode (gled,OUTPUT);     // gled is connected as output from ESP-8266
  pinMode (aqsensor,INPUT); // MQ135 is connected as INPUT to ESP-8266
  pinMode (rled,OUTPUT);
  pinMode (buz,OUTPUT);
  
  Serial.begin (115200);      //begin serial communication with baud rate of 115200

  lcd.clear();              // clear lcd
  lcd.begin (16,2);         // consider 16,2 lcd

  Serial.println("Connecting to ");
  lcd.print("Connecting.... ");
  Serial.println(ssid);
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
    Serial.print(".");
    lcd.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP Address: ");
  delay(5000);
  Serial.println(WiFi.localIP());
  delay(5000);
}

void loop() {
  // put your main code here, to run repeatedly:

    int ppm = analogRead(aqsensor); //read MQ135 analog outputs at A0 and store it in ppm

  Serial.print("Air Quality: ");  //print message in serail monitor
  Serial.println(ppm);            //print value of ppm in serial monitor

  lcd.setCursor(0,0);             // set cursor of lcd to 1st row and 1st column
  lcd.print("Air Quality: ");      // print message on lcd
  lcd.print(ppm);                 // print value of MQ135
  delay(1000);

  if (client.connect(server,80))   
                      {  
                             String postStr = apiKey;
                             postStr +="&field1=";
                             postStr += String(ppm);
                             postStr += "\r\n\r\n";
 
                             client.print("POST /update HTTP/1.1\n");
                             client.print("Host: api.thingspeak.com\n");
                             client.print("Connection: close\n");
                             client.print("X-THINGSPEAKAPIKEY: "+apiKey+"\n");
                             client.print("Content-Type: application/x-www-form-urlencoded\n");
                             client.print("Content-Length: ");
                             client.print(postStr.length());
                             client.print("\n\n");
                             client.print(postStr);
                             Serial.print("Air Quality: ");
                             Serial.print(ppm);
                             Serial.println(" PPM.Send to Thingspeak.");
                        }
  if(ppm <= 130)
  {
    digitalWrite(gled,LOW);   //jump here if ppm is not greater than threshold and turn off gled
    digitalWrite(rled,LOW);
    digitalWrite(buz,LOW);   //Turn off Buzzer
    lcd.setCursor(1,1);
    lcd.print ("AQ Level Normal");
    Serial.println("AQ Level Normal");
  }
  else if (ppm > 130 && ppm < 250)
  {
    digitalWrite(gled,HIGH);   //jump here if ppm is not greater than threshold and turn off gled
    digitalWrite(rled,LOW);
    digitalWrite(buz,LOW);   //Turn off Buzzer
    lcd.setCursor(1,1);
    lcd.print ("AQ Level Medium");
    Serial.println("AQ Level Medium");
  }
  else
  {
    lcd.setCursor(1,1);         //jump here if ppm is greater than threshold
    lcd.print("AQ Level Danger!");
    Serial.println("AQ Level Danger!");     
    digitalWrite(gled,LOW);
    digitalWrite(rled,HIGH);
    digitalWrite(buz,HIGH);     //Turn ON Buzzer
  }

          client.stop();
          Serial.println("Waiting...");
          delay(1000);

}
