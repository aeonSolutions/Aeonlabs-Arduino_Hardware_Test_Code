#include <WiFi.h>          // For ESP32; use <ESP8266WiFi.h> for ESP8266
#include <HTTPClient.h>
#include <FS.h>            // Include the SPIFFS or LittleFS library

// WiFi credentials
const char* ssid = "YOUR_SSID";
const char* password = "YOUR_PASSWORD";

// Dataverse API credentials
const char* dataverseToken = "YOUR_DATAVERSE_API_TOKEN";       // Replace with your Dataverse API token
const char* dataverseUrl = "https://your.dataverse.server";    // Replace with your Dataverse server URL
const char* datasetPersistentId = "doi:10.5072/FK2/ABCDEF";   // Replace with your Dataset Persistent Identifier

// File details
const char* filePath = "/test.txt";                           // The path to the file on the filesystem (SPIFFS/LittleFS)
const char* contentType = "text/plain";                       // Content type of the file being uploaded

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

    // Prepare the URL for Dataverse API file upload
    String apiUrl = String(dataverseUrl) + "/api/datasets/:persistentId/add?persistentId=" + String(datasetPersistentId);

    // Upload file to Dataverse
    if (WiFi.status() == WL_CONNECTED) {
        HTTPClient http;
        http.begin(apiUrl);
        http.addHeader("X-Dataverse-key", dataverseToken);        // API token for authentication
        http.addHeader("Content-Type", contentType);             // Content-Type header
        http.addHeader("Content-Disposition", "attachment; filename=\"" + String(filePath) + "\""); // Content-Disposition header

        // POST file content to Dataverse
        int httpResponseCode = http.POST(fileContent);

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
