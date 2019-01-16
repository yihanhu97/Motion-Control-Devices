/*
 * Copyright (c) 2016 Intel Corporation.  All rights reserved.
 * See the bottom of this file for the license terms.
 */

/**
 *
 * Sketch: LedControl.ino
 *
 * Description:
 *   This is a Central sketch that looks for a particular Sevice with a
 *   certain Characteristic from a Peripheral.  Upon succesful discovery,
 *   it reads the state of a button and write that value to the
 *   Peripheral Characteristic.
 *
 *
 */

#include <CurieBLE.h>

char BLE_UUID[] = "19B10000-E8F2-537E-7733-D104768A5678";

char data_UUID[] = "19B10001-E8F2-537E-7733-D104768A5600";

const int upper_left_pin = 4;
const int lower_left_pin = 5;
const int upper_right_pin = 6;
const int lower_right_pin = 7;

void setup() 
{
    // Serial.begin(115200);
    // while(!Serial){};

    pinMode(upper_left_pin, OUTPUT);
    pinMode(lower_left_pin, OUTPUT);
    pinMode(upper_right_pin, OUTPUT);
    pinMode(lower_right_pin, OUTPUT);

    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, LOW);

    // initialize the BLE hardware
    BLE.begin();

    digitalWrite(LED_BUILTIN, HIGH);

    // start scanning for peripherals
    BLE.scanForUuid(BLE_UUID);
}

void loop() 
{
    // check if a peripheral has been discovered
    BLEDevice peripheral = BLE.available();

    if (peripheral) 
    {
        // stop scanning
        BLE.stopScan();

        read_data(peripheral);

        // peripheral disconnected, start scanning again
        BLE.scanForUuid(BLE_UUID);
    }
}

void read_data(BLEDevice peripheral) 
{

    if(!peripheral.connect())
    {
        return;
    }
    else
    {
        // Serial.println("connected");
    }

    if(!peripheral.discoverAttributes())
    {
        peripheral.disconnect();
        return;
    }

    BLECharacteristic instructions = peripheral.characteristic(data_UUID);

    while (peripheral.connected()) 
    {

        String data_string = "nnn";

        if(instructions.canRead())
        {
            instructions.read();
            data_string = instructions.stringValue();
        }

        // Serial.println(data_string);

        char move_instructions = data_string.charAt(0);
        char accel_instructions = data_string.charAt(1);
        char item_instructions = data_string.charAt(2);

        if(move_instructions == 'l')
        {
            digitalWrite(upper_left_pin, LOW);
            // Serial.println("left");
        }
        else if(move_instructions == 'r')
        {
            digitalWrite(upper_right_pin, LOW);
            // Serial.println("right");
        }
        else if(move_instructions == 'f')
        {
            digitalWrite(upper_left_pin, LOW);
            digitalWrite(upper_right_pin, LOW);
            // Serial.println("forward");
        }
        else
        {
            reset_motors();

            if(accel_instructions == 'u')
            {
                digitalWrite(upper_left_pin, LOW);
                digitalWrite(lower_left_pin, LOW);
                // Serial.println("speeding up");
            }
            else if(accel_instructions == 'd')
            {
                digitalWrite(upper_right_pin, LOW);
                digitalWrite(lower_right_pin, LOW);
                // Serial.println("slowing down");
            }
            else
            {
                reset_motors();

                if(item_instructions == 'u')
                {
                    digitalWrite(lower_left_pin, LOW);
                    // Serial.println("picking up");
                }
                else if(item_instructions == 'd')
                {
                    digitalWrite(lower_right_pin, LOW);
                    // Serial.println("putting down");
                }
                else
                {
                    reset_motors();
                    // Serial.println("");
                }
            }
        }

        if(move_instructions == 'l')
        {
            digitalWrite(upper_left_pin, LOW);
        }
        else if(move_instructions == 'r')
        {
            digitalWrite(upper_right_pin, LOW);
        }
        else if(move_instructions == 'f')
        {
            digitalWrite(upper_left_pin, LOW);
            digitalWrite(upper_right_pin, LOW);
        }
        else
        {
            reset_motors();

            if(accel_instructions == 'u')
            {
                digitalWrite(upper_left_pin, LOW);
                digitalWrite(lower_left_pin, LOW);
            }
            else if(accel_instructions == 'd')
            {
                digitalWrite(upper_right_pin, LOW);
                digitalWrite(lower_right_pin, LOW);
            }
            else
            {
                reset_motors();

                if(item_instructions == 'u')
                {
                    digitalWrite(lower_left_pin, LOW);
                }
                else if(item_instructions == 'd')
                {
                    digitalWrite(lower_right_pin, LOW);
                }
                else
                {
                    reset_motors();
                }
            }
        }        
        
    }

    // Serial.println("Peripheral disconnected");
}

void reset_motors()
{
    digitalWrite(upper_left_pin, HIGH);
    digitalWrite(lower_left_pin, HIGH);

    digitalWrite(upper_right_pin, HIGH);
    digitalWrite(lower_right_pin, HIGH);
}

void flash_motors()
{
    digitalWrite(upper_left_pin, LOW);
    digitalWrite(lower_left_pin, LOW);

    digitalWrite(upper_right_pin, LOW);
    digitalWrite(lower_right_pin, LOW);

    delay(100);

    reset_motors();
}