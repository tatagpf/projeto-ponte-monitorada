 #include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <LoRa.h>


#define LARGURA_TELA 128
#define ALTURA_TELA 64


// O display OLED da BitDogLab usa o barramento Wire1 interno
Adafruit_SSD1306 display(LARGURA_TELA, ALTURA_TELA, &Wire1, -1);


// Pinos LoRa no conector central J6
const int csPin = 17;          
const int resetPin = 20;      
const int irqPin = 28;        


// Variáveis para os dados recebidos
String tAHT = "0.0", uAHT = "0.0", tBMP = "0.0", pres = "0.0", angX = "0.0", angY = "0.0";
int sinalRSSI = 0;
bool primeiroPacote = false;


// --- CONFIGURAÇÃO DOS LIMITES CRÍTICOS ---
const float LIMITE_TEMP = 40.0;    // Temperatura acima de 40°C é crítico
const float LIMITE_ANGULO = 15.0;  // Inclinação maior que 15 ou menor que -15 graus é crítico


void setup() {
  Serial.begin(115200);
 
  // Inicializa o OLED interno da BitDogLab
  Wire1.setSDA(14); Wire1.setSCL(15); Wire1.begin();
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
 
  // Tela de Inicialização Estilizada
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.drawRect(0, 0, 128, 64, SSD1306_WHITE); // Borda na tela inteira
  display.setCursor(15, 18);
  display.println("MONITOR REMOTO");
  display.setCursor(25, 32);
  display.println("Aguardando...");
  display.setCursor(20, 46);
  display.println("Sinal LoRa RX");
  display.display();


  // Inicializa o barramento SPI do LoRa no J6 da BitDogLab
  SPI.setRX(16); SPI.setTX(19); SPI.setSCK(18);
  LoRa.setPins(csPin, resetPin, irqPin);


  if (!LoRa.begin(433E6)) {
    display.clearDisplay();
    display.setCursor(10, 25);
    display.println("FALHA NO LORA!");
    display.display();
    while (1);
  }
}


void loop() {
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    String bruto = "";
    while (LoRa.available()) {
      bruto += (char)LoRa.read();
    }
    sinalRSSI = LoRa.packetRssi();
    primeiroPacote = true;


    // Decodifica a string separada por vírgulas (CSV)
    int index0 = bruto.indexOf(',');
    int index1 = bruto.indexOf(',', index0 + 1);
    int index2 = bruto.indexOf(',', index1 + 1);
    int index3 = bruto.indexOf(',', index2 + 1);
    int index4 = bruto.indexOf(',', index3 + 1);


    if (index0 != -1 && index1 != -1 && index2 != -1 && index3 != -1 && index4 != -1) {
      tAHT = bruto.substring(0, index0);
      uAHT = bruto.substring(index0 + 1, index1);
      tBMP = bruto.substring(index1 + 1, index2);
      pres = bruto.substring(index2 + 1, index3);
      angX = bruto.substring(index3 + 1, index4);
      angY = bruto.substring(index4 + 1);
    }
  }


  // Só redesenha a tela se já tiver recebido pelo menos um dado
  if (primeiroPacote) {
    // 1. CONVERSÃO PARA NÚMERO E VERIFICAÇÃO DE ESTADO CRÍTICO
    float tempNum = tAHT.toFloat();
    float angXNum = abs(angX.toFloat()); // Usa abs() para pegar o desvio tanto positivo quanto negativo
    float angYNum = abs(angY.toFloat());


    bool estadoCritico = false;


    // Se a temperatura passar do limite OU se a estrutura inclinar demais em X ou Y
    if (tempNum > LIMITE_TEMP || angXNum > LIMITE_ANGULO || angYNum > LIMITE_ANGULO) {
      estadoCritico = true;
    }


    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
   
    // --- CABEÇALHO ---
    display.setCursor(4, 2);
    display.print("LORA MONITOR");
    display.setCursor(85, 2);
    display.print(sinalRSSI); display.print("dBm");
    display.drawFastHLine(0, 12, 128, SSD1306_WHITE);
   
    // --- BLOCO 1: AMBIENTE (Lado Esquerdo) ---
    display.setCursor(2, 18);
    display.print("TEMP: "); display.print(tAHT); display.print(" C");
    display.setCursor(2, 30);
    display.print("UMID: "); display.print(uAHT); display.print(" %");
    display.setCursor(2, 42);
    display.print("PRES: "); display.print(pres);
   
    // Linha vertical separadora
    display.drawFastVLine(74, 12, 40, SSD1306_WHITE);
   
    // --- BLOCO 2: INCLINÔMETRO (Lado Direito) ---
    display.setCursor(80, 18);
    display.print("ANGULOS");
    display.setCursor(80, 30);
    display.print("X: "); display.print(angX);
    display.setCursor(80, 42);
    display.print("Y: "); display.print(angY);
   
    // --- RODAPÉ DINÂMICO ---
    display.drawFastHLine(0, 54, 128, SSD1306_WHITE);
   
    if (estadoCritico) {
      // Alerta piscante ou destacado (Muda o texto e centraliza o aviso)
      display.setCursor(12, 56);
      display.print("STATUS: ** CRITICO **");
     
      // Opcional: Desenha uma pequena borda extra piscante ou estática no rodapé para chamar atenção
      display.drawRect(0, 54, 128, 10, SSD1306_WHITE);
    } else {
      display.setCursor(12, 56);
      display.print("STATUS: OPERACIONAL");
    }
   
    display.display();
  }
 
  delay(100);
}
