#include <WiFi.h>          // For ESP32 (or use <ESP8266WiFi.h> for ESP8266)
#include <HTTPClient.h>
#include <SD.h>            // SD card library (optional, if reading from SD card)
#include <SPI.h>
#include <Base64.h>        // Base64 library (optional, for encoding chunks if necessary)

// Replace with your credentials
const char* ssid = "your_SSID";
const char* password = "your_PASSWORD";

// Replace with your GitHub credentials and repository details
const char* githubToken = "your_github_token";  // GitHub personal access token
const char* githubUser = "your_username";
const char* githubRepo = "your_repo";
const char* githubBranch = "main";  // Branch where you want to commit
const char* filePath = "/path/in/repo/filename.txt";  // Path in the repo to upload the file

// SD card setup
const int chipSelect = 5;  // SD card CS pin (adjust accordingly)
File fileToSend;

String latestCommitSHA;
String latestTreeSHA;

void setup() {
  Serial.begin(115200);
  
  // Connect to WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  // Initialize SD card
  if (!SD.begin(chipSelect)) {
    Serial.println("SD Card initialization failed!");
    return;
  }

  // Open the file for reading
  fileToSend = SD.open("/testfile.txt");
  if (!fileToSend) {
    Serial.println("Failed to open file");
    return;
  }

  // Start the process: Get latest commit SHA
  getLatestCommitSHA();
}

void getLatestCommitSHA() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    String url = "https://api.github.com/repos/" + String(githubUser) + "/" + String(githubRepo) + "/git/refs/heads/" + String(githubBranch);

    http.begin(url);
    http.addHeader("Authorization", "Bearer " + String(githubToken));
    http.addHeader("User-Agent", "Arduino");

    int httpResponseCode = http.GET();
    if (httpResponseCode > 0) {
      String response = http.getString();
      Serial.println("Latest commit response: " + response);

      int shaIndex = response.indexOf("\"sha\":\"") + 7;
      latestCommitSHA = response.substring(shaIndex, response.indexOf("\"", shaIndex));

      // Get latest tree SHA after retrieving the commit SHA
      getLatestTreeSHA();
    } else {
      Serial.println("Error getting latest commit SHA: " + http.errorToString(httpResponseCode));
    }
    http.end();
  }
}

void getLatestTreeSHA() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    String url = "https://api.github.com/repos/" + String(githubUser) + "/" + String(githubRepo) + "/git/commits/" + latestCommitSHA;

    http.begin(url);
    http.addHeader("Authorization", "Bearer " + String(githubToken));
    http.addHeader("User-Agent", "Arduino");

    int httpResponseCode = http.GET();
    if (httpResponseCode > 0) {
      String response = http.getString();
      Serial.println("Latest tree response: " + response);

      int shaIndex = response.indexOf("\"tree\":{\"sha\":\"") + 14;
      latestTreeSHA = response.substring(shaIndex, response.indexOf("\"", shaIndex));

      // Start uploading the file
      uploadFileInChunks();
    } else {
      Serial.println("Error getting latest tree SHA: " + http.errorToString(httpResponseCode));
    }
    http.end();
  }
}

void uploadFileInChunks() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    String url = "https://api.github.com/repos/" + String(githubUser) + "/" + String(githubRepo) + "/git/blobs";

    http.begin(url);
    http.addHeader("Authorization", "Bearer " + String(githubToken));
    http.addHeader("Content-Type", "application/json");
    http.addHeader("User-Agent", "Arduino");

    const size_t chunkSize = 512;
    uint8_t buffer[chunkSize];
    String encodedChunk = "";
    String blobSHA = "";
    String fileContent = "";

    // Read the file in chunks and create a blob
    while (fileToSend.available()) {
      size_t bytesRead = fileToSend.read(buffer, chunkSize);
      encodedChunk = base64::encode(buffer, bytesRead);

      // Add chunk to file content
      fileContent += encodedChunk;
    }
    fileToSend.close();

    // Create blob with the full base64 encoded content
    String payload = "{ \"content\": \"" + fileContent + "\", \"encoding\": \"base64\" }";
    int httpResponseCode = http.POST(payload);

    if (httpResponseCode > 0) {
      String response = http.getString();
      Serial.println("Blob creation response: " + response);

      int shaIndex = response.indexOf("\"sha\":\"") + 7;
      blobSHA = response.substring(shaIndex, response.indexOf("\"", shaIndex));

      // Create a new tree entry with the file blob
      createTree(blobSHA);
    } else {
      Serial.println("Error creating blob: " + http.errorToString(httpResponseCode));
    }
    http.end();
  }
}

