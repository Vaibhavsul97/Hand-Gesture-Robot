🤖 Hand Gesture Controlled Robot using Arduino Nano
This project showcases a wireless hand gesture-controlled robot using two Arduino Nano boards, an MPU6050 sensor, and nRF24L01+ modules. The system reads hand movements via the MPU6050 accelerometer and gyroscope, sends the data wirelessly, and moves a 4WD robot accordingly — forward, backward, left, or right.

🔧 Key Features
Wireless control using nRF24L01+ transceivers
Real-time gesture recognition with MPU6050
4WD movement with L298N motor driver
Ideal for hobbyists, educators, and robotics learners

🧩 Components Used
2 × Arduino Nano
2 × nRF24L01+ + adapters
1 × MPU6050 sensor
1 × 4WD chassis
1 × L298N motor driver
Batteries, jumper wires, breadboards, etc.

⚙️ How It Works
Transmitter: Reads hand tilt using the MPU6050 and transmits commands via nRF24L01+
Receiver: Receives the command and drives motors using the L298N driver

Gestures:
Tilt forward → Robot moves forward
Tilt backward → Robot moves backward
Tilt left → Robot turns left
Tilt right → Robot turns right

🛠️ Setup
Connect MPU6050 and nRF module to the transmitter Nano
Connect L298N and nRF module to the receiver Nano
Upload respective codes to both Arduinos
Ensure required libraries: MPU6050, I2Cdev, RF24, SPI

📌 Resources
MPU6050 & I2Cdev Library: GitHub - jrowberg/i2cdevlib
