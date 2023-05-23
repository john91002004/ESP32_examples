/* ***** Bluetooth Slave mode
 *     When polled, return all the data in a certain sequence. 
 */
 
#include "BluetoothSerial.h"
BluetoothSerial SerialBT; 

String name = "ESP_02"; 
bool connected_flag = false;

void setup() {
  Serial.begin(115200); 
  SerialBT.begin(name); 

  Serial.println(name + " is ready!" ); 
}

void loop() {
  if (SerialBT.connected()) {
    if (!connected_flag) {
      String sendData = "Hi, this is " + name + "!\n"; 
      SerialBT.write( (const uint8_t*) sendData.c_str(), sendData.length() );
      Serial.println(sendData);
      connected_flag = true; 
    }

    String s; 
    while (SerialBT.available()) {
      s = SerialBT.readString(); 
    }
    if (s == "OK!") {
      String ss = "I received OK!\n"; 
      SerialBT.write( (const uint8_t*) ss.c_str(), ss.length() ); 
      Serial.println("You received " + s); 
    }
    
  } else {
    if (connected_flag) {
      Serial.println("Disconnected."); 
    } 
    connected_flag = false ; 
  }
  delay(1000); 
}
