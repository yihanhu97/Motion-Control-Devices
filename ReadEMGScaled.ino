#include "CurieTimerOne.h"

bool read_sensor = false;

int timer_interval = 20000; // in microseconds, 200 hz

double x;

long time_start;

double adc_points = 1024;
double adc_voltage = 3.3;
double emg_scale = 3600;
double emg_offset = 1.5;

void toggle_sensor_reading()
{
    
    x = analogRead(0);
    read_sensor = true;
}

void setup()
{
    Serial.begin(115200);
    while(!Serial);

    delay(5000);

    CurieTimerOne.start(timer_interval, &toggle_sensor_reading);

    time_start = micros();
}

void loop()
{
    if(read_sensor)
    {

        x = (((adc_points * x / adc_voltage) - emg_offset) / emg_scale);

        Serial.print(micros() - time_start);
        Serial.print("\t");
        Serial.println(x);

        read_sensor = false;
    }
}
