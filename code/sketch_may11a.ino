#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);  // LCD setup
Adafruit_MPU6050 mpu;  // MPU6050 object

sensors_event_t baselineAccel;
bool isCalibrated = false;

// Pin configuration
const int CAR_ON_PIN = 0;           // D3
const int DRIVER_BELT_PIN = 2;      // D4
const int IGNITION_PIN = 14;        // D5
const int PASSENGER_BELT_PIN = 12;  // D6
const int BUZZER_PIN = 13;          // D7
const int TEMP_SENSOR_PIN = A0;     // A0 (LM35 sensor)

// State flags
bool isCarOn = false;
bool isDriverBeltOn = false;
bool isIgnitionOn = false;

int speed = 0;
unsigned long lastSpeedUpdate = 0;
const unsigned long speedInterval = 1000; // 1 second
const int maxSpeed = 130;
unsigned long passengerBeltStartTime = 0;
const unsigned long countdownTime = 20000; // 20 seconds

bool previousCarButtonState = HIGH;

void setup() {
    // Start Serial communication for debug output
    Serial.begin(115200);
    
    Wire.begin();
    lcd.begin(16, 2);
    lcd.backlight();

    pinMode(CAR_ON_PIN, INPUT_PULLUP);
    pinMode(DRIVER_BELT_PIN, INPUT_PULLUP);
    pinMode(IGNITION_PIN, INPUT_PULLUP);
    pinMode(PASSENGER_BELT_PIN, INPUT_PULLUP);
    pinMode(BUZZER_PIN, OUTPUT);

    displayMessage("Waiting for", "Power ON");

    // Initialize MPU6050
    Serial.println("Initializing MPU6050...");
    if (!mpu.begin()) {
        Serial.println("Couldn't find MPU6050");
        while (1); // Stop execution if the sensor is not connected
    }
    Serial.println("MPU6050 found!");

    // Calibration
    calibrateMPU();
}

