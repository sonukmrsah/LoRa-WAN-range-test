# ESP32 SX1262 FSK Receiver

This Arduino sketch configures an ESP32 as an FSK receiver using the SX1262 LoRa/FSK radio module from the RadioLib library.

## Hardware Wiring

### ESP32 to SX1262 Module

**Pin connections are IDENTICAL to the transmitter sketch:**

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

**Note:** 
- Use the **same wiring** for both transmitter and receiver ESP32 boards
- Avoid strapping pins (GPIO0, GPIO2, GPIO12, GPIO15) for DIO1/BUSY

## FSK Configuration Parameters

The receiver uses **IDENTICAL** settings to the transmitter:

```cpp
float frequency = 868.0;      // MHz (ISM band)
float bitRate = 4.8;          // kbps
float freqDev = 5.0;          // kHz (frequency deviation)
float rxBw = 156.2;           // kHz (receiver bandwidth)
uint16_t preambleLen = 16;    // bits
```

⚠️ **CRITICAL:** All FSK parameters **MUST match** between transmitter and receiver, or no packets will be received.

## Usage

1. **Connect Hardware:** Wire ESP32 to SX1262 as shown above
2. **Install RadioLib:** Sketch → Include Library → Manage Libraries → Search "RadioLib" → Install
3. **Open Sketch:** Load `ESP32_SX126x_FSK_Receiver.ino` in Arduino IDE
4. **Select Board:** Tools → Board → ESP32 → Choose your ESP32 variant
5. **Configure Port:** Tools → Port → Select COM port
6. **Upload:** Click Upload button
7. **Monitor:** Tools → Serial Monitor (115200 baud)

## Expected Output

When no packets are being received:
```
=== RadioLib ESP32 SX1262 FSK Receiver ===

[SX1262] Initializing FSK mode ... success!

[SX1262] Configuration:
  Frequency:   868.00 MHz
  Bit Rate:    4.80 kbps
  Freq Dev:    5.00 kHz
  RX BW:       156.20 kHz

[SX1262] Starting continuous RX mode...
Waiting for packets...
```

When packets are received:
```
========================================
[RX #1] Packet received!
Data:    Hello FSK!
Length:  10 bytes
RSSI:    -45 dBm
SNR:     12 dB
Freq Err:-120 Hz
========================================

========================================
[RX #2] Packet received!
Data:    Packet #1
Length:  9 bytes
RSSI:    -46 dBm
SNR:     11 dB
Freq Err:-115 Hz
========================================
```

## Testing TX and RX Together

1. **Upload transmitter sketch** to first ESP32 + SX1262
2. **Upload receiver sketch** to second ESP32 + SX1262
3. **Open Serial Monitor** for receiver (115200 baud)
4. **Power on transmitter** (or press reset)
5. **Receiver should display** packets sent by transmitter

### Expected Behavior
- Transmitter sends packet every 1 second
- Receiver displays each packet with signal quality metrics
- RSSI should be around -30 to -60 dBm when devices are 1 meter apart
- SNR should be positive (5-15 dB is typical for good signal)

## Troubleshooting

### Receiver initializes but never receives packets

**Most common causes:**
1. **Frequency mismatch** - Verify both TX and RX use same frequency (868.0 MHz)
2. **FSK parameters don't match** - Check bitRate, freqDev, rxBw, preambleLen
3. **Transmitter not working** - Test TX with blocking transmit first
4. **Distance too far** - Start with devices 1 meter apart
5. **Antenna not connected** - SX1262 needs antenna for RF transmission

### "CRC error - corrupted packet" messages
- Devices too close (< 10 cm) causing overload
- Interference from other devices
- FSK parameters slightly mismatched

### Packets received but data is garbled
- Bit rate mismatch between TX and RX
- Frequency deviation doesn't match
- Check both sketches use identical parameters

### DIO1 interrupt not firing (no packets detected)
- Verify DIO1 wired to GPIO26 (not GPIO2 or GPIO15)
- Check interrupt-capable pin (GPIO26 supports interrupts)
- Test with LED on DIO1 to verify signal changes

### Low RSSI (< -80 dBm) or negative SNR
- Devices too far apart
- Antenna not properly connected
- Obstacles blocking signal
- TX power too low (increase in transmitter)

## Signal Quality Guidelines

| RSSI (dBm) | Signal Quality |
|------------|---------------|
| -30 to -50 | Excellent     |
| -50 to -70 | Good          |
| -70 to -90 | Fair          |
| < -90      | Poor/Unstable |

| SNR (dB)   | Signal Quality |
|------------|---------------|
| > 10       | Excellent     |
| 5 to 10    | Good          |
| 0 to 5     | Fair          |
| < 0        | Poor          |

## Advanced: Custom Pin Configuration

If you need different pins, modify both TX and RX sketches:

```cpp
#define SX126x_CS   5     // Any SPI CS pin
#define SX126x_DIO1 26    // Interrupt-capable, NOT GPIO2/15
#define SX126x_RST  33    // Any GPIO
#define SX126x_BUSY 27    // Any GPIO, NOT GPIO2/15
```

## RadioLib References

- **GitHub:** https://github.com/jgromes/RadioLib
- **API Docs:** https://jgromes.github.io/RadioLib/
- **Wiki:** https://github.com/jgromes/RadioLib/wiki

## License

This example code is in the public domain.
