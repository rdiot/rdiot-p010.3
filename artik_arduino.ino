/* Samsung ARTIK 5 ARTIK Cloud using the Arduino IDE (REST) [P010.3] : http://rdiot.tistory.com/20 [RDIoT Demo] */

#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <WiFi.h>
#include <ArduinoJson.h>
#include <DebugSerial.h>
 
LiquidCrystal_I2C lcd(0x27,20,4);  // LCD2004
const int TMP36_pin = A0;
int led = 13; // LED

// Web REST API params
char server[] = "api.artik.cloud";  
int port = 443; //(port 443 is default for HTTPS)

String AuthorizationData = "Authorization: Bearer 363f74bd40d74fd882d7d9f5bf72460a";

float insTemp, blndOpen, openBlndCtrl;
//String PostData = "{\n  \"sdid\": \"_your_DEVICE_ID_goes_here_\",\n  \"type\":   \"message\",\n  \"data\": \{\"insTemp\":19.3,  \"desTemp\":19.1, \"outTemp\":23.7 \}}";  
char buf[200];

WiFiSSLClient client;
 
void setup()
{

  DebugSerial.begin(9600);  

  lcd.init();  // initialize the lcd 
  lcd.backlight();
  lcd.print("start LCD2004");
 
  delay(10000);
 
  lcd.clear();
}
 
void loop()
{
 
  lcd.setCursor(0,0);
  lcd.print("ARTIK TMP36 SENSOR");
 
  int sensorValue = analogRead(TMP36_pin);  
  //float mVoltage = sensorValue * 5000.0/1024.0;
  float mVoltage = sensorValue * 2 * 0.439453125; //ARTIK Voltage
  float temperatureC = (mVoltage - 500) / 10.0;  
 
  lcd.setCursor(0,1);
  lcd.print("temperature=" + (String)temperatureC + "'C");



 client.connect(server, port);
  if (!client.connected()) { 
  ; // service the error 
  } else {
      client.println("POST /v1.1/messages HTTP/1.1");
      client.println("Host: api.artik.cloud");
      client.println("Accept: */*");
      client.println("Content-Type: application/json");
      client.println(AuthorizationData);

      // POST data section
      client.print("Content-Length: ");

      if(temperatureC > 40)
      {
        digitalWrite(led, HIGH);
        client.println(loadBuffer(temperatureC,true,true)); // loads buf, returns length
        lcd.setCursor(0,3);
        lcd.print("status:HIGH ");
      }
      else if (temperatureC < 30)
      {
        digitalWrite(led, LOW);
        client.println(loadBuffer(temperatureC,false,false)); // loads buf, returns length
        lcd.setCursor(0,3);
        lcd.print("status:LOW  ");
      }
      else
      {
        digitalWrite(led, LOW);
        client.println(loadBuffer(temperatureC,false,true)); // loads buf, returns length
        lcd.setCursor(0,3);
        lcd.print("status:SOSO ");
      }
      
      
      client.println();
      client.println(buf);

 }

    
    DebugSerial.println("buffer==>" + (String)buf);
  delay(2000);
  
}

 int loadBuffer(float insTemp, bool blndOpen, bool openBlndCtrl) {  
   StaticJsonBuffer<200> jsonBuffer; // reserve spot in memory

   JsonObject& root = jsonBuffer.createObject(); // create root objects
     root["sdid"] = "3fd9716e2cdc4739808347c8db3f2aa9"; // FIX 
     root["type"] = "message";

   JsonObject& dataPair = root.createNestedObject("data"); // create nested objects
     dataPair["insTemp"] = insTemp;  
     dataPair["blndOpen"] = blndOpen;  
     dataPair["openBlndCtl"] = openBlndCtrl;  

   root.printTo(buf, sizeof(buf)); // JSON-print to buffer

   return (root.measureLength()); // also return length
 }
 
