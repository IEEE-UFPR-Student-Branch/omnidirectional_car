#include "NimBLEDevice.h"
#include <Arduino.h>

// Defining the pins and channels to control the wheels

// Front Left wheel
const uint8_t WHEEL_FL_PWM = 4;
const uint8_t WHEEL_FL_CHAN = 0;
const uint8_t WHEEL_FL_DIR = 3;
// Front Right wheel
const uint8_t WHEEL_FR_PWM = 5;
const uint8_t WHEEL_FR_CHAN = 1;
const uint8_t WHEEL_FR_DIR = 6;
// Rear Left wheel
const uint8_t WHEEL_RL_PWM = 2;
const uint8_t WHEEL_RL_CHAN = 2;
const uint8_t WHEEL_RL_DIR = 1;
// Rear Right wheel
const uint8_t WHEEL_RR_PWM = 7;
const uint8_t WHEEL_RR_CHAN = 3;
const uint8_t WHEEL_RR_DIR = 9;

const uint8_t PWM_RESOLUTION = 10; // Resolution of PWM to control speed wheels
const uint32_t PWM_FREQ = 2000;    // Frequency of PWM to control speed wheels

const uint8_t LED = 8; // Led pin

// Define Buzzer Characteristics
const uint8_t BUZZER = 10;           // Pin
const uint8_t BUZZER_RESOLUTION = 8; // Resolução do buzzer

// UUIDs
const char *SERVICE_UUID = "014dd1c0-9dde-4907-8c31-e7d6b7a77ddb";
const char *CHARACTERISTIC_CON_UUID = "c7be25a0-d82d-44e0-8155-3cbac410e2ed";
const char *CHARACTERISTIC_BUZ_UUID = "1e29d664-837a-42cd-8051-451e8985c08b";
const char *ADVERTISE_UUID = "1a995753-23ae-43da-95ce-5372236406ed";

// Wheel infos
struct Wheel {
  uint8_t pwmPin;
  uint8_t pwmChan;
  uint8_t dirPin;
};

// Wheels organize
struct Wheels {
  Wheel frontLeft;
  Wheel frontRight;
  Wheel rearLeft;
  Wheel rearRight;
};

// Set global wheels
const Wheels wheels = {{WHEEL_FL_PWM, WHEEL_FL_CHAN, WHEEL_FL_DIR},
                       {WHEEL_FR_PWM, WHEEL_FR_CHAN, WHEEL_FR_DIR},
                       {WHEEL_RL_PWM, WHEEL_RL_CHAN, WHEEL_RL_DIR},
                       {WHEEL_RR_PWM, WHEEL_RR_CHAN, WHEEL_RR_DIR}};

// global speed and buzzer value
float speed_x, speed_y, speed_r = 0;

/**
 * @brief set pwm to control speed between -100 and 100 and direction.
 *
 * @param wheel pins of wheel
 * @param speed speed between -1 and 1
 */
void ControlWheel(Wheel wheel, float_t speed) {
  if (speed > 0) {
    digitalWrite(wheel.dirPin, HIGH);
    speed = 1 - speed; // invert Duty cycle
  } else {
    digitalWrite(wheel.dirPin, LOW);
    speed = -speed;
  }

  // convert float [0, 1] to uint16_t
  uint16_t pwmValue = (uint16_t)(speed * 1023.0f);
  // Define PWM on channel
  ledcWrite(wheel.pwmChan, pwmValue);
}

/**
 * @brief Calculate and apply control to wheels set speed_x, speed_y or speed_r
 *
 * @param wheels struct with all wheels
 * @param speed_x + speed_y between in [-1, 1] if speed_r = 0
 * @param speed_r  between in [-1, 1] if speed_x = speed_y = 0
 */
void ControlWheels(Wheels wheels, float speed_x, float speed_y, float speed_r) {

  ControlWheel(wheels.frontLeft, speed_x - speed_y - speed_r);
  ControlWheel(wheels.frontRight, speed_x + speed_y + speed_r);
  ControlWheel(wheels.rearLeft, speed_x + speed_y - speed_r);
  ControlWheel(wheels.rearRight, speed_x - speed_y + speed_r);
}

class ServerCallback : public NimBLEServerCallbacks {

  void onConnect(NimBLEServer *pServer) {
    Serial.println("Client Connected");

    digitalWrite(LED, LOW);

    // Buzzer Sinalize

    pServer->startAdvertising();
  }

  void onDisconnect(NimBLEServer *pServer) {
    ControlWheels(wheels, 0, 0, 0); // Stop Car
    Serial.println("Client Disconnected");

    digitalWrite(LED, HIGH);

    // Buzzer Sinalize

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

    sscanf(value.c_str(), "x%fy%fr%f", &speed_x, &speed_y, &speed_r);

    ControlWheels(wheels, speed_x, speed_y, speed_r);

    Serial.printf("Received: x = %.3f y = %.3f r = %.3f\n\r", speed_x, speed_y,
                  speed_r);
  }

