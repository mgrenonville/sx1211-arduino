#include <Arduino.h>
#include <SPI.h>
#include "SX1211_driver.h"

#include <Ticker.h>
#include <AsyncMqttClient.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

#include <DNSServer.h>
#include <WiFiManager.h>
#include <ArduinoJson.h>
#include "json/helper.h"
#include "Queue.h"

#include <ArduinoOTA.h>
// #include <ESP8266mDNS.h>

AsyncMqttClient mqttClient;
Ticker mqttReconnectTimer;

Ticker wifiReconnectTimer;

SX1211_Driver sx1211;

SPIClass * spi = NULL;

helper::json::MqttMessageType mode;
boolean mqttActive;

WiFiManager wifiManager;
Queue<String> transmitQueue = Queue<String>(1);
Queue<String> networkIdQueue = Queue<String>(1);

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);

void setupOTA()
{
  ArduinoOTA.onStart([]()
                     { Serial.println("Start"); });
  ArduinoOTA.onEnd([]()
                   { Serial.println("\nEnd"); });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total)
                        { Serial.printf("Progress: %u%%\r", (progress / (total / 100))); });
  ArduinoOTA.onError([](ota_error_t error)
                     {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("End Failed"); });
  ArduinoOTA.begin();
}

void connectToMqtt()
{
  Serial.print(timeClient.getFormattedTime());

  Serial.println("Connecting to MQTT...");
  mqttClient.connect();
}

void connectToWifi()
{
  // put your setup code here, to run once:
  // reset saved settings
  // wifiManager.resetSettings();

  // set custom ip for portal
  // wifiManager.setAPStaticIPConfig(IPAddress(10,0,1,1), IPAddress(10,0,1,1), IPAddress(255,255,255,0));

  // fetches ssid and pass from eeprom and tries to connect
  // if it does not connect it starts an access point with the specified name
  // here  "AutoConnectAP"
  // and goes into a blocking loop awaiting configuration
  wifiManager.autoConnect("AutoConnectAP");
  // or use this for auto generated name ESP + ChipID
  // wifiManager.autoConnect();
  // if you get here you have connected to the WiFi
  Serial.println("connected...yeey :)");
  connectToMqtt();
  // if (MDNS.begin("frisquet-mqtt"))
  // { // Start mDNS with name esp8266
  //   Serial.println("MDNS started");
  //   MDNS.addService("ota", "tcp", 8266);
  // }
}

#if defined(ESP32)

#define SX_1211_SCK SCK
#define SX_1211_MISO MISO
#define SX_1211_MOSI MOSI
#define NSS_CONFIG 2
#define NSS_DATA 3
#define IRQ0 0
#define IRQ1 1

#else
#define SX_1211_SCK D5
#define SX_1211_MISO D6
#define SX_1211_MOSI D7
#define NSS_CONFIG D1
#define NSS_DATA D0
#define IRQ0 D3
#define IRQ1 D2

WiFiEventHandler wifiConnectHandler;
WiFiEventHandler wifiDisconnectHandler;
void onWifiConnect(const WiFiEventStationModeGotIP &event)
{
  Serial.print(timeClient.getFormattedTime());

  Serial.println("Connected to Wi-Fi.");
  connectToMqtt();
}

void onWifiDisconnect(const WiFiEventStationModeDisconnected &event)
{
  Serial.println("Disconnected from Wi-Fi.");
  mqttReconnectTimer.detach(); // ensure we don't reconnect to MQTT while reconnecting to Wi-Fi
  wifiReconnectTimer.once(2, connectToWifi);
}

#endif

void onMqttConnect(bool sessionPresent)
{
  mqttActive = true;
  Serial.println("Connected to MQTT.");
  Serial.print("Session present: ");
  Serial.println(sessionPresent);
  uint16_t packetIdSub = mqttClient.subscribe("frisquet/command", 0);
  Serial.print("Subscribing at QoS 2, packetId: ");
  Serial.println(packetIdSub);
}

void onMqttDisconnect(AsyncMqttClientDisconnectReason reason)
{
  Serial.print(timeClient.getFormattedTime());
  Serial.printf("- Disconnected from MQTT. %i\n", reason);

  mqttActive = false;

  if (WiFi.isConnected())
  {
    mqttReconnectTimer.once(2, connectToMqtt);
  }
}

void onMqttSubscribe(uint16_t packetId, uint8_t qos)
{
  Serial.println("Subscribe acknowledged.");
  Serial.print("  packetId: ");
  Serial.println(packetId);
  Serial.print("  qos: ");
  Serial.println(qos);
}

void onMqttUnsubscribe(uint16_t packetId)
{
  Serial.println("Unsubscribe acknowledged.");
  Serial.print("  packetId: ");
  Serial.println(packetId);
}

