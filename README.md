# SMB - Sistema de Monitoramento Estrutural de Pontes via LoRa

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![Platform: ESP32](https://img.shields.io/badge/Platform-ESP32-blue.svg)](https://www.espressif.com)
[![Platform: Raspberry Pi Pico W](https://img.shields.io/badge/Platform-Raspberry_Pi_Pico_W-orange.svg)](https://www.raspberrypi.com)

Este repositório contém o código-fonte e a documentação de um protótipo funcional em escala reduzida (maquete) de um **Sistema de Monitoramento Estrutural de Pontes (Structural Monitoring of Bridges - SMB)**. O sistema utiliza computação distribuída e sensores para capturar o comportamento climático e mecânico de estruturas de transportes rodoviários, trafegando os dados de forma sem fio via radiofrequência **LoRa (Long Range)**.

---

## 📋 Sumário
- [Introdução](#-introdução)
- [Objetivos](#-objetivos)
- [Arquitetura do Sistema](#%EF%B8%8F-arquitetura-do-sistema)
- [Componentes Utilizados](#%EF%B8%8F-componentes-utilizados)
- [Estrutura do Repositório](#-estrutura-do-repositório)
- [Como Executar o Projeto](#-como-executar-o-projeto)
- [Análise de Resultados](#-análise-de-resultados)

---

## 📝 Introdução
A avaliação da integridade de Obras de Arte Especiais (OAEs), como pontes, historicamente baseia-se em inspeções visuais subjetivas. O SMB surge como uma solução digital em tempo real. No entanto, a conectividade em vales e regiões isoladas é um grande desafio técnico. 

Para solucionar isso, este projeto incorpora a tecnologia de comunicação por radiofrequência **LoRa**, caracterizada pelo longo alcance de transmissão e imunidade a barreiras geográficas, permitindo conectar a unidade sensora em campo a uma central de monitoramento autônoma.

## 🎯 Objetivos
* **Geral:** Validar um ecossistema SMB de baixo custo capaz de monitorar variáveis climáticas e oscilações físicas em uma maquete de ponte utilizando comunicação LoRa.
* **Específicos:**
  * Medir temperatura, umidade e pressão atmosférica (AHT20 e BMP280).
  * Capturar acelerações e vibrações estruturais nos eixos X, Y e Z (ADXL345).
  * Transmitir os pacotes via rádio LoRa utilizando um microcontrolador **ESP32**.
  * Receber, decodificar e processar os pacotes em uma central **Raspberry Pi Pico W** (Plataforma BitDogLab).
  * Apresentar alertas dinâmicos instantâneos em um display OLED integrado.

---

## 🛠️ Componentes Utilizados

| Componente | Função |
| :--- | :--- |
| **Placa ESP32** | Unidade Sensora / Transmissora de campo |
| **Raspberry Pi Pico W** | Unidade Receptora / Central de Controle (na BitDogLab) |
| **Módulos de Rádio LoRa (433MHz)** | Comunicação sem fio de longo alcance |
| **Acelerômetro ADXL345** | Captura de oscilações e inclinações nos eixos X, Y, Z |
| **Sensor AHT20** | Monitoramento de Temperatura e Umidade Relativa |
| **Sensor BMP280** | Monitoramento de Pressão Atmosférica e Temperatura |
| **Display OLED SSD1306** | Interface visual embarcada para alertas em tempo real |

---

## ⚙️ Arquitetura do Sistema

O sistema adota uma topologia distribuída dividida em duas frentes de operação:

```text
[Sensores: ADXL345, AHT20, BMP280]
               │ (I2C / SPI)
               ▼
       [Emissor: ESP32]
               │
               ▼ (( rádio LoRa - 433MHz ))
               │
       [Receptor: Pico W / BitDogLab]
               │
               ▼ (Wire1 Interno)
       [Display OLED] -> Exibição / Status Crítico
