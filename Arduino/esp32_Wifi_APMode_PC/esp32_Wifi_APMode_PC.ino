#include<WiFi.h>
#include "ESPAsyncWebServer.h"

const char ssid[]="TestESP"; //修改為你家的WiFi網路名稱
const char pwd[]="88888888"; //修改為你家的WiFi密碼

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

String readStr() {
  return "You are good.";
}

void setup(){
  // Serial port for debugging purposes
  Serial.begin(115200);
  Serial.println();
  
  // Setting the ESP as an access point
  Serial.print("Setting AP (Access Point)…");
  // Remove the password parameter, if you want the AP (Access Point) to be open
  WiFi.softAP(ssid, pwd);

  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);

  server.on("/comm", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", readStr().c_str());
  });
  
  // Start server
  server.begin();
}
 
void loop(){
  
}
