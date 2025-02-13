#include "Wire.h"
#include <MPU6050_light.h>
// #include <esp32

MPU6050 mpu(Wire);
unsigned long timer = 0;

// PID Constants (Tune these for best performance)
float Kp = 120.0;
float Ki = 80.0;
float Kd = 0.1;

float setpoint = 0.0;  // Desired pitch angle (robot balanced at 0°)
float previous_error = 0.0;
float integral = 0.0;

// Motor Driver Pins (L298)
#define ENA 25  // Left Motor Speed (PWM)
#define IN1 26  // Left Motor Direction
#define IN2 27
#define ENB 13  // Right Motor Speed (PWM)
#define IN3 12  // Right Motor Direction
#define IN4 14

// ESP32 PWM Channels
#define PWM_CHANNEL_A  0  
#define PWM_CHANNEL_B  1  
#define PWM_FREQ       1000  // 1kHz PWM frequency
#define PWM_RES        8     // 8-bit resolution (0-255)

void setup() {
  Serial.begin(115200);
  
  // Initialize I2C for ESP32 (default SDA=21, SCL=22)
  Wire.begin(21, 22);
  Wire.setClock(400000);

  byte status = mpu.begin();
  Serial.print(F("MPU6050 status: "));
  Serial.println(status);
  while (status != 0) {} // Stop if MPU6050 not connected

  // mpu.calcOffsets();  // Uncomment if calibration is needed
  mpu.setGyroOffsets(-3.60, 1.2667, 1.12);
  mpu.setAccOffsets(-0.0167, -0.02, 0.04);
  Serial.println("MPU6050 Initialized!\n");

  // Initialize Motor Pins
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  // Configure PWM for ESP32
  //ledcSetup(PWM_CHANNEL_A, PWM_FREQ, PWM_RES);  // Set up PWM
  ledcAttach(ENA, PWM_FREQ, PWM_RES);  // Attach PWM to motor pin

  //ledcSetup(PWM_CHANNEL_B, PWM_FREQ, PWM_RES);  
  ledcAttach(ENB, PWM_FREQ, PWM_RES);  


  // Ensure motors start OFF
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}

void loop() {
  mpu.update();

  if ((millis() - timer) > 10) { // Update every 10ms
    float pitch = mpu.getAngleY();  // Use Y-axis as pitch

    // Calculate PID
    float error = setpoint - pitch;
    integral += error * ((millis()-timer)/1000.00);  // dt = 10ms = 0.01s
    float derivative = (error - previous_error) / ((millis()-timer)/1000.00);
    float PID_value = (Kp * error) + (Ki * integral) + (Kd * derivative);
    previous_error = error;

    // Limit PID output
    if (PID_value > 255) PID_value = 255;
    if (PID_value < -255) PID_value = -255;

    // Drive motors based on PID output
    driveMotors(PID_value);

    timer = millis();
  }
  delay(25);
}

// Function to control motors with L298
void driveMotors(float PID_value) {
  int speed1 = map(abs(PID_value), 0, 255, 65, 235);
  int speed2 = map(abs(PID_value), 0, 255, 55, 227);

  Serial.print("S1: "); Serial.print(speed1);
  Serial.print(" | S2: "); Serial.println(speed2);

  if (PID_value > 0) {  // Move Forward
    ledcWrite(ENA, speed1);
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
    
    ledcWrite(ENB, speed2);
    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);
  } 
  else {  // Move Backward
    ledcWrite(ENA, speed1);
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);
    
    ledcWrite(ENB, speed2);
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, HIGH);
  }
}
