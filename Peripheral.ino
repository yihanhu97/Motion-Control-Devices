
#include <CurieBLE.h>
#include "CurieIMU.h"
#include "CurieTimerOne.h"

BLEService instructions("19B10000-E8F2-537E-7733-D104768A5678"); // BLE readData Service

int notifyLength = 3; // maximum length of the notify string!
BLECharacteristic notify_instructions("19B10001-E8F2-537E-7733-D104768A5600", BLERead | BLEWrite, notifyLength);

int EMG1 = A0;
int EMG2 = A1;
int timeISR = 15000;
int ind = 0;

int EMG_reading_1; // EMG1 value
int EMG_reading_2; // EMG2 value
float gx, gy, gz; //scaled Gyro values
float ax, ay, az; //scaled Accelerometer values

String write_string;
bool hasread = false;
bool first_flag = true;
bool second_flag = true;
bool python = false;

// EMG scaling factors
const double adc_points = 1024;
const double adc_voltage = 3.3;
const double emg_scale = 3600;
const double emg_offset = 1.5;

// Accel threshold values
const double ACCEL_MEAN = 1;
const double ACCEL_TOLER = .3;


void readData(){
  EMG_reading_1 = analogRead(EMG1);
  EMG_reading_2 = analogRead(EMG2);
  CurieIMU.readGyroScaled(gx, gy, gz);
  CurieIMU.readAccelerometerScaled(ax, ay, az);
  hasread = true; 
}

void setup() {
  Serial.begin(115150);

  while(!Serial){};
  // Serial.println("BLE Data Generator Peripheral");

  // Initialize Accelerometer and Gryo
  CurieIMU.begin();
  CurieIMU.setAccelerometerRange(2);
  CurieIMU.setGyroRange(250);

  // begin initialization
  BLE.begin();

  // set advertised local name and service UUID:
  BLE.setLocalName("Data Generator");
  BLE.setAdvertisedService(instructions);

  // add the characteristic to the service
  instructions.addCharacteristic(notify_instructions);
  
  // add service
  BLE.addService(instructions);
   
  // start advertising
  BLE.advertise();
}

void loop() {
  // listen for BLE peripherals to connect:
  BLEDevice central = BLE.central();

  // if a central is connected to peripheral:
  if (central) {
    // Serial.print("Connected to central: ");
    // print the central's MAC address:
    // Serial.println(central.address());
    CurieTimerOne.start(timeISR, &readData);
    // t0 = millis();
 
    String move_direction = "n";
    // all directions are n, l, r, f
    // Nothing, Left, Right, Forward

    String accel_command = "n";
    // all accel are n, u, d
    // Nothing, speed Up, speed Down

    String EMG_directions = "n";
    //all pickup/drop are n, u, d
    // Nothing, pickup, drop

    // while the central is still connected to peripheral:
    while (central.connected()) {
      if(hasread){
         if(!python)
         {
           EMG_reading_1 = (((adc_points * EMG_reading_1 / adc_voltage) - emg_offset) / emg_scale);
           EMG_reading_2 = (((adc_points * EMG_reading_2 / adc_voltage) - emg_offset) / emg_scale);
           Serial.print(write_string);
           Serial.print("\t");
           Serial.print(EMG_reading_1);
           Serial.print("\t");
           
           Serial.println(EMG_reading_2);          
           ind++;
         }
          
          // move direction command
          if(ay >= -(ACCEL_MEAN + ACCEL_TOLER) && ay <= -(ACCEL_MEAN - ACCEL_TOLER))
          {
            move_direction = "l";
          }
          else if(ay <= ACCEL_MEAN + ACCEL_TOLER && ay >= ACCEL_MEAN - ACCEL_TOLER)
          {
            move_direction = "r";
          }
          else if(ax <= ACCEL_MEAN + ACCEL_TOLER && ax >= ACCEL_MEAN - ACCEL_TOLER)
          {
            move_direction = "f";
          }
          else if(ax >= -(ACCEL_MEAN + ACCEL_TOLER) && ax <= -(ACCEL_MEAN - ACCEL_TOLER))
          {
            accel_command = "u";
          }
          else if(az >= -(ACCEL_MEAN + ACCEL_TOLER) && az <= -(ACCEL_MEAN - ACCEL_TOLER))
          {
            accel_command = "d";
          }
          else
          {
            move_direction = "n";
            accel_command = "n";   
          }
          
          //speed command
//          if(gz >= 150 && first_flag && second_flag && accel_command != "d")
//          {
//            accel_command = "u";
//            first_flag = false;
//            second_flag = false;
//          }
//          else if(gz <= -150 && accel_command != "d")
//          {
//            first_flag = true;
//          }
//          else if(gz >= -100 && first_flag && accel_command != "d")
//          {
//            second_flag = true;
//            accel_command = "n";
//          }
//
//          if(gz <= -150 && first_flag && second_flag && accel_command != "u")
//          {
//            accel_command = "d";
//            first_flag = false;
//            second_flag = false;
//          }
//          else if(gz >= 150 && accel_command != "u")
//          {
//            first_flag = true;
//          }
//          else if(gz <= 100 && first_flag && accel_command != "u")
//          {
//            second_flag = true;
//            accel_command = "n";
//          }

          //Pickup/drop command
          if(ind == 30 && !python)
          {
            python = true;
            //CurieTimerOne.pause();
            //while(!Serial.available());
            //EMG_directions = Serial.readString();
            ind = 0;       
          }

          if(python && Serial.available())
          {
            EMG_directions = Serial.readString();
            python = false;
          }

          write_string = move_direction + accel_command + EMG_directions;

          notify_instructions.writeString(write_string);

          hasread = false;
      }
    }
    
    // when the central disconnects, print it out:
    Serial.print(F("Disconnected from central: "));
    Serial.println(central.address());
  }
}
