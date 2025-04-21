// Transmitter
// These are needed for MPU
#include "I2Cdev.h"
#include "MPU6050_6Axis_MotionApps20.h"

// These are needed for RF handling
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

#define PRINT_DEBUG   // Uncomment this line if you want to print the MPU6050 initialization information on serial monitor

#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
    #include "Wire.h"
#endif

// MPU control/status vars
MPU6050 mpu;
bool dmpReady = false;  // set true if DMP init was successful
uint8_t devStatus;      // return status after each device operation (0 = success, !0 = error)
uint16_t packetSize;    // expected DMP packet size (default is 42 bytes)
uint8_t fifoBuffer[64]; // FIFO storage buffer
Quaternion q;           // [w, x, y, z]         quaternion container
VectorFloat gravity;    // [x, y, z]            gravity vector
float ypr[3];           // [yaw, pitch, roll]   yaw/pitch/roll container

// RF control
const uint64_t pipeOut = 0xF9E8F0F0E1LL;   // IMPORTANT: The same as in the receiver
RF24 radio(8, 9); // select CE, CSN pin

struct PacketData {
  byte xAxisValue;
  byte yAxisValue;
} data;

void setupRadioTransmitter() {
  Serial.println(F("Initializing radio transmitter..."));
  radio.begin();
  radio.setDataRate(RF24_250KBPS);
  radio.openWritingPipe(pipeOut);
  radio.stopListening(); // start the radio communication for Transmitter  
  Serial.println(F("Radio transmitter initialized."));
}

void setupMPU() {
  // Join I2C bus (I2Cdev library doesn't do this automatically)
  #if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
      Wire.begin();
      Wire.setClock(400000); // 400kHz I2C clock
  #elif I2CDEV_IMPLEMENTATION == I2CDEV_BUILTIN_FASTWIRE
      Fastwire::setup(400, true);
  #endif

  #ifdef PRINT_DEBUG
    Serial.println(F("Initializing I2C devices..."));
  #endif
  
  mpu.initialize();

  #ifdef PRINT_DEBUG  
    Serial.println(F("Testing device connections..."));
    if (mpu.testConnection()) {
        Serial.println(F("MPU6050 connection successful"));
    } else {
        Serial.println(F("MPU6050 connection failed"));
        return; // Exit setupMPU if the connection fails
    }
    Serial.println(F("Send any character to begin DMP programming and demo: "));
    while (Serial.available() && Serial.read()); // empty buffer
    while (!Serial.available());                 // wait for data
    while (Serial.available() && Serial.read()); // empty buffer again
    Serial.println(F("Initializing DMP..."));
  #endif
  
  devStatus = mpu.dmpInitialize();

  if (devStatus == 0) {
      mpu.CalibrateAccel(6);
      mpu.CalibrateGyro(6);
      #ifdef PRINT_DEBUG      
        mpu.PrintActiveOffsets();
        Serial.println(F("DMP enabled."));
      #endif
      mpu.setDMPEnabled(true);
      dmpReady = true;
      packetSize = mpu.dmpGetFIFOPacketSize();
      #ifdef PRINT_DEBUG      
        Serial.println(F("DMP ready! Waiting for first interrupt..."));
      #endif
  } else {
      #ifdef PRINT_DEBUG       
        Serial.print(F("DMP Initialization failed (code "));
        Serial.print(devStatus);
        Serial.println(F(")"));
      #endif
  }
}

void setup() {
  Serial.begin(115200);
  setupRadioTransmitter();   
  setupMPU();
  #ifdef PRINT_DEBUG
    Serial.println(F("Setup complete"));
  #endif
}

void loop() {
  if (!dmpReady) return;

  // Read a packet from FIFO. Get the latest packet
  if (mpu.dmpGetCurrentFIFOPacket(fifoBuffer)) {  
    // Display Euler angles in degrees
    mpu.dmpGetQuaternion(&q, fifoBuffer);
    mpu.dmpGetGravity(&gravity, &q);
    mpu.dmpGetYawPitchRoll(ypr, &q, &gravity);

    int xAxisValue = constrain(ypr[2] * 180 / M_PI, -90, 90);
    int yAxisValue = constrain(ypr[1] * 180 / M_PI, -90, 90);
    data.xAxisValue = map(xAxisValue, -90, 90, 0, 254); 
    data.yAxisValue = map(yAxisValue, -90, 90, 254, 0);

    radio.write(&data, sizeof(PacketData));

    #ifdef PRINT_DEBUG  
      Serial.print(F("X Axis: "));
      Serial.print(xAxisValue);
      Serial.print(F(", Y Axis: "));
      Serial.println(yAxisValue);        
    #endif
  }
}
