#include "mbed.h"

#define freq_length 5
#define FFT_sample_number 256
#define FFT_error_sample 100

/** pin setting **/
AnalogOut Aout(D7);
AnalogIn Ain(A0);
DigitalIn Up_Button(D8);
DigitalIn Down_Button(D9);
DigitalIn Select_Button(D10);

/** function declaring **/
void frequency_selecting_mode(void);
void funtion_generating_mode(void);


int Mode;   // Mode 1 for function generating, Mode 0 for selecting frequency
float freq[freq_length] = {4.90, 6.12, 7.88, 12.12, 23.75}; // frequency list
int freq_sel = 0; // frequency selector
int delay_target[freq_length] = {50, 40, 31, 20, 10}; // DAC delay decide
int step_count = 0; // DAC step count
float FFT_data[FFT_error_sample + FFT_sample_number]; // FFT array
int FFT_sel = 0; // FFT array selector
int step = 0;   // full system step count
int i = 0;
float sample_dump;  // Dump  unused ADC data

int main()
{
    printf("System Ready\n");
    ThisThread::sleep_for(1s);
    Mode = 0;   // initialize mode

    while(Mode == 0) frequency_selecting_mode();
    while(Mode == 1) funtion_generating_mode();
}



// This function select the frequecny of funtion generator want to generating
void frequency_selecting_mode(void) {
    if (Up_Button.read() == 1) {
        ThisThread::sleep_for(10ms);
        while (Up_Button.read());      // wait for release
        if (freq_sel < freq_length -1) {
            freq_sel++;
            printf("Frequency select to %g\n", freq[freq_sel]); 
        } 
        else printf("%g is the max frequency.\n", freq[freq_sel]);
    }
    if (Down_Button.read() == 1) {
        while (Down_Button.read()) ;    // wait for release
        ThisThread::sleep_for(10ms);
        if (freq_sel > 0) {
            freq_sel--;
            printf("Frequency select to %g\n", freq[freq_sel]); 
        } 
        else printf("%g is the min frequency.\n", freq[freq_sel]);
    }
    if (Select_Button.read() == 1){
        ThisThread::sleep_for(10ms);
        while (Select_Button.read() == 1);
        Mode = 1;
        printf("Function generator started with %g hz.\n", freq[freq_sel]);
    }
}

// This funtion make ADC and DAC work
void funtion_generating_mode(void){
    // Decide voltage
    if(step % delay_target[freq_sel] == 0) {
            if (step_count < 40) Aout = (step_count / 40.0);
            else if(step_count < 50) Aout = 1;
            else Aout = 1 - ((step_count - 50) / 50.0);
            step_count++;
            if (step_count == 99) step_count = 0;
    }

    // read_data into FFT array
    if(step % 100 == 0) {
        if (FFT_sel < FFT_error_sample + FFT_sample_number){
            FFT_data[FFT_sel] = Ain;
            FFT_sel++;
        }
        else sample_dump = Ain;
    }
    
    // Output data to FFT
    if (Select_Button.read() == 1 && FFT_sel == FFT_error_sample + FFT_sample_number){
        ThisThread::sleep_for(10ms);
        while (Select_Button.read() == 1) ;
        for(FFT_sel = FFT_error_sample; FFT_sel <  FFT_error_sample  + FFT_sample_number; FFT_sel++){
            printf("%f\r\n", FFT_data[FFT_sel]);
            wait_us(10);
        }
        FFT_sel = 0;
    }
    // Delay
    wait_us(39);
    step++;
    if(step == 25600) step = 0;
}