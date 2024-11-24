#include <Arduino.h>
#include <BLEDevice.h>

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

  BLEDevice::init("Node-1");

  BLEServer *pServer = BLEDevice::createServer();
  BLEService *pService = pServer->createService(SERVICE_UUID);

  pCharacteristic = pService->createCharacteristic(
      CHARACTERISTIC_UUID_1,
      BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY);

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

  float pack1 = v1;
  float pack2 = v2 - pack1;
  float pack3 = v3 - (pack1 + pack2);

  /*
    Serial.printf("adc1: %d, v1: %.2fV, pack1: %.2fV\n", adc1, v1, pack1);
    Serial.printf("adc2: %d, v2: %.2fV, pack2: %.2fV\n", adc2, v2, pack2);
    Serial.printf("adc3: %d, v3: %.2fV, pack3: %.2fV\n", adc3, v3, pack3);
  */

  int temp_adc = analogReadMilliVolts(TEMP_PIN);

  float v_therm = temp_adc / 1000.0;
  float r_therm = 10.0 * (v_therm) / (3.3 - v_therm);

  float temp =
      1.0 / (1.0 / 298.15 + (1 / 3435.0) * log(r_therm / 10.0)) - 273.15;

  // Serial.printf("\ntemp_adc: %d, r_therm: %0.2fkΩ, temp: %0.2f°C\n\n",
  // temp_adc, r_therm, temp);

  String value = "N1,V1:" + String(pack1) + ",V2:" + String(pack2) +
                 ",V3:" + String(pack3) + ",T:" + String(temp);

  pCharacteristic->setValue(value.c_str());
  pCharacteristic->notify();

  Serial.println(value);

  delay(1000);
}
