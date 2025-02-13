#include <Wire.h>
#include "MAX30100_PulseOximeter.h"

#define I2C_SDA 4       // SDA pin for I2C
#define I2C_SCL 5       // SCL pin for I2C
#define ECG_PIN A0       // Analog pin for ECG sensor (GPIO36 is VP)

// Thresholds for abnormal readings
#define SPO2_LOW 95      // Minimum acceptable SpO2 value
#define HR_LOW 60        // Minimum heart rate (bpm)
#define HR_HIGH 100      // Maximum heart rate (bpm)

// Pulse Oximeter object
PulseOximeter pox;
unsigned long lastUpdate = 0;

void setup() {
    Serial.begin(115200);
    Serial.println("Initializing...");
    Wire.begin(I2C_SDA, I2C_SCL); // Initialize I2C
    
    // Initialize MAX30100
    if (!pox.begin()) {
        Serial.println("ERROR: MAX30100 initialization failed. Check wiring!");
        while (1); // Stop execution if the sensor isn't detected
    }
    Serial.println("MAX30100 initialized successfully.");
    Serial.println("Open Serial Plotter to view ECG graph.");
}

void loop() {
    // Update MAX30100 sensor
    pox.update();

    // Read and plot ECG data
    int ecgValue = analogRead(ECG_PIN);
    Serial.print("ECG: ");
    Serial.print(ecgValue);
    Serial.print(" ");

    // Every second, display heart rate and SpO2
    if (millis() - lastUpdate > 1000) {
        float heartRate = pox.getHeartRate();
        float spo2 = pox.getSpO2();

        if (heartRate > 0 && spo2 > 0) {
            Serial.print("HR: ");
            Serial.print(heartRate);
            Serial.print(" bpm, SpO2: ");
            Serial.print(spo2);
            Serial.println(" %");

            // Check for abnormal values and print alerts
            if (spo2 < SPO2_LOW) {
                Serial.println("ALERT: Low SpO2 detected!");
            }
            if (heartRate < HR_LOW) {
                Serial.println("ALERT: Heart rate too low!");
            } else if (heartRate > HR_HIGH) {
                Serial.println("ALERT: Heart rate too high!");
            }
        } else {
            Serial.println("Waiting for MAX30100 data...");
        }

        lastUpdate = millis();
    }

    // Short delay for stability
    delay(100);
}
