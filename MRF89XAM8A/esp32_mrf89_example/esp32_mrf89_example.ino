#include <SPI.h>
#include <RH_MRF89.h>

// Define MRF89XAM8A pins connected to ESP32
#define MRF89_SS_PIN    5   // Chip Select (Slave Select) pin
#define MRF89_INT_PIN   34   // Interrupt pin
#define MRF89_RST_PIN   4   // Reset pin

// Define ESP32 Hardware SPI pins (defaults for most ESP32 boards, but good to be explicit)
#define HSPI_SCK        18
#define HSPI_MISO       19
#define HSPI_MOSI       23

// Singleton instance of the radio driver with custom pins
// The constructor takes (slaveSelectPin, interruptPin, spi)
// We are using the default HardwareSPI object 'hardware_spi' for the third argument
RH_MRF89 mrf89(MRF89_SS_PIN, MRF89_INT_PIN);

void setup()
{
  Serial.begin(115200);
  while (!Serial); // Wait for serial port to connect.

  // Set ESP32 hardware SPI pins
  SPI.begin(HSPI_SCK, HSPI_MISO, HSPI_MOSI, MRF89_SS_PIN);
  // It is recommended to use SPI.setPins if you are not using default VSPI pins
  // or if you want to explicitly define them for clarity.

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

  Serial.println("MRF89 initialized successfully.");
  // Optional: Set transmit power, frequency, modem config etc.
  // mrf89.setTxPower(RH_MRF89_TXOPVAL_13DBM); // Max power 13dBm
  // mrf89.setFrequency(915.0);
}

void loop()
{
  Serial.println("Sending to MRF89 server...");
  // Send a message
  const char* msg = "Hello from ESP32!";
  mrf89.send((uint8_t*)msg, strlen(msg));
  mrf89.waitPacketSent();

  // Now wait for a reply
  uint8_t buf[RH_MRF89_MAX_MESSAGE_LEN];
  uint8_t len = sizeof(buf);

  Serial.println("Waiting for reply...");
  if (mrf89.waitAvailableTimeout(3000)) // Wait for up to 3 seconds for a reply
  {
    if (mrf89.recv(buf, &len))
    {
      Serial.print("Got reply: ");
      buf[len] = 0; // Null-terminate the received data for printing as string
      Serial.println((char*)buf);
      Serial.print("RSSI: ");
      Serial.println(mrf89.lastRssi(), DEC);
    }
    else
    {
      Serial.println("Receive failed.");
    }
  }
  else
  {
    Serial.println("No reply received within timeout. Is the server running?");
  }
  delay(2000); // Wait before sending next message
}