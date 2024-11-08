/* below I2C channels verified - on 10/21/2024
21,22 - Ch1 - Working
19,23 - Ch2 - Working
18,5 - Ch3 - Working
17,16 - Ch4 - Working
15,14 - Ch5 - Working
25,13 - Ch6 - Working
33,32 - Ch7 - Working
27,26 - Ch8 - Working
*/
#include <Wire.h>
#include <Adafruit_DRV2605.h>

// Define the I2C addresses for each multiplexer
#define I2C_MUX_ADDR_1 0x70  // I2C address for Mux1
#define I2C_MUX_ADDR_2 0x71  // I2C address for Mux2
#define I2C_MUX_ADDR_3 0x72  // I2C address for Mux3
#define I2C_MUX_ADDR_4 0x73  // I2C address for Mux4
#define I2C_MUX_ADDR_5 0x74  // I2C address for Mux5
#define I2C_MUX_ADDR_6 0x75  // I2C address for Mux6
#define I2C_MUX_ADDR_7 0x76  // I2C address for Mux7
#define I2C_MUX_ADDR_8 0x77  // I2C address for Mux8

// Declare DRV2605 objects
Adafruit_DRV2605 drv1, drv2, drv3, drv4, drv5, drv6, drv7, drv8;
Adafruit_DRV2605 drv9, drv10, drv11, drv12, drv13, drv14, drv15, drv16;
// Add more drv objects for the remaining drivers if needed

// Define I2C pins for each mux
#define SDA_PIN_1 5         // I2C SDA pin for Mux1
#define SCL_PIN_1 18         // I2C SCL pin for Mux1

#define SDA_PIN_2 21         // I2C SDA pin for Mux2
#define SCL_PIN_2 19         // I2C SCL pin for Mux2

#define SDA_PIN_3 22         // I2C SDA pin for Mux3
#define SCL_PIN_3 23          // I2C SCL pin for Mux3

#define SDA_PIN_4 33         // I2C SDA pin for Mux4
#define SCL_PIN_4 32         // I2C SCL pin for Mux4

#define SDA_PIN_5 25         // I2C SDA pin for Mux5
#define SCL_PIN_5 26         // I2C SCL pin for Mux5

#define SDA_PIN_6 14         // I2C SDA pin for Mux6
#define SCL_PIN_6 27         // I2C SCL pin for Mux6

#define SDA_PIN_7 13         // I2C SDA pin for Mux7
#define SCL_PIN_7 17         // I2C SCL pin for Mux7

#define SDA_PIN_8 15         // I2C SDA pin for Mux8
#define SCL_PIN_8 16         // I2C SCL pin for Mux8

// Create separate TwoWire instances for each I2C bus
TwoWire wire1 = TwoWire(0);
TwoWire wire2 = TwoWire(1);
TwoWire wire3 = TwoWire(2);
TwoWire wire4 = TwoWire(3);
TwoWire wire5 = TwoWire(4);
TwoWire wire6 = TwoWire(5);
TwoWire wire7 = TwoWire(6);
TwoWire wire8 = TwoWire(7);

// Function to select the mux channel
void selectMuxChannel(TwoWire &wire, uint8_t muxAddr, uint8_t channel) {
  if (channel > 7) return;  // Check if the channel is valid
  wire.beginTransmission(muxAddr);
  wire.write(1 << channel);  // Select the channel
  wire.endTransmission();
}

void setup() {
  Serial.begin(115200);
  delay(10);

  // Initialize the I2C buses
  wire1.begin(SDA_PIN_1, SCL_PIN_1);
  wire2.begin(SDA_PIN_2, SCL_PIN_2);
  wire3.begin(SDA_PIN_3, SCL_PIN_3);
  wire4.begin(SDA_PIN_4, SCL_PIN_4);
  wire5.begin(SDA_PIN_5, SCL_PIN_5);
  wire6.begin(SDA_PIN_6, SCL_PIN_6);
  wire7.begin(SDA_PIN_7, SCL_PIN_7);
  wire8.begin(SDA_PIN_8, SCL_PIN_8);

  // Initialize DRV2605s on each multiplexer
  Adafruit_DRV2605* drvArray[] = {&drv1, &drv2, &drv3, &drv4, &drv5, &drv6, &drv7, &drv8, &drv9, &drv10, &drv11, &drv12, &drv13, &drv14, &drv15, &drv16};

  // Loop through all 8 I2C buses and initialize DRV2605 drivers
  for (uint8_t i = 0; i < 8; i++) {
    selectMuxChannel(wire1, I2C_MUX_ADDR_1, i);
    if (!drvArray[i]->begin(&wire1)) {
      Serial.print("Failed to find DRV2605 on Mux1 channel ");
      Serial.println(i);
      while (1);
    }
    drvArray[i]->selectLibrary(1);
    drvArray[i]->setMode(DRV2605_MODE_INTTRIG);
  }

  // Repeat this process for all other muxes (you can copy the same logic for Mux2 to Mux8)
  // Example for Mux2:
  for (uint8_t i = 0; i < 8; i++) {
    selectMuxChannel(wire2, I2C_MUX_ADDR_2, i);
    if (!drvArray[i]->begin(&wire2)) {
      Serial.print("Failed to find DRV2605 on Mux2 channel ");
      Serial.println(i);
      while (1);
    }
    drvArray[i]->selectLibrary(1);
    drvArray[i]->setMode(DRV2605_MODE_INTTRIG);
  }
}

void loop() {
  // Activate all drivers simultaneously
  for (uint8_t i = 0; i < 8; i++) {
    selectMuxChannel(wire1, I2C_MUX_ADDR_1, i);
    drv1.setWaveform(0, 15);  // Play effect 15
    drv1.go();
    // Add code for other drivers
  }

  delay(1000);  // Delay between activations
}
