# ESP32 SX1262 FSK Transmitter

This Arduino sketch configures an ESP32 as an FSK transmitter using the SX1262 LoRa/FSK radio module from the RadioLib library.

## Hardware Wiring

### ESP32 to SX1262 Module

| ESP32 Pin | GPIO | SX1262 Pin | Function |
|-----------|------|-----------|----------|
| D5        | 5    | NSS       | Chip Select (SPI) |
| D26       | 26   | DIO1      | Interrupt (RISING) |
| D33       | 33   | NRST      | Reset |
| D27       | 27   | BUSY      | Busy Signal |
| D23       | 23   | MOSI      | Master Out (SPI) |
| D19       | 19   | MISO      | Master In (SPI) |
| D18       | 18   | SCK       | Clock (SPI) |
| GND       | -    | GND       | Ground |
| 3V3       | -    | VCC       | 3.3V Power |

**Note:** Some SX1262 boards have different pin labels. Check your module datasheet:
- NSS = CS (Chip Select)
- NRST = RST (Reset)
- DIO1 = IRQ (Interrupt)
 
**ESP32 note:** Avoid strapping pins (GPIO0, GPIO2, GPIO12, GPIO15) for DIO1/BUSY.

## FSK Configuration Parameters

The sketch uses these default FSK settings:

```cpp
float frequency = 868.0;      // MHz (ISM band)
float bitRate = 4.8;          // kbps
float freqDev = 5.0;          // kHz (frequency deviation)
float rxBw = 156.2;           // kHz (receiver bandwidth)
int8_t txPower = 10;          // dBm
uint16_t preambleLen = 16;    // bits
```

### Adjusting Parameters

**Frequency (ISM Bands):**
- 868 MHz (Europe)
- 915 MHz (North America)
- 433 MHz (Industrial)

**Bit Rate:**
- 1.2, 2.4, 4.8, 9.6 kbps (typical values)
- Higher rates = faster transmission but more power

**Frequency Deviation:**
- Typical: 2.5-5.0 kHz
- Must match receiver configuration

**TX Power:**
- SX1262 supports: -9 to +22 dBm
- Higher = greater range but more power consumption

## Usage

1. **Connect Hardware:** Wire ESP32 to SX1262 as shown above
2. **Install RadioLib:** Sketch → Include Library → Manage Libraries → Search "RadioLib" → Install
3. **Open Sketch:** Load `ESP32_SX126x_FSK_Transmitter.ino` in Arduino IDE
4. **Select Board:** Tools → Board → ESP32 → Choose your ESP32 variant
5. **Configure Port:** Tools → Port → Select COM port
6. **Upload:** Click Upload button
7. **Monitor:** Tools → Serial Monitor (115200 baud)

## Expected Output

```
=== RadioLib ESP32 SX1262 FSK Transmitter ===

[SX1262] Initializing FSK mode ... success!

[SX1262] Configuration:
  Frequency:   868.00 MHz
  Bit Rate:    4.80 kbps
  Freq Dev:    5.00 kHz
  RX BW:       156.20 kHz
  TX Power:    10 dBm

[SX1262] Sending first packet ... packet sent!
transmission finished!
  Packet length: 11 bytes
[SX1262] Sending packet #1 ... 
transmission finished!
  Packet length: 9 bytes
...
```

## Troubleshooting

### "failed, code" on initialization
- **Possible causes:**
  - Wrong pin connections
  - SX1262 not powered (check 3.3V)
  - SPI communication issue
  - Defective module

### Transmission completes but receiver doesn't get data
- Verify **frequency matches** on transmitter and receiver
- Check **bit rate, frequency deviation, and bandwidth** are identical
- Test with devices **close together** (< 1 meter)
- Ensure receiver is in RX mode **before** transmitter sends

### DIO1 interrupt not firing
- Check DIO1 wiring to ESP32 GPIO14
- Verify pin is interrupt-capable (GPIO14 supports interrupts)
- Try debugging with `attachInterrupt()` test code

### Module gets hot or won't transmit
- Check power supply (needs stable 3.3V, ≥ 500mA)
- Reduce TX power if getting errors
- Ensure proper heat dissipation

## RadioLib References

- **GitHub:** https://github.com/jgromes/RadioLib
- **API Docs:** https://jgromes.github.io/RadioLib/
- **Wiki:** https://github.com/jgromes/RadioLib/wiki

## Additional Resources

- **SX1262 Datasheet:** https://semtech.my.salesforce.com/sfc/p/E0000000JelG/a2St000000Rbsv_EAB
- **ESP32 Pinout:** https://randomnerdtutorials.com/esp32-pinout-reference-gpios/

## License

This example code is in the public domain.
