//Panggil library yang dibutuhkan
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <SimpleTimer.h>
#include <ESP8266_Lib.h>
#include <BlynkSimpleShieldEsp8266.h>
#include <SoftwareSerial.h>

//Konfigurasi variable - variable
#define ESP8266_BAUD 115200
#define BLYNK_PRINT Serial
SimpleTimer timer;
SoftwareSerial EspSerial(2, 3); //Konfigurasi PIN SoftSerial
ESP8266 wifi(&EspSerial);
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE); //Konfigurasi ADDRESS LCD i2c

//Konfigurasi Auth Token BLYNK
char auth[] = "6e42cc7dcc3b42349a1a53c895c30a08";

//Konfigurasi Wifi
char ssid[] = "TPLink";
char pass[] = "123456789";

void setup()
{
  Serial.begin(9600); //Memulai BAUDRATE SerialHard DEBUG
  setup_lcd();        //Memulai LCD i2c
  //EspSerial.begin(ESP8266_BAUD); //Memulai BAUDRATE SerialSoft ESP
  //Blynk.begin(auth, wifi, ssid, pass); //Memulai menghubing ke server BLYNK
  timer.setInterval(1000, info_lcd); //Konfigurasi, memanggail funsgi info_lcd per 1 detik
  //timer.setInterval(1000, kirim_data); //Konfigurasi, memanggail funsgi kirim_data per 1 detik
}

void loop()
{
  timer.run(); //Menjalankan timer
  //Blynk.run(); //Menjalankan BLYNK
}

//Fungsi kirim_data, mengirim data ke server BLYNK
void kirim_data()
{
  Blynk.virtualWrite(V0, cari_tegangan(0)); //Kirim data Tegangan batteri di PIN ANALOG yg disesuaikan
  Blynk.virtualWrite(V1, cari_tegangan(1)); //Kirim data Tegangan batteri di PIN ANALOG yg disesuaikan
  Blynk.virtualWrite(V2, cari_tegangan(2)); //Kirim data Tegangan batteri di PIN ANALOG yg disesuaikan
  Blynk.virtualWrite(V3, cari_arus(3));     //Kirim data Arus di PIN ANALOG yg disesuaikan
}

//Fungsi setup_lcd, konfigurasi LCD i2c
void setup_lcd()
{
  lcd.begin(20, 4); //inilisasi ukuran LCD
  lcd.backlight();  //Menyalakan backlight LCD

  //Papan Judul
  lcd.setCursor(0, 0);          //Atur kursor lcd pada baris ke 1 dan kolom ke 1
  lcd.print("Hello, ARDUINO "); //Menampilkan info, silahkan diubah
  lcd.setCursor(0, 1);          //Atur kursor lcd pada baris ke 2 dan kolom ke 1
  lcd.print("Hello, ARDUINO "); //Menampilkan info, silahkan diubah
}

//Fungsi info_lcd, menampilakn pembacaan sensor2
void info_lcd()
{
  lcd.setCursor(0, 0);      //Atur kursor lcd pada baris ke 1 dan kolom ke 1
  lcd.print("V_BATT   : "); //Menampilkan info, silahkan diubah
  lcd.print(cari_tegangan(2));
  lcd.setCursor(15, 0);                                 //Atur kursor lcd pada baris ke 1 dan kolom ke 16
  int level_batt = map(level_batt, 12.7, 13.7, 0, 100); //Mapping pembacaan tegangan ke persentase; variable tegangan,tegangan terendah, tegangan tertinggi, persentase terendah,  persentase tertinggi
  lcd.print(level_batt);                                //Menampilkan persentase battery level

  lcd.setCursor(0, 1);      //Atur kursor lcd pada baris ke 2 dan kolom ke 1
  lcd.print("V_SURYA  : "); //Menampilkan info, silahkan diubah
  lcd.print(cari_tegangan(1));

  lcd.setCursor(0, 2);      //Atur kursor lcd pada baris ke 3 dan kolom ke 1
  lcd.print("V_KINCIR : "); //Menampilkan info, silahkan diubah
  lcd.print(cari_tegangan(0));

  lcd.setCursor(0, 3);      //Atur kursor lcd pada baris ke 4 dan kolom ke 1
  lcd.print("ARUS     : "); //Menampilkan info, silahkan diubah
  lcd.print(cari_arus(3));
}

//Fungsi cari_tegangan, menghitung pembacaan tegangan dari PIN ANALOG
//Menggunakan rumus hitung tegangan dengan metode VOLTAGE DIVIDER
//Membaca sampel data dari PIN ANALOG
float cari_tegangan(int pin)
{

  //Konfigurasi variable - variable
  float vout = 0.0;
  float vin = 0.0;
  float R1 = 30000.0;  //Nilai resistor 1 30K
  float R2 = 7500.0;   //Nilai resistor 2 7.5K
#define NUM_SAMPLES 10 //Variable jumlah sample data yg akan diambil, kali
  int sum = 0;
  unsigned char sample_count = 0;
  float voltage = 0.0;

  while (sample_count < NUM_SAMPLES)
  {
    sum += analogRead(pin);
    sample_count++;
  }

  voltage = (((float)sum / (float)NUM_SAMPLES * readVcc()) / 1024.0);
  return voltage / (R2 / (R1 + R2));
  sample_count = 0;
  sum = 0;
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
