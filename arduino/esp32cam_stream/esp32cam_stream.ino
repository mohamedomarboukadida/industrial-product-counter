/*
 * Industrial Product Counter - ESP32-CAM Streaming Server
 * 
 * This code sets up the ESP32-CAM as a web server that streams
 * JPEG images for real-time object counting with OpenCV.
 * 
 * Author: Mohamed Omar Boukadida
 * License: MIT
 * Repository: https://github. com/mohamedomarboukadida/industrial-product-counter
 */

#include <WebServer.h>
#include <WiFi.h>
#include <esp32cam.h>

// ===== WIFI CONFIGURATION =====
// Replace with your network credentials
const char* WIFI_SSID = "your_wifi_ssid";
const char* WIFI_PASS = "your_wifi_password";

// ===== SERVER CONFIGURATION =====
WebServer server(80);

// ===== RESOLUTION SETTINGS =====
static auto loRes = esp32cam::Resolution::find(320, 240);   // Fast, low quality
static auto midRes = esp32cam::Resolution::find(640, 480);  // Balanced
static auto hiRes = esp32cam::Resolution::find(800, 600);   // Detailed, slower

/**
 * Captures and serves a JPEG image
 */
void serveJpg()
{
  auto frame = esp32cam::capture();
  if (frame == nullptr) {
    Serial.println("CAPTURE FAIL");
    server.send(503, "", "");
    return;
  }
  
  Serial.printf("CAPTURE OK %dx%d %db\n", 
                frame->getWidth(), 
                frame->getHeight(),
                static_cast<int>(frame->size()));

  server.setContentLength(frame->size());
  server. send(200, "image/jpeg");
  WiFiClient client = server.client();
  frame->writeTo(client);
}

/**
 * Handler for low resolution images
 */
void handleJpgLo()
{
  if (! esp32cam::Camera. changeResolution(loRes)) {
    Serial.println("SET-LO-RES FAIL");
  }
  serveJpg();
}

/**
 * Handler for medium resolution images
 */
void handleJpgMid()
{
  if (! esp32cam::Camera.changeResolution(midRes)) {
    Serial.println("SET-MID-RES FAIL");
  }
  serveJpg();
}

/**
 * Handler for high resolution images
 */
void handleJpgHi()
{
  if (!esp32cam::Camera.changeResolution(hiRes)) {
    Serial.println("SET-HI-RES FAIL");
  }
  serveJpg();
}

/**
 * Handler for root path - displays available endpoints
 */
void handleRoot()
{
  String html = "<html><head>";
  html += "<title>ESP32-CAM Object Counter</title>";
  html += "<style>body{font-family:Arial;text-align:center;padding:50px;}";
  html += "a{display:block;margin:20px;padding:15px;background:#007bff;";
  html += "color:white;text-decoration:none;border-radius:5px;}</style>";
  html += "</head><body>";
  html += "<h1>ESP32-CAM Streaming Server</h1>";
  html += "<p>Select resolution:</p>";
  html += "<a href='/cam-lo. jpg'>Low Resolution (320x240)</a>";
  html += "<a href='/cam-mid.jpg'>Medium Resolution (640x480)</a>";
  html += "<a href='/cam-hi.jpg'>High Resolution (800x600)</a>";
  html += "</body></html>";
  server.send(200, "text/html", html);
}

void setup()
{
  Serial.begin(115200);
  Serial. println();
  Serial.println("========================================");
  Serial.println("  Industrial Product Counter");
  Serial.println("  ESP32-CAM Streaming Server");
  Serial.println("========================================");
  
  // Initialize camera
  {
    using namespace esp32cam;
    Config cfg;
    cfg. setPins(pins::AiThinker);
    cfg.setResolution(hiRes);
    cfg.setBufferCount(2);
    cfg. setJpeg(80);

    bool ok = Camera.begin(cfg);
    Serial.println(ok ? "✓ Camera initialized" : "✗ Camera FAILED");
    
    if (!ok) {
      Serial. println("Restarting in 5 seconds...");
      delay(5000);
      ESP. restart();
    }
  }
  
  // Connect to WiFi
  WiFi.persistent(false);
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  
  Serial.print("Connecting to WiFi");
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 30) {
    delay(500);
    Serial.print(".");
    attempts++;
  }
  
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("\n✗ WiFi connection FAILED");
    Serial.println("Restarting in 5 seconds...");
    delay(5000);
    ESP. restart();
  }
  
  Serial.println("\n✓ WiFi Connected!");
  Serial. println("----------------------------------------");
  Serial.print("Camera URL: http://");
  Serial. println(WiFi.localIP());
  Serial.println("----------------------------------------");
  Serial.println("Available endpoints:");
  Serial. println("  /cam-lo.jpg  - Low Resolution");
  Serial.println("  /cam-mid.jpg - Medium Resolution");
  Serial.println("  /cam-hi.jpg  - High Resolution");
  Serial.println("========================================");

  // Setup URL handlers
  server.on("/", handleRoot);
  server.on("/cam-lo. jpg", handleJpgLo);
  server.on("/cam-mid.jpg", handleJpgMid);
  server.on("/cam-hi.jpg", handleJpgHi);

  server.begin();
  Serial.println("✓ HTTP server started");
}

void loop()
{
  server.handleClient();
}