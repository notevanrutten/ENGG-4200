#include <Arduino.h>
#include <NimBLEDevice.h>

#define PACK_NUMBER 1

#define VOLTAGE_PIN_1 34
#define VOLTAGE_PIN_2 25
#define VOLTAGE_PIN_3 32

#define TEMP_PIN 2

#define SERVICE_UUID "75f0a01e-af65-4099-b8ba-2af4e66fc482"
#define CHARACTERISTIC_UUID_1 "9f4ae00e-c22a-4595-a7fd-f4176d084213"
#define CHARACTERISTIC_UUID_2 "e373f7eb-8b75-40ce-be1c-29fae2db6725"
#define CHARACTERISTIC_UUID_3 "08b109db-00f7-46a6-9c25-e6c6d0ec7ce6"

BLECharacteristic *pCharacteristic;

void setup() {
  Serial.begin(115200);
  Serial.println();

  BLEDevice::init(("Pack-" + String(PACK_NUMBER)).c_str());
  BLEDevice::setMTU(64);

  BLEServer *pServer = BLEDevice::createServer();
  BLEService *pService = pServer->createService(SERVICE_UUID);

  pCharacteristic = pService->createCharacteristic(CHARACTERISTIC_UUID_1,
                                                   NIMBLE_PROPERTY::NOTIFY);

  pService->start();

  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(true);
  pAdvertising->start();
}

void loop() {
  int adc1 = analogReadMilliVolts(VOLTAGE_PIN_1);
  int adc2 = analogReadMilliVolts(VOLTAGE_PIN_2);
  int adc3 = analogReadMilliVolts(VOLTAGE_PIN_3);

  float v1 = (adc1 / 1000.0) * ((47.0 + 10.0) / 10.0);
  float v2 = (adc2 / 1000.0) * ((47.0 + 10.0) / 10.0);
  float v3 = (adc3 / 1000.0) * ((47.0 + 10.0) / 10.0);

  v2 -= v1;
  v3 -= v2 + v1;

  int temp_adc = analogReadMilliVolts(TEMP_PIN);

  float v_therm = temp_adc / 1000.0;
  float r_therm = 10.0 * (v_therm) / (3.3 - v_therm);

  float temp =
      1.0 / (1.0 / 298.15 + (1 / 3435.0) * log(r_therm / 10.0)) - 273.15;

  String msg = String(PACK_NUMBER) + "," + String(v1) + "," + String(v2) + "," +
               String(v3) + "," + String(temp);

  pCharacteristic->setValue(msg);
  pCharacteristic->notify();

  Serial.println(msg);

  delay(1000);
}
