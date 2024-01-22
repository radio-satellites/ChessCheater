#include <WiFi.h>
#include <HTTPClient.h>
#include "Adafruit_ThinkInk.h"
#include <Adafruit_NeoPixel.h>

#define EPD_DC      7 
#define EPD_CS      8
#define EPD_BUSY    -1
#define SRAM_CS     -1 
#define EPD_RESET   6
#define EPD_SPI &SPI // primary SPI

const char* ssid = "SSID";
const char* password = "PASSWORD";

String serverName = "http://192.168.137.1:8080";

ThinkInk_290_Grayscale4_T5 display(EPD_DC, EPD_RESET, EPD_CS, SRAM_CS, EPD_BUSY,
                                   EPD_SPI);

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(4, PIN_NEOPIXEL, NEO_GRB + NEO_KHZ800);                                   

#define COLOR1 EPD_BLACK
#define COLOR2 EPD_LIGHT
#define COLOR3 EPD_DARK                                   

String payload;
char payload_char[60];

char pieces[7] = {'P','R','N','B','Q','K','O'};
char numbers[9] = {'1','2','3','4','5','6','7','8','O'};
char letters[9] = {'a','b','c','d','e','f','g','h','-'};

String other_move = "Pa1";

int pieces_index = 0;
int numbers_index = 0;
int letters_index = 0;
int letters_index_2 = 0;

bool finishedEntry = false;
bool white = true;

void getMove(){
  //Check WiFi connection status
  if(WiFi.status()== WL_CONNECTED){  
      String serverPath = serverName+"/getMove";       
      payload = httpGETRequest(serverPath.c_str());
      Serial.println(payload);
   }
  else {
    Serial.println("WiFi Disconnected");
  }
}

int clearBoard(){
  //Check WiFi connection status
  if(WiFi.status()== WL_CONNECTED){  
      String serverPath = serverName+"/clearBoard";       
      payload = httpGETRequest(serverPath.c_str());
      if (payload != String("OK")){
        String serverPath = serverName+"/clearBoard";       
        payload = httpGETRequest(serverPath.c_str());
        return -1;
      }
      else{
        return 0;
      }
   }
  else {
    Serial.println("WiFi Disconnected");
    return -2;
  }
}

void viewBoard(){
  if(WiFi.status()== WL_CONNECTED){ 
      String serverPath = serverName+"/drawBoard";        
      payload = httpGETRequest(serverPath.c_str());
      Serial.println(payload);
      display.clearBuffer();
      display.setTextSize(2);
      display.setCursor(0, 0);
      display.print(payload);
      display.display();
   }
  else {
    Serial.println("WiFi Disconnected");
  }
}

void postMove(String Move){
  //Check WiFi connection status
  if(WiFi.status()== WL_CONNECTED){ 
      String serverPath = serverName+"/pushMove?move="+Move;        
      payload = httpGETRequest(serverPath.c_str());
      Serial.println(payload);
   }
  else {
    Serial.println("WiFi Disconnected");
  }
}

void motorTwitch(){
  digitalWrite(14,HIGH);
  delay(100);
  digitalWrite(14,LOW);
}

void showMove(){
  getMove();
  display.clearBuffer();
  display.setTextSize(6);
  display.setTextColor(EPD_BLACK);
  display.setCursor(20, 40);
  payload.toCharArray(payload_char, sizeof(payload));
  display.print(payload_char);
  display.display();
}

