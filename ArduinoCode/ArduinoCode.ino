#include <ClickEncoder.h>
#include <TimerOne.h>
#include <HID-Project.h>
#include <HID-Settings.h>
#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
// You must also install Adafruit BusIO

// Graph
int graphArray[78] = {0};
int graphArray2[78] = {0};

// OLED setup
#define OLED_ADDR   0x3C

Adafruit_SSD1306 display(-1);

// Encoder setup
#define ENCODER_PINA     6
#define ENCODER_PINB     7
#define ENCODER_BTN      4
ClickEncoder *encoder;
int16_t last, value;

// Loop counter
int loopCounter = 0;

// Button setup
const int  buttonOnePin = 5;    // the pin that the pushbutton is attached to
int lastButtonOneState = 0;
const int  buttonTwoPin = 8;
int lastButtonTwoState = 0;
const int  buttonThreePin = 9;
int lastButtonThreeState = 0;

// Keyboard keys
const char ctrlKey = KEY_LEFT_CTRL;// for windows
const char altKey = KEY_LEFT_ALT;// for windows
const char delKey = KEY_DELETE;// for windows

int i = 0;

// String  var = getValue( StringVar, ',', 2); // if  a,4,D,r  would return D        
String getValue(String data, char separator, int index)
{
    int found = 0;
    int strIndex[] = { 0, -1 };
    int maxIndex = data.length();

    for (int i = 0; i <= maxIndex && found <= index; i++) {
        if (data.charAt(i) == separator || i == maxIndex) {
            found++;
            strIndex[0] = strIndex[1] + 1;
            strIndex[1] = (i == maxIndex) ? i+1 : i;
        }
    }
    return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}

void timerIsr() {
    encoder->service();
}

void setup() {
  Serial.begin(115200);

  Consumer.begin();
  Keyboard.begin();

  pinMode(buttonOnePin, INPUT_PULLUP);
  pinMode(buttonTwoPin, INPUT_PULLUP);
  pinMode(buttonThreePin, INPUT_PULLUP);

  encoder = new ClickEncoder(ENCODER_PINA, ENCODER_PINB, ENCODER_BTN, 2, LOW);
  // Enable acceleration of rotary encoder
  //encoder->setAccelerationEnabled(true);

  //Initialize button state to not trigger on startup
  lastButtonOneState = digitalRead(buttonOnePin);
  lastButtonTwoState = digitalRead(buttonTwoPin);
  lastButtonThreeState = digitalRead(buttonThreePin);

  // Microseconds
  Timer1.initialize(1000);
  Timer1.attachInterrupt(timerIsr); 
  last = -1;
  
  // initialize and clear display
  display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR);
  display.clearDisplay();
  display.display();
}

void loop() {
     // Play pause
    handleButton(buttonThreePin, lastButtonThreeState, MEDIA_PLAY_PAUSE, MEDIA_PLAY_PAUSE);
    
      // Lock
    handleLockButton(buttonTwoPin, lastButtonTwoState);
    
    // Audio
    handleMacroButton(buttonOnePin, lastButtonOneState);
    
    // Handle rotary
    handleRotary();
    
    // Get data from pc and update screen
    loopCounter = loopCounter + 1;
    if(loopCounter==100){
      handlePcConnection();
      loopCounter=0;
    }
    
    Serial.println("loop");
    delay(50);
}

void handlePcConnection(){
    if(Serial.available() > 0) {
      String serialResponse = Serial.readStringUntil('\r\n');
      String upload = getValue(serialResponse, ',', 0);
      String download = getValue(serialResponse, ',', 1);
      String cpu = getValue(serialResponse, ',', 2);
      String mem = getValue(serialResponse, ',', 3);
  
      int cpuNumber = cpu.toInt();
      int memNumber = mem.toInt();
    
      updateScreen(cpuNumber, memNumber, download, upload);
  }else{
      noConnectionScreen();
  }
}

void noConnectionScreen(){
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(2,2);
  display.drawRect(0,0,128,32, WHITE);
  display.print("NO CONNECTION");
  display.display();
}

