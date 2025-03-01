#include <WiFi.h>
#include <WebServer.h>
#include <Arduino.h>

// Function declaration BEFORE setup()
void handleRoot();

// WiFi Credentials
const char *ssid = "EU Guest";         // Change this to your WiFi name
const char *password = "GuestAccess!"; // Change this to your WiFi password

// Manual Static IP Configuration
IPAddress local_IP(172, 16, 1, 200); // Set within your router's subnet
IPAddress gateway(172, 16, 1, 1);    // Router's gateway
IPAddress subnet(255, 255, 255, 0);  // Subnet mask

WebServer server(80);

// PIR Motion Sensor Pins
const int PIR1 = 19;   // First PIR motion sensor (Entry)
const int PIR2 = 18;   // Second PIR motion sensor (Exit)
const int ledpin1 = 5; // LED for PIR1 (Entry)
const int ledpin2 = 4; // LED for PIR2 (Exit)

// People counter variables
int counter = 0;
bool pir1_triggered = false;
bool pir2_triggered = false;
unsigned long lastTriggerTime = 0;
const unsigned long debounceTime = 500; // 500ms debounce

void setup()
{
    // Set up PIR sensors and LEDs
    pinMode(PIR1, INPUT);
    pinMode(PIR2, INPUT);
    pinMode(ledpin1, OUTPUT);
    pinMode(ledpin2, OUTPUT);

    // Start Serial Monitor
    Serial.begin(115200);
    Serial.println("People Counter Initialized.");

    // Assign Static IP BEFORE Connecting to WiFi
    if (!WiFi.config(local_IP, gateway, subnet))
    {
        Serial.println("⚠ Failed to configure Static IP! Reverting to DHCP.");
    }

    // Connect to WiFi
    WiFi.begin(ssid, password);
    Serial.println("Connecting to WiFi...");

    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 20)
    { // Limit attempts
        delay(1000);
        Serial.print(".");
        attempts++;
    }

    if (WiFi.status() == WL_CONNECTED)
    {
        Serial.println("\n✅ Connected to WiFi!");
        Serial.print("🌍 ESP32 IP Address: ");
        Serial.println(WiFi.localIP());
    }
    else
    {
        Serial.println("\n❌ Failed to connect to WiFi!");
    }

    // Web server route
    server.on("/", handleRoot);
    server.begin();
    Serial.println("🌐 Web Server Started!");
}

// Function definition AFTER setup()
void handleRoot()
{
    String html = "<html><head><meta http-equiv='refresh' content='2'></head>"
                  "<body style='font-family: Arial, sans-serif; text-align: center;'>"
                  "<h1>People Counter</h1>"
                  "<h2 style='font-size: 50px; color: blue;'>" +
                  String(counter) + "</h2>"
                                    "</body></html>";
    server.send(200, "text/html", html);
}

void loop()
{
    server.handleClient();

    // Read PIR sensors
    int pir1_state = digitalRead(PIR1);
    int pir2_state = digitalRead(PIR2);
    unsigned long currentTime = millis();

    Serial.print("ESP32 IP Address: ");
    Serial.println(WiFi.localIP());

    // PIR1 (Entry Sensor)
    if (pir1_state == HIGH && !pir1_triggered && (currentTime - lastTriggerTime > debounceTime))
    {
        pir1_triggered = true;
        lastTriggerTime = currentTime;
        counter++;
        Serial.print("Person entered. Counter: ");
        Serial.println(counter);
        digitalWrite(ledpin1, HIGH);
        delay(500);
        digitalWrite(ledpin1, LOW);
    }
    if (pir1_state == LOW && pir1_triggered)
    {
        pir1_triggered = false;
    }

    // PIR2 (Exit Sensor)
    if (pir2_state == HIGH && !pir2_triggered && (currentTime - lastTriggerTime > debounceTime))
    {
        pir2_triggered = true;
        lastTriggerTime = currentTime;
        counter--;
        if (counter < 0)
            counter = 0; // Prevent negative count
        Serial.print("Person exited. Counter: ");
        Serial.println(counter);
        digitalWrite(ledpin2, HIGH);
        delay(500);
        digitalWrite(ledpin2, LOW);
    }
    if (pir2_state == LOW && pir2_triggered)
    {
        pir2_triggered = false;
    }
}