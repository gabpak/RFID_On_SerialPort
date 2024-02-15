
#include <SPI.h>
#include <MFRC522.h>

#define RST_PIN        9          // Configurable, see typical pin layout above
#define SS_PIN         10         // Configurable, see typical pin layout above

#define LED_ACCES      3 // D2
#define LED_DENIED     2 // D3

MFRC522 mfrc522(SS_PIN, RST_PIN);  // Create MFRC522 instance
int cardUID[4];
int password[4] = {0xE0, 0x2C, 0x62, 0x19};

void setup() {
	Serial.begin(9600);		// Initialize serial communications with the PC
  // Output led
  pinMode(LED_ACCES, OUTPUT);
  pinMode(LED_DENIED, OUTPUT);

	while (!Serial);		// Do nothing if no serial port is opened (added for Arduinos based on ATMEGA32U4)
	SPI.begin();			// Init SPI bus
	mfrc522.PCD_Init();		// Init MFRC522
	delay(4);				// Optional delay. Some board do need more time after init to be ready, see Readme
}

void loop() {
	// Reset the loop if no new card present on the sensor/reader. This saves the entire process when idle.
	if ( ! mfrc522.PICC_IsNewCardPresent()) {
		return;
	}

	// Select one of the cards
	if ( ! mfrc522.PICC_ReadCardSerial()) {
		return;
	}

  // Only get the UID
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
    digitalWrite(LED_ACCES, HIGH);
  } else {
    Serial.print("Access denied\n");
    digitalWrite(LED_DENIED, HIGH);
  }

  Serial.print("\n");
  delay(1000); // wait for a second

  // Reset LED
  digitalWrite(LED_ACCES, LOW);
  digitalWrite(LED_DENIED, LOW);

}
