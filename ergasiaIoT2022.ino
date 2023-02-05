#include <ThingerESP32.h> //εδω κανουμε include ολες τις απαραιτητες βιβλιοθηκες για την ευρυθμη λειτουργια του προγραμματος
#include "DHT.h"
#include <LiquidCrystal_I2C.h>
#include <WiFi.h>

#define LED_OUT 10
#define USERNAME "" //Εδω βαζουμε το ονομα χρηστη στην πλατφορμα Thinger.io
#define DEVICE_ID "" //Εδω γραφουμε τις πληροφοριες της πλακετας οπως την περασαμε στο Thinger.io
#define DEVICE_CREDENTIAL "" //Eδω γραφουμε τα credentials της πλακετας οπως τα βαλαμε στην αρχικη εγγραφη στην πλατφορμα Thinger.io


#define SSID "" //Εδω βαζουμε το ονομα του wifi απο το οποιο ειμαστε συνδεδεμενοι 
#define SSID_PASSWORD "" //Τοποθετουμε τον κωδικο απο το δικτυο wifi ωστε να συνδεθει η πλακετα 

#define DHTPIN 4     // εδω συνδεομαστε με το πιν στο οποιο ειναι συνδεδεμενος ο αισθητηρας (D4)
#define DHTTYPE DHT11   // εδω δινουμε τον τυπο του αισθητηρα που χρησιμοποιουμε (DHT 11)

// οριζουμε τον αριθμο των στηλων κι των γραμμων της οθονης που χρησιμοποιουμε
int lcdColumns = 16; //set the LCD number of columns
int lcdRows = 2; //set the lcd number of rows
int SensorValue[30];
int j;
long result;
// set LCD address, number of columns and rows
// if you don't know your display address, run an I2C scanner sketch
LiquidCrystal_I2C lcd(0x27, lcdColumns, lcdRows);

DHT dht(DHTPIN, DHTTYPE); //declare the DHT PIN, WITH THE TYPE (INITIALIZE THE DHT11 SENSOR)
ThingerESP32 thing(USERNAME, DEVICE_ID, DEVICE_CREDENTIAL); // εδω δημιουργουμε συνδεση αναμεσα στην πλακετα ESP32 κι στην πλατφορμα Thinger.io

void setup() {
  // open serial for debugging
  Serial.begin(115200); //intialize the serial baud rate to 115200 bauds/sec


  thing.add_wifi(SSID, SSID_PASSWORD); //establish the Wi-Fi connection
  
  pinMode(18,OUTPUT);
  pinMode(19, OUTPUT);

  lcd.init(); //εντολη να αναψει η οθονη LCD                     
  lcd.backlight();  // turn on LCD backlight 
  lcd.setCursor(0, 0);
  lcd.print("Your connection");
  lcd.setCursor(0, 1);
  lcd.print("was successful!");
  delay(2500); // //delay 10 seconds before clearing the lcd display (taking another measurement)
  lcd.clear();
  dht.begin(); // εδω ο αισθητηρας ξεκιναει να παιρνει μετρησεις 
  pinMode(LED_OUT,OUTPUT);
  // digital pin control example (i.e. turning on/off a light, a relay, configuring a parameter, etc)
  thing["led"] << digitalPin(LED_OUT);

  // resource output example (i.e. reading a sensor value)
  thing["millis"] >> outputValue(millis());

}
//float humidity;//edo ξεκιανω την μετρησεις υγρασ
void loop() {
  //delay(1000);   // Wait a second between measurements.
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();
  float h = dht.readHumidity();
 
  thing.handle(); //handle the connection
  thing["VALUES:"] >> [t ,h,result](pson& out){ //output into the Thinger.io API the sensor values //
  out["TEMPERATURE"] = t; //output the real values    }; //output into the Thinger.i
  out["HUMIDITY"] = h;
  out["avg_temp"] = result;
  };
  
  //delay(1000); //wait for 1 second before running again!
  if( t  >= 24.5){      
    digitalWrite(18, HIGH); 
    delay(50);            
    digitalWrite(18, LOW);  
    delay(50);               
    //digitalWrite(19, HIGH); 
    //delay(50);              
    //digitalWrite(19, LOW);  
    //delay(50);                
    
  }
  else if( t < 24.5 ){

    //digitalWrite(18, LOW);
    digitalWrite(19, HIGH);
    delay(50);
    digitalWrite(19,LOW);
    delay(50);
  
  }
  // Print a message into the Thinger.io API Console
  // REALVALUE = (t);

  // Check if any reads failed and exit early (to try again).
  if (isnan(t) || isnan(h)) { //if the value reading failed
    Serial.println(F("Failed to read from DHT sensor!")); //then print the right message into the Serial Port 
    lcd.setCursor(0, 0); //also set the cursor to first column, first row 
    lcd.print("FAILURE"); //print the right message into the LCD Display
    return; 
  }
  
  
  for (j = 0; j < 30; j++) {
    SensorValue[j] = dht.readTemperature();
  }

  // we now have 10 readings, so average them
  result = 0;
  for (j = 0; j < 30; j++) {
    result = result + SensorValue[j]; // add them up
  }
  result = result / 30;      

  // set cursor to first column, first row
  lcd.setCursor(0, 0);
  lcd.print(t);   // print the temperature
  lcd.setCursor(0,1); //set the cursor to the first column, second row
  lcd.print("Celsius"); //print the right message into the Serial port
    // clears the display to print new message
  Serial.print(F("%  Temperature: "));
  Serial.print(t);
  delay(1000);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(h);  
  lcd.setCursor(0, 1);
  lcd.print("Humidity");
  Serial.print(F("°C "));
  Serial.print(F("Humidity: "));
  Serial.print(h);
    // clears the display to print new message
  delay(1000); //DELAY 1000 1 SEC//delay 10 seconds before clearing the lcd display (taking another measurement)
  lcd.clear(); //clear the LCD Display
}

