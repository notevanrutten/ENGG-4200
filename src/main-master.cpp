#include <Arduino.h>
#include <BLEDevice.h>

BLEUUID SERVICE_UUID("75f0a01e-af65-4099-b8ba-2af4e66fc482");
BLEUUID CHARACTERISTIC_UUID_1("9f4ae00e-c22a-4595-a7fd-f4176d084213");
BLEUUID CHARACTERISTIC_UUID_2("e373f7eb-8b75-40ce-be1c-29fae2db6725");
BLEUUID CHARACTERISTIC_UUID_3("08b109db-00f7-46a6-9c25-e6c6d0ec7ce6");

BLEAdvertisedDevice *myDevice;
bool doConnect = false;

void notifyCallback(BLERemoteCharacteristic *pCharacteristic, uint8_t *pData,
                    size_t length, bool isNotify) {
  for (size_t i = 0; i < length; i++) {
    Serial.print((char)pData[i]);
  }
  Serial.println();
}

void connectToServer() {
  Serial.println(myDevice->getAddress().toString().c_str());

  BLEClient *pClient = BLEDevice::createClient();
  pClient->connect(myDevice);
  pClient->setMTU(64);

  BLERemoteService *pRemoteService = pClient->getService(SERVICE_UUID);
  if (pRemoteService == nullptr) {
    Serial.println("Failed to find service");
    return;
  }

  BLERemoteCharacteristic *pCharacteristic1 =
      pRemoteService->getCharacteristic(CHARACTERISTIC_UUID_1);
  if (pCharacteristic1 != nullptr) {
    pCharacteristic1->registerForNotify(notifyCallback);
  }

  BLERemoteCharacteristic *pCharacteristic2 =
      pRemoteService->getCharacteristic(CHARACTERISTIC_UUID_2);
  if (pCharacteristic2 != nullptr) {
    pCharacteristic2->registerForNotify(notifyCallback);
  }

  BLERemoteCharacteristic *pCharacteristic3 =
      pRemoteService->getCharacteristic(CHARACTERISTIC_UUID_3);
  if (pCharacteristic3 != nullptr) {
    pCharacteristic3->registerForNotify(notifyCallback);
  }

  Serial.println("Connected to server");
}

class MyAdvertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks {
  void onResult(BLEAdvertisedDevice advertisedDevice) {
    if (advertisedDevice.haveServiceUUID() &&
        advertisedDevice.isAdvertisingService(SERVICE_UUID)) {

      BLEDevice::getScan()->stop();

      myDevice = new BLEAdvertisedDevice(advertisedDevice);

      doConnect = true;
    }
  }
};

void setup() {
  Serial.begin(115200);

  BLEDevice::init("Master-Controller");

  BLEScan *pBLEScan = BLEDevice::getScan();
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setActiveScan(true);
  pBLEScan->start(30);
}

void loop() {
  if (doConnect) {
    connectToServer();
    doConnect = false;
  }

  delay(1000);
}
