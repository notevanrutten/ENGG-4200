#include <Arduino.h>
#include <NimBLEDevice.h>
#include <PubSubClient.h>
#include <WiFi.h>

#include "secrets.h"

BLEUUID SERVICE_UUID("75f0a01e-af65-4099-b8ba-2af4e66fc482");
BLEUUID CHARACTERISTIC_UUID_1("9f4ae00e-c22a-4595-a7fd-f4176d084213");
BLEUUID CHARACTERISTIC_UUID_2("e373f7eb-8b75-40ce-be1c-29fae2db6725");
BLEUUID CHARACTERISTIC_UUID_3("08b109db-00f7-46a6-9c25-e6c6d0ec7ce6");

const int PACK_COUNT = 3;
BLEAdvertisedDevice *devices[PACK_COUNT] = {nullptr};
bool doConnect[PACK_COUNT] = {false};
int connectedCount = 0;

WiFiClient wifiClient;
PubSubClient pubSubClient(wifiClient);

void notifyCallback(BLERemoteCharacteristic *pCharacteristic, uint8_t *pData,
                    size_t length, bool isNotify) {
  std::string msg((char *)pData, length);
  Serial.println(msg.c_str());

  int pack;
  float v1, v2, v3, temp;

  sscanf(msg.c_str(), "%d,%f,%f,%f,%f", &pack, &v1, &v2, &v3, &temp);

  if (pubSubClient.connected()) {
    pubSubClient.publish("/homeassistant/pack1/voltage1", "Hello");
    pubSubClient.publish("/homeassistant/pack1/voltage1", "Hello");
    pubSubClient.publish("/homeassistant/pack1/voltage1", "Hello");
    pubSubClient.publish("/homeassistant/pack1/voltage1", "Hello");
    pubSubClient.publish("/homeassistant/pack1/voltage1", "Hello");
  }
}

void connectToServer(int deviceIndex) {
  if (devices[deviceIndex] == nullptr) {
    return;
  }

  BLEClient *pClient = BLEDevice::createClient();
  if (!pClient->connect(devices[deviceIndex])) {
    Serial.println("Failed to connect to BLE device");
    return;
  }

  BLERemoteService *pRemoteService = pClient->getService(SERVICE_UUID);
  if (pRemoteService == nullptr) {
    Serial.println("Failed to find BLE service");
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

  Serial.println(
      ("Connected to BLE device: " + devices[deviceIndex]->getName()).c_str());
  connectedCount++;
}

class MyAdvertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks {
  void onResult(BLEAdvertisedDevice *advertisedDevice) override {
    if (connectedCount >= PACK_COUNT) {
      BLEDevice::getScan()->stop();
      Serial.println("All devices connected, stopping scan");
      return;
    }

    if (advertisedDevice->isAdvertisingService(SERVICE_UUID)) {
      for (int i = 0; i < PACK_COUNT; i++) {
        if (devices[i] == nullptr) {
          Serial.println(
              ("Found BLE device: " + advertisedDevice->getName()).c_str());

          devices[i] = advertisedDevice;
          doConnect[i] = true;
          break;
        }
      }
    }
  }
};

void setup() {
  Serial.begin(115200);
  Serial.println();
  /*
  WiFi.begin(wifi_ssid, wifi_password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
  Serial.println("Connected to WiFi: " + String(wifi_ssid));

  pubSubClient.setServer(mqtt_server, 1883);
  */
  BLEDevice::init("Master-Controller");
  BLEDevice::setMTU(64);

  BLEScan *pBLEScan = BLEDevice::getScan();
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setActiveScan(true);

  Serial.println("Starting BLE scan...");
  pBLEScan->start(10);
}

void loop() {
  for (int i = 0; i < PACK_COUNT; i++) {
    if (doConnect[i]) {
      connectToServer(i);
      doConnect[i] = false;
    }
  }
  /*
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
  */
  delay(1000);
}
