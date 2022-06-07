#include "mbed.h"
#include "bbcar.h"
#include "math.h"
#include <cmath>
DigitalIn but(BUTTON1);
DigitalOut led1(LED1);
Ticker servo_ticker;
DigitalInOut pin5(D5);
float start, last;
int work = 0;
PwmOut pin10(D10), pin11(D11);
BBCar car(pin10, pin11, servo_ticker);
//------------ XBee comunication ----------//
BufferedSerial XBee(D1, D0, 9600);
char messagebuffer[128];
//----------------------------------------//

int main() {
     // XBee initialize
    XBee.set_format(
      /* bits */ 8,
      /* parity */ BufferedSerial::None,
      /* stop bit */ 1);
    memset(messagebuffer, '\0', 128); //clear buffer

    sprintf(messagebuffer, "Hello system on \r\n");
    printf("%s", messagebuffer);
    XBee.write(messagebuffer, strlen(messagebuffer));
    memset(messagebuffer, '\0', 128); //clear buffer



    car.stop();
    parallax_ping  ping1(pin5);
    sprintf(messagebuffer, "System ready. \r\n");
    printf("%s", messagebuffer);
    XBee.write(messagebuffer, strlen(messagebuffer));
    memset(messagebuffer, '\0', 128); //clear buffer


    while(1) {
        if (!but) {
            ThisThread::sleep_for(1s);
            printf("GO!\n");
            last = start = ping1;
            ThisThread::sleep_for(10ms);
            car.servo0.set_speed(30);
            car.servo1.set_speed(30);

            float measure = ping1;
            ThisThread::sleep_for(10ms);
            while((measure - last) < 30) {
                last = measure;
                measure = ping1;
                printf("%f, \n", measure);
                ThisThread::sleep_for(10ms);
            }
            car.stop();

            sprintf(messagebuffer, "measure done \n");
            printf("%s", messagebuffer);
            XBee.write(messagebuffer, strlen(messagebuffer));
            memset(messagebuffer, '\0', 128); //clear buffer
            
            sprintf(messagebuffer, "start is %f, last is %f \n", start, last);
            printf("%s", messagebuffer);
            XBee.write(messagebuffer, strlen(messagebuffer));
            memset(messagebuffer, '\0', 128); //clear buffer

            sprintf(messagebuffer, "width is %f\n", sqrt(last * last - start * start));
            printf("%s", messagebuffer);
            XBee.write(messagebuffer, strlen(messagebuffer));
            memset(messagebuffer, '\0', 128); //clear buffer
        }
    }   
}