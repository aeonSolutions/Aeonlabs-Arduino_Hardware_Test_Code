#include <WiFi.h>          // For ESP32; use <ESP8266WiFi.h> for ESP8266
#include <HTTPClient.h>
#include <FS.h>            // Include the SPIFFS or LittleFS library

// WiFi credentials
const char* ssid = "YOUR_SSID";
const char* password = "YOUR_PASSWORD";

// Zenodo API credentials
const char* zenodoToken = "YOUR_ZENODO_ACCESS_TOKEN";    // Replace with your Zenodo API token
const char* zenodoDepositId = "YOUR_DEPOSIT_ID";         // Replace with your Zenodo deposit ID
const char* zenodoUrl = "https://zenodo.org/api/deposit/depositions"; // Base URL for Zenodo deposit

// File details
const char* filePath = "/test.txt";                     // The path to the file on the filesystem (SPIFFS/LittleFS)
const char* fileName = "test.txt";                      // Name of the file to be uploaded

void setup() {
    Serial.begin(115200);
    delay(1000);

    // Connect to WiFi
    Serial.println("Connecting to WiFi...");
    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.print(".");
    }

    Serial.println("\nConnected to WiFi!");
  
    // Initialize filesystem
    if (!SPIFFS.begin(true)) {
        Serial.println("Failed to mount file system");
        return;
    }

    // Check if the file exists
    if (!SPIFFS.exists(filePath)) {
        Serial.println("File not found!");
        return;
    }

    // Read the file content
    File file = SPIFFS.open(filePath, "r");
    if (!file) {
        Serial.println("Failed to open file for reading");
        return;
    }

    // Read file into a String
    String fileContent;
    while (file.available()) {
        fileContent += char(file.read());
    }
    file.close();

    // Prepare the URL for Zenodo API file upload
    String apiUrl = String(zenodoUrl) + "/" + String(zenodoDepositId) + "/files?access_token=" + String(zenodoToken);

    // Upload file to Zenodo
    if (WiFi.status() == WL_CONNECTED) {
        HTTPClient http;
        http.begin(apiUrl);
        http.addHeader("Content-Type", "application/json"); // Content-Type header

        // Construct multipart/form-data body
        String boundary = "----WebKitFormBoundary7MA4YWxkTrZu0gW";
        String body = "--" + boundary + "\r\n";
        body += "Content-Disposition: form-data; name=\"file\"; filename=\"" + String(fileName) + "\"\r\n";
        body += "Content-Type: application/octet-stream\r\n\r\n";
        body += fileContent + "\r\n";
        body += "--" + boundary + "--\r\n";

        // Add headers for multipart/form-data
        http.addHeader("Content-Type", "multipart/form-data; boundary=" + boundary);

        int httpResponseCode = http.POST(body);

        if (httpResponseCode > 0) {
            String response = http.getString();
            Serial.println("File uploaded successfully!");
            Serial.println("Response: " + response);
        } else {
            Serial.println("Error uploading file: " + http.errorToString(httpResponseCode).c_str());
        }

        http.end();
    }

    // Disconnect WiFi after operation
    WiFi.disconnect(true);
}

void loop() {
    // Nothing to do here
}
