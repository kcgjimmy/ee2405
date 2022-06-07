#include "mbed.h"
#include "bbcar.h"
#include <cstdio>

Ticker encoder_ticker; // for encoder
DigitalIn encoder(D12); // for encoder
Ticker servo_ticker; // for BBcar
DigitalInOut pin5(D5); // for ping sensor
DigitalInOut QTI_RR(D9), QTI_R(D8), QTI_L(D7), QTI_LL(D6); // for QTI sensor

PwmOut pin10(D10), pin11(D11);
BBCar car(pin10, pin11, servo_ticker);
float last_ping = 0;
char pattern = 0;
char QTI_load(BBCar &car, DigitalInOut &QTI_RR, DigitalInOut &QTI_R, DigitalInOut &QTI_L, DigitalInOut &QTI_LL);
void QTI_setcar(BBCar &car , char pattern);

//------------ XBee comunication ----------//
BufferedSerial XBee(D1, D0, 9600);
char messagebuffer[128];
//----------------------------------------//


volatile int steps;
volatile int encoder_last;
void encoder_control();

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
    encoder_ticker.attach(&encoder_control, 1ms);
    steps = 0;
    encoder_last = 0;
    parallax_ping  ping1(pin5);
    while (1) {
        pattern = QTI_load(car, QTI_RR, QTI_R, QTI_L, QTI_LL);
        QTI_setcar(car, pattern);
        //printf("%f \n", (float)ping1); // ping distance (cm)
        //printf("%f\n ", (float)steps * 6.5 * 3.14 / 32);  // encoder distance has travelled (cm)
    }
}
char QTI_load(BBCar &car, DigitalInOut &QTI_RR, DigitalInOut &QTI_R, DigitalInOut &QTI_L, DigitalInOut &QTI_LL) {
    char pattern = 0;
    QTI_LL.output(); QTI_L.output(); QTI_R.output(); QTI_RR.output();
    // Prdouce impulse, charge cap
    QTI_RR = QTI_LL = QTI_R = QTI_L = 1;
    wait_us(230);
    
    // Set input mode
    QTI_LL.input(); QTI_L.input(); QTI_R.input(); QTI_RR.input();
    // Wait for decaying
    wait_us(230);
    
    // Read in data
    pattern = QTI_LL * 0b0001 + QTI_L * 0b0010 + QTI_R * 0b0100 + QTI_RR * 0b1000;
    return pattern;
}
void QTI_setcar(BBCar &car , char pattern) {
    switch (pattern) {
        case 0b0110: // go straight 
            car.goStraight(20);
            break;
        case 0b1000: // big left
            car.servo0.set_factor(1);
            car.servo1.set_factor(1);
            car.servo0.set_speed(-100);
            car.servo1.set_speed(-100);
            break;
        case 0b1100: // medium left
            car.turn(100, 0.001);
            break;
        case 0b0100: // small left 
            car.turn(50, 0.1);
            break;
        case 0b0010: // small right    
            car.turn(50, -0.1);
            break;
        case 0b0011: // medium right    
            car.turn(100, -0.001);
            break;
        case 0b0001: // large right  
            //car.turn(200, -0.01);
            car.servo0.set_factor(1);
            car.servo1.set_factor(1);
            car.servo0.set_speed(100);
            car.servo1.set_speed(100);
            break;
        case 0b1111: // task line
            car.stop();
            
            sprintf(messagebuffer, "Distance = %f \r\n", (float)steps * 6.5 * 3.14 / 32);
            printf("%s", messagebuffer);
            XBee.write(messagebuffer, strlen(messagebuffer));
            memset(messagebuffer, '\0', 128); //clear buffer
            printf("%f\n ", (float)steps * 6.5 * 3.14 / 32);  // encoder distance has travelled (cm)
            
            ThisThread::sleep_for(2s);
            steps = 0;
            encoder_last = 0;
            car.goStraight(20);
            ThisThread::sleep_for(1s);
            break;
        default:
            car.stop();
        }
}
void encoder_control() {
   int value = encoder;
   if (!encoder_last && value) steps++;
   encoder_last = value;
}