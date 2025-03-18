#define BLYNK_TEMPLATE_ID "TMPL3J3eOvLYK"
#define BLYNK_TEMPLATE_NAME "Irrigation monitoring system"
#define BLYNK_AUTH_TOKEN "jMq298jXkcvOapK5ZKEBt5opiTTgU2wq"

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

#define BLYNK_PRINT Serial

LiquidCrystal_I2C lcd(0x27, 16, 2);  // LCD 16 columns, 2 rows

char ssid[] = "Ramu";      // Enter your Wi-Fi name
char pass[] = "12345678";  // Enter your Wi-Fi password

BlynkTimer timer;

// Crop recommendation function (updated for every 10% interval)
String getCropRecommendation(int moisturePercentage) {
  if (moisturePercentage < 10) {
    return "Crops: Cactus, Aloe";
  } else if (moisturePercentage < 20) {
    return "Crops: Cactus, Succulent";
  } else if (moisturePercentage < 30) {
    return "Crops: Corn, Wheat";
  } else if (moisturePercentage < 40) {
    return "Crops: Barley, Lentils";
  } else if (moisturePercentage < 50) {
    return "Crops: Beans, Peas";
  } else if (moisturePercentage < 60) {
    return "Crops: Potatoes, Tomatoes";
  } else if (moisturePercentage < 70) {
    return "Crops: Lettuce, Spinach";
  } else if (moisturePercentage < 80) {
    return "Crops: Rice, Celery";
  } else if (moisturePercentage < 90) {
    return "Crops: Watercress, Spinach";
  } else {
    return "Crops: Wetland Plants";
  }
}

// Function to scroll long text on the LCD
void scrollText(String message, int row) {
  int messageLength = message.length();
  if (messageLength <= 16) {
    lcd.setCursor(0, row);
    lcd.print(message);  // No need to scroll if text fits
  } else {
    for (int i = 0; i < messageLength - 15; i++) {
      lcd.setCursor(0, row);
      lcd.print(message.substring(i, i + 16));  // Scroll text
      delay(500);  // Adjust scrolling speed
    }
  }
}

void displayInitialMoisture() {
  // Initial moisture level set to 0 before sensor readings
  int initialMoisture = 0;
  Blynk.virtualWrite(V1, initialMoisture);  // Send initial 0% to Blynk

  // Display on LCD
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Moisture: 0 %");

  // Initial crop recommendation based on 0 moisture
  String initialCropRecommendation = getCropRecommendation(initialMoisture);

  // Scroll the recommendation if it's too long
  scrollText(initialCropRecommendation, 1);

  // Send crop recommendation to Blynk
  Blynk.virtualWrite(V2, initialCropRecommendation);

  Serial.println("Initial Moisture: 0%");
  Serial.println(initialCropRecommendation);
}

void moisture() {
  // Read the analog input (A0) for soil moisture
  int value = analogRead(A0);  
  int moisturePercentage = map(value, 0, 1023, 0, 100);  // Convert to percentage (0 to 100)

  // Send moisture percentage to Blynk app
  Blynk.virtualWrite(V1, moisturePercentage);  

  // Crop recommendation based on sensor readings
  String cropRecommendation = getCropRecommendation(moisturePercentage);

  // Update LCD display with the sensor readings
  lcd.clear();  // Clear the display
  lcd.setCursor(0, 0);
  lcd.print("Moisture: ");
  lcd.print(moisturePercentage);
  lcd.print(" %");  // Display moisture percentage on LCD

  // Scroll the crop recommendation if it's too long for the LCD
  scrollText(cropRecommendation, 1);

  // Send crop recommendation to Blynk
  Blynk.virtualWrite(V2, cropRecommendation);  // Send crop recommendation to Blynk app

  // Print to Serial Monitor (for debugging purposes)
  Serial.println(moisturePercentage);
  Serial.println(cropRecommendation);
}

void setup() {
  Serial.begin(9600);   // Start the Serial communication
  Wire.begin(D2, D1);   // Initialize I2C communication
  lcd.init();           // Initialize the LCD
  lcd.backlight();      // Turn on the LCD backlight
  pinMode(D4, OUTPUT);  // Set D4 as an output pin for additional control (e.g., pump or valve)
  digitalWrite(D4, HIGH);

  // Display initial message on LCD
  lcd.setCursor(0, 0);
  lcd.print("IrrigationSystem");

  // Set initial moisture level to 0 and display it
  displayInitialMoisture();

  // Connect to Wi-Fi and Blynk
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
  timer.setInterval(1000L, moisture);  // Call the moisture function every second
}

void loop() {
  Blynk.run();    // Run the Blynk process
  timer.run();    // Run the Blynk timer
}