void onMqttMessage(char *topic, char *payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total)
{
  Serial.println("Publish received.");
  Serial.print("  topic: ");
  Serial.println(topic);
  Serial.print("  qos: ");
  Serial.println(properties.qos);
  Serial.print("  dup: ");
  Serial.println(properties.dup);
  Serial.print("  retain: ");
  Serial.println(properties.retain);
  Serial.print("  len: ");
  Serial.println(len);
  Serial.print("  index: ");
  Serial.println(index);
  Serial.print("  total: ");
  Serial.println(total);
  StaticJsonDocument<200> doc;

  DeserializationError error = deserializeJson(doc, payload);

  String jsonType = doc["type"];
  helper::json::MqttMessageType parsedType = helper::json::parseJsonType(jsonType.c_str());
  Serial.printf("Deserialization error: %s, type: %s, parsedType: %i", error.c_str(), jsonType.c_str(), parsedType);
  switch (parsedType)
  {

  case helper::json::SLEEP:
    mode = helper::json::SLEEP;
    break;

  case helper::json::SET_NETWORK_ID:
  {
    String networkId = doc["network_id"];

    networkIdQueue.push(networkId);
    break;
  }

  case helper::json::SEND:
  {
    String payloadStr = doc["payload"];
    transmitQueue.push(payloadStr);

    break;
  }
  case helper::json::LISTEN:
    mode = helper::json::LISTEN;
    break;
  }
}

void onMqttPublish(uint16_t packetId)
{
  Serial.println("Publish acknowledged.");
  Serial.print("  packetId: ");
  Serial.println(packetId);
}

void configureSX1211()
{

#if defined(ESP32)

  boolean success = true;
  spi = new SPIClass(0);
  spi->begin(SX_1211_SCK, SX_1211_MISO, SX_1211_MOSI);
  // spi->setHwCs(false);
#else
  pinMode(SX_1211_MISO, INPUT);
  pinMode(SX_1211_MOSI, OUTPUT);
  pinMode(SX_1211_SCK, OUTPUT);
  boolean success = spi->pins(SX_1211_SCK, SX_1211_MISO, SX_1211_MOSI, 0);
  spi->begin();
#endif

  // Serial.printf("SPI Info: _spi:%p", spi->_spi);
  
  sx1211.begin(spi, NSS_CONFIG, NSS_DATA, IRQ0, IRQ1);
  

  sx1211.writeRawConfig(SX1211_REG_MCPARAM1, SX_1211_MC1_BAND_868 | SX_1211_MC1_RPS_SELECT_1 | SX_1211_MC1_VCO_TRIM_00 | SX_1211_MC1_SLEEP);
  // sx1211.writeRawConfig(SX1211_REG_MCPARAM2, 0x8C);
  sx1211.writeRawConfig(SX1211_REG_MCPARAM2, SX_1211_MC2_MODULATION_FSK | SX_1211_MC2_DATA_MODE_PACKET | SX_1211_MC2_OOK_THRESH_TYPE_PEAK | SX_1211_MC2_GAIN_IF_00); // Mode packet

  // sx1211.writeRawConfig(SX1211_REG_FDEV, 0x07);
  sx1211.writeRawConfig(SX1211_REG_FDEV, SX_1211_FDEV_50);

  // sx1211.writeRawConfig(SX1211_REG_BITRATE, 0x07);
  sx1211.writeRawConfig(SX1211_REG_BITRATE, SX_1211_BITRATE_25000);

  // sx1211.writeRawConfig(SX1211_REG_OOKFLOORTHRESH, 0x0C);
  sx1211.writeRawConfig(SX1211_REG_OOKFLOORTHRESH, SX_1211_OOKFLOORTHRESH_VALUE);

  // sx1211.writeRawConfig(SX1211_REG_MCPARAM6, 0xCF);
  sx1211.writeRawConfig(SX1211_REG_MCPARAM6, SX_1211_MC6_FIFO_SIZE_64 | SX_1211_MC6_FIFO_THRESH_VALUE);

  // Configured values for my usecase
  sx1211.writeRawConfig(SX1211_REG_R1, 0x7F);
  sx1211.writeRawConfig(SX1211_REG_P1, 0x65);
  sx1211.writeRawConfig(SX1211_REG_S1, 0x4A);
  sx1211.writeRawConfig(SX1211_REG_R2, 0x77);
  sx1211.writeRawConfig(SX1211_REG_P2, 0x5F);
  sx1211.writeRawConfig(SX1211_REG_S2, 0x19);

  // For OOK, not usefull
  sx1211.writeRawConfig(SX1211_REG_PARAMP, 0x38);

  // sx1211.writeRawConfig(SX1211_REG_IRQPARAM1, 0x88);
  sx1211.writeRawConfig(SX1211_REG_IRQPARAM1, SX_1211_IRQ0_RX_STDBY_FIFOEMPTY | SX_1211_IRQ1_RX_STDBY_CRCOK | SX_1211_IRQ1_TX_TXDONE);

  sx1211.writeRawConfig(SX1211_REG_IRQPARAM2, 0x19);

  sx1211.writeRawConfig(SX1211_REG_RSSIIRQTHRESH, 0x82);
  sx1211.writeRawConfig(SX1211_REG_RXPARAM1, 0xE3);
  sx1211.writeRawConfig(SX1211_REG_RXPARAM2, 0x38);

  // sx1211.writeRawConfig(SX1211_REG_RXPARAM3, 0x38);
  byte network_id[] = {0x12, 0x34, 0x56, 0x78};
  sx1211.setSyncWord(network_id, 4);

  sx1211.writeRawConfig(SX1211_REG_TXPARAM, 0x50);
  sx1211.writeRawConfig(SX1211_REG_PKTPARAM1, 0x40);
  sx1211.writeRawConfig(SX1211_REG_OSCPARAM, 0x3c);

  sx1211.writeRawConfig(SX1211_REG_RES19, 0x07);
  sx1211.writeRawConfig(SX1211_REG_RXPARAM6, 0x00);

  sx1211.setAddress(0x08); // Address 0x80, chaudiere

  // disable address filtering.
  sx1211.writeRawConfig(SX1211_REG_PKTPARAM3, 0xE8);

  sx1211.sendConfig();
};

