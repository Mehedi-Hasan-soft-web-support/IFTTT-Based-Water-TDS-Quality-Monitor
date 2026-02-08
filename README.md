# ESP32 TDS Sensor - Google Sheets Logger via IFTTT

একটি দ্রুত এবং সহজ ESP32 ভিত্তিক TDS (Total Dissolved Solids) সেন্সর যা স্বয়ংক্রিয়ভাবে পানির গুণমান মাপে এবং Google Sheets এ ডেটা লগ করে IFTTT ব্যবহার করে।

## ✨ Features

- 🚀 **দ্রুত আপলোড**: 500-800ms এ Google Sheets এ ডেটা পাঠায়
- 📊 **স্বয়ংক্রিয় লগিং**: প্রতি 60 সেকেন্ডে TDS রিডিং রেকর্ড করে
- 🌡️ **তাপমাত্রা ক্ষতিপূরণ**: সঠিক পরিমাপের জন্য তাপমাত্রা সমন্বয়
- 💧 **পানির গুণমান বিশ্লেষণ**: স্বয়ংক্রিয়ভাবে পানির মান নির্ধারণ করে
- 📱 **সিরিয়াল মনিটর**: রিয়েল-টাইম ডেটা প্রদর্শন
- ☁️ **ক্লাউড স্টোরেজ**: Google Drive এ স্বয়ংক্রিয়ভাবে সংরক্ষণ

## 🛠️ Hardware Requirements

- ESP32 Development Board
- TDS Sensor (Analog)
- Jumper Wires
- USB Cable (for programming)

## 📌 Wiring Diagram

```
TDS Sensor    →    ESP32
─────────────────────────
VCC           →    3.3V
GND           →    GND
Signal        →    GPIO 34
```

## 📦 Software Requirements

- Arduino IDE (1.8.x or later)
- ESP32 Board Support
- Required Libraries:
  - WiFi.h (built-in)
  - HTTPClient.h (built-in)

## ⚙️ IFTTT Setup

