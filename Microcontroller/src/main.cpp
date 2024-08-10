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
void ControlWheel(Wheel wheel, float_t speed) {
  if (speed > 0) {
    digitalWrite(wheel.dirPin, HIGH);
  } else {
    digitalWrite(wheel.dirPin, LOW);
    speed = -speed;
  }

  // convert float [0, 100] to uint16_t
  uint16_t pwmValue = trunc(speed * 655.35);
  ledcWrite(wheel.pwmPin, pwmValue);
}

/**
 * @brief Calculate and apply control to wheels
 *
 * @param wheels struct with all wheels
 * @param speed_x + speed_y between -100 and 100
 */
void ControlDir(Wheels wheels, float speed_x, float speed_y) {

  ControlWheel(wheels.frontLeft, speed_x - speed_y);
  ControlWheel(wheels.frontRight, speed_x + speed_y);
  ControlWheel(wheels.rearLeft, speed_x + speed_y);
  ControlWheel(wheels.rearRight, speed_x - speed_y);
}

/**
 * @brief Control rotation the car
 *
 * @param wheels struct with all wheels
 * @param speed_rot speed to ratate
 */
void ControlRot(Wheels wheels, float speed_rot) {
  ControlWheel(wheels.frontLeft, -speed_rot);
  ControlWheel(wheels.frontRight, speed_rot);
  ControlWheel(wheels.rearLeft, -speed_rot);
  ControlWheel(wheels.rearRight, speed_rot);
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

  // Set mote resolution to PWM
  ledcSetup(0, 20000, 16);
  ledcSetup(1, 20000, 16);
  ledcSetup(2, 20000, 16);
  ledcSetup(3, 20000, 16);

  const Wheels wheels = {{WHEEL_FL_PWM, WHEEL_FL_DIR},
                         {WHEEL_FR_PWM, WHEEL_FR_DIR},
                         {WHEEL_RL_PWM, WHEEL_RL_DIR},
                         {WHEEL_RR_PWM, WHEEL_RR_DIR}};

  ControlDir(wheels, 0, 0);
  ControlRot(wheels, 0);

  // Start serial
  Serial.begin(115200);
}

void loop() {}
