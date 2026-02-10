/*
  RadioLib ESP32 SX126x FSK Transmitter Example

  This sketch configures an ESP32 with SX1262 module as an FSK transmitter.
  FSK (Frequency Shift Keying) is a digital modulation technique.
  
  ESP32 Pin Connections to SX1262 (recommended safe pins):
  - GPIO 5   (D5)  -> NSS (Chip Select)
  - GPIO 26 (D26)  -> DIO1 (Interrupt)
  - GPIO 33 (D33)  -> NRST (Reset)
  - GPIO 27 (D27)  -> BUSY
  - GPIO 19 (D19)  -> MISO
  - GPIO 23 (D23)  -> MOSI
  - GPIO 18 (D18)  -> SCK (Clock)
  - GND           -> GND
  - 3.3V          -> VCC
  
  For detailed RadioLib documentation:
  https://jgromes.github.io/RadioLib/
*/

#include <RadioLib.h>

// Define ESP32 pins for SX1262
// Avoid ESP32 strapping pins (GPIO0, GPIO2, GPIO12, GPIO15) for DIO1/BUSY.
#define SX126x_CS   5
// #define SX126x_DIO1 26
#define SX126x_DIO1 2
// #define SX126x_RST  33
#define SX126x_RST  4
// #define SX126x_BUSY 27
#define SX126x_BUSY 15

// Create SX1262 instance with hardware SPI
SX1262 radio = new Module(SX126x_CS, SX126x_DIO1, SX126x_RST, SX126x_BUSY);

// Transmission state
int transmissionState = RADIOLIB_ERR_NONE;
volatile bool transmittedFlag = false;

// FSK modulation parameters
float frequency = 868.0;
float bitRate = 2.4;      // Slower
float freqDev = 5.0;
float rxBw = 156.2;
int8_t txPower = 22;      // MAXIMUM POWER
uint16_t preambleLen = 32;

// Interrupt handler - called when transmission is finished
#if defined(ESP8266) || defined(ESP32)
  ICACHE_RAM_ATTR
#endif
void setFlag(void) {
  transmittedFlag = true;
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  Serial.println("\n\n");
  Serial.println("=== RadioLib ESP32 SX1262 FSK Transmitter ===");
  Serial.println();

  // Initialize SX1262 with FSK modulation
  Serial.print("[SX1262] Initializing FSK mode ... ");
  int state = radio.beginFSK(
    frequency,      // Carrier frequency (MHz)
    bitRate,        // Bit rate (kbps)
    freqDev,        // Frequency deviation (kHz)
    rxBw,           // Receiver bandwidth (kHz)
    txPower,        // TX power (dBm)
    preambleLen     // Preamble length (bits)
  );

  if (state == RADIOLIB_ERR_NONE) {
    Serial.println("success!");
  } else {
    Serial.print("failed, code ");
    Serial.println(state);
    while (true) { 
      delay(10); 
    }
  }

  // Set packet-sent interrupt action
  radio.setPacketSentAction(setFlag);

  // Optional: Print configuration
  Serial.println();
  Serial.println("[SX1262] Configuration:");
  Serial.print("  Frequency:   ");
  Serial.print(frequency);
  Serial.println(" MHz");
  Serial.print("  Bit Rate:    ");
  Serial.print(bitRate);
  Serial.println(" kbps");
  Serial.print("  Freq Dev:    ");
  Serial.print(freqDev);
  Serial.println(" kHz");
  Serial.print("  RX BW:       ");
  Serial.print(rxBw);
  Serial.println(" kHz");
  Serial.print("  TX Power:    ");
  Serial.print(txPower);
  Serial.println(" dBm");
  Serial.println();

  // Send first packet
  Serial.print("[SX1262] Sending first packet ... ");
  transmissionState = radio.startTransmit("Hello FSK!");
  float txStartMs = millis();
  
  if (transmissionState == RADIOLIB_ERR_NONE) {
    Serial.println("packet sent!");
  } else {
    Serial.print("failed, code ");
    Serial.println(transmissionState);
  }
}

int packetCount = 0;
unsigned long txStartMs = 0;

void loop() {
  // Check if transmission finished
  if (transmittedFlag) {
    transmittedFlag = false;
    txStartMs = 0;

    if (transmissionState == RADIOLIB_ERR_NONE) {
      Serial.println("transmission finished!");
      
      // Get packet length for data rate info (if available)
      size_t packetLen = radio.getPacketLength();
      Serial.print("  Packet length: ");
      Serial.print(packetLen);
      Serial.println(" bytes");
      
    } else {
      Serial.print("transmission failed, code ");
      Serial.println(transmissionState);
    }

    // Clean up after transmission
    radio.finishTransmit();

    // Wait before next transmission
    delay(1000);

    // Send next packet
    Serial.print("[SX1262] Sending packet #");
    Serial.print(++packetCount);
    Serial.print(" ... ");

    String message = "Packet #" + String(packetCount);
    transmissionState = radio.startTransmit(message);
    txStartMs = millis();

    if (transmissionState != RADIOLIB_ERR_NONE) {
      Serial.print("failed, code ");
      Serial.println(transmissionState);
    }
  } else if (txStartMs != 0 && (millis() - txStartMs) > 3000) {
    // No DIO1 interrupt received within timeout
    uint32_t irq = radio.getIrqFlags();
    Serial.print("[SX1262] WARNING: TX IRQ timeout. IRQ flags: 0x");
    Serial.println(irq, HEX);
    Serial.println("Check DIO1 wiring and avoid GPIO2/GPIO15 strapping pins.");
    radio.finishTransmit();
    txStartMs = 0;
    delay(1000);
  }
}
