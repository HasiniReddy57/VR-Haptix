#include <Wire.h>
#include <Adafruit_DRV2605.h>
#include <WiFi.h>

// Replace with your network credentials
const char* ssid = "***********";
const char* password = "*********";

WiFiServer server(8080);  // Create a server on port 8080

// Define I2C multiplexers' addresses
#define I2C_MUX1_ADDR 0x70  // I2C address for mux1
//#define I2C_MUX2_ADDR 0x77  // I2C address for mux2

Adafruit_DRV2605 drv1, drv2, drv3, drv4, drv5, drv6, drv7, drv8; // Motors on mux1
//Adafruit_DRV2605 drv9, drv10, drv11, drv12, drv13, drv14; // Motors on mux2

#define SDA_PIN1 15       // Custom I2C SDA pin for Mux1
#define SCL_PIN1 16        // Custom I2C SCL pin for Mux1
//#define SDA_PIN2 19         // Custom I2C SDA pin for Mux2
//#define SCL_PIN2 21         // Custom I2C SCL pin for Mux2

#define DRV1_CHANNEL 0     // Channel for drv1
#define DRV2_CHANNEL 1     // Channel for drv2
#define DRV3_CHANNEL 2     // Channel for drv3
#define DRV4_CHANNEL 3     // Channel for drv4
#define DRV5_CHANNEL 4     // Channel for drv5
#define DRV6_CHANNEL 5     // Channel for drv6
#define DRV7_CHANNEL 6     // Channel for drv7
#define DRV8_CHANNEL 7     // Channel for drv8
/*
#define DRV9_CHANNEL 0     // Channel for drv9 on mux2
#define DRV10_CHANNEL 1    // Channel for drv10 on mux2
#define DRV11_CHANNEL 2    // Channel for drv11 on mux2
#define DRV12_CHANNEL 3    // Channel for drv12 on mux2
#define DRV13_CHANNEL 4    // Channel for drv13 on mux2
#define DRV14_CHANNEL 5    // Channel for drv14 on mux2
*/
TwoWire myWire1 = TwoWire(0); // Create a new TwoWire instance for the first I2C bus (Mux1)
TwoWire myWire2 = TwoWire(1); // Create a new TwoWire instance for the second I2C bus

void selectMuxChannel(TwoWire &wire, uint8_t muxAddr, uint8_t channel) {
  if (channel > 7) return;
  wire.beginTransmission(muxAddr);
  wire.write(1 << channel);
  wire.endTransmission();
}


//void resetDevice(Adafruit_DRV2605 &drv) {
//  drv.writeRegister8(0x16, 0x01); // Reset command for DRV2605L
//  delay(10);  // Small delay to allow reset to complete
//}


void setup() {
  Serial.begin(115200);
  delay(10);

  // Connect to WiFi
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  server.begin();  // Start the server

  // Initialize I2C buses for the multiplexers
  myWire1.begin(SDA_PIN1, SCL_PIN1);
  //myWire2.begin(SDA_PIN2, SCL_PIN2);

  // Initialize drv1 to drv8 on the first mux
  Adafruit_DRV2605* drvArray1[] = {&drv1, &drv2, &drv3, &drv4, &drv5, &drv6, &drv7, &drv8};
  for (uint8_t i = 0; i < 8; i++) {
    selectMuxChannel(myWire1, I2C_MUX1_ADDR, i);  // Select the channel on mux1
  //  resetDevice(*drvArray1[i]);  // Perform a reset before initialization
  //drvArray1[i]->writeRegister8(DRV2605_REG_MODE, 0x80);
  delay(100);
    if (!drvArray1[i]->begin(&myWire1)) {
      Serial.print("Failed to find DRV2605 on mux1 channel ");
      Serial.println(i);
      while (1); // Stop if a device isn't found
    }
    drvArray1[i]->selectLibrary(1);
    drvArray1[i]->setMode(DRV2605_MODE_INTTRIG);
  }
/*
  // Initialize drv9 to drv14 on the second mux
  Adafruit_DRV2605* drvArray2[] = {&drv9, &drv10, &drv11, &drv12, &drv13, &drv14};
  for (uint8_t i = 0; i < 6; i++) {
    selectMuxChannel(myWire2, I2C_MUX2_ADDR, i);  // Select the channel on mux2
    //resetDevice(*drvArray2[i]);  // Perform a reset before initialization
    writeRegister8(DRV2605_REG_MODE, 0x80);
    if (!drvArray2[i]->begin(&myWire2)) {
      Serial.print("Failed to find DRV2605 on mux2 channel ");
      Serial.println(i);
      while (1); // Stop if a device isn't found
    }
    drvArray2[i]->selectLibrary(1);
    drvArray2[i]->setMode(DRV2605_MODE_INTTRIG);
  }*/
}

void vibrateSelectedMotors(uint8_t* channels, uint8_t numChannels) {
  Adafruit_DRV2605* drvArray1[] = {&drv1, &drv2, &drv3, &drv4, &drv5, &drv6, &drv7, &drv8};
  
  for (uint8_t i = 0; i < numChannels; i++) {
    uint8_t channel = channels[i];
    selectMuxChannel(myWire1, I2C_MUX1_ADDR, channel);  // Select the corresponding channel on mux
    drvArray1[channel]->setWaveform(0, 15);  // Play effect 15
    drvArray1[channel]->go();
  }
}

void loop() {
  WiFiClient client = server.available();

  if (client) {
    Serial.println("New client connected");

    while (client.connected()) {
      if (client.available()) {
        String message = client.readStringUntil('\n');
        message.trim();  // Remove any trailing newline characters
        Serial.println("Received message: " + message);

        if (message == "vibrate_selected") {
          uint8_t selectedChannels[] = {DRV1_CHANNEL, DRV2_CHANNEL, DRV4_CHANNEL, DRV5_CHANNEL};
          vibrateSelectedMotors(selectedChannels, 4);  // Vibrate motors 1, 2, 4, and 5
        }
      }
    }

    client.stop();
    Serial.println("Client disconnected");
  }
}