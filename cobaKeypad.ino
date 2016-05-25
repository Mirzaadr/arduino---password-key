#include <Keypad.h>
#include <LiquidCrystal.h>
#include <EEPROM.h>
#include <SPI.h>
#include <Ethernet.h>
#include <Servo.h>

const byte numRows= 4; //number of rows on the keypad
const byte numCols= 4; //number of columns on the keypad
int cursorx = 0;
char keypressed;
int clearPin = 50;
int deletePin = 52;
int clearState = 0;
int deleteState = 0;
int clearLastState = 0;
int deleteLastState = 0;

unsigned long previousMillis = 0;

const long intervalRefresh = 1000 * 60 * 5;
const long intervalPassword = 1000 * 60 * 60;

boolean conn = false;

int addressUlang = 0;
int addressPass = 1;
int randomVal = 1;
int salah = 0;

char key[6];

Servo myServo;


//keymap defines the key pressed according to the row and columns just as appears on the keypad
char keymap[numRows][numCols]= 
{
{'1', '2', '3', 'A'}, 
{'4', '5', '6', 'B'}, 
{'7', '8', '9', 'C'},
{'*', '0', 'E', 'D'}
};

char keys[]= 
{'1', '2', '3', 'A', 
 '4', '5', '6', 'B', 
 '7', '8', '9', 'C',
 '*', '0', 'E', 'D'};
const byte keyLength = sizeof(keys)/sizeof(keys[0]);
const int passLen = 6;
char pass[passLen+1];


//Code that shows the the keypad connections to the arduino terminals
byte rowPins[numRows] = {22, 24, 26, 28}; //Rows 0 to 3
byte colPins[numCols]= {30, 32, 34, 36}; //Columns 0 to 3
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };

char server[] = "patpatstudio.com";
IPAddress ip(192, 168, 0, 177);

//initializes an instance of the Keypad class
Keypad myKeypad= Keypad(makeKeymap(keymap), rowPins, colPins, numRows, numCols);
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

EthernetClient client;
String password;

void setup()
{
  lcd.begin(16, 2);
  lcd.print("Connecting . . .");
  myServo.attach(8);  
  Serial.begin(9600);
  // Print a message to the LCD.

  randomVal = EEPROM.read(addressUlang);
  Serial.println("val = " + String(randomVal));
  for(int x=0; x < randomVal ; x++)
    {
  for (int n=0; n < passLen; n++){
    pass[n] = keys[random(0, keyLength)];
  }
    }
    
  for (int i = 0; i < passLen; i++){
    Serial.print(String(pass[i]));
    password +=  pass[i];
  }

  Serial.println();

  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  // start the Ethernet connection:
  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
    // try to congifure using IP address instead of DHCP:
    Ethernet.begin(mac, ip);
  }
  else {
    delay(1000);
    Serial.println("connected");
    lcd.clear();
    lcd.print("Masukkan pass :");
    conn = true;
    client.connect(server, 80);
    client.println("GET http://patpatstudio.com/mikro/keamanan.php?kode=arduino&pass=" + password);
    Serial.println("GET http://patpatstudio.com/mikro/keamanan.php?kode=arduino&pass=" + password);
    client.println();
    client.stop();
  }
    
}

void loop()
{
  
  if(!conn) {

  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
    // try to congifure using IP address instead of DHCP:
    lcd.clear();
    lcd.print("Connecting . . .");
    conn = false;
    Ethernet.begin(mac, ip);
  }
  else {
    delay(1000);
    lcd.clear();
    lcd.print("Masukkan pass :");
    Serial.println("connected");
    conn = true;
  }
    
    
  }
  
  else{
    
    unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= intervalRefresh) {
    // save the last time you blinked the LED
    previousMillis = currentMillis;
    client.connect(server, 80);
    client.println("GET http://patpatstudio.com/mikro/keamanan.php?kode=haha&pass=865622029775033");
    client.println();
    client.stop();
  }
  
  if (currentMillis - previousMillis >= intervalPassword) {
    // save the last time you blinked the LED
    previousMillis = currentMillis;
    randomPass();
    EEPROM.write(addressUlang, randomVal);
    client.connect(server, 80);
    client.println("GET http://patpatstudio.com/mikro/keamanan.php?kode=arduino&pass=" + password);
    Serial.println("GET http://patpatstudio.com/mikro/keamanan.php?kode=arduino&pass=" + password);
    client.println();
    client.stop();
  }
                  
  
