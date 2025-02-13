#define ENA 9  // Left Motor Speed (PWM)
#define IN1 5  // Left Motor Direction
#define IN2 6  
#define ENB 10 // Right Motor Speed (PWM)
#define IN3 7  // Right Motor Direction
#define IN4 8  

int leftPWM = 0;
int rightPWM = 0;

void setup() {
    Serial.begin(115200);
    
    pinMode(ENA, OUTPUT);
    pinMode(IN1, OUTPUT);
    pinMode(IN2, OUTPUT);
    pinMode(ENB, OUTPUT);
    pinMode(IN3, OUTPUT);
    pinMode(IN4, OUTPUT);
    
    // Set initial motor direction (Forward)
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);
}

void loop() {
    if (Serial.available()) {
        String input = Serial.readStringUntil('\n');  // Read input until newline
        input.trim();  // Remove any whitespace

        int lIndex = input.indexOf('L');
        int rIndex = input.indexOf('R');

        if (lIndex != -1) {
            leftPWM = input.substring(lIndex + 1, rIndex != -1 ? rIndex : input.length()).toInt();
        }
        if (rIndex != -1) {
            rightPWM = input.substring(rIndex + 1).toInt();
        }

        leftPWM = constrain(leftPWM, 0, 255);
        rightPWM = constrain(rightPWM, 0, 255);

        analogWrite(ENA, leftPWM);
        analogWrite(ENB, rightPWM);

        Serial.print("Left PWM: ");
        Serial.print(leftPWM);
        Serial.print(" | Right PWM: ");
        Serial.println(rightPWM);
    }
}
