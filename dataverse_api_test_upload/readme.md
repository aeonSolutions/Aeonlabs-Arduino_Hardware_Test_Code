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

 **Navigation** | [AeonLabs Main Index](https://github.com/aeonSolutions/aeonSolutions/blob/main/aeonSolutions-Main-Index.md)  >> [Open Source Code Software](https://github.com/aeonSolutions/aeonlabs-open-software-catalogue)  >>   Hardware Test Code  >>  Dataverse API - Test File Upload
 
<p align="right">
   <img height="25" src="https://github.com/aeonSolutions/aeonSolutions/blob/main/media/language-icon.png"> 
 <a href="https://github-com.translate.goog/aeonSolutions/Aeonlabs-Arduino_Hardware_Test_Code?_x_tr_sl=en&_x_tr_tl=nl&_x_tr_hl=en&_x_tr_pto=wapp">Change Language</a> <br>
Last update: 26-08-2024 <br>
<sub> <a href="https://www.rapidtables.com/convert/number/hex-to-binary.html?x=64">HEX to Bin Conveter</a> </sub>
</p>

To upload a file to a **Dataverse** repository using an Arduino device, we'll need to make HTTP requests to the Dataverse API. Dataverse provides an API that supports the ingestion of files into datasets, which we'll use to perform the upload.

Since Arduino devices like the ESP8266 or ESP32 are capable of connecting to the internet and making HTTP requests, we'll base this example on using an **ESP32** or **ESP8266** with WiFi connectivity.

### Prerequisites

1. **Arduino Board with WiFi Capabilities**: ESP8266 or ESP32.
2. **Dataverse Installation and API Token**: You need access to a Dataverse repository and an API token to authenticate API requests.
3. **Arduino IDE**: Ensure the latest version is installed with the ESP8266 or ESP32 board support.

### Steps to Upload a File to Dataverse Using Arduino

1. **Install Required Libraries**: Ensure the Arduino IDE has the necessary libraries. We will use the `WiFi` library for networking and `HTTPClient` for HTTP requests.
   
2. **Set Up Dataverse**: You must have an account and an API token for a Dataverse repository.

3. **Prepare the Dataset**: You should have a dataset ready in Dataverse to which you will upload the file. You need the dataset's Persistent Identifier (PID).

### Arduino Code Example

Below is the Arduino code to upload a file to a Dataverse dataset.

```cpp
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
```

### Key Components of the Code

1. **WiFi Setup**: Connects the ESP8266/ESP32 to a WiFi network.
2. **File System Initialization**: Mounts the SPIFFS or LittleFS filesystem to read the file content.
3. **File Reading**: Reads the file content from the filesystem into a String.
4. **HTTP POST Request**: Sends an HTTP POST request to the Dataverse API to upload the file into a dataset.
5. **Error Handling**: Handles various error cases, including connection and file reading errors.

### Additional Notes

- **Libraries**: Ensure you have the required libraries (`WiFi.h`, `HTTPClient.h`, and the appropriate filesystem library) installed in your Arduino IDE.
- **Dataverse API Token**: Ensure the API token has the necessary permissions for file upload.
- **Security**: Never hardcode sensitive information like passwords or tokens in your source code. Consider using environment variables or secure methods for storing credentials.
- **File Format and Content Type**: Adjust the `contentType` variable to match the type of file you are uploading. For example, if you are uploading a JSON file, set `contentType` to `"application/json"`.

By following these steps and using this code, you should be able to upload a file from your Arduino board to a Dataverse repository.
