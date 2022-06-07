#include "mbed.h"
#include <cmath>
#include <cstdio>

EventQueue print_queue(32 * EVENTS_EVENT_SIZE);
Thread waveform_thread;
Thread print_thread;
AnalogOut aout(PA_4);
void W();

float waveform_count = 0;
float temp;
float sample;
int main()
{
    double rads = 0.0;
    printf("HELLO\n");
    waveform_thread.start(W);
    //print_thread.start(callback(&print_queue, &EventQueue::dispatch_forever));
    while (1) {
      // sinewave output
      /*
        for (int i = 0; i <= 6; i++) {
            rads = (pi * i) / 180.0f;
            sample = (uint16_t)(amplitude * (offset * (tanh(rads + pi))) + offset);
            
            if(i < 3) sample = 3*tanh(i);
            else sample = 3*(1-tanh(i-3));
            aout.write_u16(sample);
        }*/
        /*
        if (waveform_count <= 50) {
            sample  = tanh(waveform_count/100.0);
        }
        else sample  = 1-tanh((waveform_count-50.0)/100.0);
        waveform_count++;
        if (waveform_count > 100) waveform_count = 0;
        aout = sample * (3.0/3.3);
        ThisThread::sleep_for(1ms);*/
    ThisThread::sleep_for(1s);

   }
}



void W(){
    while(1) {
        if (waveform_count <= 50)
            sample  = tanh(waveform_count/50.0*3);
        else 
            sample  = 1-tanh(3*(waveform_count-50.0)/50.0);
        waveform_count++;
        if (waveform_count > 100) {
            waveform_count = 0;
            print_queue.call(printf, "END\n");
        }
        aout = sample * (3.0/3.3);
        ThisThread::sleep_for(1ms);
    }

};