#include "mbed.h"
#include "bbcar.h"
#include <cstdio>
#include <cstring>
#include <string>
#include "string.h"
#include "mbed_events.h"
using namespace std::chrono;

///// erpc include  //
#include "drivers/DigitalOut.h"

#include "erpc_simple_server.h"
#include "erpc_basic_codec.h"
#include "erpc_crc16.h"
#include "UARTTransport.h"
#include "DynamicMessageBufferFactory.h"
#include "final_erpc_service_server.h"
////////////////////////////////////



/***  Macros for setting console flow control. */
#define CONSOLE_FLOWCONTROL_RTS     1
#define CONSOLE_FLOWCONTROL_CTS     2
#define CONSOLE_FLOWCONTROL_RTSCTS  3
#define mbed_console_concat_(x) CONSOLE_FLOWCONTROL_##x
#define mbed_console_concat(x) mbed_console_concat_(x)
#define CONSOLE_FLOWCONTROL mbed_console_concat(MBED_CONF_TARGET_CONSOLE_UART_FLOW_CONTROL)





//////////////////////////////////////
Ticker encoder_ticker; // for encoder
DigitalIn encoder(D12); // for encoder
Ticker servo_ticker; // for BBcar
DigitalInOut pin5(D5); // for ping sensor
DigitalInOut QTI_RR(D9), QTI_R(D8), QTI_L(D7), QTI_LL(D6); // for QTI sensor

PwmOut pin10(D10), pin11(D11);
BBCar car(pin10, pin11, servo_ticker);
volatile float last_ping = 100;

char reverse_state = 0;
char pattern = 0;
char QTI_load(BBCar &car, DigitalInOut &QTI_RR, DigitalInOut &QTI_R, DigitalInOut &QTI_L, DigitalInOut &QTI_LL);
void QTI_setcar(BBCar &car , char pattern);



parallax_ping  ping1(pin5);
volatile float steps;
volatile int encoder_last;
void encoder_control();
//////////////////////////////////////


/****** erpc declarations *******/
float Get_data_from_mbed(uint8_t data) {
    if (data == 1) return (float)steps * 6.5 * 3.14 / 32;  // Encdoer
    else if (data == 2) return last_ping;   // Ping
    return 0;
}

/** erpc infrastructure */
ep::UARTTransport uart_transport(D1, D0, 9600);
ep::DynamicMessageBufferFactory dynamic_mbf;
erpc::BasicCodecFactory basic_cf;
erpc::Crc16 crc16;
erpc::SimpleServer rpc_server;
/** Final_ERPC service */
Final_ERPC_service Final_ERPC;





//------------ XBee comunication ----------//
// BufferedSerial XBee(D1, D0, 9600);
// char messagebuffer[128];
//----------------------------------------//

void CAR_function() {
    while (1) {
        //last_ping = ping1;
        if (last_ping < 15) {
            reverse_state = 1;
            car.goStraight(-30);
            ThisThread::sleep_for(500ms);
            car.stop();
            ThisThread::sleep_for(100ms);
            car.servo0.set_factor(1);
            car.servo1.set_factor(1);
            car.servo0.set_speed(100);
            car.servo1.set_speed(100);
            ThisThread::sleep_for(200ms);
            while ( QTI_load(car, QTI_RR, QTI_R, QTI_L, QTI_LL) != 0b0110)
                ThisThread::sleep_for(1ms);
            car.stop();
        } 
        else {
            pattern = QTI_load(car, QTI_RR, QTI_R, QTI_L, QTI_LL);
            QTI_setcar(car, pattern);
        }
        ThisThread::sleep_for(1ms);
        
        //printf("%f   ", (float)ping1); // ping distance (cm)
        //printf("%f\n ", (float)steps * 6.5 * 3.14 / 32);  // encoder distance has travelled (cm)
        
        // // Xbee system
        // sprintf(messagebuffer, "Distance = %f \r\n", (float)steps * 6.5 * 3.14 / 32);
        // printf("%s", messagebuffer);
        // XBee.write(messagebuffer, strlen(messagebuffer));
        // memset(messagebuffer, '\0', 128); //clear buffer
    }
}
void ping_check() {
    while(1) {
    last_ping = ping1;   
    ThisThread::sleep_for(500ms);
    }
}


