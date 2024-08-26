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

 **Navigation** | [AeonLabs Main Index](https://github.com/aeonSolutions/aeonSolutions/blob/main/aeonSolutions-Main-Index.md)  >> [Open Source Code Software](https://github.com/aeonSolutions/aeonlabs-open-software-catalogue)  >>   Hardware Test Code  >> Zenodo API - Test File Upload
 
<p align="right">
   <img height="25" src="https://github.com/aeonSolutions/aeonSolutions/blob/main/media/language-icon.png"> 
 <a href="https://github-com.translate.goog/aeonSolutions/Aeonlabs-Arduino_Hardware_Test_Code?_x_tr_sl=en&_x_tr_tl=nl&_x_tr_hl=en&_x_tr_pto=wapp">Change Language</a> <br>
Last update: 26-08-2024 <br>
<sub> <a href="https://www.rapidtables.com/convert/number/hex-to-binary.html?x=64">HEX to Bin Conveter</a> </sub>
</p>

To upload a file to Zenodo using an Arduino, particularly an ESP8266 or ESP32, you need to interact with the Zenodo REST API. Zenodo requires authentication through a personal access token and allows file uploads to a specific deposit.

### Prerequisites

1. **Arduino Board with WiFi Capabilities**: ESP8266 or ESP32.
2. **Zenodo Account**: You need a Zenodo account with an API token that has the necessary permissions.
3. **Arduino IDE**: Ensure the latest version is installed with the ESP8266 or ESP32 board support.

### Steps to Upload a File to Zenodo Using Arduino

1. **Install Required Libraries**: Ensure the Arduino IDE has the necessary libraries. We will use the `WiFi` library for networking and `HTTPClient` for HTTP requests.
   
2. **Obtain Zenodo Access Token**: Go to Zenodo and generate a personal access token from your account settings.

3. **Create a New Deposit on Zenodo**: You can create a new deposit via the Zenodo web interface or API. The deposit ID will be required to upload files.

### Arduino Code Example

Here's the Arduino code to upload a file to a Zenodo deposit using the Zenodo API.

```cpp
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
```

### Key Components of the Code

1. **WiFi Setup**: Connects the ESP8266/ESP32 to a WiFi network.
2. **File System Initialization**: Mounts the SPIFFS or LittleFS filesystem to read the file content.
3. **File Reading**: Reads the file content from the filesystem into a String.
4. **HTTP POST Request**: Sends an HTTP POST request to the Zenodo API to upload the file to a deposit.
5. **Error Handling**: Handles various error cases, including connection and file reading errors.

### Additional Notes

- **Libraries**: Ensure you have the required libraries (`WiFi.h`, `HTTPClient.h`, and the appropriate filesystem library) installed in your Arduino IDE.
- **Zenodo API Token**: Ensure the API token has the necessary permissions for file upload.
- **File Format and Content Type**: The file upload requires a correct MIME type. Adjust the content type to match the file type being uploaded.

By following these steps and using this code, you should be able to upload a file from your Arduino board to a Zenodo deposit. Remember to replace placeholders with your specific credentials and information.