void loop() {
    bool currentCarButtonState = digitalRead(CAR_ON_PIN);
    if (previousCarButtonState == HIGH && currentCarButtonState == LOW) {
        isCarOn = !isCarOn;
        if (isCarOn) {
            displayMessage("System Status", "Powered ON");
            tone(BUZZER_PIN, 1000, 200);
            isDriverBeltOn = false;
            isIgnitionOn = false;
        } else {
            displayMessage("System Status", "Powered OFF");
            tone(BUZZER_PIN, 500, 300);
            isDriverBeltOn = false;
            isIgnitionOn = false;
        }
    }
    previousCarButtonState = currentCarButtonState;

    if (!isCarOn) {
        displayMessage("System is OFF", "Press PWR Btn");
        tone(BUZZER_PIN, 500, 300);
    }

    if (isCarOn) {
        if (!isDriverBeltOn) {
            if (isIgnitionOn) {
                displayMessage("Fasten Driver", "Seatbelt Now!");
                tone(BUZZER_PIN, 1000, 200);
            }
            if (buttonPressed(DRIVER_BELT_PIN)) {
                isDriverBeltOn = true;
                displayMessage("Driver Belt", "Secured OK");
            }
        }

        if (isDriverBeltOn && !isIgnitionOn) {
            displayMessage("Press Ignition", "Button to Start");
            tone(BUZZER_PIN, 1000, 200);
            if (buttonPressed(IGNITION_PIN)) {
                isIgnitionOn = true;
                displayMessage("Ignition", "Started OK");
            }
        }

        if (!isDriverBeltOn && buttonPressed(IGNITION_PIN)) {
            displayMessage("Fasten Driver", "Before Ignition");
            tone(BUZZER_PIN, 1000, 500);
            isIgnitionOn = false;
        }

        if (isIgnitionOn) {
            if (millis() - lastSpeedUpdate >= speedInterval) {
                lastSpeedUpdate = millis();
                if (speed < maxSpeed) speed += 5;
            }

            // Temperature Reading
            float temperature = analogRead(TEMP_SENSOR_PIN) * (3.3 / 1023.0) * 100;

            lcd.setCursor(0, 0);
            lcd.print("Speed:");
            lcd.print(speed);
            lcd.print("km/h  "); // padding

            lcd.setCursor(0, 1);
            lcd.print("Temp:");
            lcd.print(temperature, 1);
            lcd.print(" C    "); // padding

            // Check for fall detection
            if (isCalibrated) {
                checkForFall();
            }

            // Passenger seatbelt alert
            if (speed > 20) {
                bool passengerBeltFastened = digitalRead(PASSENGER_BELT_PIN) == LOW;
                if (!passengerBeltFastened) {
                    if (passengerBeltStartTime == 0) passengerBeltStartTime = millis();
                    unsigned long remainingTime = countdownTime - (millis() - passengerBeltStartTime);

                    if (remainingTime > 0) {
                        lcd.setCursor(0, 1);
                        lcd.print("Belt in: ");
                        lcd.print(remainingTime / 1000);
                        lcd.print(" sec ");
                        tone(BUZZER_PIN, 1000, 200);
                        speed -= 5;
                    } else {
                        displayMessage("Passenger Belt", "Please Fasten!");
                        tone(BUZZER_PIN, 1000);
                        speed -= 5;
                        if (speed < 0) speed = 0;
                    }
                } else {
                    passengerBeltStartTime = 0;
                    speed += 5;
                    noTone(BUZZER_PIN);
                }
            } else {
                noTone(BUZZER_PIN);
            }
        }
    }

    if (!isCarOn || !isDriverBeltOn || !isIgnitionOn) {
        if (speed > 0 && millis() - lastSpeedUpdate >= speedInterval) {
            lastSpeedUpdate = millis();
            speed -= 5;
            if (speed < 0) speed = 0;

            lcd.setCursor(0, 0);
            lcd.print("Speed: ");
            lcd.print(speed);
            lcd.print(" km/h");

            lcd.setCursor(0, 1);
            lcd.print("System Slowing..");
        }

        if (speed == 0 && (!isCarOn || !isDriverBeltOn || !isIgnitionOn)) {
            lcd.setCursor(0, 1);
            lcd.print("System Idle     ");
            noTone(BUZZER_PIN);
        }
    }

    if (isIgnitionOn && !buttonPressed(IGNITION_PIN)) {
        isIgnitionOn = false;
        displayMessage("Ignition OFF", "Standby Mode");
    }

    if (isDriverBeltOn && !buttonPressed(DRIVER_BELT_PIN)) {
        isDriverBeltOn = false;
        displayMessage("Driver Belt", "Released");
    }

    delay(300);
}

bool buttonPressed(int pin) {
    return digitalRead(pin) == LOW;
}

void displayMessage(const String &line1, const String &line2) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(line1.substring(0, 16));
    lcd.setCursor(0, 1);
    lcd.print(line2.substring(0, 16));
    delay(1500);
}

// Function to calibrate the MPU6050
void calibrateMPU() {
    sensors_event_t a, g, temp;
    mpu.getEvent(&a, &g, &temp);

    baselineAccel = a;
    isCalibrated = true;

    Serial.println("MPU6050 Calibration done.");
    Serial.print("Baseline Acceleration -> X: ");
    Serial.print(baselineAccel.acceleration.x);
    Serial.print(" Y: ");
    Serial.print(baselineAccel.acceleration.y);
    Serial.print(" Z: ");
    Serial.println(baselineAccel.acceleration.z);
}

// Function to detect falls based on deviation from baseline
void checkForFall() {
    sensors_event_t a, g, temp;
    mpu.getEvent(&a, &g, &temp);

    Serial.print("Current Accel -> X: ");
    Serial.print(a.acceleration.x);
    Serial.print(" Y: ");
    Serial.print(a.acceleration.y);
    Serial.print(" Z: ");
    Serial.println(a.acceleration.z);

    // Difference from baseline
    float diffX = abs(a.acceleration.x - baselineAccel.acceleration.x);
    float diffY = abs(a.acceleration.y - baselineAccel.acceleration.y);
    float diffZ = abs(a.acceleration.z - baselineAccel.acceleration.z);

    // Customize thresholds based on testing
    if (diffX > 3.0 || diffY > 3.0 || diffZ > 3.0) {
        Serial.println("⚠️ Fall Detected based on deviation from normal position!");
        tone(BUZZER_PIN, 2000, 500); // Activate buzzer
        displayMessage("Fall Detected!", "Check immediately!");
    }
}
