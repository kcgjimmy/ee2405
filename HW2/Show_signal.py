import matplotlib.pyplot as plt
import numpy as np
import serial
import time

Fs = 10.0;  # sampling rate
Ts = 1.0/Fs; # sampling interval
t = np.arange(0,1,Ts) # time vector; create Fs samples between 0 and 1.0 sec.
y = np.arange(0,1,Ts) # signal vector; create Fs samples



serdev = 'COM4'
s = serial.Serial(serdev)
print("ready to insert data")
for x in range(0, int(Fs)):
    line = s.readline() # Read an echo string from B_L4S5I_IOT01A terminated with '\n'
    # print line
    y[x] = float(line)
    print(y[x])



fig, ax = plt.subplots(1, 1)
ax.plot(t,y)
ax.set_xlabel('Time')
ax.set_ylabel('Amplitude')

plt.show()
s.close()