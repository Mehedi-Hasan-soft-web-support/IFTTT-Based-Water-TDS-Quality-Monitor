/*
 * ESP32 TDS Sensor - FAST Google Sheets via IFTTT
 * 
 * Your IFTTT is ready to use!
 * Just update WiFi credentials below and upload
 * 
 * Expected speed: 500-800ms per upload (10x faster!)
 */

#include <WiFi.h>
#include <HTTPClient.h>

// ========== CONFIGURE THESE ==========
const char* ssid = "Me.";           // Change this
const char* password = "mehedi113";   // Change this

// IFTTT Configuration (Already set for you!)
String IFTTT_KEY = "ej6PXE8QlFSPJW_jqS1eeeehBYTKBCxmWJEBflPnvTB";
String IFTTT_EVENT = "tds_reading";

// Measurement interval (change if needed)
unsigned long measureInterval = 60000;  // 60 seconds
// =====================================

// TDS Sensor Configuration
#define TDS_PIN 34
#define VREF 3.3
#define SCOUNT 30

// Timing
unsigned long lastMeasureTime = 0;
float temperature = 25.0;

int analogBuffer[SCOUNT];

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  Serial.println("\n╔════════════════════════════════╗");
  Serial.println("║  ESP32 TDS - FAST Logger      ║");
  Serial.println("║  Using IFTTT Webhooks          ║");
  Serial.println("╚════════════════════════════════╝\n");
  
  pinMode(TDS_PIN, INPUT);
  
  connectWiFi();
  
  Serial.println("\n✓ System Ready!");
  Serial.println("✓ Data will log to Google Sheets");
  Serial.println("✓ Check: Google Drive > IFTTT > TDS_Monitor\n");
}

void loop() {
  unsigned long currentTime = millis();
  
  if (currentTime - lastMeasureTime >= measureInterval) {
    lastMeasureTime = currentTime;
    
    // Read TDS value
    float tdsValue = readTDS();
    String quality = getWaterQuality(tdsValue);
    
    // Display on Serial Monitor
    Serial.println("┌─────────────────────────┐");
    Serial.print("│ TDS: ");
    Serial.print(tdsValue, 2);
    Serial.println(" ppm");
    Serial.print("│ Temp: ");
    Serial.print(temperature, 1);
    Serial.println(" °C");
    Serial.print("│ Quality: ");
    Serial.println(quality);
    Serial.println("├─────────────────────────┤");
    
    // Send to Google Sheets via IFTTT
    if (WiFi.status() == WL_CONNECTED) {
      unsigned long startTime = millis();
      bool success = sendToIFTTT(tdsValue, temperature, quality);
      unsigned long elapsed = millis() - startTime;
      
      if (success) {
        Serial.print("│ ✓ Uploaded: ");
        Serial.print(elapsed);
        Serial.println(" ms");
      } else {
        Serial.println("│ ✗ Upload failed");
      }
    } else {
      Serial.println("│ ⚠ WiFi disconnected");
      connectWiFi();
    }
    
    Serial.println("└─────────────────────────┘\n");
  }
  
  delay(100);
}

void connectWiFi() {
  Serial.print("Connecting to WiFi: ");
  Serial.println(ssid);
  
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(500);
    Serial.print(".");
    attempts++;
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\n✓ WiFi Connected!");
    Serial.print("  IP Address: ");
    Serial.println(WiFi.localIP());
    Serial.print("  Signal: ");
    Serial.print(WiFi.RSSI());
    Serial.println(" dBm");
  } else {
    Serial.println("\n✗ WiFi Connection Failed!");
  }
}

float readTDS() {
  // Fill analog buffer with samples
  for (int i = 0; i < SCOUNT; i++) {
    analogBuffer[i] = analogRead(TDS_PIN);
    delay(10);
  }
  
  // Sort buffer for median filtering
  for (int i = 0; i < SCOUNT - 1; i++) {
    for (int j = i + 1; j < SCOUNT; j++) {
      if (analogBuffer[i] > analogBuffer[j]) {
        int temp = analogBuffer[i];
        analogBuffer[i] = analogBuffer[j];
        analogBuffer[j] = temp;
      }
    }
  }
  
  // Calculate average (remove outliers)
  long sum = 0;
  for (int i = 5; i < SCOUNT - 5; i++) {
    sum += analogBuffer[i];
  }
  
  float averageVoltage = (sum / (SCOUNT - 10)) * (VREF / 4095.0);
  
  // Temperature compensation
  float compensationCoefficient = 1.0 + 0.02 * (temperature - 25.0);
  float compensationVoltage = averageVoltage / compensationCoefficient;
  
  // Convert to TDS value
  float tdsValue = (133.42 * compensationVoltage * compensationVoltage * compensationVoltage 
                    - 255.86 * compensationVoltage * compensationVoltage 
                    + 857.39 * compensationVoltage) * 0.5;
  
  return tdsValue;
}

bool sendToIFTTT(float tds, float temp, String quality) {
  HTTPClient http;
  
  // Build IFTTT webhook URL
  String url = "http://maker.ifttt.com/trigger/" + IFTTT_EVENT + "/with/key/" + IFTTT_KEY;
  
  // Create JSON payload
  String jsonData = "{";
  jsonData += "\"value1\":\"" + String(tds, 2) + "\",";
  jsonData += "\"value2\":\"" + String(temp, 1) + "\",";
  jsonData += "\"value3\":\"" + quality + "\"";
  jsonData += "}";
  
  // Send HTTP POST request
  http.begin(url);
  http.addHeader("Content-Type", "application/json");
  http.setTimeout(5000);  // 5 second timeout
  
  int httpCode = http.POST(jsonData);
  
  bool success = (httpCode == 200 || httpCode == 201);
  
  if (!success) {
    Serial.print("  HTTP Error: ");
    Serial.println(httpCode);
  }
  
  http.end();
  return success;
}

String getWaterQuality(float tds) {
  if (tds < 50) return "Low Minerals";
  else if (tds < 150) return "Excellent";
  else if (tds < 250) return "Good";
  else if (tds < 300) return "Fair";
  else if (tds < 500) return "Poor";
  else return "Unacceptable";
}

/*
 * ============================================
 * WATER QUALITY REFERENCE (Drinking Water)
 * ============================================
 * 0-50 ppm:     Low minerals (too pure)
 * 50-150 ppm:   Excellent ✓
 * 150-250 ppm:  Good ✓
 * 250-300 ppm:  Fair
 * 300-500 ppm:  Poor
 * 500+ ppm:     Unacceptable
 * ============================================
 * 
 * Your data will be saved to:
 * Google Drive > IFTTT > TDS_Monitor
 * 
 * Spreadsheet format:
 * Column A: TDS (ppm)
 * Column B: Temperature (°C)
 * Column C: Water Quality
 * Column D: Timestamp (auto-added by IFTTT)
 * 
 * Expected upload speed: 500-800ms
 * ============================================
 */
