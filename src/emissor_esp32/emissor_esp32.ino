#include <SPI.h>
#include <Wire.h>
#include <LoRa.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_AHTX0.h>
#include <Adafruit_BMP280.h>
#include <Adafruit_ADXL345_U.h>


// Pinos LoRa na ESP32
const int csPin = 5;          
const int resetPin = 4;      
const int irqPin = 2;        


// Instanciação dos Sensores
Adafruit_AHTX0 aht;
Adafruit_BMP280 bmp(&Wire);
Adafruit_ADXL345_Unified accel = Adafruit_ADXL345_Unified(12345);


void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("--- EMISSOR MULTI-SENSOR ESP32 ---");


  // Inicializa I2C compartilhado para todos os sensores
  Wire.begin(21, 22);


  // Inicializa SPI e LoRa
  SPI.begin(18, 19, 23, 5);
  LoRa.setPins(csPin, resetPin, irqPin);
  if (!LoRa.begin(433E6)) {
    Serial.println("Erro ao iniciar LoRa!");
    while (1);
  }


  // Inicializa os chips I2C
  aht.begin(&Wire);
  if (!bmp.begin(0x76) && !bmp.begin(0x77)) {
    Serial.println("Erro no BMP280!");
  }
  if (!accel.begin()) {
    Serial.println("Erro no ADXL345!");
  }
 
  accel.setRange(ADXL345_RANGE_2_G);
  Serial.println("Todos os sistemas prontos.");
}


void loop() {
  // Leituras AHT20
  sensors_event_t hum_aht, temp_aht;
  aht.getEvent(&hum_aht, &temp_aht);


  // Leituras BMP280
  float t_bmp = bmp.readTemperature();
  float pressao = bmp.readPressure() / 100.0;


  // Leituras ADXL345 e Cálculo de Ângulos
  sensors_event_t event_adxl;
  accel.getEvent(&event_adxl);
  float angX = atan2(event_adxl.acceleration.y, sqrt(event_adxl.acceleration.x * event_adxl.acceleration.x + event_adxl.acceleration.z * event_adxl.acceleration.z)) * 180.0 / PI;
  float angY = atan2(-event_adxl.acceleration.x, sqrt(event_adxl.acceleration.y * event_adxl.acceleration.y + event_adxl.acceleration.z * event_adxl.acceleration.z)) * 180.0 / PI;


  // Monta o pacote de dados compactado em String (CSV)
  // Formato: TempAHT,UmidAHT,TempBMP,Pressao,AngX,AngY
  String pacote = String(temp_aht.temperature, 1) + "," +
                  String(hum_aht.relative_humidity, 1) + "," +
                  String(t_bmp, 1) + "," +
                  String(pressao, 1) + "," +
                  String(angX, 1) + "," +
                  String(angY, 1);


  Serial.print("Transmitindo: ");
  Serial.println(pacote);


  // Envia via rádio LoRa
  LoRa.beginPacket();
  LoRa.print(pacote);
  LoRa.endPacket();


  delay(2000); // Transmite a cada 2 segundos
}
