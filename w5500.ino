#include <SPI.h>
#include <Ethernet.h>
#include <PubSubClient.h>

// Topics for mqtt
#define PUBLISH_TOPIC "/esp32/publish"
#define SUBSCRIBE_TOPIC "/esp32/subscribe"

// MAC address for your controller below.
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
 
// Set the static IP address to use if the DHCP fails to assign
#define MYIPADDR 192,168,1,28
#define MYIPMASK 255,255,255,0
#define MYDNS 192,168,1,1
#define MYGW 192,168,1,1

const char* mqtt_server = "IP ADDRESS MQTT SERVER";

long lastMessage = 0;
char message[20];
int mqttPublishDelay = 3; // Delay to publish mqtt in seconds 

EthernetClient ethClient;
PubSubClient client(ethClient);
 
void setup() 
{
  Serial.begin(9600);
  Serial.println("Begin Ethernet");
  Ethernet.init(5);   // MKR ETH Shield

  if (Ethernet.begin(mac)) 
  { // Dynamic IP setup
      Serial.println("DHCP OK!");
  }
  else
  {
    Serial.println("Failed to configure Ethernet using DHCP");
    if (Ethernet.hardwareStatus() == EthernetNoHardware) 
    {
      Serial.println("Ethernet shield was not found.  Sorry, can't run without hardware. :(");
      return;
    } 
    else if (Ethernet.linkStatus() == LinkOFF) 
    {
      Serial.println("Ethernet cable is not connected.");
      return;
    } 
    else
    {
      IPAddress ip(MYIPADDR);
      IPAddress dns(MYDNS);
      IPAddress gw(MYGW);
      IPAddress sn(MYIPMASK);
      Ethernet.begin(mac, ip, dns, gw, sn);
      Serial.println("STATIC OK!");
    }
  }
  
  Serial.print("Local IP : ");
  Serial.println(Ethernet.localIP());
  Serial.print("Subnet Mask : ");
  Serial.println(Ethernet.subnetMask());
  Serial.print("Gateway IP : ");
  Serial.println(Ethernet.gatewayIP());
  Serial.print("DNS Server : ");
  Serial.println(Ethernet.dnsServerIP());

  Serial.println("Ethernet Successfully Initialized");
   
  client.setServer(mqtt_server, 1883);
  client.setCallback(receivedCallback);  
}


void mqttconnect() 
{
  while (!client.connected()) 
  {
    Serial.print("MQTT connecting ...");
    
    String clientId = "Flaeron";
    
    if (client.connect(clientId.c_str())) 
    {
      Serial.println("MQTT Connected");
      client.subscribe(SUBSCRIBE_TOPIC);
    } 
    else 
    {
      Serial.print("Failed, status code =");
      Serial.print(client.state());
      Serial.println("try again in 5 seconds");
      delay(5000);
    }
  }
}

void receivedCallback(char* topic, byte* payload, unsigned int length) 
{
  // Print if receive subscribed topic
  Serial.print("Message received: ");
  Serial.println(topic);
  for (int i = 0; i < length; i++) 
    Serial.print((char)payload[i]);
}
 
void loop() 
{
  mqttLogic();
}

void mqttLogic()
{
  // Trying to reconnect if needed
  if (!client.connected())
    mqttconnect();
  // listen for incomming subscribed topic-process-invoke receivedCallback
  client.loop();
  // Publish topic every 3 seconds
  if (millis() - lastMessage > mqttPublishDelay * 1000) 
  {
    lastMessage = millis();
    snprintf(message, 5, "%i", 123);
    client.publish(PUBLISH_TOPIC, message);
  }
}
