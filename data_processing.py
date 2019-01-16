import serial
import matplotlib.pyplot as plt
import matplotlib.animation as animation
from scipy import signal as sig
import numpy as np

ser = serial.Serial("/dev/cu.usbmodem1411", 115200)

N = 30

EMG_scaled_values1 = np.zeros(N)
EMG_scaled_values2 = np.zeros(N)

filter_order = 3
sample_hz = 50
nyqz_hz = sample_hz / 2

low_cutoff = 10
low_frac = low_cutoff / nyqz_hz

high_cutoff = 20
high_frac = high_cutoff / nyqz_hz

b_low, a_low = sig.butter(filter_order, low_frac, "lowpass", analog = False)
b_high, a_high = sig.butter(filter_order, high_frac, "highpass", analog = False)

def grab_samples():
    global EMG_scaled_values1
    global EMG_scaled_values2

    for i in range(N):
        new_data = ser.readline().decode("utf-8")
        new_data = new_data.split("\t")

        print(new_data)

        EMG_scaled_values1[i] = float(new_data[1])
        EMG_scaled_values2[i] = float(new_data[2])

def filtering():
    # lowpass
    # print(EMG_scaled_values1, EMG_scaled_values2)

    EMG_low_pass1 = sig.lfilter(b_low, a_low, EMG_scaled_values1)
    EMG_low_pass2 = sig.lfilter(b_low, a_low, EMG_scaled_values2)

    # highpass
    
    EMG_high_low1 = sig.lfilter(b_high, a_high, EMG_low_pass1)
    EMG_high_low2 = sig.lfilter(b_high, a_high, EMG_low_pass2)

    # rectified 
    EMG_rect1 = np.absolute(EMG_high_low1)
    EMG_rect2= np.absolute(EMG_high_low2)

    # smooth
    box = sig.boxcar(10)
    EMG_smoothed1 = sig.lfilter(box, 1, EMG_rect1)
    EMG_smoothed2 = sig.lfilter(box, 1, EMG_rect2)

    return [np.average(EMG_smoothed1), np.average(EMG_smoothed2)]

def run_real_time():

    while(True):
        plt.pause(0.000001)           # wait before updating figure
        grab_samples()             # grab N samples from Serial
        avg_data = filtering()
        # the first data is controlled by the left calf, pick up
        # the second data is controlled by the right calf, drop

        print(avg_data)

        if(avg_data[0] > .25):
            ser.write("u".encode("ascii")) #.encode("ascii")
        elif(avg_data[1] > .25):
        	ser.write("d".encode("ascii")) #.encode("ascii"))
        else:
            ser.write("n".encode("ascii")) #.encode("ascii"))
    
run_real_time()    

ser.close()




