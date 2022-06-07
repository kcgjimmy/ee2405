import matplotlib.pyplot as plt
import numpy as np
import serial
import time

Fs = 256.0;  # sampling rate
Ts = 1.0/Fs; # sampling interval
t = np.arange(0,1,Ts) # time vector; create Fs samples between 0 and 1.0 sec.
y = np.arange(0,1,Ts) # signal vector; create Fs samples
y_FIR = np.arange(0,1,Ts) # signal vector; create Fs samples

n = len(y) # length of the signal
k = np.arange(n)
T = n/Fs
frq = k/T # a vector of frequencies; two sides frequency range
frq = frq[range(int(n/2))] # one side frequency range

serdev = 'COM4'
s = serial.Serial(serdev)
print("ready to insert data")
for x in range(0, int(Fs)):
    line=s.readline() # Read an echo string from B_L4S5I_IOT01A terminated with '\n'
    # print line
    y[x] = float(line)
    print(y[x])

for x in range(0, int(Fs)):
    if(x == 0):
        y_FIR[0] = (0 + y[0] / 2)
    else :
        y_FIR[x] = (y[x] + y[x-1]) / 2

Y = np.fft.fft(y)/n*2 # fft computing and normalization
Y = Y[range(int(n/2))] # remove the conjugate frequency parts

Y_FIR = np.fft.fft(y_FIR)/n*2
Y_FIR = Y_FIR[range(int(n/2))]

fig, ax = plt.subplots(2, 2)

ax[0,0].plot(t,y)
ax[0,0].set_xlabel('Time')
ax[0,0].set_ylabel('Amplitude')
ax[1,0].plot(frq,abs(Y),'r') # plotting the spectrum
ax[1,0].set_xlabel('Freq (Hz)')
ax[1,0].set_ylabel('|Y(freq)|')

ax[0,1].plot(t,y_FIR)
ax[0,1].set_xlabel('Time_FIR')
ax[0,1].set_ylabel('Amplitude_FIR')
ax[1,1].plot(frq,abs(Y_FIR),'r') # plotting the spectrum
ax[1,1].set_xlabel('Freq_FIR (Hz)')
ax[1,1].set_ylabel('|Y_FIR(freq)|')
plt.show()
s.close()