#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Stepper.h>

// Wi-Fi credentials
const char* ssid = "Test";
const char* password = "12345678";

// Stepper motor settings
#define STEPS_PER_REV 2048
Stepper stepper(STEPS_PER_REV, 34, 35, 32, 33); // IN1–IN4

// OLED display settings
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Web server
AsyncWebServer server(80);

// Global direction flags
volatile bool rotatingLeft = false;
volatile bool rotatingRight = false;

void setup() {
  Serial.begin(115200);

  // Wi-Fi setup (dynamic IP)
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("Connecting to Wi-Fi...");
  display.display();

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  // Show IP on OLED
  Serial.println("\nWiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  display.clearDisplay();
  display.setCursor(0, 10);
  display.println("WiFi connected!");
  display.setCursor(0, 30);
  display.print("IP: ");
  display.println(WiFi.localIP());
  display.display();

  // Stepper motor setup
  stepper.setSpeed(10); // RPM

  // Web dashboard
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    String html = "<html><head><title>ESP32 Motor Control</title></head><body style='text-align:center;font-family:sans-serif;'>";
    html += "<h2>Stepper Motor Control</h2>";
    html += "<a href='/left'><button>⬅ Left</button></a> ";
    html += "<a href='/stop'><button>⏹ Stop</button></a> ";
    html += "<a href='/right'><button>➡ Right</button></a>";
    html += "</body></html>";
    request->send(200, "text/html", html);
  });

  server.on("/left", HTTP_GET, [](AsyncWebServerRequest *request){
    rotatingLeft = true;
    rotatingRight = false;
    request->redirect("/");
  });

  server.on("/right", HTTP_GET, [](AsyncWebServerRequest *request){
    rotatingRight = true;
    rotatingLeft = false;
    request->redirect("/");
  });

  server.on("/stop", HTTP_GET, [](AsyncWebServerRequest *request){
    rotatingLeft = false;
    rotatingRight = false;
    request->redirect("/");
  });

  server.begin();
}

void loop() {
  if (rotatingLeft) {
    stepper.step(-100);
  } else if (rotatingRight) {
    stepper.step(100);
  }
}
