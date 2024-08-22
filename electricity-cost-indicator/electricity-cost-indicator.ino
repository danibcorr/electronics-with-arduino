// Libraries
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

// Define the Wi-Fi network name and password
const char* ssid = "";
const char* password = "";

// URL of the API that provides the electricity price in JSON format
const char* url = "https://api.preciodelaluz.org/v1/prices/now?zone=PCB";

// Define the pin numbers to which the LEDs are connected
const int pinLED_low = 15, pinLED_high = 4;

void setup() 
{
  Serial.begin(115200);

  while(!Serial)
  {
    delay(100);
  }

  // Set the LED pins as output
  pinMode(pinLED_low, OUTPUT);
  pinMode(pinLED_high, OUTPUT);

  // Start the Wi-Fi connection
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) 
  {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  
  Serial.print("Connected to network ");
  Serial.print(ssid);
  Serial.println(".");
}

void loop() 
{
  // Make an HTTP request to get the electricity price
  HTTPClient http;

  http.begin(url);
  
  int httpResponseCode = http.GET();

  if (httpResponseCode == 200) 
  {
    // If the request is successful, get the JSON
    String payload = http.getString();

    // Parse the JSON
    DynamicJsonDocument doc(1024);
    deserializeJson(doc, payload);

    // Get the values of the parameters of interest
    String isCheapString = doc["is-cheap"], priceString = doc["price"];
    float priceFloat = priceString.toFloat() / 1000;

    if (isCheapString == "false") 
    {
      digitalWrite(pinLED_high, HIGH);
      digitalWrite(pinLED_low, LOW);
      Serial.print("High price: ");
    } 
    else 
    {
      digitalWrite(pinLED_high, LOW);
      digitalWrite(pinLED_low, HIGH);
      Serial.print("Low price: ");
    }

    Serial.print(priceFloat);
    Serial.println(" €/kWh.");
  } 
  else 
  {
    Serial.print("HTTP request error. Response code: ");
    Serial.println(httpResponseCode);
  }

  // Wait for some time before making another request; the API allows up to 20 requests per minute
  // In this case, a request is made every minute
  delay(60000);
}