int main() {
    
    // // XBee initialize
    // XBee.set_format(
    //   /* bits */ 8,
    //   /* parity */ BufferedSerial::None,
    //   /* stop bit */ 1);
    // memset(messagebuffer, '\0', 128); //clear buffer
    
    /////////////////////////////////////////////////////////////
    // Initialize the rpc server
    uart_transport.setCrc16(&crc16);

    // Set up hardware flow control, if needed
    #if CONSOLE_FLOWCONTROL == CONSOLE_FLOWCONTROL_RTS
    uart_transport.set_flow_control(mbed::SerialBase::RTS, STDIO_UART_RTS, NC);
    #elif CONSOLE_FLOWCONTROL == CONSOLE_FLOWCONTROL_CTS
    uart_transport.set_flow_control(mbed::SerialBase::CTS, NC, STDIO_UART_CTS);
    #elif CONSOLE_FLOWCONTROL == CONSOLE_FLOWCONTROL_RTSCTS
    uart_transport.set_flow_control(mbed::SerialBase::RTSCTS, STDIO_UART_RTS, STDIO_UART_CTS);
    #endif

    printf("Initializing server.\n");
    rpc_server.setTransport(&uart_transport);
    rpc_server.setCodecFactory(&basic_cf);
    rpc_server.setMessageBufferFactory(&dynamic_mbf);

    // Add the final_erpc service to the server
    printf("Adding final_erpc server.\n");
    rpc_server.addService(&Final_ERPC);

    // Run the server. This should never exit
    printf("Running server.\n");
    
    // erpc thread setting //
    //Thread message_thread(osPriorityNormal);
    //erpc_thread.start(&erpc_service_thread_function);
    //erpc_thread.start(callback(&rpc_server, &erpc::SimpleServer::run));
    //message_thread.start(data_transfer);
    
    
    /////////////////////////////////////////////////////////////
    car.stop();
    encoder_ticker.attach(&encoder_control, 1ms);
    steps = 0;
    encoder_last = 0;

    printf("CAR go\n");
    //CAR thread;
    Thread CAR_Thread(osPriorityNormal);
    CAR_Thread.start(CAR_function);

    printf("Ping go\n");
    //Ping thread;
    Thread Ping_Thread(osPriorityNormal);
    Ping_Thread.start(ping_check);
    
    
    rpc_server.run();
    
   
};
char QTI_load(BBCar &car, DigitalInOut &QTI_RR, DigitalInOut &QTI_R, DigitalInOut &QTI_L, DigitalInOut &QTI_LL) {
    char pattern = 0;
    QTI_LL.output(); QTI_L.output(); QTI_R.output(); QTI_RR.output();
    // Prdouce impulse, charge cap
    QTI_RR = QTI_LL = QTI_R = QTI_L = 1;
    //ThisThread::sleep_for(1ms);
    wait_us(230);
    
    // Set input mode
    QTI_LL.input(); QTI_L.input(); QTI_R.input(); QTI_RR.input();
    // Wait for decaying
    //ThisThread::sleep_for(1ms);
    //wait_us(230);
    wait_us(150);
    // Read in data
    pattern = QTI_LL * 0b0001 + QTI_L * 0b0010 + QTI_R * 0b0100 + QTI_RR * 0b1000;
    return pattern;
}
void QTI_setcar(BBCar &car, char pattern) {
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
            car.turn(200, 0.01);
            break;
        case 0b0100: // small left 
            car.turn(50, 0.1);
            break;
        case 0b0010: // small right    
            car.turn(50, -0.1);
            break;
        case 0b0011: // medium right    
            car.turn(200, -0.01);
            break;
        case 0b0001: // large right  
            //car.turn(200, -0.01);
            car.servo0.set_factor(1);
            car.servo1.set_factor(1);
            car.servo0.set_speed(100);
            car.servo1.set_speed(100);
            break;
        case 0b0111: // branch, turn right
            car.goStraight(40);
            ThisThread::sleep_for(200ms);
            
            car.stop();
            ThisThread::sleep_for(100ms);
            
            car.servo0.set_factor(1);
            car.servo1.set_factor(1);
            car.servo0.set_speed(100);
            car.servo1.set_speed(100);
            ThisThread::sleep_for(500ms);

            car.stop();
            ThisThread::sleep_for(100ms);

            car.goStraight(50);
            ThisThread::sleep_for(200ms);
           
            break;
        
        case 0b1110: // branch, left side has road but move forward
            car.goStraight(20);
            break;
         
        case 0b1111: // T-shape, turn right when not reversed, left for reversed
            if (reverse_state == 0) {  // turn right
                car.goStraight(20);
                ThisThread::sleep_for(500ms);
                car.stop();
                ThisThread::sleep_for(100ms);
                car.servo0.set_factor(1);
                car.servo1.set_factor(1);
                car.servo0.set_speed(100);
                car.servo1.set_speed(100);
                ThisThread::sleep_for(50ms);
                while (QTI_load(car, QTI_RR, QTI_R, QTI_L, QTI_LL) != 0b0110) 
                    ThisThread::sleep_for(1ms);;
                car.stop();
            } 
            else { // turn left 
                car.goStraight(20);
                ThisThread::sleep_for(500ms);
                car.stop();
                ThisThread::sleep_for(100ms);
                car.servo0.set_factor(-1);
                car.servo1.set_factor(-1);
                car.servo0.set_speed(100);
                car.servo1.set_speed(100);
                ThisThread::sleep_for(50ms);
                while (QTI_load(car, QTI_RR, QTI_R, QTI_L, QTI_LL) != 0b0110) 
                    ThisThread::sleep_for(1ms);;
                car.stop();
                reverse_state = 0; // reset reversed type
            }
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