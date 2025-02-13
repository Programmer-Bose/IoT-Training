#include "Wire.h"
#include <MPU6050_light.h>

MPU6050 mpu(Wire);
unsigned long timer = 0;

// PID Constants (Tune these for best performance)
float Kp = 85.0;
float Ki = 8.0;
float Kd = 1.0;

float setpoint = 0;  // Desired pitch angle (robot balanced at 0°)
float previous_error = 0.0;
float integral = 0.0;

// Motor Driver Pins (L298)
#define ENA 9  // Left Motor Speed (PWM)
#define IN1 5    // Left Motor Direction
#define IN2 6
#define ENB 10    // Right Motor Speed (PWM)
#define IN3 7    // Right Motor Direction
#define IN4 8

void setup() {
  Serial.begin(9600);
  Wire.begin();
  Wire.setClock(400000);
  
  byte status = mpu.begin();
  Serial.print(F("MPU6050 status: "));
  Serial.println(status);
  while (status != 0) {} // Stop if MPU6050 not connected

  // Serial.println(F("Calculating offsets, do not move MPU6050"));
  // delay(1000);
  // mpu.calcOffsets();
  mpu.setGyroOffsets(-2.42,0.12,-0.40);
  mpu.setAccOffsets(0.01,-0.06,0.13);
  Serial.println("Done!\n");

  // Initialize Motor Pins
  pinMode(ENA, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(ENB, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

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

    // Print values for Serial Plotter
    // Serial.print("Pitch:");
    // Serial.print(pitch);
    // Serial.print(",");
    // Serial.print("PID:");
    // Serial.println(PID_value);

    // Drive motors based on PID output
    driveMotors(PID_value);

    timer = millis();
  }
  delay(25);
}

// Function to control motors with L298
void driveMotors(float PID_value) {
  int speed = abs(PID_value);  // Convert PID output to positive for PWM
  speed = map(speed,0,255,80,255);
  
  if (PID_value > 0) {  // Move Forward
    analogWrite(ENA, speed);
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
    
    analogWrite(ENB, speed);
    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);
  } 
  else {  // Move Backward
    analogWrite(ENA, speed);
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);
    
    analogWrite(ENB, speed);
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, HIGH);
  }
}

