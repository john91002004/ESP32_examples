/*
    Video: https://www.youtube.com/watch?v=oCMOYS71NIU
    Based on Neil Kolban example for IDF: https://github.com/nkolban/esp32-snippets/blob/master/cpp_utils/tests/BLE%20Tests/SampleNotify.cpp
    Ported to Arduino ESP32 by Evandro Copercini
    updated by chegewara

   Create a BLE server that, once we receive a connection, will send periodic notifications.
   The service advertises itself as: 4fafc201-1fb5-459e-8fcc-c5c9c331914b
   And has a characteristic of: beb5483e-36e1-4688-b7f5-ea07361b26a8

   The design of creating the BLE server is:
   1. Create a BLE Server
   2. Create a BLE Service
   3. Create a BLE Characteristic on the Service
   4. Create a BLE Descriptor on the characteristic
   5. Start the service.
   6. Start advertising.

   A connect hander associated with the server starts a background task that performs notification
   every couple of seconds.
*/
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include <string.h>

BLEServer* pServer = NULL;
BLECharacteristic* p1 = NULL, *p2 = NULL, *p3 = NULL;
int deviceConnected = 0;
bool oldDeviceConnected = false;
uint32_t value = 0;
char* s[] = {"This is p1: ", "This is p2: ", "URLGREAT", "ROCF", "ISO27001 is good!"} ; 

// See the following for generating UUIDs:
// https://www.uuidgenerator.net/

#define SERVICE_UUID "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define C1_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"
#define C2_UUID "5a95d12e-fa74-48d7-b473-e644e750e7be"
#define C3_UUID "6a8ab204-228d-44d4-9162-2e743e3a02b3"

class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      deviceConnected++;
      Serial.print("We get one more connected. Totol connection: ");
      Serial.println(deviceConnected); 
      BLEDevice::startAdvertising(); 
    };

    void onDisconnect(BLEServer* pServer) {
      deviceConnected--;
      Serial.print("We get one less connected. Totol connection: ");
      Serial.println(deviceConnected); 
      pServer->startAdvertising();  
      Serial.println("restart advertising");
    }
};



void setup() {
  Serial.begin(115200);

  // Create the BLE Device
  BLEDevice::init("ESP32_notify_server");

  // Create the BLE Server
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  // Create the BLE Service
  BLEService *pService = pServer->createService(SERVICE_UUID);

  // Create a BLE Characteristic
  p1 = pService->createCharacteristic(
                      C1_UUID,
                      BLECharacteristic::PROPERTY_READ   |
                      BLECharacteristic::PROPERTY_WRITE  |
                      BLECharacteristic::PROPERTY_NOTIFY |
                      BLECharacteristic::PROPERTY_INDICATE
                    );
  
  p2 = pService->createCharacteristic(
                      C2_UUID,
                      BLECharacteristic::PROPERTY_READ   |
                      BLECharacteristic::PROPERTY_WRITE  |
                      BLECharacteristic::PROPERTY_NOTIFY |
                      BLECharacteristic::PROPERTY_INDICATE
                    );

  p3 = pService->createCharacteristic(
                      C3_UUID,
                      BLECharacteristic::PROPERTY_READ   |
                      BLECharacteristic::PROPERTY_WRITE  |
                      BLECharacteristic::PROPERTY_NOTIFY |
                      BLECharacteristic::PROPERTY_INDICATE
                    );

  // https://www.bluetooth.com/specifications/gatt/viewer?attributeXmlFile=org.bluetooth.descriptor.gatt.client_characteristic_configuration.xml
  // Create a BLE Descriptor
  p1->addDescriptor(new BLE2902());
  p2->addDescriptor(new BLE2902());
  p3->addDescriptor(new BLE2902());

  // Start the service
  pService->start();

  // Start advertising
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(false);
  pAdvertising->setMinPreferred(0x0);  // set value to 0x00 to not advertise this parameter
  BLEDevice::startAdvertising();
  Serial.println("Waiting a client connection to notify...");
}

void loop() {
    // notify changed value
    if (deviceConnected) {
      if (p1->getLength() != 4 ) {    // the value of p1->setValue() will be set to p1->getValue() which is weird, so kill it
        Serial.print("p1 got message with length ");
        Serial.print(p1->getLength());
        Serial.print(": "); 
        Serial.println(p1->getValue().c_str()); 
      }
      if (p2->getLength() != 4 ) {    // the value of p1->setValue() will be set to p1->getValue() which is weird, so kill it
        Serial.print("p2 got message with length ");
        Serial.print(p2->getLength());
        Serial.print(": "); 
        Serial.println(p2->getValue().c_str()); 
      }
      if (p3->getLength() != 4 ) {    // the value of p1->setValue() will be set to p1->getValue() which is weird, so kill it
        Serial.print("p3 got message with length ");
        Serial.print(p3->getLength());
        Serial.print(": "); 
        Serial.println(p3->getValue().c_str()); 
      }

      value = value + 31; 
      p1->setValue((uint8_t*) &value, 4); 
      p1->notify();
      Serial.print("p1 notify ");
      Serial.println(value); 
      
      value = value + 31; 
      p2->setValue((uint8_t*) &value, 4); 
      p2->notify();
      Serial.print("p2 notify ");
      Serial.println(value); 

      value = value + 31; 
      p3->setValue((uint8_t*) &value, 4); 
      p3->notify();
      Serial.print("p3 notify ");
      Serial.println(value); 
      delay(5000);
      
    }

    // disconnecting
    if (!deviceConnected && oldDeviceConnected) {
        delay(500); // give the bluetooth stack the chance to get things ready
        oldDeviceConnected = false;
    }
    // connecting
    if (deviceConnected && !oldDeviceConnected) {
        // do stuff here on connecting
        oldDeviceConnected = true;
    }
}