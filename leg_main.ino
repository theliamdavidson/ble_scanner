/*
  Battery Monitor

  This example creates a Bluetooth® Low Energy peripheral with the standard battery service and
  level characteristic. The A0 pin is used to calculate the battery level.

  The circuit:
  - Arduino MKR WiFi 1010, Arduino Uno WiFi Rev2 board, Arduino Nano 33 IoT,
    Arduino Nano 33 BLE, or Arduino Nano 33 BLE Sense board.

  You can use a generic Bluetooth® Low Energy central app, like LightBlue (iOS and Android) or
  nRF Connect (Android), to interact with the services and characteristics
  created in this sketch.

  This example code is in the public domain.
*/

#include <ArduinoBLE.h>

 // Bluetooth® Low Energy Battery Service
BLEService leg_values("ABCD");

// Bluetooth® Low Energy Battery Level Characteristic
BLEUnsignedCharCharacteristic FSR1("50000000-0000-0000-0000-efb2643ccd5a",  // standard 16-bit characteristic UUID
    BLERead | BLENotify); // remote clients will be able to get notifications if this characteristic changes
BLEUnsignedCharCharacteristic FSR2("50000001-0000-0000-0000-efb2643ccd5a",  // standard 16-bit characteristic UUID
    BLERead | BLENotify);

int oldvalue1 = 0;  // last reading from analog input
int oldvalue2 = 0;  // last reading from analog input
int oldvalue3 = 0;  // last reading from analog input
int oldvalue4 = 0;  // last reading from analog input

long previousMillis = 0;  // last time the battery level was checked, in ms

void setup() {
  Serial.begin(9600);    // initialize serial communication
  while (!Serial);

  pinMode(LED_BUILTIN, OUTPUT); // initialize the built-in LED pin to indicate when a central is connected

  // begin initialization
  if (!BLE.begin()) {
    Serial.println("starting BLE failed!");

    while (1);
  }

  /* Set a local name for the Bluetooth® Low Energy device
     This name will appear in advertising packets
     and can be used by remote devices to identify this Bluetooth® Low Energy device
     The name can be changed but maybe be truncated based on space left in advertisement packet
  */
  BLE.setLocalName("leg");
  BLE.setAdvertisedService(leg_values); // add the service UUID
  leg_values.addCharacteristic(FSR1); // add the battery level characteristic
  leg_values.addCharacteristic(FSR2); // add the battery level characteristic
  BLE.addService(leg_values); // Add the battery service
  FSR1.writeValue(oldvalue1); // set initial value for this characteristic
  FSR2.writeValue(oldvalue2); 

  /* Start advertising Bluetooth® Low Energy.  It will start continuously transmitting Bluetooth® Low Energy
     advertising packets and will be visible to remote Bluetooth® Low Energy central devices
     until it receives a new connection */

  // start advertising
  BLE.advertise();

  Serial.println("Bluetooth® device active, waiting for connections...");
}

void loop() {
  // wait for a Bluetooth® Low Energy central
  BLEDevice central = BLE.central();

  // if a central is connected to the peripheral:
  if (central) {
    Serial.print("Connected to central: ");
    // print the central's BT address:
    Serial.println(central.address());
    // turn on the LED to indicate the connection:
    digitalWrite(LED_BUILTIN, HIGH);

    // check the battery level every 200ms
    // while the central is connected:
    while (central.connected()) {
      long currentMillis = millis();
      // if 200ms have passed, check the battery level:
      if (currentMillis - previousMillis >= 200) {
        previousMillis = currentMillis;
        updateFSRs();
      }
    }
    // when the central disconnects, turn off the LED:
    digitalWrite(LED_BUILTIN, LOW);
    Serial.print("Disconnected from central: ");
    Serial.println(central.address());
  }
}

void updateFSRs() {
  /* 
    Read the current voltage level on the analog input pins.
  */
  int fsr1 = analogRead(A0);
  int fsr1mapped = map(fsr1, 0, 1023, 0, 100);

  if (fsr1mapped != oldvalue1) {      // if the battery level has changed
    Serial.print(fsr1mapped);
    Serial.print(" ");
    FSR1.writeValue(fsr1mapped);  // and update the battery level characteristic
    oldvalue1 = fsr1mapped;           // save the level for next comparison
  }

  int fsr2 = analogRead(A1);
  int fsr2mapped = map(fsr2, 0, 1023, 0, 100);

  if (fsr2mapped != oldvalue2) {      // if the battery level has changed
    Serial.println(fsr2mapped);
    FSR2.writeValue(fsr2mapped);  // and update the battery level characteristic
    oldvalue2 = fsr2mapped;           // save the level for next comparison
  }
}
