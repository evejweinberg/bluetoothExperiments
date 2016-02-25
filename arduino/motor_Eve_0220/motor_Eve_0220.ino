/*
 Stepper Motor Control - one step at a time
 This program drives a unipolar or bipolar stepper motor.
 The motor will step one step at a time, very slowly.  You can use this to
 test that you've got the four wires of your stepper wired to the correct
 pins. If wired correctly, all steps should be in the same direction.

 Use this also to count the number of steps per revolution of your motor,
 if you don't know it.  Then plug that number into the oneRevolution
 example to see if you got it right.
*/
#include <Stepper.h>

// Import libraries (BLEPeripheral depends on SPI)
#include <SPI.h>
#include <BLEPeripheral.h>

// define pins (varies per shield/board)
// https://github.com/sandeepmistry/arduino-BLEPeripheral#pinouts
// Blend
#define BLE_REQ     9
#define BLE_RDY     8
// don't use a reset pin, since you need pin 5 for stepper2
#define BLE_RST     -1

// create peripheral instance, see pinouts above
BLEPeripheral blePeripheral = BLEPeripheral(BLE_REQ, BLE_RDY, BLE_RST);

// create service
BLEService motorService = BLEService("EE10"); 

// create characteristic
BLECharCharacteristic startedOrStoppedChar = BLECharCharacteristic("EE11", BLERead | BLEWrite);
BLEDescriptor descriptor = BLEDescriptor("2901", "Start/Stop");

bool motorOn = true;
int stepCount = 10;                   // number of steps the motor has taken
const int stepsPerRevolution = 200;  // change this to fit the number of steps per revolution for your motor

// The Blend board uses pin 9 so use another pin for the first stepper or adjust the jumpers on the board
//Stepper myStepper(stepsPerRevolution, 9,10,11,12);
Stepper myStepper2(stepsPerRevolution, 4,5,6,7);

void setup() {
  // initialize the serial port:
  Serial.begin(9600);

  /////////////////
  // set advertised local name and service UUID
  blePeripheral.setLocalName("Motor");
  blePeripheral.setDeviceName("Motor");
  blePeripheral.setAdvertisedServiceUuid(motorService.uuid());

  // add service and characteristics
  blePeripheral.addAttribute(motorService);
  blePeripheral.addAttribute(startedOrStoppedChar);
  blePeripheral.addAttribute(descriptor);

  // assign event handlers for characteristic
  // the event that happens, and the callback
  startedOrStoppedChar.setEventHandler(BLEWritten, characteristicWritten);

  // begin initialization
  blePeripheral.begin();

  Serial.println("motor sketch initialized");

  //////////////////
}

void loop() {
  blePeripheral.poll();
  if (motorOn){
    myStepper2.step(1);
    Serial.print("steps1:" );
    Serial.println(stepCount);
    stepCount++;

    // If this delay causes a problem with Bluetooth, try adding a read interval like
    // https://github.com/don/ITP-BluetoothLE/blob/master/arduino/Button_v2/Button_v2.ino#L53
    delay(10);
  } else{
    myStepper2.step(0);
  }
}

// central wrote new value to characteristic, update motor direction
void characteristicWritten(BLECentral& central, BLECharacteristic& characteristic) {
  Serial.print(F("motor event, written: "));

  uint8_t value = startedOrStoppedChar.value();

  if (value == 1) {
    Serial.println(F("GO"));
    motorOn = true;
  } else {
    Serial.println(F("pause"));
    motorOn = false;
  }
}
