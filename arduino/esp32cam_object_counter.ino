#include <WebServer.h>
#include <WiFi.h>
#include <esp32cam.h>

// ===== CONFIGURE YOUR WIFI CREDENTIALS =====
const char* WIFI_SSID = "your_ssid";      // Replace with your WiFi name
const char* WIFI_PASS = "your_password";  // Replace with your WiFi password

WebServer server(80);

// Define resolutions
static auto loRes = esp32cam::Resolution::find(320, 240);
static auto midRes = esp32cam::Resolution::find(640, 480);  // Fixed: standard resolution
static auto hiRes = esp32cam::Resolution::find(800, 600);

void serveJpg()
{
  auto frame = esp32cam::capture();
  if (frame == nullptr) {
    Serial.println("CAPTURE FAIL");
    server.send(503, "", "");
    return;
  }
  Serial.printf("CAPTURE OK %dx%d %db\n", frame->getWidth(), frame->getHeight(),
                static_cast<int>(frame->size()));

  server.setContentLength(frame->size());
  server. send(200, "image/jpeg");
  WiFiClient client = server.client();
  frame->writeTo(client);
}

void handleJpgLo()  // Fixed: 'void' lowercase
{
  if (! esp32cam::Camera. changeResolution(loRes)) {
    Serial.println("SET-LO-RES FAIL");
  }
  serveJpg();
}

void handleJpgHi()  // Fixed: 'void' lowercase
{
  if (! esp32cam::Camera.changeResolution(hiRes)) {
    Serial.println("SET-HI-RES FAIL");
  }
  serveJpg();
}

void handleJpgMid()  // Fixed: 'void' lowercase
{
  if (!esp32cam::Camera.changeResolution(midRes)) {
    Serial. println("SET-MID-RES FAIL");
  }
  serveJpg();
}

void setup()  // Fixed: 'void' lowercase
{
  Serial.begin(115200);
  Serial.println();
  
  // Camera configuration
  {
    using namespace esp32cam;
    Config cfg;
    cfg. setPins(pins::AiThinker);
    cfg. setResolution(hiRes);
    cfg.setBufferCount(2);
    cfg.setJpeg(80);

    bool ok = Camera.begin(cfg);
    Serial.println(ok ? "CAMERA OK" : "CAMERA FAIL");
  }
  
  // WiFi configuration
  WiFi.persistent(false);
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println("\nWiFi Connected!");
  Serial. print("Camera Stream Ready!  Go to: http://");
  Serial.println(WiFi.localIP());
  Serial.println("  /cam-lo.jpg  - Low Resolution (320x240)");
  Serial. println("  /cam-mid.jpg - Medium Resolution (640x480)");
  Serial. println("  /cam-hi.jpg  - High Resolution (800x600)");

  // Setup URL handlers
  server.on("/cam-lo. jpg", handleJpgLo);
  server.on("/cam-hi.jpg", handleJpgHi);
  server.on("/cam-mid. jpg", handleJpgMid);

  server.begin();
}

void loop()  // Fixed: 'void' lowercase
{
  server.handleClient();
}