//Konfigurasi variable - variable sebelum panggi library
#define ESP8266_BAUD 9600
#define BLYNK_PRINT Serial

//Panggil library yang dibutuhkan
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <SimpleTimer.h>
#include <ESP8266_Lib.h>
#include <BlynkSimpleShieldEsp8266.h>
#include <SoftwareSerial.h>

#include <SD.h>
#include <SPI.h>
#include <DS3231.h>

//Konfigurasi variable - variable
SimpleTimer timer;
//SoftwareSerial EspSerial(31, 30); //Konfigurasi PIN SoftSerial
ESP8266 wifi(&Serial1);
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE); //Konfigurasi ADDRESS LCD i2c

//Konfigurasi Auth Token BLYNK
char auth[] = "c01678a1ded946cdb3f12990c534d23f";

//Konfigurasi Wifi
char ssid[] = "hasanbasri93";
char pass[] = "123456789";

#define pin_arus 0;
#define pin_vbatt 1;
#define pin_vfan 2;
#define pin_vsolar 3;


void setup()
{
  setup_lcd();        //Memulai LCD i2c
  timer.setInterval(1000, info_lcd); //Konfigurasi, memanggail funsgi info_lcd per 1 detik
  timer.setInterval(1000, kirim_data); //Konfigurasi, memanggail funsgi kirim_data per 1 detik
  Serial.begin(9600); //Memulai BAUDRATE SerialHard DEBUG
  Serial1.begin(ESP8266_BAUD); //Memulai BAUDRATE SerialSoft ESP
  Blynk.begin(auth, wifi, ssid, pass); //Memulai menghubing ke server BLYNK
}

void loop()
{
  timer.run(); //Menjalankan timer
  Blynk.run(); //Menjalankan BLYNK
}

//Fungsi kirim_data, mengirim data ke server BLYNK
void kirim_data()
{
  Blynk.virtualWrite(V0, cari_tegangan(pin_vbatt)); //Kirim data Tegangan batteri di PIN ANALOG yg disesuaikan
  Blynk.virtualWrite(V1, cari_tegangan(pin_vfan)); //Kirim data Tegangan batteri di PIN ANALOG yg disesuaikan
  Blynk.virtualWrite(V2, cari_tegangan(pin_solar)); //Kirim data Tegangan batteri di PIN ANALOG yg disesuaikan
  Blynk.virtualWrite(V3, cari_arus(pin_arus));     //Kirim data Arus di PIN ANALOG yg disesuaikan
}

//Fungsi setup_lcd, konfigurasi LCD i2c
void setup_lcd()
{
  lcd.begin(20, 4); //inilisasi ukuran LCD
  lcd.backlight();  //Menyalakan backlight LCD

  //Papan Judul
  lcd.setCursor(0, 0);          //Atur kursor lcd pada baris ke 1 dan kolom ke 1
  lcd.print("ADE NOVIANTO"); //Menampilkan info, silahkan diubah
  lcd.setCursor(0, 1);          //Atur kursor lcd pada baris ke 2 dan kolom ke 1
  lcd.print("PROTOTYPE BCR"); //Menampilkan info, silahkan diubah
  lcd.setCursor(0, 3);          //Atur kursor lcd pada baris ke 2 dan kolom ke 1
  lcd.print("Connecting..."); //Menampilkan info, silahkan diubah
}

//Fungsi info_lcd, menampilakn pembacaan sensor2
void info_lcd()
{
  lcd.setCursor(0, 0);      //Atur kursor lcd pada baris ke 1 dan kolom ke 1
  lcd.print("V_BATT   : "); //Menampilkan info, silahkan diubah
  lcd.print(cari_tegangan(pin_vbatt));
  lcd.print(" V");

  lcd.setCursor(0, 1);      //Atur kursor lcd pada baris ke 2 dan kolom ke 1
  lcd.print("V_SURYA  : "); //Menampilkan info, silahkan diubah
  lcd.print(cari_tegangan(pin_vsolar));
  lcd.print(" V");

  lcd.setCursor(0, 2);      //Atur kursor lcd pada baris ke 3 dan kolom ke 1
  lcd.print("V_KINCIR : "); //Menampilkan info, silahkan diubah
  lcd.print(cari_tegangan(pin_vfan));
  lcd.print(" V");

  lcd.setCursor(0, 3);      //Atur kursor lcd pada baris ke 4 dan kolom ke 1
  lcd.print("ARUS     : "); //Menampilkan info, silahkan diubah
  lcd.print(cari_arus(pin_arus));
  lcd.print(" A");

}

//Fungsi cari_tegangan, menghitung pembacaan tegangan dari PIN ANALOG
//Menggunakan rumus hitung tegangan dengan metode VOLTAGE DIVIDER
//Membaca sampel data dari PIN ANALOG
float cari_tegangan(int pin)
{


  //Konfigurasi variable - variable
  float vout = 0.0;
  float vin = 0.0;
  float R1 = 30000.0; //
  float R2 = 7500.0; //
  int value = 0;

  vout = (value * 5.0) / 1024.0; // see text
  vin = vout / (R2 / (R1 + R2));

  value = analogRead(pin);
  vout = (value * 5.0) / 1024.0; // see text
  vin = vout / (R2 / (R1 + R2));
`
  return vin;
}

//Fungsi cari_arus, menghitung pembacaan ARUS dari PIN ANALOG
float cari_arus(int pin)
{
  //Konfigurasi variable - variable
  int mVperAmp = 66; // Data 66 dari DATASHEET sensor
  int RawValue = 0;
  int ACSoffset = 2500; // Data 2500 dari DATASHEET sensor
  double Voltage = 0;
  double Amps = 0;
  double Vcc;

  Vcc = readVcc() / 1000.0;
  RawValue = analogRead(pin);
  Voltage = (RawValue / 1024.0) * Vcc;
  Amps = ((Voltage - ACSoffset) / mVperAmp);
  return RawValue;
}

//Fungsi readVcc, menghitung pembacaan TEGANGAN 5V dari internal Microcontroller
long readVcc()
{
  long result;
  ADMUX = _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
  delay(2);
  ADCSRA |= _BV(ADSC);
  while (bit_is_set(ADCSRA, ADSC))
    ;
  result = ADCL;
  result |= ADCH << 8;
  result = 1125300L / result;
  return result;
}