keypressed = myKeypad.getKey();

int clearReading = digitalRead(clearPin);
int deleteReading = digitalRead(deletePin);
if(clearReading != clearState && clearReading != clearLastState) {

  clearState = clearReading;
  clearState = digitalRead(clearPin);

  if (clearState == HIGH) {
        lcdReset();
  }

 clearState = 0;
  
}

if(deleteReading != deleteState && deleteReading != deleteLastState && cursorx != 0) {

  deleteState = deleteReading;
  deleteState = digitalRead(deletePin);

  if (deleteState == HIGH) {
     cursorx--;
     key[cursorx] = keypressed;
     lcd.setCursor(cursorx, 1);
     lcd.print(' ');
  }
  
  deleteState = 0;
}

if (keypressed != NO_KEY)
{
  if(cursorx < 5)
  {   
    lcdPrint();
    tone(25, 16.5, 500);
    cursorx++;   
  }
  else
  {
    lcdPrint();
    Serial.print(String(key[0]) + String(key[1]) + String(key[2]) + String(key[3]) + String(key[4]) + String(key[5]) + "\n");
    if(array_cmp(key, pass) == true){
      Serial.print("Password sesuai\n");
      myServo.write(180);
      randomPass();
      EEPROM.write(addressUlang, randomVal);
              if (client.connect(server, 80)) {
                  Serial.println("connected");
                  // Make a HTTP request:
                  client.println("GET http://patpatstudio.com/mikro/keamanan.php?kode=arduino&pass=" + password);
                  Serial.println("GET http://patpatstudio.com/mikro/keamanan.php?kode=arduino&pass=" + password);
                  client.println();
                  client.stop();
                } else {
                  // if you didn't get a connection to the server:
                  Serial.println("connection failed");
                  conn = false;
                }
      salah = 0;
    } else {
      Serial.print("Password tidak sesuai\n");
      salah++;
      if (salah == 3) {
        myServo.write(90);
        randomPass();
        EEPROM.write(addressUlang, randomVal);
        if (client.connect(server, 80)) {
              Serial.println("connected");
              // Make a HTTP request:
              client.println("GET http://patpatstudio.com/mikro/keamanan.php?kode=arduino&pass=" + password);
              Serial.println("GET http://patpatstudio.com/mikro/keamanan.php?kode=arduino&pass=" + password);
              client.println();
              client.stop();
            } else {
              // if you didn't get a connection to the server:
              Serial.println("connection failed");
              conn = false;
            }
        salah = 0;  
      }
      
    }
    delay(1000);
    lcdReset();
  }
}

  clearLastState = clearReading;  
  deleteLastState = deleteReading;

}  
}

void lcdPrint(){
    key[cursorx] = keypressed;
    lcd.setCursor(cursorx, 1);
    lcd.print(keypressed);
  }
  
void lcdReset(){
    lcd.clear();
    cursorx = 0;
    clearState = 0;
    deleteState = 0;
    clearLastState = 0;
    deleteLastState = 0;
    lcd.print("Masukkan pass :");
    lcd.setCursor(0, 1);
  }

void randomPass(){
  
  password = "";

  for (int n=0; n < passLen; n++){
    pass[n] = keys[random(0, keyLength)];
  }
    
  for (int i = 0; i < passLen; i++){
    password +=  pass[i];
  }
  Serial.print(password);
  randomVal++;
  Serial.println("\nval = " + String(randomVal));
}

boolean array_cmp(char *a, char*b){
  for(int n = 0; n < passLen; n++){
    if(a[n] != b[n]){
    return false;
    }
  }
  return true;
}

