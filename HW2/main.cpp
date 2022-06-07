#include "mbed.h"
#include "uLCD_4DGL.h"
using namespace std::chrono;


Thread waveform_sam_thread;
Thread waveform_gen_thread;
Thread print_thread;
EventQueue print_queue(32 * EVENTS_EVENT_SIZE);

InterruptIn button_off(BUTTON1);
InterruptIn button_on(D8);

uLCD_4DGL uLCD(D1, D0, D2);
AnalogOut Aout(D7);
AnalogIn Ain(A0);

bool gen_signal_done = 1;
bool waveform_presignal = 0;
int waveform_count = 1;     // waveform index
int sample_count = 0;       // sample array index
int Genwave = 0;            // on to generate waveform, off to close 
float sample_data[1000];    // sample array
float sample_dump;          // saple trash

void SampleWaveform();
void ProduceWaveform();
void waveform();
void Genwave_OFF();
void Genwave_ON();
void waveform2Python();
void waveform_output();
void uLCD_waveformGenerating();
void uLCD_DataTransfer();
void uLCD_interuptDetect();
void uLCD_DataTransferFinish();
/*************************************/
void ProduceWaveform()
{
    while (1) {
        if (Genwave) {                  
            if (waveform_presignal) {   // Annouce that everything is ready, waveform ready to produce/.
                uLCD_interuptDetect();
                ThisThread::sleep_for(1s);
                uLCD_waveformGenerating();
                waveform_presignal = 0;
                ThisThread::sleep_for(1s);
                gen_signal_done = 1;
            }                           
            if (waveform_count <= 40) Aout = (3 / 3.3) * (waveform_count / 40.0);
            else if(waveform_count <= 50) Aout = (3 / 3.3) * 1;
            else Aout = (3 / 3.3) * (1 - ((waveform_count - 50) / 50.0));
    
            waveform_count++;
            if (waveform_count > 101) waveform_count = 1;
            ThisThread::sleep_for(1ms);
        }
        else {
            ThisThread::sleep_for(1ms);
        }
    }
}

void SampleWaveform() 
{
    while (1) {
        if (Genwave && gen_signal_done) {                  
            if (sample_count < 3000) {
                if (sample_count >= 2000)
                    sample_data[sample_count - 2000] = Ain;
                else
                    sample_dump = Ain;
                sample_count++;
            }
            else
                sample_dump = Ain;
            ThisThread::sleep_for(1ms);
        }
        else {                   
            ThisThread::sleep_for(1ms);
        }
    }
}

/*************************************/
void waveform_output()
{
    if (Genwave == 0) {
        ThisThread::sleep_for(1s);
        print_queue.call(uLCD_DataTransfer);
        print_queue.call(waveform2Python);
        print_queue.call(uLCD_DataTransferFinish);
    }
}
void waveform2Python()
{
    for (int print_count = 0; print_count < 1001; print_count++)
        printf("%f\r\n", sample_data[print_count]);;
}
/*************************************/
void Genwave_OFF()
{
    if (Genwave == 1) {
        gen_signal_done = 0;
        print_queue.call(uLCD_interuptDetect);
        Genwave = 0;
        print_queue.call(waveform_output);
    }
}
void Genwave_ON()
{
    if (Genwave == 0) {
        sample_count = 0;
        waveform_presignal = 1;
        Genwave = 1;
    }
}
/*************************************/
void uLCD_setup() 
{
    uLCD.background_color(BLACK);
    uLCD.color(WHITE);
    uLCD.printf("uLCD Loading...\n");
    ThisThread::sleep_for(1s);
    uLCD.locate(0, 0);
    uLCD.printf("System Status: \n");
    uLCD_waveformGenerating();
}
void uLCD_waveformGenerating()
{
    uLCD.locate(0, 4);
    uLCD.printf("Waveform is Generating.        \n");
}
void uLCD_DataTransfer()
{
    uLCD.locate(0, 4);
    uLCD.printf("Data is transfering.           \n");
}
void uLCD_interuptDetect()
{
    uLCD.locate(0, 4);
    uLCD.printf("Interupt Detect.               \n");
}
void uLCD_DataTransferFinish()
{
    uLCD.locate(0, 4);
    uLCD.printf("Data transfering is finished.  \n");
}

/*************************************/

int main()
{
    printf("System loading...\n");
    uLCD_setup();
    printf("System Ready\n");

    print_thread.start(callback(&print_queue, &EventQueue::dispatch_forever));
    waveform_gen_thread.start(ProduceWaveform);
    waveform_sam_thread.start(SampleWaveform);
    button_off.rise(&Genwave_OFF);
    button_on.rise(&Genwave_ON);

    printf("Waveform start producing\n");
    ThisThread::sleep_for(1s);
    Genwave = 1;
}