void createTree(String blobSHA) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    String url = "https://api.github.com/repos/" + String(githubUser) + "/" + String(githubRepo) + "/git/trees";

    http.begin(url);
    http.addHeader("Authorization", "Bearer " + String(githubToken));
    http.addHeader("Content-Type", "application/json");
    http.addHeader("User-Agent", "Arduino");

    // Create tree payload with the blob SHA
    String payload = "{"
                     "\"base_tree\": \"" + latestTreeSHA + "\","
                     "\"tree\": ["
                     "{ \"path\": \"" + filePath + "\", \"mode\": \"100644\", \"type\": \"blob\", \"sha\": \"" + blobSHA + "\" }"
                     "]"
                     "}";
    int httpResponseCode = http.POST(payload);

    if (httpResponseCode > 0) {
      String response = http.getString();
      Serial.println("Tree creation response: " + response);

      int shaIndex = response.indexOf("\"sha\":\"") + 7;
      String newTreeSHA = response.substring(shaIndex, response.indexOf("\"", shaIndex));

      // Commit the tree
      createCommit(newTreeSHA);
    } else {
      Serial.println("Error creating tree: " + http.errorToString(httpResponseCode));
    }
    http.end();
  }
}

void createCommit(String newTreeSHA) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    String url = "https://api.github.com/repos/" + String(githubUser) + "/" + String(githubRepo) + "/git/commits";

    http.begin(url);
    http.addHeader("Authorization", "Bearer " + String(githubToken));
    http.addHeader("Content-Type", "application/json");
    http.addHeader("User-Agent", "Arduino");

    // Create commit payload
    String commitMessage = "Upload file to GitHub from Arduino";
    String payload = "{"
                     "\"message\": \"" + commitMessage + "\","
                     "\"tree\": \"" + newTreeSHA + "\","
                     "\"parents\": [\"" + latestCommitSHA + "\"]"
                     "}";

    int httpResponseCode = http.POST(payload);

    if (httpResponseCode > 0) {
      String response = http.getString();
      Serial.println("Commit creation response: " + response);

      int shaIndex = response.indexOf("\"sha\":\"") + 7;
      String newCommitSHA = response.substring(shaIndex, response.indexOf("\"", shaIndex));

      // Update the reference (branch) to point to the new commit
      updateReference(newCommitSHA);
    } else {
      Serial.println("Error creating commit: " + http.errorToString(httpResponseCode));
    }
    http.end();
  }
}

void updateReference(String newCommitSHA) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    String url = "https://api.github.com/repos/" + String(githubUser) + "/" + String(githubRepo) + "/git/refs/heads/" + String(githubBranch);

    http.begin(url);
    http.addHeader("Authorization", "Bearer " + String(githubToken));
    http.addHeader("Content-Type", "application/json");
    http.addHeader("User-Agent", "Arduino");

    // Update ref payload
    String payload = "{ \"sha\": \"" + newCommitSHA + "\", \"force\": true }";

    int httpResponseCode = http.PATCH(payload);

    if (httpResponseCode > 0) {
      String response = http.getString();
      Serial.println("Reference update response: " + response);
    } else {
      Serial.println("Error updating reference: " + http.errorToString(httpResponseCode));
    }
    http.end();
  }
}

void loop() {
  // Nothing in loop
}
