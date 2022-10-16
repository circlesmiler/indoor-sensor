#include "PietteTech_DHT.h"

/*
 * ATTENTION: Use Photon Firmware 2.2.0, not 3.x!!!
 */
#define DHTTYPE DHT22 // Sensor type DHT11/21/22/AM2301/AM2302
#define DHTPIN D2     // Digital pin for communications
#define LED D0
#define LED2 D7

// Lib instantiate
PietteTech_DHT DHT(DHTPIN, DHTTYPE);

double humidity;
double temp;
double humidityThreshold = 60.0;
int nextRead = 0;

int setTimeZone(String command);
int setHumidityThreshold(String command);

STARTUP(WiFi.selectAntenna(ANT_AUTO));

void setup()
{
  Serial.begin(9600);
  while (!Serial.available() && millis() < 5000)
  {
    Serial.println("Press any key to start.");
    Particle.process();
    delay(1000);
  }
  Serial.println("DHT Simple program using DHT.acquireAndWait");
  Serial.print("LIB version: ");
  Serial.println(DHTLIB_VERSION);
  Serial.println("---------------");
  DHT.begin();
  pinMode(LED, OUTPUT);
  pinMode(LED2, OUTPUT);

  Time.zone(2);

  Particle.variable("temp", temp);
  Particle.variable("humidity", humidity);
  Particle.function("setHumidityThreshold", setHumidityThreshold);
  Particle.function("setTimeZone", setTimeZone);
}

void loop()
{
  readDHT();
  updateLED();
}

void updateLED()
{
  if (humidity > humidityThreshold)
  {
    digitalWrite(LED, HIGH); // sets the LED on
    // digitalWrite(LED2, HIGH); // sets the LED on
  }
  else
  {
    digitalWrite(LED, LOW); // sets the LED off
    // digitalWrite(LED2, LOW); // sets the LED off
  }
}

void readDHT()
{
  if (nextRead < Time.now())
  {
    Serial.print("\n");
    Serial.print("Retrieving information from sensor: ");
    Serial.print("Read sensor: ");

    int result = DHT.acquireAndWait(1000); // wait up to 1 sec (default indefinitely)

    switch (result)
    {
    case DHTLIB_OK:
      Serial.println("OK");
      break;
    case DHTLIB_ERROR_CHECKSUM:
      Serial.println("Error\n\r\tChecksum error");
      break;
    case DHTLIB_ERROR_ISR_TIMEOUT:
      Serial.println("Error\n\r\tISR time out error");
      break;
    case DHTLIB_ERROR_RESPONSE_TIMEOUT:
      Serial.println("Error\n\r\tResponse time out error");
      break;
    case DHTLIB_ERROR_DATA_TIMEOUT:
      Serial.println("Error\n\r\tData time out error");
      break;
    case DHTLIB_ERROR_ACQUIRING:
      Serial.println("Error\n\r\tAcquiring");
      break;
    case DHTLIB_ERROR_DELTA:
      Serial.println("Error\n\r\tDelta time to small");
      break;
    case DHTLIB_ERROR_NOTSTARTED:
      Serial.println("Error\n\r\tNot started");
      break;
    default:
      Serial.println("Unknown error");
      break;
    }

    Serial.print("Humidity (%): ");
    Serial.println(DHT.getHumidity(), 2);

    Serial.print("Temperature (oC): ");
    Serial.println(DHT.getCelsius(), 2);

    Serial.print("Temperature (oF): ");
    Serial.println(DHT.getFahrenheit(), 2);

    Serial.print("Temperature (K): ");
    Serial.println(DHT.getKelvin(), 2);

    Serial.print("Dew Point (oC): ");
    Serial.println(DHT.getDewPoint());

    Serial.print("Dew Point Slow (oC): ");
    Serial.println(DHT.getDewPointSlow());

    Serial.print("Heat Index (oC) / (oF): ");
    Serial.print(DHT.getHeatIndex());
    Serial.print(" / ");
    Serial.println(DHT.CtoF(DHT.getHeatIndex()));

    double h = DHT.getHumidity();
    double t = DHT.getCelsius();
    double f = DHT.getFahrenheit();

    if (isnan(h) || isnan(t) || isnan(f) || t < 0)
    {
      Serial.println("Failed to read from DHT sensor!");
      nextRead = Time.now() + 2; // Warte 2 Sekunden, wenn Fehler beim Lesen
      return;
    }

    nextRead = Time.now() + 60;

    humidity = h;
    temp = t;

    Particle.publish("dp_temp", String(temp, 1));
  }
}

// this function automagically gets called upon a matching POST request
int setTimeZone(String timeZone)
{
  int tz = atoi(timeZone);
  Time.zone(tz);
  return tz;
}

int setHumidityThreshold(String command)
{
  int ht = atoi(command);
  humidityThreshold = ht;
  return ht;
}