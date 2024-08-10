#include <Arduino.h>

// Defining the pins to control the wheels

// Front Left wheel
#define WHEEL_FL_PWM 5
#define WHEEL_FL_DIR 6
// Front Right wheel
#define WHEEL_FR_PWM 4
#define WHEEL_FR_DIR 3
// Rear Left wheel
#define WHEEL_RL_PWM 7
#define WHEEL_RL_DIR 10
// Rear Right wheel
#define WHEEL_RR_PWM 2
#define WHEEL_RR_DIR 1

#define LED 8 // Led pin

// Wheel infos
struct Wheel {
  uint8_t pwmPin;
  uint8_t dirPin;
};

// Wheels organize
struct Wheels {
  Wheel frontLeft;
  Wheel frontRight;
  Wheel rearLeft;
  Wheel rearRight;
};

/**
 * @brief set pwm to control speed between -100 and 100 and direction.
 *
 * @param wheel pins of wheel
 * @param speed speed between -100 and 100
 */
void ControlWheel(Wheel wheel, int8_t speed) {
  if (speed > 0) {
    digitalWrite(wheel.dirPin, HIGH);
  } else {
    digitalWrite(wheel.dirPin, LOW);
  }

  analogWrite(wheel.pwmPin, map(speed, -100, 100, 0, 255));
}

/**
 * @brief Calculate and apply control to wheels
 *
 * @param wheels struct with all wheels
 * @param speed speed between -100 and 100
 * @param angle angle of direction
 */
void ControlCar(Wheels wheels, int8_t speed, float angle) {
  ControlWheel(wheels.frontLeft, speed * cos(angle + PI / 4));
  ControlWheel(wheels.frontRight, speed * cos(angle - PI / 4));
  ControlWheel(wheels.rearLeft, speed * cos(angle + 3 * PI / 4));
  ControlWheel(wheels.rearRight, speed * cos(angle - 3 * PI / 4));
}

void setup() {

  // Set pin mode to all pins
  pinMode(WHEEL_FL_PWM, OUTPUT);
  pinMode(WHEEL_FL_DIR, OUTPUT);
  pinMode(WHEEL_FR_PWM, OUTPUT);
  pinMode(WHEEL_FR_DIR, OUTPUT);
  pinMode(WHEEL_RR_PWM, OUTPUT);
  pinMode(WHEEL_RR_DIR, OUTPUT);
  pinMode(WHEEL_RL_DIR, OUTPUT);
  pinMode(WHEEL_RL_PWM, OUTPUT);
  pinMode(LED, OUTPUT);

  const Wheels wheels = {{WHEEL_FL_PWM, WHEEL_FL_DIR},
                         {WHEEL_FR_PWM, WHEEL_FR_DIR},
                         {WHEEL_RL_PWM, WHEEL_RL_DIR},
                         {WHEEL_RR_PWM, WHEEL_RR_DIR}};

  ControlCar(wheels, 0, 0);

  // Start serial
  Serial.begin(115200);
}

void loop() {}
