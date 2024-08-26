#include <WiFi.h>          // For ESP32; use <ESP8266WiFi.h> for ESP8266
#include <HTTPClient.h>
#include <FS.h>            // Include the SPIFFS or LittleFS library

// WiFi credentials
const char* ssid = "YOUR_SSID";
const char* password = "YOUR_PASSWORD";

// GitHub credentials
const char* githubToken = "YOUR_GITHUB_PERSONAL_ACCESS_TOKEN"; // Replace with your GitHub token
const char* githubUser = "YOUR_GITHUB_USERNAME";              // Replace with your GitHub username
const char* githubRepo = "YOUR_GITHUB_REPO";                  // Replace with your GitHub repository
const char* githubBranch = "main";                            // Replace with the branch you want to commit to

// File details
const char* filePath = "/test.txt";                           // The path to the file on the filesystem (SPIFFS/LittleFS)
const char* githubFilePath = "test.txt";                      // Path in the GitHub repository

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

    String fileContent;
    while (file.available()) {
        fileContent += char(file.read());
    }
    file.close();

    // Base64 encode the file content (GitHub API requires base64 encoded content)
    String base64Content = base64::encode(fileContent);

    // Prepare JSON payload
    String jsonPayload = "{ \"message\": \"Add " + String(githubFilePath) + " via Arduino\", \"content\": \"" + base64Content + "\", \"branch\": \"" + String(githubBranch) + "\" }";
    
    // Create the GitHub API URL
    String githubUrl = "https://api.github.com/repos/" + String(githubUser) + "/" + String(githubRepo) + "/contents/" + String(githubFilePath);

    // Upload file to GitHub
    if (WiFi.status() == WL_CONNECTED) {
        HTTPClient http;
        http.begin(githubUrl);
        http.addHeader("Authorization", "token " + String(githubToken));
        http.addHeader("Content-Type", "application/json");

        int httpResponseCode = http.PUT(jsonPayload); // Use PUT method to create/update file

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
