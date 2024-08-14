#include "HWCDC.h"
#include "NimBLEDevice.h"
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

// UUIDs

#define SERVICE_UUID "014dd1c0-9dde-4907-8c31-e7d6b7a77ddb"
#define CHARACTERISTIC_UUID "c7be25a0-d82d-44e0-8155-3cbac410e2ed"
#define ADVERTISE_UUID "1a995753-23ae-43da-95ce-5372236406ed"

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

// global speed state
float x, y, r = 0;

/**
 * @brief set pwm to control speed between -100 and 100 and direction.
 *
 * @param wheel pins of wheel
 * @param speed speed between -1 and 1
 */
void ControlWheel(Wheel wheel, float_t speed) {
  if (speed > 0) {
    digitalWrite(wheel.dirPin, HIGH);
  } else {
    digitalWrite(wheel.dirPin, LOW);
    speed = -speed;
  }

  // convert float [0, 1] to uint16_t
  uint16_t pwmValue = trunc(speed * 65535);
  ledcWrite(wheel.pwmPin, pwmValue);
}

/**
 * @brief Calculate and apply control to wheels
 *
 * @param wheels struct with all wheels
 * @param speed_x + speed_y between -1 and 1
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
 * @param speed_rot speed to ratate value [-1, 1]
 */
void ControlRot(Wheels wheels, float speed_rot) {
  ControlWheel(wheels.frontLeft, -speed_rot);
  ControlWheel(wheels.frontRight, speed_rot);
  ControlWheel(wheels.rearLeft, -speed_rot);
  ControlWheel(wheels.rearRight, speed_rot);
}

class ServerCallback : public NimBLEServerCallbacks {

  void onConnect(NimBLEServer *pServer) {
    Serial.println("Client Connected");

    digitalWrite(LED, LOW);
    // Buzzer sinalize

    pServer->startAdvertising();
  }

  void onDisconnect(NimBLEServer *pServer) {
    Serial.println("Client Disconnected");

    digitalWrite(LED, HIGH);
    // Buzzer sinalize
    //
    pServer->startAdvertising();
  }
};

/**
 * @class ControlCarCallback
 * @brief Class to callback that will control car
 *
 */
class ControlCarCallback : public NimBLECharacteristicCallbacks {

  /**
   * @brief ControlCar with received value as "x0.356y0.631r0.000"
   */
  void onWrite(NimBLECharacteristic *chr) {
    std::string value = chr->getValue();

    sscanf(value.c_str(), "x%fy%fr%f", &x, &y, &r);

    ControlDir({{WHEEL_FL_PWM, WHEEL_FL_DIR},
                {WHEEL_FR_PWM, WHEEL_FR_DIR},
                {WHEEL_RL_PWM, WHEEL_RL_DIR},
                {WHEEL_RR_PWM, WHEEL_RR_DIR}},
               x, y);
    ControlRot({{WHEEL_FL_PWM, WHEEL_FL_DIR},
                {WHEEL_FR_PWM, WHEEL_FR_DIR},
                {WHEEL_RL_PWM, WHEEL_RL_DIR},
                {WHEEL_RR_PWM, WHEEL_RR_DIR}},
               r);

    Serial.printf("x:%.3f y:%.3f r:%.3f\n", x, y, r);
  }
};

void setup() {
  // Turn off led
  pinMode(LED, OUTPUT);
  digitalWrite(LED, HIGH);

  // Set pin mode to all pins
  pinMode(WHEEL_FL_PWM, OUTPUT);
  pinMode(WHEEL_FL_DIR, OUTPUT);
  pinMode(WHEEL_FR_PWM, OUTPUT);
  pinMode(WHEEL_FR_DIR, OUTPUT);
  pinMode(WHEEL_RR_PWM, OUTPUT);
  pinMode(WHEEL_RR_DIR, OUTPUT);
  pinMode(WHEEL_RL_DIR, OUTPUT);
  pinMode(WHEEL_RL_PWM, OUTPUT);

  // Set resolution to PWM
  ledcSetup(0, 20000, 16);
  ledcSetup(1, 20000, 16);
  ledcSetup(2, 20000, 16);
  ledcSetup(3, 20000, 16);

  // Attach pins to PWM
  ledcAttachPin(WHEEL_FL_PWM, 0);
  ledcAttachPin(WHEEL_FR_PWM, 1);
  ledcAttachPin(WHEEL_RR_PWM, 2);
  ledcAttachPin(WHEEL_RL_PWM, 3);

  const Wheels wheels = {{WHEEL_FL_PWM, WHEEL_FL_DIR},
                         {WHEEL_FR_PWM, WHEEL_FR_DIR},
                         {WHEEL_RL_PWM, WHEEL_RL_DIR},
                         {WHEEL_RR_PWM, WHEEL_RR_DIR}};

  ControlDir(wheels, x, y);
  ControlRot(wheels, r);

  // Start serial
  Serial.begin(115200);

  NimBLEDevice::init("RamoCar");

  NimBLEServer *pServer = NimBLEDevice::createServer();
  pServer->setCallbacks(new ServerCallback());

  NimBLEService *pService = pServer->createService(SERVICE_UUID);
  NimBLECharacteristic *pCharacteristic = pService->createCharacteristic(
      CHARACTERISTIC_UUID, NIMBLE_PROPERTY::WRITE);
  pCharacteristic->setCallbacks(new ControlCarCallback());

  pService->start();

  NimBLEAdvertising *pAdvertising = NimBLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(ADVERTISE_UUID);
  pAdvertising->start();
}

void loop() {}
