/*----- Xin Wu Environment Monitoring Project -----
 *   This Porject is multiple use for monitoring machine room. 
 *   Every function has been packed as functions. You can choose whatever you want. 
 *   These function will include sensor functions and communication functions. 
 *   -----
 *   Basically, we suggest you use bluetooth communication between ESP32s, and use wifi communication between ESP32 and PC(or server).
 *   -----
 *   So, the toppology we design in the beginning is: 
 *       All sensor ESP32s will be in bluetooth Slave mode and wait to be polled by a Master ESP32. 
 *       And this Master ESP32 has to poll all the slave ESP32s and return data gathered to PC through wifi.
 *   -----
 *   On the side of PC, we have a python script to poll Master ESP32 through wifi devices. 
 * ----------  
 * Sensor Features: 
 *   Humidity and Temperature
 *   Photosensor
 *   Ultra-sound distance detector 
 *   Moving detector through Infra-red
 *   Water sensor
 *(x)Camera    
 * ----------
 * Communication Features:
 *   Wifi 
 *   Bluetooth  
 */

#include <WiFi.h>
#include "ESPAsyncWebServer.h"
#include <SimpleDHT.h>

// Constants and Declarations
int DELAY_MILLI = 100 ;
String dataStr(); 
void DHT_Poll(); 
void water_level_Poll(); 
void light_sensor_Poll();
void moving_detect_Poll(); 
void dist_detect_Poll(); 

// Variables 
byte temp; 
byte humi; 
int water_level_val;
int light_val; 
int movingDetector_val; 
String moving_str; 
unsigned long distance; 


// DHT initialization 
int pinDHT11 = 15; 
SimpleDHT11 dht; 

// Water Sensor Init
int water_sensor = 36; 

// Light Sensor
int light_sensor = 4 ; 

// MOviing Detector (InfraRed Sensor) Init
int inPin = 18 ; 

// Distance Detector(ULtra-Sound) Init
int trigPin = 17; 
int echoPin = 16; 


// Wifi Initialization 
// Default IP: 192.168.4.1
const char ssid[]="ESP_00"; //修改為你家的WiFi網路名稱
const char pwd[]="88888888"; //修改為你家的WiFi密碼
// Create AsyncWebServer object on port 80
AsyncWebServer server(80);


void setup(){
  // Serial port for debugging purposes
  Serial.begin(115200);
  Serial.println();
  
  // Setting the ESP as an access point
  Serial.print("Setting AP (Access Point)…");
  WiFi.softAP(ssid, pwd);

  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);

  server.on("/comm", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", dataStr().c_str());
  });
  
  // Start server
  server.begin();

  // Sensor Init
  pinMode(water_sensor, INPUT); 
  pinMode(light_sensor, INPUT); 
  pinMode(inPin, INPUT); 
  pinMode(trigPin, OUTPUT); 
  pinMode(echoPin, INPUT); 
  
}
 
void loop(){
  DHT_Poll(); 
  delay(50); 
  water_level_Poll(); 
  delay(50); 
  light_sensor_Poll(); 
  delay(50); 
  moving_detect_Poll();
  delay(50); 
  dist_detect_Poll();
  delay(1000); 
}

void DHT_Poll() {
  int err = SimpleDHTErrSuccess; 
  err = dht.read(pinDHT11, &temp, &humi, NULL);
  if (err != SimpleDHTErrSuccess) {
    Serial.print("Read DHT11 failed, err="); 
    Serial.println(err); 
    delay(DELAY_MILLI); 
  } 
  else {
    Serial.print("Humi = ") ; 
    Serial.print((float)humi); 
    Serial.print("%, Temp = "); 
    Serial.print((float)temp); 
    Serial.println("C "); 
  }
}

void water_level_Poll() {  // 2500 up -> too high; 800 ~ 2500 -> medium; 0~ 800 -> low
  water_level_val = analogRead(water_sensor); 
  Serial.print("Water level: ");
  Serial.println(water_level_val);  
}

void light_sensor_Poll() {
  light_val = analogRead(light_sensor); 
  Serial.print("light_sensor got value: "); 
  Serial.println(light_val);
}

void moving_detect_Poll(){
  movingDetector_val = digitalRead(inPin);
  if (movingDetector_val == HIGH) {
    moving_str = "Somebody is moving!!!"; 
    Serial.println(moving_str) ; 
  }
  else {
    moving_str = "Nobody is moving!!!" ; 
    Serial.println(moving_str); 
  } 
}

void dist_detect_Poll() {
  distance = ping()/58; // Calculate distance
  Serial.print("Closest object: ");
  Serial.print(distance); 
  Serial.println("cm"); 
}

unsigned long ping(){
  digitalWrite(trigPin, HIGH);  // create 10 microseconds duration pulse 
  delayMicroseconds(10); 
  digitalWrite(trigPin, LOW); 
  return pulseIn(echoPin, HIGH); // pulseIn function is built-in. Calculate the duration from LOW to HIGH at echoPin. 
}

String dataStr() {
  float h = (float)humi; 
  float t = (float)temp; 
  String ht = "Humi = " + (String)h + ", Temp = " + (String)t + "\n"; 
  String wl = "Water level = " + (String)water_level_val + "\n"; 
  String ls = "Light value = " + (String)light_val + "\n" ; 
  String md = moving_str + "\n" ; 
  String dd = "Closest object is " + (String)distance + " cm away." + "\n"; 
  return ht + wl + ls + md + dd;
}