  /**
   * @brief Read value state of wheels.
   *
   * @param chr
   */
  void onRead(NimBLECharacteristic *chr) {
    Serial.printf(
        "FL: PWMpin = %d; Duty cycle: %u; DIRpin = %d; level = %s\n\r",
        wheels.frontLeft.pwmPin, ledcRead(wheels.frontLeft.pwmChan),
        wheels.frontLeft.dirPin,
        digitalRead(wheels.frontLeft.dirPin) ? "HIGH" : "LOW");
    Serial.printf(
        "FR: PWMpin = %d; Duty cycle: %u; DIRpin = %d; level = %s\n\r",
        wheels.frontRight.pwmPin, ledcRead(wheels.frontRight.pwmChan),
        wheels.frontRight.dirPin,
        digitalRead(wheels.frontRight.dirPin) ? "HIGH" : "LOW");
    Serial.printf(
        "RL: PWMpin = %d; Duty cycle: %u; DIRpin = %d; level = %s\n\r",
        wheels.rearLeft.pwmPin, ledcRead(wheels.rearLeft.pwmChan),
        wheels.rearLeft.dirPin,
        digitalRead(wheels.rearLeft.dirPin) ? "HIGH" : "LOW");
    Serial.printf(
        "RR: PWMpin = %d; Duty cycle: %u; DIRpin = %d; level = %s\n\r",
        wheels.rearRight.pwmPin, ledcRead(wheels.rearRight.pwmChan),
        wheels.rearRight.dirPin,
        digitalRead(wheels.rearRight.dirPin) ? "HIGH" : "LOW");
  }
};

/**
 * @class ControlCarBuzCallback
 * @brief Class to callback that will control the buzzer of car
 *
 */
class ControlCarBuzCallback : public NimBLECharacteristicCallbacks {

  void onWrite(NimBLECharacteristic *chr) {
    std::string value = chr->getValue();

    // buzzer Sinalize with diferents tons

    Serial.printf("Received buz = %s\n\r", value == "1" ? "true" : "false");
  }
};

void setup() {
  // Turn off led
  pinMode(LED, OUTPUT);
  digitalWrite(LED, HIGH);

  // Set direction pin
  pinMode(WHEEL_FL_DIR, OUTPUT);
  pinMode(WHEEL_FR_DIR, OUTPUT);
  pinMode(WHEEL_RL_DIR, OUTPUT);
  pinMode(WHEEL_RR_DIR, OUTPUT);

  // Set resolution and frequency to wheel's PWM
  ledcSetup(WHEEL_FL_CHAN, PWM_FREQ, PWM_RESOLUTION);
  ledcSetup(WHEEL_FR_CHAN, PWM_FREQ, PWM_RESOLUTION);
  ledcSetup(WHEEL_RL_CHAN, PWM_FREQ, PWM_RESOLUTION);
  ledcSetup(WHEEL_RR_CHAN, PWM_FREQ, PWM_RESOLUTION);
  // Set resolution and frquency to buzzer's PWM
  ledcSetup(4, 0, BUZZER_RESOLUTION);

  // Attach pins to PWM
  ledcAttachPin(WHEEL_FL_PWM, WHEEL_FL_CHAN);
  ledcAttachPin(WHEEL_FR_PWM, WHEEL_FR_CHAN);
  ledcAttachPin(WHEEL_RL_PWM, WHEEL_RL_CHAN);
  ledcAttachPin(WHEEL_RR_PWM, WHEEL_RR_CHAN);

  ledcAttachPin(BUZZER, 4);

  ControlWheels(wheels, speed_x, speed_y, speed_r);

  // Start serial
  Serial.begin(115200);

  // Set Bluetooth Name
  NimBLEDevice::init("RamoCar");

  // Create end set Callback to Server
  NimBLEServer *pServer = NimBLEDevice::createServer();
  pServer->setCallbacks(new ServerCallback());

  // Create Service and Characteristic to control car
  NimBLEService *pService = pServer->createService(SERVICE_UUID);
  // Create and set Callback to control direction and rotation of car
  NimBLECharacteristic *pCharacteristicCar = pService->createCharacteristic(
      CHARACTERISTIC_CON_UUID, NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::WRITE);
  pCharacteristicCar->setCallbacks(new ControlCarCallback());
  // Create and set Callback to control Buzzer of car
  NimBLECharacteristic *pCharacteristicBuz = pService->createCharacteristic(
      CHARACTERISTIC_BUZ_UUID, NIMBLE_PROPERTY::WRITE);
  pCharacteristicBuz->setCallbacks(new ControlCarBuzCallback());

  // Start service
  pService->start();

  // Create and set UUI to Advertising
  NimBLEAdvertising *pAdvertising = NimBLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(ADVERTISE_UUID);
  pAdvertising->start();
}

void loop() { delay(10000); }
