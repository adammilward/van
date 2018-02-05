/*
 * VoltMeter.cpp
 *
 *  Created on: 18 Nov 2017
 *      Author: Adam Milward
 */

#include "VoltMeter.h"

VoltMeter::VoltMeter() {
    retrieveCalibration();
}

void VoltMeter::toggleConfigMode() {
    configMode = (configMode == OFF) ? ON : OFF;
}

float VoltMeter::getVoltage(int pinNumber) {
    delay(10);
    analogRead(pinNumber);
    delay(10);
    return (configMode) ?
            analogRead(pinNumber):
            analogRead(pinNumber)*convFactor[pinNumber] ;
}

void VoltMeter::setPin(int pin, float newValue) {
    float analogValue = 0;
    analogRead(pin);
    delay(10);
    int i = 3;
    while (i--) {
        analogValue += analogRead(pin);
        delay(10);
    }
    analogValue /= 3;
    convFactor[pin] = newValue/analogValue;
    Gbl::strPtr->print(pin);
    Gbl::strPtr->print(F(" set to "));
    Gbl::strPtr->println(newValue);
}

float VoltMeter::getCalibration(int pin) {
    return convFactor[pin];
}

void VoltMeter::saveCalibration() {
    for (int pin = 0; pin < numberOfPins; pin++) {
        float value = convFactor[pin];
        int ee = pin*sizeof(value);
        byte* p = (byte*)(void*)&value;
        for (byte i = 0; i < sizeof(value); i++) {
            EEPROM.write(ee++, *p++);
        }
    }
    Gbl::strPtr->print(F("calibration saved"));
}

void VoltMeter::retrieveCalibration() {
    for (int pin = 0; pin < numberOfPins; pin++) {
        float value = 0.0;
        int ee = pin*sizeof(value);
        byte* p = (byte*)(void*)&value;
        for (byte i = 0; i < sizeof(value); i++) {
            *p++ = EEPROM.read(ee++);
            if (value != 0) {
                convFactor[pin] = value;
            }
        }
    }
}