void updateScreen(int cpu, int mem, String dl, String ul){
  display.clearDisplay();
  
  // Graphs
  drawGraph(cpu);  
  drawGraph2(mem);

  // display a line of text
  display.setTextSize(1);
  display.setTextColor(WHITE);
  
  display.setCursor(0,0);
  String proc = "%";
  String cpuText = "CPU: ";
  String cpuTextFinal = cpuText + cpu + proc;
  display.print(cpuTextFinal);
  
  display.setCursor(0,8);
  String memText = "Mem: ";
  String memTextFinal = memText + mem + proc;
  display.print(memTextFinal);
  
  display.setCursor(0,16);
  String dlText = "DL: ";
  String kbs = "KB/s";
  String dlFinalText = dlText + dl + kbs; 
  display.print(dlFinalText); 
  
  display.setCursor(0,24);
  String ulText = "UL: ";
  String ulFinalText = ulText + ul + kbs; 
  display.print(ulFinalText);

  // update display with all of the above graphics
  display.display();
}

// newValue 0-100
void drawGraph(int newValue){
  float valuesPerPixel = 12.5;
  int graphWidth = 78;
  int startx = 50;
  int ypixel = newValue/valuesPerPixel;

  for(int j=0;j<77;j++){
    graphArray[j] = graphArray[j+1];
  }
  graphArray[graphWidth-1] = ypixel;

  for(int k=0;k<graphWidth;k++){
    display.drawLine(startx + k, 7-graphArray[k], startx + k, 7, WHITE);
  }
}

void drawGraph2(int newValue){
  float valuesPerPixel = 12.5;
  int graphWidth = 78;
  int startx = 50;
  int ypixel = newValue/valuesPerPixel;

  for(int j=0;j<77;j++){
    graphArray2[j] = graphArray2[j+1];
  }
  graphArray2[graphWidth-1] = ypixel;

  for(int k=0;k<graphWidth;k++){
    display.drawLine(startx + k, 15-graphArray2[k], startx + k, 15, WHITE);
  }
}

void handleButton(int buttonPin, int & lastButtonState, ConsumerKeycode onKey, ConsumerKeycode offKey){
  int buttonState = digitalRead(buttonPin);
  if (buttonState != lastButtonState) {
    if (buttonState == LOW) {
      Consumer.write(onKey);
    } else {
      Consumer.write(offKey);
    }
    lastButtonState = buttonState;
    // Delay a little bit to avoid bouncing
    delay(50);
  }
}

void handleMacroButton(int buttonPin, int & lastButtonState){
  int buttonState = digitalRead(buttonPin);
  if (buttonState != lastButtonState) {
    if (buttonState == LOW) {
      Keyboard.press(KEY_LEFT_CTRL);
      Keyboard.press(KEY_LEFT_SHIFT);
      Keyboard.press(KEY_LEFT_ALT);
      Keyboard.press('A');
      delay(10);
      Keyboard.release(KEY_LEFT_CTRL);
      Keyboard.release(KEY_LEFT_SHIFT);
      Keyboard.release(KEY_LEFT_ALT);
      Keyboard.release('A');
    } else {
      Keyboard.press(KEY_LEFT_CTRL);
      Keyboard.press(KEY_LEFT_SHIFT);
      Keyboard.press(KEY_LEFT_ALT);
      Keyboard.press('B');
      delay(10);
      Keyboard.release(KEY_LEFT_CTRL);
      Keyboard.release(KEY_LEFT_SHIFT);
      Keyboard.release(KEY_LEFT_ALT);
      Keyboard.release('B');
    }
    lastButtonState = buttonState;
    // Delay a little bit to avoid bouncing
    delay(50);
  }
}

void handleLockButton(int buttonPin, int & lastButtonState){
  int buttonState = digitalRead(buttonPin);
  if (buttonState != lastButtonState) {
    if (buttonState == LOW) {
      Keyboard.press(KEY_LEFT_GUI);
      Keyboard.press('l');
      delay(10);
      Keyboard.release(KEY_LEFT_GUI);
      Keyboard.release('l');
    } else {
      Keyboard.press(KEY_ENTER);
      Keyboard.release(KEY_ENTER);
      delay(1000);
      Keyboard.print("6210");
      Keyboard.press(KEY_ENTER);
      Keyboard.release(KEY_ENTER);
    }
    lastButtonState = buttonState;
    // Delay a little bit to avoid bouncing
    delay(50);
  }
}

void handleRotary(){
  value += encoder->getValue();
  
  if (value != last) {
    if(last>value){
      Consumer.write(MEDIA_VOL_DOWN);
    }else{
      Consumer.write(MEDIA_VOL_UP);
    }
    last = value;
  }

  // Mute
  ClickEncoder::Button b = encoder->getButton();
  if (b != ClickEncoder::Open) {
    switch (b) {
      case ClickEncoder::Clicked:
        Consumer.write(MEDIA_VOL_MUTE);
        break;
    }
  }
}
