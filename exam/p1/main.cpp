#include "mbed.h"
#include "uLCD_4DGL.h"


uLCD_4DGL uLCD(D1, D0, D2);
InterruptIn button(BUTTON1);
Ticker flipper;
int state;
int m_s = 0;
int s = 0;
int m = 0;
void flip() {
    m_s++;
    if (m_s == 100) {
        m_s = 0;
        s++;
    }
    if (s == 60) {
        s = 0;
        m++;
    }
}
void set()
{
    if(state == 0) {
        flipper.attach(&flip, 10ms);
        state = 1;
    }
    else if(state == 1) {
        flipper.detach();
        state = 2;
    } 
    else if(state == 2) state = 3;
    else if(state == 3){
        flipper.attach(&flip, 10ms);
        state = 1;
    } 

};
int main() {
  printf("uLCD printing...\n");
  uLCD.color(WHITE);
  uLCD.printf("\nHello uLCD World\n");
  ThisThread::sleep_for(1s);
  
  state = 0;
  button.rise(&set);

    while(1) {
        if (state == 0) {
            ThisThread::sleep_for(100ms);
        }

        else if(state == 1){
            uLCD.locate(1, 2);
            uLCD.printf("%2d:%2d:%2d",m,s ,m_s);
        }
        else if (state == 2){

            ThisThread::sleep_for(100ms);
        }
        else {
            m = 0;
            s = 0;
            m_s = 0;
            uLCD.locate(1, 2);
            uLCD.printf("%2d:%2d:%2d",m,s ,m_s);

        }
      
    }




}
/*
  uLCD.printf("Counting down:\n");
  uLCD.text_width(4); // 4X size text
  uLCD.text_height(4);
  uLCD.color(RED);
  for (int i = 10; i >= 0; --i) {
    uLCD.locate(1, 2);
    uLCD.printf("%2d", i);
    ThisThread::sleep_for(1000ms);
  }
  printf("Done.\n");
}*/