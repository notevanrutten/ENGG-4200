#include <Arduino.h>
#include <NimBLEDevice.h>
#include <PubSubClient.h>
#include <WiFi.h>

#include "secrets.h"

BLEUUID SERVICE_UUID("75f0a01e-af65-4099-b8ba-2af4e66fc482");
BLEUUID CHARACTERISTIC_UUID_1("9f4ae00e-c22a-4595-a7fd-f4176d084213");
BLEUUID CHARACTERISTIC_UUID_2("e373f7eb-8b75-40ce-be1c-29fae2db6725");
BLEUUID CHARACTERISTIC_UUID_3("08b109db-00f7-46a6-9c25-e6c6d0ec7ce6");

BLEAdvertisedDevice *myDevice;
bool doConnect = false;

WiFiClient wifiClient;
PubSubClient pubSubClient(wifiClient);

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
  // pClient->setMTU(64);

  BLERemoteService *pRemoteService = pClient->getService(SERVICE_UUID);
  if (pRemoteService == nullptr) {
    Serial.println("Failed to find service");
    return;
  }

  BLERemoteCharacteristic *pCharacteristic1 =
      pRemoteService->getCharacteristic(CHARACTERISTIC_UUID_1);
  if (pCharacteristic1 != nullptr) {
    pCharacteristic1->subscribe(true, notifyCallback);
  }

  BLERemoteCharacteristic *pCharacteristic2 =
      pRemoteService->getCharacteristic(CHARACTERISTIC_UUID_2);
  if (pCharacteristic2 != nullptr) {
    pCharacteristic2->subscribe(true, notifyCallback);
  }

  BLERemoteCharacteristic *pCharacteristic3 =
      pRemoteService->getCharacteristic(CHARACTERISTIC_UUID_3);
  if (pCharacteristic3 != nullptr) {
    pCharacteristic3->subscribe(true, notifyCallback);
  }

  Serial.println("Connected to server");
}

class MyAdvertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks {
  void onResult(BLEAdvertisedDevice *advertisedDevice) override {
    if (advertisedDevice->isAdvertisingService(SERVICE_UUID)) {

      BLEDevice::getScan()->stop();

      myDevice = advertisedDevice;

      doConnect = true;
    }
  }
};

void setup() {
  Serial.begin(115200);
  Serial.println();

  WiFi.begin(wifi_ssid, wifi_password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
  Serial.println("Connected to WiFi: " + String(wifi_ssid));

  pubSubClient.setServer(mqtt_server, 1883);

  /*
    BLEDevice::init("Master-Controller");

    BLEScan *pBLEScan = BLEDevice::getScan();
    pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
    pBLEScan->setActiveScan(true);
    pBLEScan->start(30);
    */
}

void loop() {
  if (doConnect) {
    connectToServer();
    doConnect = false;
  }

  if (!pubSubClient.connected()) {
    while (!pubSubClient.connected()) {
      if (pubSubClient.connect("ESP32Client", mqtt_user, mqtt_password)) {
        pubSubClient.publish("/homeassistant/pack1/voltage1", "Hello");
      } else {
        Serial.println("PubSubClient could not connect");
        delay(1000);
      }
    }
  }
  pubSubClient.loop();

  pubSubClient.publish("/homeassistant/pack1/voltage1", "Hello");
  delay(1000);
}
