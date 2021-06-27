
 
#include <Adafruit_GFX.h>       // include Adafruit graphics library
#include <Adafruit_ILI9341.h>   // include Adafruit ILI9341 TFT library
 
#define TFT_CS    8      // TFT CS  pin is connected to arduino pin 8
#define TFT_RST   9      // TFT RST pin is connected to arduino pin 9
#define TFT_DC    10     // TFT DC  pin is connected to arduino pin 10
// initialize ILI9341 TFT library
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_RST);
 
int8_t Ftime[240];
int8_t Fspectr[240];
float tsample, Fsample;

void setup() {
  Serial.begin(9600);
  Serial.println("ILI9341 Test!"); 
 
  tft.begin();
 
  // read diagnostics (optional but can help debug problems)
  uint8_t x = tft.readcommand8(ILI9341_RDMODE);
 
  
  int q = 1;
  for(int q=1; q<8; q++){
    /* математическое моделирование */
    for(int t=0; t<240; t++){     // инкримент времени
      tsample = 30*cos(q*((2*3.14/240) * t)) + 20*cos(2*q*((2*3.14/240) * t)) + 10*cos((5+q)*((2*3.14/240) * t));     // формула формирования выборки сигнала во временной области
      Ftime[t] = (int8_t)tsample;           // заполнение массива временных отсчётов (выборок)
      tft.drawPixel(t,Ftime[t] + 60,0x0000);    // индикация сигнла во временной области на дисплее
    }
    /* расчёт спектра */
    for(int n=0; n<60; n++){            // инкримент частоты гармоники
      Fsample = 0;
      for(int i=0; i<240; i++){         // инкримент времени для текущей гармоники
        Fsample = Fsample + Ftime[i] * cos(n*(2*3.14/240) * i) + Ftime[i] * sin(n*(2*3.14/240) * i);  // форомула расчёта спектра (преобразование Фурье)
      }
      Fsample = 64 - Fsample/64;          // нормализация частотного отсчёта для корректной индикации на диспелее
      Fspectr[n] = (uint8_t)Fsample;        // заполнение буфера спектра частотными отсчётами (выборками)
      tft.drawFastVLine(n,65-Fspectr[n],30,0x0000);  // индикация спектра на дисплее
    }
  
  for(int i=0;i<240;i++)  tft.drawPixel(i,Ftime[i] + 60,0xfffff);
  // очистить дисплей
  for(int i=0;i<240;i++)  tft.drawLineVertical(i,50,0xfffff);
  for(int i=0;i<240;i++)  tft.drawLineVertical(i,70,0x0000);
  }

    while(1) {}
 
}
 
 
void loop(void) {
  for(uint8_t rotation=0; rotation<4; rotation++) {
    tft.setRotation(rotation);
    
    delay(1000);
  }
}
 


 
