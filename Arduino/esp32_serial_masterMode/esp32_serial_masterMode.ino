/* ***** Master mode for Bluetooth communication *****
 *     This code allows you to poll a list of bluetooth defined by name instead of address, 
 * and receive data from them.
 */

/*
 * Problem 1: I can't reduce time period of SerialBT.connect() <- 30 secs 
 */

#include "BluetoothSerial.h"

#if !defined(CONFIG_BT_SPP_ENABLED)
#error "Serial Bluetooth not available or not enabled."
#endif 

BluetoothSerial SerialBT; 

String MACadd = "AA:BB:CC:11:22:33";
uint8_t address[6] = {0xAA, 0xBB, 0xCC, 0x11, 0x22, 0x33}; 
String name[5] = {"ESP_02", "ESP_03", "ESP_04", "ESP_05", "ESP_06"};
const char *pin = "1234"; 
bool connected, disconnected;

void setup() {
  Serial.begin(115200); 
  SerialBT.begin("ESP_01", true); 
  Serial.println("The device ESP_01 started in master mode, make sure remote BT device is on!");

  // connect(address) is fast (upto 10 secs max), connect(name) is slow (upto 30 secs max) as it needs to resolve name to address
  /*connected = SerialBT.connect(name); 

  if (connected){
    Serial.println("Connected successfully!"); 
  } else {
    while (!SerialBT.connected(10000)) {
      Serial.println("Failed to connect. Make sure remote device is available and in range then restart app.");
    }
  }
*/
  // Disconnect 
  /*
   * if (SerialBT.disconnect()){
   *  Serial.println("Disconnect Successfully!");
   * }
   */
}

void loop() {
  for (int i=0;i<2;i++){
    connected = SerialBT.connect(name[i]); 
    Serial.print(name[i]); 
    
    if (connected){  
      Serial.println(" connected successfully!"); 
      
      String OK = "OK!"; 
      SerialBT.write( (const uint8_t*) OK.c_str(), OK.length() );
      
      delay(5000);
      if (SerialBT.available()) {
        Serial.print(name[i]); 
        Serial.print(" said: "); 
      }
      while (SerialBT.available()){
        Serial.write(SerialBT.read());
      }
      
      disconnected = SerialBT.disconnect(); 
      if (disconnected) {
        Serial.print(name[i]); 
        Serial.println(" disconnected.");
      }
    }
    else {
      Serial.println(" connection failed."); 
    }
    delay(1000); 
  }
}
