#include<WiFi.h>
#include<Adafruit_MQTT.h>
#include<Adafruit_MQTT_Client.h>
#include<DHT.h>

//  rgb led details
byte rpin = 25;
byte gpin = 26;
byte bpin = 27;
byte rchannel = 0;
byte gchannel = 1;
byte bchannel = 2;
byte resolution = 8;
int frequency = 5000;

//  dht details
byte dht_pin = 4;
#define dht_type DHT11
DHT dht(dht_pin , dht_type);

//  wifi credentials
const char ssid[] = "Paresh";
const char password[] = "12345678";

//  io details
#define IO_USERNAME  "adeev_mardia"
#define IO_KEY       "MV3Q7U6VDXYJJZN0QRBZ"
#define IO_BROKER    "io.adafruit.com"
#define IO_PORT       1883

//  client details
WiFiClient wificlient;
Adafruit_MQTT_Client mqtt(&wificlient , IO_BROKER , IO_PORT , IO_USERNAME , IO_KEY);

/*  
  Create feed object to be subscribed
  SYNTAX FOR REFERENCE : 
  Adafruit_MQTT_Subscribe FEED OBJECT = Adafruit_MQTT_Subscribe(&mqtt , IO_USERNAME"/feeds/FEED NAME");
*/


Adafruit_MQTT_Subscribe red = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/red");
Adafruit_MQTT_Subscribe green = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/green");
Adafruit_MQTT_Subscribe blue = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/blue");


/*  
  create feed objects to publish data
  SYNTAX FOR REFERENCE : 
  Adafruit_MQTT_Publish feed object = Adafruit_MQTT_Publish(&mqtt , IO_USERNAME"/feeds/FEED NAME");  
 */

Adafruit_MQTT_Publish celcius= Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/celcius");
Adafruit_MQTT_Publish farenheit = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/farenheit");
Adafruit_MQTT_Publish kelvin = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/kelvin");
Adafruit_MQTT_Publish humidity = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/humidity");
Adafruit_MQTT_Publish dew_point= Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/dew_point");



void setup()
{
  Serial.begin(115200);

  //  connecting with wifi
  Serial.print("Connecting with : ");
  Serial.println(ssid);
  WiFi.begin(ssid , password);
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.println("Connected !");
  Serial.print("IP assigned by AP : ");
  Serial.println(WiFi.localIP());
  Serial.println();

  //  RGB led setup
  ledcSetup(rchannel , frequency , resolution);
  ledcSetup(gchannel , frequency , resolution);
  ledcSetup(bchannel , frequency , resolution);

  //  attaching pins with channel
  ledcAttachPin(rpin , rchannel);
  ledcAttachPin(gpin , gchannel);
  ledcAttachPin(bpin , bchannel);

  //  dht setup
  dht.begin();

  //  feeds to be subscribed


  
}

void loop()
{
  //  connecting with server
  mqttconnect();

  //  reading values from dht sensor
  float tempc = dht.readTemperature();
  float tempf = dht.readTemperature(true);
  float tempk = tempc + 273.15;
  float humidity = dht.readHumidity();
  float dew_point = (tempc - (100 - humidity) / 5);  //  dew point in celcius

  if (isnan(tempc)  ||  isnan(tempf)  ||  isnan(humidity))
  {
    Serial.println("Sensor not working!");
    delay(1000);
    return;
  }

  //  printing these values on serial monitor
  String val = String(tempc) + " *C" + "\t" + String(tempf) + " *F" + "\t" + String(tempk) + " *K" + "\t" + 
               String(humidity) + " %RH" + "\t" + String(dew_point) + " *C";
  Serial.println(val);
  

  //  publish the DHT data to the feeds
  while ((subscription=mqtt.readSubscription(5000))) {
    if (subscription==&sw1) {
      stringOne = (char *)sw1.lastread;
      Serial.print(F("stringOne: "));
      Serial.println(stringOne);

      if (stringOne=="ON"){
        digitalWrite(LED_1, HIGH);
      }
      if (stringOne=="OFF"){
        digitalWrite(LED_1, LOW);
      }
    }
    if (subscription==&sw2) {
      stringTwo = (char *)sw2.lastread;
      Serial.print(F("stringTwo: "));
      Serial.println(stringTwo);

      if (stringTwo=="ON"){
        digitalWrite(LED_2, HIGH);
      }
      if (stringTwo=="OFF"){
        digitalWrite(LED_2, LOW);
      }
    }
  }
  if (!Temperature.publish(q)) {
    Serial.println("Temperature Failed!");
  }
  else{
    Serial.println("Temperature ok!");
  }
  if (!Temperature.publish(p)) {
    Serial.println("Pressure Failed!");
  }
  else{
    Serial.println("Pressure ok!");
  }
  
  

  //  subscribe the slider values to control RGB
  
  
}

void mqttconnect()
{
  //  if already connected, return
  if (mqtt.connected())return;

  //  if not, connect
  else
  {
    while (true)
    {
      int connection = mqtt.connect();  //  mqq client has all details of client, port , username, key
      if (connection  ==  0)
      {
        Serial.println("Connected to IO");
        break;  //  connected
      }
      else
      {
        Serial.println("Can't Connect");
        mqtt.disconnect();
        Serial.println(mqtt.connectErrorString(connection));  //  printing error message
        delay(5000);  //  wait for 5 seconds
      }
    }
  }


  //  wait for some time
  delay(5000);
}

void makecolor(byte r , byte g , byte b)
{
  //  printing values
  Serial.print("RED : ");
  Serial.print(r);
  Serial.print('\t');
  Serial.print("GREEN : ");
  Serial.print(g);
  Serial.print('\t');
  Serial.print("BLUE : ");
  Serial.println(b);

  //  writing values
  ledcWrite(rchannel , r);
  ledcWrite(gchannel , g);
  ledcWrite(bchannel , b);
}