void moveEntry(){
  display.clearBuffer();
  display.setTextSize(6);
  display.setTextColor(EPD_BLACK);
  display.setCursor(20, 40);
  display.display();

  while(!finishedEntry){
    if (! digitalRead(BUTTON_A)) {
      pieces_index++;
      if (pieces_index > 6){
        pieces_index = 0;
      }
      other_move = String(pieces[pieces_index])+String(letters[letters_index])+String(numbers[numbers_index]);
      display.clearBuffer();
      display.setCursor(20, 40);
      display.print(other_move);
      display.display();
    }
    if (! digitalRead(BUTTON_B)) {
      letters_index++;
      if (letters_index > 8){
        letters_index = 0;
      }
      other_move = String(pieces[pieces_index])+String(letters[letters_index])+String(numbers[numbers_index]);
      display.clearBuffer();
      display.setCursor(20, 40);
      display.print(other_move);
      display.display();
    }
    if (! digitalRead(BUTTON_C)) {
       numbers_index++;
      if (numbers_index > 8){
        numbers_index = 0;
      }
      other_move = String(pieces[pieces_index])+String(letters[letters_index])+String(numbers[numbers_index]);
      display.clearBuffer();
      display.setCursor(20, 40);
      display.print(other_move);
      display.display();
    }
    if (!digitalRead(BUTTON_D)) {
      finishedEntry = true;
      other_move = String(pieces[pieces_index])+String(letters[letters_index])+String(numbers[numbers_index]);
    }
  }
  finishedEntry = false;
  display.clearBuffer();
  display.display();

  if (pieces[pieces_index] == 'P'){
    //This is a pawn. It might have captured something!
    display.setCursor(10, 20);
    display.setTextSize(2);
    display.print("RankC Y/N:");
    display.setCursor(20,40);
    display.setTextSize(6);
    display.print("a");
    display.display();

    letters_index_2 = 0;
    
    while (!finishedEntry){
      if (! digitalRead(BUTTON_A)) {
        letters_index_2++;
        if (letters_index_2 > 7){
          letters_index_2 = 0;
        }
        display.clearBuffer();
        display.setCursor(20,40);
        display.print(letters[letters_index_2]);
        display.display();

        
      }
      if (! digitalRead(BUTTON_B)){
        //We have finished entry
        other_move =  String(letters[letters_index_2])+String('x')+String(letters[letters_index])+String(numbers[numbers_index]);
        finishedEntry = true;
        letters_index = 0;
      }
      if (! digitalRead(BUTTON_C)){
        finishedEntry = true;
      }
    }
    finishedEntry = false;
  }

  postMove(other_move);
  delay(100); 
}

void setup() {
  Serial.begin(115200);
  display.begin(THINKINK_MONO);
  
  // Neopixel power
  pinMode(NEOPIXEL_POWER, OUTPUT);
  digitalWrite(NEOPIXEL_POWER, LOW); // on

  pinMode(BUTTON_A, INPUT_PULLUP);
  pinMode(BUTTON_B, INPUT_PULLUP);
  pinMode(BUTTON_C, INPUT_PULLUP);
  pinMode(BUTTON_D, INPUT_PULLUP);

  pixels.begin();
  pixels.setBrightness(20);
  pixels.fill(0xFF0066); //Pink
  pixels.show();
  
  display.clearBuffer();
  display.setTextSize(2);
  display.setTextColor(EPD_BLACK);
  display.setCursor(60, 0);
  display.print("NOT SUSPICIOUS");
  display.display();

  while (!finishedEntry){
    if (!digitalRead(BUTTON_D)) {
      finishedEntry = true;
    }
  }
  finishedEntry = false;
  
  display.clearBuffer();
  display.setCursor(0, 0);
  display.print("BaguetteLabs");
  display.setTextSize(3);
  display.setCursor(40, 40);
  display.print("ChessCheater");
  display.setTextSize(1);
  display.setCursor(100,100);
  display.print("Connecting to WiFi...");
  display.display();

  delay(3000);
    
  WiFi.begin(ssid, password);
  Serial.println("Connecting");
  
  int number = 3;
  bool green = true;
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    if (green){
      pixels.setPixelColor(number, 0, 255, 0);
      pixels.show();
      number = number - 1;
      if (number == -1){
        number = 3;
        green = false;
      }
    }
    else{
      pixels.setPixelColor(number, 0, 0, 255);
      pixels.show();
      number = number - 1;
      if (number == -1){
        number = 3;
        green = true;
      }
    }
    
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());

  pixels.clear();
  pixels.show();

  digitalWrite(NEOPIXEL_POWER, HIGH); // off

  clearBoard(); 

  display.clearBuffer();
  display.setTextSize(3);
  display.setCursor(40, 0);
  display.print("ChessCheater");
  display.setTextSize(2);
  display.setCursor(90, 60);
  display.print("New Game");
  display.setTextSize(2);
  display.setCursor(220,100);
  display.print("Black");
  display.setCursor(0,100);
  display.print("White");
  display.display();

  while (!finishedEntry){
    if (! digitalRead(BUTTON_A)) {
      finishedEntry = true;
      white = true;
    }
    if (! digitalRead(BUTTON_D)) {
      finishedEntry = true;
      white = false;
    }
    
  }
  finishedEntry = false;
  
}

void loop() {
  
  if (white){
    showMove();
    delay(5000);
    moveEntry();
    viewBoard();
    delay(2000);
  }
  else{
    moveEntry();
    showMove();
    delay(5000);
    viewBoard();
    delay(2000);
  }  

}

String httpGETRequest(const char* serverName) {
  WiFiClient client;
  HTTPClient http;

  http.begin(client, serverName);
  
  // Send HTTP POST request
  int httpResponseCode = http.GET();
  
  String payload = "{}"; 
  
  if (httpResponseCode > 0) {
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
    payload = http.getString();
  }
  else {
    Serial.print("Error code: ");
    Serial.println(httpResponseCode);
  }
  // Free resources
  http.end();

  return payload;
}
