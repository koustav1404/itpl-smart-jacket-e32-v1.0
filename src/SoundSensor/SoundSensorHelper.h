#ifndef SOUND_SENSOR_HELPER_H
#define SOUND_SENSOR_HELPER_H

#include "config.h"
#include "./Interfaces/BridgeInterface.h"

#include <Wire.h>
#include <HardwareSerial.h>

#define A_PIN 35
#define D_PIN 34

unsigned int output;
const int sampleWindow = 10;
unsigned int sample;
unsigned long millisCurrent = 0;
unsigned long millisLast = 0;
unsigned long millisElapsed = 0;
int sampleBufferValue = 0;

int analogIo()
{
    unsigned long start_time = millis();
    float PeakToPeak = 0;
    int db;
    unsigned int maximum_signal = 0;    // minimum value
    unsigned int minimum_signal = 4095; // maximum value
    while (millis() - start_time < sampleWindow)
    {
        output = analogRead(A_PIN);
        if (output < 4095)
        {
            if (output > maximum_signal)
            {
                maximum_signal = output;
            }
            else if (output < minimum_signal)
            {
                minimum_signal = output;
            }
        }
    }
    PeakToPeak = maximum_signal - minimum_signal;
    db = map(PeakToPeak, 15, 500, 24, 120);
    return db;
}

int digitalIo()
{
    sampleBufferValue = 0;
    millisElapsed = 0;
    int db;
    while (millisElapsed < sampleWindow)
    {
        millisCurrent = millis();
        if (digitalRead(D_PIN) == LOW)
            sampleBufferValue++;
        millisElapsed = millisCurrent - millisLast;
    }
    millisLast = millisCurrent;
    db = map(sampleBufferValue, 0, 15000, 20, 120);
    return db;
}

void soundSensorSetup()
{
    MN_DEBUGLN("SoundSensor Setup");
    pinMode(A_PIN, INPUT);
    pinMode(D_PIN, INPUT);
    delay(50);
}

void soundSensorLoop()
{
    if (sound_sensor_mutex)
    {

        for (int i = 0; i < SOUND_SAMPLE_POINTS; i++)
        {
            sound_sensor_buffer[i][0] = digitalIo();
            if (SOUND_SAMPLE_POINTS - 1 == i)
            {
                end_timestamp = millis(); // should be modified
                sound_sensor_mutex = false;
            }
            delay(ONE_SEC / SOUND_SAMPLING_RATE);
        }
    }
    else
    {
        delay(10);
    }
}

#endif