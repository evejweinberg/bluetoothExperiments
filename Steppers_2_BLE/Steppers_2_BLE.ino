
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

 bool motorOn = false;

#include <Stepper.h>

// Import libraries (BLEPeripheral depends on SPI)
#include <SPI.h>
#include <BLEPeripheral.h>

// define pins (varies per shield/board)
// https://github.com/sandeepmistry/arduino-BLEPeripheral#pinouts
// Blend
#define BLE_REQ     9
#define BLE_RDY     8
#define BLE_RST     5

// create peripheral instance, see pinouts above
BLEPeripheral blePeripheral = BLEPeripheral(BLE_REQ, BLE_RDY, BLE_RST);

// create service
BLEService motorService = BLEService("EE10");

// create switch characteristic
BLECharCharacteristic startedOrStoppedChar = BLECharCharacteristic("EE11", BLERead | BLEWrite);
//BLEDescriptor descriptor = BLEDescriptor("2901", "Start/Stop");

////////////////////////////////////////


const int stepsPerRevolution = 200;  // change this to fit the number of steps per revolution
// for your motor


//Stepper myStepper(stepsPerRevolution, 9,10,11,12);
Stepper myStepper2(stepsPerRevolution, 4,5,6,7);

int stepCount = 0;         // number of steps the motor has taken

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
//  blePeripheral.addAttribute(descriptor);

  // assign event handlers for characteristic
  //the event that happens, and the callback
  startedOrStoppedChar.setEventHandler(BLEWritten, characteristicWritten);
  
  // begin initialization
  blePeripheral.begin();
  Serial.println("motor initializded");

  //////////////////
}

void loop() {
  // step one step:
  if (motorOn){
  myStepper2.step(1);
  Serial.print("steps1:" );
  Serial.println(stepCount);
  stepCount++;
  delay(10);
  } else{
    myStepper2.step(0);
  }
}


void characteristicWritten(BLECentral& central, BLECharacteristic& characteristic) {
  // central wrote new value to characteristic, update LED
  Serial.print(F("motor event, written: "));

  if (startedOrStoppedChar.value()) {
    Serial.println(F("GO"));
    motorOn = true;
  } else {
    Serial.println(F("pause"));
    motorOn = false;
  }
}