### Step 1: Create IFTTT Account
1. যান [IFTTT.com](https://ifttt.com)
2. একটি বিনামূল্যে অ্যাকাউন্ট তৈরি করুন

### Step 2: Create Applet
1. **Create** বাটনে ক্লিক করুন
2. **If This** তে ক্লিক করুন → **Webhooks** খুঁজুন
3. **Receive a web request** নির্বাচন করুন
4. Event Name: `tds_reading` লিখুন

### Step 3: Configure Google Sheets Action
1. **Then That** তে ক্লিক করুন → **Google Sheets** খুঁজুন
2. **Add row to spreadsheet** নির্বাচন করুন
3. Configuration:
   - Spreadsheet name: `TDS_Monitor`
   - Formatted row: `{{Value1}} ||| {{Value2}} ||| {{Value3}} ||| {{OccurredAt}}`
   - Drive folder path: `IFTTT/TDS_Monitor`

### Step 4: Get Your Webhook Key
1. যান [IFTTT Webhooks Settings](https://ifttt.com/maker_webhooks/settings)
2. আপনার Webhook Key কপি করুন

## 🔧 Installation & Configuration

### 1. Arduino IDE Setup

```bash
# ESP32 Board Manager URL যোগ করুন:
File → Preferences → Additional Board Manager URLs
```
Add: `https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json`

```bash
# ESP32 Board Install করুন:
Tools → Board → Boards Manager → Search "ESP32" → Install
```

### 2. Code Configuration

কোডের এই অংশগুলো আপডেট করুন:

```cpp
// WiFi Credentials
const char* ssid = "YourWiFiName";        // আপনার WiFi নাম
const char* password = "YourPassword";     // আপনার WiFi পাসওয়ার্ড

// IFTTT Configuration
String IFTTT_KEY = "your_ifttt_webhook_key";  // আপনার IFTTT key
String IFTTT_EVENT = "tds_reading";            // ইভেন্ট নাম (একই রাখুন)
```

### 3. Upload Code

1. ESP32 বোর্ড USB দিয়ে কম্পিউটারে সংযুক্ত করুন
2. Arduino IDE তে:
   - **Tools → Board → ESP32 Dev Module** নির্বাচন করুন
   - **Tools → Port** সঠিক COM port নির্বাচন করুন
3. **Upload** বাটনে ক্লিক করুন

## 📊 Data Format

আপনার Google Sheets এ ডেটা এভাবে সংরক্ষিত হবে:

| Column A (TDS) | Column B (Temp) | Column C (Quality) | Column D (Timestamp) |
|----------------|-----------------|-------------------|----------------------|
| 125.45 ppm     | 25.0 °C         | Excellent         | January 15, 2025 at 10:30AM |
| 180.23 ppm     | 24.5 °C         | Good              | January 15, 2025 at 10:31AM |

## 💧 Water Quality Reference

| TDS Range (ppm) | Quality Category | Status |
|----------------|------------------|--------|
| 0-50           | Low Minerals     | ⚠️ Too Pure |
| 50-150         | Excellent        | ✅ Best |
| 150-250        | Good             | ✅ Acceptable |
| 250-300        | Fair             | ⚡ Moderate |
| 300-500        | Poor             | ⚠️ Not Ideal |
| 500+           | Unacceptable     | ❌ Unsafe |

## 🔍 Serial Monitor Output

```
╔════════════════════════════════╗
║  ESP32 TDS - FAST Logger      ║
║  Using IFTTT Webhooks          ║
╚════════════════════════════════╝

Connecting to WiFi: YourNetwork
✓ WiFi Connected!
  IP Address: 192.168.1.100
  Signal: -45 dBm

✓ System Ready!
✓ Data will log to Google Sheets
✓ Check: Google Drive > IFTTT > TDS_Monitor

┌─────────────────────────┐
│ TDS: 125.45 ppm
│ Temp: 25.0 °C
│ Quality: Excellent
├─────────────────────────┤
│ ✓ Uploaded: 650 ms
└─────────────────────────┘
```

## ⚡ Customization Options

### পরিমাপের সময় পরিবর্তন করুন

```cpp
unsigned long measureInterval = 60000;  // 60 seconds (default)

// উদাহরণ:
// 30 seconds: 30000
// 2 minutes:  120000
// 5 minutes:  300000
```

### তাপমাত্রা আপডেট করুন

```cpp
float temperature = 25.0;  // আপনার এলাকার গড় তাপমাত্রা দিন
```

## 🐛 Troubleshooting

### WiFi সংযোগ সমস্যা
- SSID এবং Password সঠিক আছে কিনা চেক করুন
- 2.4GHz WiFi ব্যবহার করুন (5GHz সমর্থিত নয়)
- রাউটারের কাছে ESP32 রাখুন

### IFTTT আপলোড ব্যর্থ
- IFTTT Webhook Key সঠিক আছে কিনা যাচাই করুন
- ইন্টারনেট সংযোগ চেক করুন
- IFTTT Applet 'Connected' আছে কিনা দেখুন

### TDS রিডিং 0 দেখাচ্ছে
- সেন্সর সংযোগ চেক করুন
- সেন্সর পানিতে ডুবানো আছে কিনা নিশ্চিত করুন
- GPIO 34 পিন সঠিকভাবে সংযুক্ত আছে কিনা দেখুন

## 📁 File Structure

```
ESP32-TDS-Logger/
│
├── ESP32_TDS_IFTTT.ino    # Main Arduino sketch
└── README.md              # This file
```

## 🤝 Contributing

Contributions, issues, and feature requests are welcome!

## 📄 License

This project is open source and available under the [MIT License](LICENSE).

## 👨‍💻 Author

Created with ❤️ for water quality monitoring

## 🔗 Useful Links

- [ESP32 Documentation](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/)
- [IFTTT Webhooks](https://ifttt.com/maker_webhooks)
- [TDS Sensor Guide](https://wiki.dfrobot.com/Gravity__Analog_TDS_Sensor___Meter_For_Arduino_SKU__SEN0244)

## 📞 Support

যদি কোনো সমস্যা হয়, তাহলে GitHub Issues ব্যবহার করুন অথবা IFTTT কমিউনিটি ফোরাম দেখুন।

---

**Note**: এই প্রজেক্ট শুধুমাত্র পানির মান পর্যবেক্ষণের জন্য। পানীয় জলের নিরাপত্তা নিশ্চিত করতে পেশাদার পরীক্ষা করান।
