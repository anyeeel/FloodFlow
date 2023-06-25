#if defined(ESP32)
#include <WiFi.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#endif
#include <Firebase_ESP_Client.h>
#include <addons/TokenHelper.h>
#include <WiFiUdp.h>
#include <NTPClient.h>

// Define NTP Client to get time (All of this is needed to have a correct timestamp format)
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org");

const int triggerPin = D7;  // Trigger pin of the ultrasonic sensor
const int echoPin = D6;     // Echo pin of the ultrasonic sensor

const char* ssid = "B310_74CBE";
const char* password = "4AETY8T3BQR";

// Firebase configuration
#define FIREBASE_API_KEY "AIzaSyDCZAR5sLXy_H2bwkwSAJ0g2lgiR0MuwlI"
#define FIREBASE_PROJECT_ID "ababonbabantocs2"

#define USER_EMAIL "admin1@gmail.com"
#define USER_PASSWORD "123456"

//Define Firebase Data object
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

String uid;

String path = "FloodFlow";  // set the Firestore collection id here

void setup() {

  Serial.begin(115200);
  pinMode(triggerPin, OUTPUT);
  pinMode(echoPin, INPUT);

  timeClient.begin();

  WiFi.begin(ssid, password);
  Serial.println("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.println(".");
    delay(3000);
  }
  Serial.println("");
  Serial.print("WiFi connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  Serial.printf("Firebase Client v%s\n\n", FIREBASE_CLIENT_VERSION);

  delay(3000);  // delay start for 3 seconds

  config.api_key = FIREBASE_API_KEY;

  /* Assign the user sign in credentials */
  auth.user.email = "admin1@gmail.com";
  auth.user.password = "123456";

  config.token_status_callback = tokenStatusCallback;  //see addons/TokenHelper.h

  Firebase.begin(&config, &auth);

  Firebase.reconnectWiFi(true);

  delay(3000);  // delay start for 3 seconds

  //----------------------------------------------
  // Getting the user UID might take a few seconds
  //-----------------------------------------------
  Serial.println("Getting User UID");
  while ((auth.token.uid) == "") {
    Serial.print('.');
    delay(1000);
  }
  //-----------------
  // Print user UID
  //------------------
  uid = auth.token.uid.c_str();
  Serial.print("User UID: ");
  Serial.println(uid);
}

void loop() {
  FirebaseJson content;

// Trigger the ultrasonic sensor
  digitalWrite(triggerPin, LOW);
  delayMicroseconds(2);
  digitalWrite(triggerPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(triggerPin, LOW);

  // Measure the duration of the pulse
  long duration = pulseIn(echoPin, HIGH);

  // Calculate the distance in centimeters
  float distance = duration * 0.0343 / 2;
  float waterLevel = 25  - distance;  // 25 estimated container length
float previousDistance = 0;  // Variable to store the previous distance value
  // Check if the distance has changed
  if (distance != previousDistance) {
    // Print the distance
    Serial.print("Distance: ");
    Serial.print(distance);
    Serial.println(" cm");

    // Print the water level
    Serial.print("Water Level: ");
    Serial.print(waterLevel);
    Serial.println(" cm");

    // Determine the status
    String status;
    if (waterLevel <=8){
      status = "Normal";
    } else if (waterLevel <= 12) {
      status = "Warning";
    } else if (waterLevel <= 17) {
      status = "Evacuate";
    } else {
      status = "Danger";
    }

    // Print the status
    Serial.print("Status: ");
    Serial.println(status);

    // Update the previous distance value
    previousDistance = distance;

    content.set("fields/WaterLevel/doubleValue", double(waterLevel));
  content.set("fields/Status/stringValue", (status));

  // Get current date and time
  timeClient.update();
  time_t now = timeClient.getEpochTime();

  if (now == 0) {
    Serial.println("Error getting current time");
  } else {
    // Set timestamp field with current time
    char timestampStr[30];
    strftime(timestampStr, sizeof(timestampStr), "%FT%TZ", gmtime(&now));
    content.set("fields/DateTime/timestampValue", timestampStr);
  }

 // Randomize Document ID
  String randomStr = "";
  const char* chars = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";

  for (int i = 0; i < 20; i++) {
    randomStr += chars[random(0, 61)];
  }
  uid = randomStr + String(millis());
 
  // Create and Send Document to Firestore
  Serial.print("Creating document... ");
  delay(3000);  // delay start for 3 seconds
  Serial.println("Done");
  delay(3000);  // delay start for 3 seconds
  Serial.println("");
  Serial.print("Sending document.... ");

}

  // Creates the Document and Send to Firestore
  if (Firebase.Firestore.createDocument(&fbdo, FIREBASE_PROJECT_ID, "", path.c_str(), content.raw())) {
    Serial.println("Success");
    Serial.println(fbdo.payload().c_str());
  } else {
    Serial.print("Failed, Reason: ");
    Serial.println(fbdo.errorReason());
  }

  delay(10000);  // 10 secs delay
}
