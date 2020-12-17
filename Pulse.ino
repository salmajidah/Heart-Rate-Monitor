/*
 * Tugas Besar EB3105 Pengukuran dan Intrumentasi Biomedika
 * ALAT PENDETEKSI DENYUT JANTUNG BERBASIS ARDUINO
 * 
 * oleh:
 * Putri Yulianti Suhayat (18318004)
 * Salma Majidah (18318010)
 * Siti Aisyah (18318019) 
 * 
 * Program Studi Sarjana Teknik Biomedis - Sekolah Teknik Elektro dan Informatika
 * Institut Teknologi Bandung
 * 
 */

#include <FilterDerivative.h>
#include <FilterOnePole.h>
#include <Filters.h>
#include <FilterTwoPole.h>
#include <FloatDefine.h>
#include <RunningStatistics.h>
#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

//OLED
#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);

//INISIASI VARIABEL
int sensorPin = A0;                                
float sensorValue = 0;                             
float lowpass = 0;  
float highpass = 0;   
int count = 9;
unsigned long starttime = 0;
int heartrate;
boolean counted = false;
float filterFrequency = 3.5  ;
float filterFrequency1 = 0.6 ;

//INISIASI AWAL ARDUINO (LED, serial, OLED)
void setup (void) {
  pinMode (13, OUTPUT);                              // D13 LED as Status Indicator
  Serial.begin (115200);                               
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  
  display.setRotation(2);
  display.clearDisplay();
}

//PROGRAM UTAMA
void loop (){
  FilterOnePole lowpassFilter( LOWPASS, filterFrequency );    //Inisiasi filter lowpass
  FilterOnePole highpassFilter( HIGHPASS, filterFrequency1 ); //inisiasi filter highpass
  while(true){
    starttime = millis();
    while (millis()<starttime+10000)                          // Membaca denyut jantung selama 10 detik
  {
    sensorValue = analogRead(sensorPin);
    highpass = 100*highpassFilter.input(sensorValue);         //pemrosesan amplifikasi dan filter highpass
    lowpass = lowpassFilter.input(highpass);                  //pemrosesan filter lowpass
    Serial.print ("hasil sinyal = ");
    Serial.println (lowpass);                                 //cetak hasil ke serial monitor
    if(lowpass>-400 && lowpass<400){
      if(lowpass>60 && counted == false){
        digitalWrite(13, HIGH);                               //nyalakan LED
        count++;                                              //menghitung 1 denyut jantung
        counted =true;
      }else if(lowpass<0){
        digitalWrite(13, LOW);                                //matikan LED
        counted = false;
      }
    } else{                                                   //penghitungan denyut akan diulang apabila ditengah pengukuran terdapat nilai tidak normal
      count =0;
      starttime=millis();
    }
  delay(50);
  }
  
  heartrate = count*6;                                        //konversi denyut jantung ke dalam bpm
  count = 0;  
  if (heartrate>50 && heartrate<200){
    DrawBPM();                                                //apabila denyut jantung normal maka hasil pengukuran akan ditampilkan
  } else{
    Drawerror();                                              //apabila denyut jantung tidak normal, maka akan ditampilkan pesan error
  }
  }
}

//Menampilkan hasil pengukuran
void DrawBPM(void) {
  display.setTextSize(2);
  display.setTextColor(WHITE, BLACK);
  display.setCursor(0, 11);
  display.clearDisplay();
  display.print(heartrate);
  display.println(" bpm");

  display.display();
}

//Menampilkan pesan error
void Drawerror(void) {
  display.setTextSize(2);
  display.setTextColor(WHITE, BLACK);
  display.setCursor(0, 11);
  display.clearDisplay();
  display.println("error!!");

  display.display();
}
