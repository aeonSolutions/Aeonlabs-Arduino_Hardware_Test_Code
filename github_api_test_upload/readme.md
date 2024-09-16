[![](https://dcbadge.vercel.app/api/server/hw3j3RwfJf) ](https://discord.gg/hw3j3RwfJf)
 [![Donate](https://img.shields.io/badge/donate-$-brown.svg?style=for-the-badge)](http://paypal.me/mtpsilva)
<a href="https://github.com/sponsors/aeonSolutions">
   <img height="40" src="https://github.com/aeonSolutions/PCB-Prototyping-Catalogue/blob/main/media/become_a_github_sponsor.png">
</a>
[<img src="https://cdn.buymeacoffee.com/buttons/v2/default-yellow.png" data-canonical-src="https://cdn.buymeacoffee.com/buttons/v2/default-yellow.png" height="30" />](https://www.buymeacoffee.com/migueltomas)
![](https://img.shields.io/github/last-commit/aeonSolutions/aeonlabs-open-software-catalogue?style=for-the-badge)
<img src="https://us-central1-trackgit-analytics.cloudfunctions.net/token/ping/m054utqmhdrb4aztkb5m" alt="trackgit-views" />
![](https://views.whatilearened.today/views/github/aeonSolutions/Aeonlabs-Arduino_Hardware_Test_Code.svg)
[![Open Source Love svg1](https://badges.frapsoft.com/os/v1/open-source.svg?v=103)](#)

 **Navigation** | [AeonLabs Main Index](https://github.com/aeonSolutions/aeonSolutions/blob/main/aeonSolutions-Main-Index.md)  >> [Open Source Code Software](https://github.com/aeonSolutions/aeonlabs-open-software-catalogue)  >>   Hardware Test Code  >>  GitHub API - Test File Upload
 
<p align="right">
   <img height="25" src="https://github.com/aeonSolutions/aeonSolutions/blob/main/media/language-icon.png"> 
 <a href="https://github-com.translate.goog/aeonSolutions/Aeonlabs-Arduino_Hardware_Test_Code?_x_tr_sl=en&_x_tr_tl=nl&_x_tr_hl=en&_x_tr_pto=wapp">Change Language</a> <br>
Last update: 16-09-2024 <br>
<sub> <a href="https://www.rapidtables.com/convert/number/hex-to-binary.html?x=64">HEX to Bin Conveter</a> </sub>
</p>

To upload a file to GitHub from an Arduino, we need to make HTTP requests to GitHub's REST API. Most Arduino boards, especially those based on ESP8266 or ESP32, can connect to the internet and perform HTTP operations. In this example, we'll assume you're using an ESP32 or ESP8266 board, which has built-in WiFi capabilities.

### Requirements

1. **Arduino Board with WiFi Capabilities**: ESP8266 or ESP32.
2. **GitHub Personal Access Token**: You need a personal access token with the proper permissions to access your repository.
3. **Arduino IDE**: Ensure the latest version is installed with the ESP8266 or ESP32 board support.

### Steps

1. **Install Required Libraries**: Ensure the Arduino IDE has the necessary libraries. We will use the `WiFi` library for networking and `HTTPClient` for HTTP requests.

2. **GitHub Setup**: Create a repository or use an existing one. Generate a personal access token from your GitHub account with the required scopes (e.g., `repo` for full control of private repositories or `public_repo` for public repositories).

### Arduino Code Example

This example demonstrates uploading a file to a GitHub repository. It reads the content of a file from the filesystem (SPIFFS or LittleFS) on the ESP8266/ESP32 and uploads it to GitHub.

```cpp
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
```

### Key Components of the Code

1. **WiFi Setup**: Connects the ESP8266/ESP32 to a WiFi network.
2. **File System Initialization**: Mounts the SPIFFS or LittleFS filesystem to read the file content.
3. **File Reading**: Reads the file content from the filesystem.
4. **Base64 Encoding**: Encodes the file content in Base64, as required by GitHub's API.
5. **HTTP PUT Request**: Sends an HTTP PUT request to GitHub's API to create or update a file in the repository.
6. **Error Handling**: Handles various error cases, including connection and file reading errors.

### Additional Notes

- **Libraries**: Make sure that you have the required libraries (`WiFi.h`, `HTTPClient.h`, and the appropriate filesystem library) installed in your Arduino IDE.
- **GitHub Permissions**: The personal access token must have the correct permissions for repository access.
- **Security**: Never hardcode sensitive information like passwords or tokens in your source code. Consider using environment variables or secure methods for storing credentials.

By following these steps and using this code, you should be able to upload a file from your Arduino board to a GitHub repository.
