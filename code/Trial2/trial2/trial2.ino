#include <Wire.h>
#include <Adafruit_DRV2605.h>
#include <WiFi.h>

const char* ssid = "MMM Lab";
const char* password = "90557866";

// Static IP configuration
IPAddress local_IP(192,168,0,101);    // Set a unique static IP address for this ESP32
IPAddress gateway(192,168,0,1);       // Usually your router IP
IPAddress subnet(255,255,255,0);      // Subnet mask
IPAddress primaryDNS(8,8,8,8);        // Optional, Google DNS server
IPAddress secondaryDNS(8,8,4,4);      // Optional, secondary DNS

WiFiServer server(8080);  // Create a server on port 8080

// Define I2C multiplexers' addresses
#define I2C_MUX1_ADDR 0x70  // I2C address for mux1

Adafruit_DRV2605 drv1, drv2, drv3, drv4, drv5, drv6, drv7, drv8; // Motors on mux1

#define SDA_PIN1 15       // Custom I2C SDA pin for Mux1
#define SCL_PIN1 16        // Custom I2C SCL pin for Mux1

#define DRV1_CHANNEL 0     // Channel for drv1
#define DRV2_CHANNEL 1     // Channel for drv2
#define DRV3_CHANNEL 2     // Channel for drv3
#define DRV4_CHANNEL 3     // Channel for drv4
#define DRV5_CHANNEL 4     // Channel for drv5
#define DRV6_CHANNEL 5     // Channel for drv6
#define DRV7_CHANNEL 6     // Channel for drv7
#define DRV8_CHANNEL 7     // Channel for drv8

TwoWire myWire1 = TwoWire(0); // Create a new TwoWire instance for the first I2C bus (Mux1)

void selectMuxChannel(TwoWire &wire, uint8_t muxAddr, uint8_t channel) {
  if (channel > 7) return;
  wire.beginTransmission(muxAddr);
  wire.write(1 << channel);
  wire.endTransmission();
}

void setup() {
  Serial.begin(115200);
  delay(10);

  // Configure static IP
  if (!WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS)) {
    Serial.println("Failed to configure static IP");
  }

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

  // Initialize drv1 to drv8 on the first mux
  Adafruit_DRV2605* drvArray1[] = {&drv1, &drv2, &drv3, &drv4, &drv5, &drv6, &drv7, &drv8};
  for (uint8_t i = 0; i < 8; i++) {
    selectMuxChannel(myWire1, I2C_MUX1_ADDR, i);  // Select the channel on mux1
    delay(100);
    if (!drvArray1[i]->begin(&myWire1)) {
      Serial.print("Failed to find DRV2605 on mux1 channel ");
      Serial.println(i);
      while (1); // Stop if a device isn't found
    }
    drvArray1[i]->selectLibrary(1);
    drvArray1[i]->setMode(DRV2605_MODE_INTTRIG);
  }
}

void vibrateSelectedMotors(uint8_t* channels, uint8_t numChannels, uint8_t intensity) {
  Adafruit_DRV2605* drvArray1[] = {&drv1, &drv2, &drv3, &drv4, &drv5, &drv6, &drv7, &drv8};
  
  uint8_t waveform = 0;
  // Set waveform based on intensity
  switch (intensity) {
    case 1:
      waveform = 10;  // Low intensity
      break;
    case 2:
      waveform = 15;  // Medium intensity
      break;
    case 3:
      waveform = 20;  // High intensity
      break;
    default:
      waveform = 15;  // Default to medium if invalid input
      break;
  }

  for (uint8_t i = 0; i < numChannels; i++) {
    uint8_t channel = channels[i];
    selectMuxChannel(myWire1, I2C_MUX1_ADDR, channel); 
    drvArray1[channel]->setWaveform(0, waveform); 
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
        message.trim();  
        Serial.println("Received message: " + message);

        if (message == "vibrate_selected") {
          uint8_t selectedChannels[] = {DRV1_CHANNEL, DRV2_CHANNEL, DRV4_CHANNEL, DRV5_CHANNEL};
          vibrateSelectedMotors(selectedChannels, 4, 2);
        }
      }
    }

    client.stop();
    Serial.println("Client disconnected");
  }
}
