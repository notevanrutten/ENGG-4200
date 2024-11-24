#include <Arduino.h>

#define VOLTAGE_PIN_1 34
#define VOLTAGE_PIN_2 25
#define VOLTAGE_PIN_3 32

#define TEMP_PIN 2

void setup() { Serial.begin(115200); }

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

  Serial.printf("adc1: %d, v1: %.2fV, pack1: %.2fV\n", adc1, v1, pack1);
  Serial.printf("adc2: %d, v2: %.2fV, pack2: %.2fV\n", adc2, v2, pack2);
  Serial.printf("adc3: %d, v3: %.2fV, pack3: %.2fV\n", adc3, v3, pack3);

  int temp_adc = analogReadMilliVolts(TEMP_PIN);

  float v_therm = temp_adc / 1000.0;
  float r_therm = 10.0 * (v_therm) / (3.3 - v_therm);

  float temp =
      1.0 / (1.0 / 298.15 + (1 / 3435.0) * log(r_therm / 10.0)) - 273.15;

  Serial.printf("\ntemp_adc: %d, r_therm: %0.2fkΩ, temp: %0.2f°C\n\n", temp_adc,
                r_therm, temp);

  delay(1000);
}
