#include "mbed.h"
#include "stm32l475e_iot01_accelero.h"
#include "uLCD_4DGL.h"
#include <cmath>

uLCD_4DGL uLCD(D1, D0, D2);
// DigitalIn button(BUTTON1);
InterruptIn button(BUTTON1);
int state = 0;
//InterruptIn btnRecord(BUTTON1);
//EventQueue queue(32 * EVENTS_EVENT_SIZE);
//Thread t;

int16_t pDataXYZ[3] = {0};
//int idR[32] = {0};
//int indexR = 0;

/*
void record(void) {
   BSP_ACCELERO_AccGetXYZ(pDataXYZ);
   printf("%d, %d, %d\n", pDataXYZ[0], pDataXYZ[1], pDataXYZ[2]);
}

void startRecord(void) {
   printf("---start---\n");
   idR[indexR++] = queue.call_every(1ms, record);
   indexR = indexR % 32;
}

void stopRecord(void) {
   printf("---stop---\n");
   for (auto &i : idR)
      queue.cancel(i);
}*/


// a1 == 0 0 1000
// a2 == x y z
float AB = 0;
float A = 0;
float dot;
float x;
float y;
float z;
float data[10];
float out;

void changestate(){
    state = !state;
}
int main() {
    button.rise(&changestate);
    ThisThread::sleep_for(1s);
    printf("uLCD printing...\n");
    uLCD.printf("\nHello uLCD World\n");
    ThisThread::sleep_for(1s);

    printf("Done.\n");
    printf("Start accelerometer init\n");
    BSP_ACCELERO_Init();
    ThisThread::sleep_for(1s);
    while(1) {
        BSP_ACCELERO_AccGetXYZ(pDataXYZ);
        //printf("%d, %d, %d\n", pDataXYZ[0], pDataXYZ[1], pDataXYZ[2]);
        /*
        uLCD.locate(1, 2);
        uLCD.printf("X:%d     ", pDataXYZ[0]);
        uLCD.locate(1, 3); 
        uLCD.printf("Y:%d     ", pDataXYZ[1]);
        uLCD.locate(1, 4);
        uLCD.printf("Z:%d     ", pDataXYZ[2]);
        */
        
        if (state == 1) {
            for(int i = 0; i < 10; i++) {
                x = pDataXYZ[0] / 1024.0;
                y = pDataXYZ[1] / 1024.0;
                z = pDataXYZ[2] / 1024.0;   
                AB = z;
                A = sqrt(x*x + y*y + z*z);
                dot = AB / A;
                data[i] = dot;
                out += dot;
                ThisThread::sleep_for(10ms);
            }
            out = out / 10;
            printf("%g \r\n", out);
            out = 0;
        }
        else{

        ThisThread::sleep_for(100ms);   
        }
    }
   /*
   t.start(callback(&queue, &EventQueue::dispatch_forever));
   btnRecord.fall(queue.event(startRecord));
   btnRecord.rise(queue.event(stopRecord));*/
}
