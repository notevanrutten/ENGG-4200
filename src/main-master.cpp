#include <Arduino.h>
#include <BLEDevice.h>

#define SERVICE_UUID BLEUUID("75f0a01e-af65-4099-b8ba-2af4e66fc482")
#define CHARACTERISTIC_UUID BLEUUID("9f4ae00e-c22a-4595-a7fd-f4176d084213")

class MyAdvertisedCallbacks : public BLEAdvertisedDeviceCallbacks {
  void onResult(BLEAdvertisedDevice advertisedDevice) {
    if (advertisedDevice.haveServiceUUID() &&
        advertisedDevice.isAdvertisingService(SERVICE_UUID)) {
      BLEClient *pClient = BLEDevice::createClient();
      pClient->connect(&advertisedDevice);
      BLERemoteService *pRemoteService = pClient->getService(SERVICE_UUID);
      if (pRemoteService != nullptr) {
        BLERemoteCharacteristic *pRemoteCharacteristic =
            pRemoteService->getCharacteristic(CHARACTERISTIC_UUID);
        if (pRemoteService != nullptr) {
          Serial.println(pRemoteCharacteristic->readValue().c_str());
        }
      }
    }
  }
};

void setup() {
  Serial.begin(115200);

  BLEDevice::init("Master-Controller");
  BLEScan *pBLEScan = BLEDevice::getScan();

  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedCallbacks());
  pBLEScan->setActiveScan(true);
  pBLEScan->start(30);
}

void loop() { delay(1000); }
