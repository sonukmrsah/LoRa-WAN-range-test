/*
  RadioLib ESP32 SX126x FSK Receiver Example

  This sketch configures an ESP32 with SX1262 module as an FSK receiver.
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
  
  Note: Pin connections are IDENTICAL to transmitter sketch.
  You can use the same wiring for both TX and RX modules.
  
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

// Reception state
volatile bool receivedFlag = false;

// FSK modulation parameters - MUST MATCH TRANSMITTER
float frequency = 868.0;
float bitRate = 2.4;
float freqDev = 5.0;
float rxBw = 156.2;
uint16_t preambleLen = 32;

// Interrupt handler - called when a packet is received
#if defined(ESP8266) || defined(ESP32)
  ICACHE_RAM_ATTR
#endif
void setFlag(void) {
  receivedFlag = true;
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  Serial.println("\n\n");
  Serial.println("=== RadioLib ESP32 SX1262 FSK Receiver ===");
  Serial.println();

  // Initialize SX1262 with FSK modulation
  Serial.print("[SX1262] Initializing FSK mode ... ");
  int state = radio.beginFSK(
    frequency,      // Carrier frequency (MHz)
    bitRate,        // Bit rate (kbps)
    freqDev,        // Frequency deviation (kHz)
    rxBw,           // Receiver bandwidth (kHz)
    1.6,        // TX power (ignored for RX)
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

  // Set packet-received interrupt action
  radio.setPacketReceivedAction(setFlag);

  // Print configuration
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
  Serial.println();

  // Start continuous reception
  Serial.println("[SX1262] Starting continuous RX mode...");
  state = radio.startReceive();
  
  if (state == RADIOLIB_ERR_NONE) {
    Serial.println("Waiting for packets...");
    Serial.println();
  } else {
    Serial.print("Failed to start RX, code ");
    Serial.println(state);
    while (true) {
      delay(10);
    }
  }
}

int packetCount = 0;

void loop() {
  // Check if a packet was received
  if (receivedFlag) {
    receivedFlag = false;

    // Buffer to store received data
    String receivedData;
    int state = radio.readData(receivedData);

    if (state == RADIOLIB_ERR_NONE) {
      // Packet received successfully
      packetCount++;
      
      Serial.println("========================================");
      Serial.print("[RX #");
      Serial.print(packetCount);
      Serial.println("] Packet received!");
      
      Serial.print("Data:    ");
      Serial.println(receivedData);
      
      Serial.print("Length:  ");
      Serial.print(receivedData.length());
      Serial.println(" bytes");
      
      // Get signal quality metrics
      Serial.print("RSSI:    ");
      Serial.print(radio.getRSSI());
      Serial.println(" dBm");
      
      Serial.print("SNR:     ");
      Serial.print(radio.getSNR());
      Serial.println(" dB");
      
      Serial.print("Freq Err:");
      Serial.print(radio.getFrequencyError());
      Serial.println(" Hz");
      
      Serial.println("========================================");
      Serial.println();
      
    } else if (state == RADIOLIB_ERR_CRC_MISMATCH) {
      Serial.println("[RX] CRC error - corrupted packet!");
      
    } else {
      Serial.print("[RX] Failed to read packet, code ");
      Serial.println(state);
    }

    // Restart reception after processing
    radio.startReceive();
  }
}
