# 🚘 IoT-Based Smart Seatbelt Warning and Safety Monitoring System

This project is a smart IoT-based vehicle safety system using an ESP32 or Arduino-compatible board. It ensures **seatbelt compliance**, **monitors car temperature**, and **detects car flip/tilt using MPU6050**. It provides real-time alerts and display feedback to enhance safety for both driver and passengers.

## 🔧 Features

- 🪢 **Driver & Passenger Seatbelt Detection**
  - Detects whether seatbelts are fastened before or after ignition.
  - Sounds an alert if belts are not worn within 20 seconds of starting the car.
  
- 🔊 **Buzzer Alerts**
  - Provides audible warnings when seatbelts are not fastened.
  - Alerts when the car is flipped/upside down.

- 🌡️ **Temperature Monitoring**
  - Uses an LM35 analog sensor to display current vehicle temperature on an LCD.

- 🔄 **Car Flip Detection**
  - MPU6050 accelerometer detects if the car is upside down.
  - Displays critical warning and sounds buzzer in such cases.

- 📟 **16x2 LCD Display**
  - Displays real-time status such as "Car On", "Seatbelt Not Fastened", "Temp: XX°C", and "Car Flipped!".

## 🧰 Components Used

| Component           | Description             |
|--------------------|-------------------------|
| ESP8266            | Microcontroller board   |
| MPU6050            | Accelerometer + Gyro sensor |
| LM35               | Temperature sensor      |
| 16x2 LCD (I2C)     | Display for feedback    |
| Buzzer             | Audio alert             |
| Push Buttons       | Simulate ignition & car ON |
| Jumper Wires       | For connections         |
| Breadboard         | For prototyping         |

## ⚙️ How It Works

### 🟢 Start Sequence:
1. User presses "Car ON" button.
2. Ignition is turned ON.
3. LCD displays "Car On. Wear Seatbelt".

### 🔐 Seatbelt Check:
- If belts are not fastened within 20 seconds, buzzer turns ON and alert is shown on LCD.
- If both driver and passenger belts are worn, system proceeds normally.

### 🔄 Car Flip Detection:
- MPU6050 continuously monitors acceleration vectors.
- If orientation changes significantly (e.g., flipped), buzzer activates and LCD shows **"Car Flipped!"**.

### 🌡️ Temperature:
- Reads analog data from LM35 and converts it to Celsius.
- Displays `Temp: XX°C` on the LCD.

## 📚 What I Learned

- Interfacing MPU6050 using I2C with Adafruit libraries.
- Calibrating and using accelerometer data for orientation logic.
- Using `millis()` for timing countdowns (e.g., seatbelt timer).
- Reading and interpreting analog temperature sensor values (LM35).
- LCD communication via I2C and real-time feedback.
- Safety system design using state-based logic in embedded systems.

## 🧪 Test Scenarios

| Scenario                     | Expected Output                         |
|-----------------------------|------------------------------------------|
| Car ON, belts off           | "Wear Seatbelt" + buzzer after 20s      |
| Belts ON within 20s         | System proceeds, shows temp             |
| Ignition ON, MPU detects flip | "Car Flipped!" + buzzer                |
| LM35 reads >30°C            | Displays real-time temperature          |

## 🔌 Setup Instructions

1. Connect all components as per your wiring setup.
2. Upload the code to ESP8266 using Arduino IDE.
3. Open Serial Monitor for debugging (baud rate: 115200).
4. Observe behavior by pressing buttons and simulating tilt.

## 🔮 Future Enhancements

- 📱 Add Bluetooth/Wi-Fi to notify emergency contacts on car flip.
- 📊 Log temperature and status data to cloud (e.g., Firebase).
- 📷 Add camera trigger on crash detection.
- 📲 Mobile app integration for real-time health of vehicle.
- 🚦 Integrate speedometer using encoder/sensor.

## 📄 License

This project is open-source under the MIT License.

---

Made with ❤️ using sensors, logic, and care for driver safety.
