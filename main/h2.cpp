extern "C" {
    void h2_task(void *pvParameters);
}

#include "Arduino.h"
#include "mqtt.h"

extern QueueHandle_t xQueue_mqtt_tx;

#define LOW_PRESSURE_H2_SENSOR_PIN 34
#define LED_PIN 16
// Low pressure H2 sensor
#define Voltage_Resolution 4095
#define ADC_Bit_Resolution 12 
#define H2_SENSOR_RATING 5 // [bar]

unsigned long previousMillis = 0;
const long interval = 3000; // 10 seconds interval for sensing and publishing
const char *topic_h2_fuelcell_line_pressure = "sensor/ESPFuelCell/h2_fuelcell_line_pressure";

void h2_task(void *pvParameters) {
    Serial.begin(115200);

    pinMode(LOW_PRESSURE_H2_SENSOR_PIN, INPUT);
    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, LOW);

    while (1) {
        unsigned long currentMillis = millis();
        if (currentMillis - previousMillis >= interval) {
            previousMillis = currentMillis;

            int adcValue = analogRead(LOW_PRESSURE_H2_SENSOR_PIN);
            if (adcValue < 0 || adcValue > 4095) {
                Serial.println("Error: ADC value out of range!");
                return;
            }

            float H2_pressure = (adcValue / (float)Voltage_Resolution) * H2_SENSOR_RATING;
            int blinkDelay = map(adcValue, 0, 4095, 1000, 100);

            Serial.println("-----------------------------------------------------------");
            Serial.printf("ADC Value: %d\n", adcValue);
            Serial.printf("H2 Pressure: %.2f bar\n", H2_pressure);
            Serial.printf("MQTT Message Sent: %.2f bar\n", H2_pressure);
            Serial.printf("Blink Delay: %d ms\n", blinkDelay);
            Serial.println("-----------------------------------------------------------");

            char var[16];
            dtostrf(H2_pressure, 6, 2, var);

            MQTT_t mqttBuf;
            memset(&mqttBuf, 0, sizeof(mqttBuf));
            mqttBuf.topic_type = PUBLISH;
            strcpy(mqttBuf.topic, topic_h2_fuelcell_line_pressure);
            mqttBuf.topic_len = strlen(mqttBuf.topic);
            strcpy(mqttBuf.data, var);
            mqttBuf.data_len = strlen(var);

            xQueueSend(xQueue_mqtt_tx, &mqttBuf, portMAX_DELAY);

            // LED blinking behavior based on ADC value
            if (adcValue == 0) {
                digitalWrite(LED_PIN, HIGH); // Stay off at 0
            } else {
                digitalWrite(LED_PIN, LOW);
                delay(blinkDelay);
                digitalWrite(LED_PIN, HIGH);
                delay(blinkDelay);
            }
        }
    }
}