void showConfig()
{
  byte *configWritten = sx1211.readConfig();
  Serial.write("Config: ");
  for (byte address = 0; address < 32; address++)
  {
    Serial.printf("%02X", configWritten[address]);
  }
  Serial.write("\n");
}

void setup()
{

  Serial.begin(115200);

  // wifiConnectHandler = WiFi.onStationModeGotIP(onWifiConnect);
  // wifiDisconnectHandler = WiFi.onStationModeDisconnected(onWifiDisconnect);

  mqttClient.onConnect(onMqttConnect);
  mqttClient.onDisconnect(onMqttDisconnect);
  // mqttClient.onSubscribe(onMqttSubscribe);
  // mqttClient.onUnsubscribe(onMqttUnsubscribe);
  mqttClient.onMessage(onMqttMessage);
  // mqttClient.onPublish(onMqttPublish);

  mqttClient.setServer("192.168.254.17", 1883);
  // mqttClient.setServer(MQTT_HOST, MQTT_PORT);
  connectToWifi();

  configureSX1211();

  byte network_id[] = {0x12, 0x34, 0x56, 0x78};
  sx1211.setSyncWord(network_id, 4);
  sx1211.sendConfig();
  showConfig();

  timeClient.begin();
  setupOTA();

  Serial.println("Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void loop()
{

  // transmitQueue.push(String("12345678"));

  timeClient.update();

  // Serial.println(timeClient.getFormattedTime());

  if (networkIdQueue.count() > 0)
  {
    showConfig();
    String networkId = networkIdQueue.pop();
    Serial.printf("Will set networkId to : %s", networkId.c_str());
    helper::conversion::ByteBuffer networkIdXX = helper::conversion::unhexlify(networkId.c_str());
    sx1211.setSyncWord(&networkIdXX[0], networkIdXX.size() - 1);
    sx1211.sendConfig();
    showConfig();
  }
  if (mode == helper::json::LISTEN && mqttActive)
  {
    sx1211.setMode(SX1211_MODE_RX);
    if (sx1211.hasAvailableData())
    {
      byte data[90];
      byte read = sx1211.receive(data);

      Serial.printf("Read: %d : \n", read);
      if (read < 64)
      {

        StaticJsonDocument<300> doc;
        char payload[64 * 2];
        for (int i = 0; i <= read; i++)
        {
          Serial.printf("%02X ", data[i]);
          sprintf(payload + 2 * i, "%02X", data[i]);
        }

        Serial.println("\n Payload for json:");
        Serial.println(payload);
        doc["data"] = payload;
        String output;
        serializeJson(doc, output);
        mqttClient.publish("frisquet/receive", 0, false, output.c_str(), output.length());
      }
    }
  }
  else
  {
    sx1211.setMode(SX1211_MODE_STBY);
    delay(1000);
  }
  if (transmitQueue.count() > 0)
  {
    String payloadStr = transmitQueue.pop();
    helper::conversion::ByteBuffer payload = helper::conversion::unhexlify(payloadStr.c_str());
    sx1211.transmit(payload[0], &payload[1]);
  }

  ArduinoOTA.handle();
  // MDNS.update();
}
