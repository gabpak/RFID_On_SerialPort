#include <SPI.h>
#include <MFRC522.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// RFID
#define RST_PIN        8          // Configurable, see typical pin layout above
#define SS_PIN         10         // Configurable, see typical pin layout above

// LED
#define RGB_LED_RED         5 // D5
#define RGB_LED_GREEN       6 // D6
#define RGB_LED_BLUE        7 // D7

#define LED_NIGHT     9 // D9

const byte COLOR_BLACK = 0b000;
const byte COLOR_RED = 0b100;
const byte COLOR_GREEN = 0b010;
const byte COLOR_BLUE = 0b001;
const byte COLOR_MAGENTA = 0b101;
const byte COLOR_CYAN = 0b011;
const byte COLOR_YELLOW = 0b110;
const byte COLOR_WHITE = 0b111;

// OLED
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET    -1 // Reset pin # (or -1 if sharing Arduino reset pin)

// BUZZER
#define BUZZER_PIN  4  // D4

// LDR
#define LDR_PIN   A0
int ldrValue { 0 };

MFRC522 mfrc522(SS_PIN, RST_PIN);  // Create MFRC522 instance
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

int cardUID[4] = {0x00, 0x00, 0x00, 0x00};
int password[4] = {0xE0, 0x2C, 0x62, 0x19}; // To modify with the database

void displayColor(byte color) {
  digitalWrite(RGB_LED_RED, !bitRead(color, 2));
  digitalWrite(RGB_LED_GREEN, !bitRead(color, 1));
  digitalWrite(RGB_LED_BLUE, !bitRead(color, 0));
}

void setup() {
  // Serial
	Serial.begin(9600);		// Initialize serial communications with the PC

  // ------------------------------------

  // LED
  pinMode(RGB_LED_GREEN, OUTPUT); // Set the LED pin as an output
  pinMode(RGB_LED_RED, OUTPUT); // Set the LED pin as an output
  pinMode(RGB_LED_BLUE, OUTPUT); // Set the LED pin as an output
  displayColor(COLOR_BLACK);

  // ------------------------------------

  // LED NIGHT
  pinMode(LED_NIGHT, OUTPUT); // Set the LDR pin as an input
  // LDR_PIN
  pinMode(LDR_PIN, INPUT); // Set the LDR pin as an input

  // ------------------------------------

  // BUZZER
  pinMode(BUZZER_PIN, OUTPUT); // Set the BUZZER pin as an output

  // RFID
	while (!Serial);		// Do nothing if no serial port is opened (added for Arduinos based on ATMEGA32U4)
	SPI.begin();			// Init SPI bus
	mfrc522.PCD_Init();		// Init MFRC522
	delay(4);

  // OLED
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3C for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
}

// ------------------------------------ LOOP ------------------------------------

void loop() {
	delay(50);
  // LDR
  ldrValue = analogRead(LDR_PIN);
  if(ldrValue < 200){
    digitalWrite(LED_NIGHT, HIGH);
  } else {
    digitalWrite(LED_NIGHT, LOW);
  }
  
  // Reset the loop if no new card present on the sensor/reader. This saves the entire process when idle.
	if (!mfrc522.PICC_IsNewCardPresent()) {
		return;
	}

	// Select one of the cards
	if (!mfrc522.PICC_ReadCardSerial()) {
		return;
	}

  // Get the UID
  for(int i{0}; i < 4; i++){
    cardUID[i] = mfrc522.uid.uidByte[i];
  }

  // Print the UID
  Serial.print("\n");
  Serial.print("Card UID: ");
  for(int i{0}; i < 4; i++){
    Serial.print(cardUID[i], HEX);
  }
  Serial.print("\n");

  if(cardUID[0] == password[0] && cardUID[1] == password[1] && cardUID[2] == password[2] && cardUID[3] == password[3]){
    Serial.print("Access granted\n");
    displayColor(COLOR_GREEN);
    // BUZZER
    digitalWrite(BUZZER_PIN, HIGH);
    delay(100);
    digitalWrite(BUZZER_PIN, LOW);
    delay(900);
  } else {
    Serial.print("Access denied\n");
    displayColor(COLOR_RED);
    // BUZZER
    digitalWrite(BUZZER_PIN, HIGH);
    delay(1000);
    digitalWrite(BUZZER_PIN, LOW);
  }

  // Clear the display buffer.
  display.clearDisplay();

  // Write the UID
  display.setTextSize(1);      // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE); // Draw white text
  display.setCursor(0,0);     // Start at top-left corner
  display.print("Card UID: ");
  
  for(int i{0}; i < 4; i++){
    display.print(cardUID[i], HEX);
  }

  // Display the buffer
  display.display();

  // Reset LED
  displayColor(COLOR_BLACK);
}
