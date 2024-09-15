/*
 Copyright (c) 2024 Miguel Tomas, http://www.aeonlabs.science  https://github.com/aeonSolutions

License Attribution-NonCommercial-ShareAlike 4.0 International (CC BY-NC-SA 4.0)
You are free to:
   Share — copy and redistribute the material in any medium or format
   Adapt — remix, transform, and build upon the material

The licensor cannot revoke these freedoms as long as you follow the license terms. Under the following terms:
Attribution — You must give appropriate credit, provide a link to the license, and indicate if changes were made. 
You may do so in any reasonable manner, but not in any way that suggests the licensor endorses you or your use.

NonCommercial — You may not use the material for commercial purposes.

ShareAlike — If you remix, transform, or build upon the material, you must distribute your contributions under
 the same license as the original.

No additional restrictions — You may not apply legal terms or technological measures that legally restrict others
 from doing anything the license permits.

Notices:
You do not have to comply with the license for elements of the material in the public domain or where your use
 is permitted by an applicable exception or limitation.
No warranties are given. The license may not give you all of the permissions necessary for your intended use. 
For example, other rights such as publicity, privacy, or moral rights may limit how you use the material.


Before proceeding to download any of AeonLabs software solutions for open-source development
 and/or PCB hardware electronics development make sure you are choosing the right license for your project. See 
https://github.com/aeonSolutions/PCB-Prototyping-Catalogue/wiki/AeonLabs-Solutions-for-Open-Hardware-&-Source-Development
 for Open Hardware & Source Development for more information.

*/

#include <WiFi.h>               // For ESP32 WiFi
#include <WiFiClientSecure.h>    // For HTTPS connections
#include <ArduinoJson.h>         // For parsing JSON response
#include "Base64.h"              // For Base64 decoding

// WiFi credentials
const char* ssid = "ssid";
const char* password = "password";

// GitHub API and repository information
const char* token = "token";  // GitHub PAT (with repo scope)
const char* repoOwner = "aeonSolutions";      // GitHub username or organization
const char* repoName = "AeonLabs-Safety-Health";     // Repository name
const char* filePath = "Measurements/datasets/dataset.csv";   // Path to the file in the repo (e.g., "README.md")
const char* branch = "main";             // Branch name (e.g., main or master)

    // Branch name (e.g., main or master)

WiFiClientSecure client;

// Function to get the SHA of a file from GitHub
void getFileSHAFromGitHub() {
  // Create the URL for the file in the repository
  String url = "/repos/" + String(repoOwner) + "/" + String(repoName) + "/contents/" + String(filePath) + "?ref=" + String(branch);
  // Make sure to trust the GitHub SSL certificate
  client.setInsecure();  // Bypasses SSL certificate verification
  // Connect to GitHub
  if (client.connect("api.github.com", 443)) {
    Serial.println("Connected to GitHub");

    // Send the GET request
    client.println("GET " + url + " HTTP/1.1");
    client.println("Host: api.github.com");
    client.print("Authorization: token ");
    client.println(token);                 // Send the GitHub PAT for authorization
    client.println("User-Agent: ESP32");
    client.println("Connection: close");
    client.println();

    // Wait for the response
    while (client.connected()) {
      String line = client.readStringUntil('\n');
      if (line == "\r") {
        break;  // End of headers
      }
    }

    // Read the response body
    String jsonResponse = client.readString();
    Serial.println("Received JSON response:");

    // Print the JSON response for debugging
    Serial.println(jsonResponse);

    // Parse the JSON response
    StaticJsonDocument<4000> doc;
    DeserializationError error = deserializeJson(doc, jsonResponse);
    if (error) {
      Serial.print("Failed to parse JSON: ");
      Serial.println(error.c_str());
      return;
    }

    // Get the SHA of the file
    const char* sha = doc["sha"];
    if (sha) {
      Serial.print("File SHA: ");
      Serial.println(sha);
    } else {
      Serial.println("SHA not found in the response.");
    }
  } else {
    Serial.println("Connection to GitHub failed");
  }
}

void setup() {
  Serial.begin(115200);  // Start Serial communication

  // Connect to WiFi
  WiFi.begin(ssid, password);
  WiFi.setTxPower( WIFI_POWER_11dBm );
  Serial.println("Connecting to WiFi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("\nConnected to WiFi");

  // Fetch the SHA of the file from GitHub

}

void loop() {
  // Nothing to do in the loop
    getFileSHAFromGitHub();
    delay(5000);
}

