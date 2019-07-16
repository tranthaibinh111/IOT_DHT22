// Them thu vien
// Hổ trợ kết nối wifi
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <WiFiClient.h>
// Hổ trợ post data
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>
// Gọi thư viện DHT22
#include <DHT.h> 
// Hổ trợ lấy time
#include <NTPClient.h>
#include <WiFiUdp.h>

// KEY sensor
const char* keySensor = "a81bff24-0435-47c2-823b-a2232a9aa75e";

// Thông số WiFi
const char* ssid = "Home1";                   // Tên Wifi
const char* password = "doimatthienthan";     // Mật khẩu
const char* api = "http://192.168.1.5:8000/iot/cron_handle/";
const char* token = "Token c2202e220bed4f78ed4c78e69d5f73cfcbd66482"
ESP8266WiFiMulti WiFiMulti;

/*------------------------DHT SENSOR------------------------*/
//Đọc dữ liệu từ DHT22 ở chân 2 trên mạch Arduino
const uint8_t DHTPIN = D2;
//Khai báo loại cảm biến, có 2 loại là DHT11 và DHT22
const uint8_t DHTTYPE = DHT22;
 
DHT dht(DHTPIN, DHTTYPE);
/*----------------------------------------------------------*/

// Định nghĩa NTP Client dùng lấy thời gian
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org");

void setup() 
{
  
  // Khởi động serial
  Serial.begin(115200);
  // Kết nối với WiFi
  WiFi.begin(ssid, password);
  
  // Kiểm tra xem đã kết nối chưa
  while (WiFi.status() != WL_CONNECTED) 
  {
  
    delay(1000);
    Serial.println("Connecting...");
  
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP()); 
  
  // Khởi động cảm biến
  dht.begin();

  // Khỏi động NTPClient
  timeClient.begin();
}

void loop() 
{
  // Kiểm tra trạng thái wifi đã kết nối chưa
  if (WiFi.status() == WL_CONNECTED) { 
    // Cập nhật thời gian
    timeClient.update();
    int timestamp = timeClient.getEpochTime();

    //Đọc độ ẩm
    float h = dht.readHumidity();
    //Đọc nhiệt độ
    float t = dht.readTemperature();

    // Khai báo tỉnh buffer JSON 
    const size_t capacity = JSON_OBJECT_SIZE(2) + JSON_OBJECT_SIZE(3);
    DynamicJsonDocument doc(capacity);
    
    doc["sensor"] = keySensor;
    
    JsonObject data = doc.createNestedObject("data_json");
    data["humidity"] = h;
    data["temperature"] = t;
    data["timestamp"] = timestamp;
    // Lấy data
    char output[300];
    serializeJson(doc, output);

    serializeJsonPretty(doc, Serial);

    // Khai báo client và HTTPClient
    WiFiClient client;
    HTTPClient http;
    
    // Chỉ định địa chỉ post data
    http.begin(client, api);
    // Khai báo content-type header
    http.addHeader("Content-Type", "application/json");
    http.addHeader("Authorization", token);
    int httpCode = http.POST(output);
    //Get the response payload
    String payload = http.getString();

    //Print HTTP return code
    Serial.println(httpCode);
    //Print request response payload   
    Serial.println(payload);
   
    // Kết thúc connection
    http.end();
  } else {
    Serial.println("Error in WiFi connection");
  }
  delay(5000);
}
