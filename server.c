#include <WiFi.h>
#include <WebServer.h>

const char *ssid = "EU Guest";
const char *password = "GuestAccess!";

WebServer server(80);

int peopleCount = 0; // Example variable to store the count

void handleRoot()
{
    String html = "<html><head><meta http-equiv='refresh' content='2'></head>"
                  "<body><h1>People Count: " +
                  String(peopleCount) + "</h1></body></html>";
    server.send(200, "text/html", html);
}

void setup()
{
    Serial.begin(115200);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(1000);
        Serial.println("Connecting...");
    }
    Serial.println("Connected to WiFi");

    server.on("/", handleRoot);
    server.begin();
    Serial.println("Server started!");
}

void loop()
{
    server.handleClient();

    // Simulate people counting (replace with real sensor logic)
    peopleCount = random(0, 20);
    delay(2000);
}
