#include <SPI.h>
#include <RH_MRF89.h>

// Define MRF89XAM8A pins connected to ESP32
#define MRF89_SS_PIN    5   // Chip Select (Slave Select) pin
#define MRF89_INT_PIN   2   // Interrupt pin
#define MRF89_RST_PIN   4   // Reset pin

// Define ESP32 Hardware SPI pins (defaults for most ESP32 boards, but good to be explicit)
#define HSPI_SCK        18
#define HSPI_MISO       19
#define HSPI_MOSI       23

// Singleton instance of the radio driver with custom pins
RH_MRF89 mrf89(MRF89_SS_PIN, MRF89_INT_PIN);

void setup()
{
  Serial.begin(115200);
  while (!Serial); // Wait for serial port to connect.

  // Set ESP32 hardware SPI pins
  SPI.begin(HSPI_SCK, HSPI_MISO, HSPI_MOSI, MRF89_SS_PIN);

  // Manually control the MRF89 RST pin
  pinMode(MRF89_RST_PIN, OUTPUT);
  digitalWrite(MRF89_RST_PIN, HIGH); // Ensure MRF89 is not in reset initially
  delay(10);
  digitalWrite(MRF89_RST_PIN, LOW);  // Briefly pulse reset
  delay(10);
  digitalWrite(MRF89_RST_PIN, HIGH); // Release from reset
  delay(10); // Wait for module to come up

  if (!mrf89.init())
  {
    Serial.println("MRF89 init failed. Check wiring!");
    while (1); // Halt if initialization fails
  }

  Serial.println("MRF89 initialized successfully as Transmitter.");
}

void loop()
{
  static int messageCount = 0;
  String message = "Hello from ESP32 Transmitter! Msg #" + String(messageCount++);
  
  Serial.print("Sending: ");
  Serial.println(message);

  // Send a message
  mrf89.send((uint8_t*)message.c_str(), message.length());
  mrf89.waitPacketSent();
  
  delay(2000); // Wait before sending next message
}