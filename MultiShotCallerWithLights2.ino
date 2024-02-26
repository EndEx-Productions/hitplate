#include <FastLED.h>

// How high the sample must be to verify a hit (0-1024)
#define HIT_THRESHOLD 950

// How long the hit LED stays lit in ms
#define HIT_ALERT_TIME_MS 200

// How many leds are in the strip total? (We assume front has 10 leds and back has 10 leds)
#define NUM_LEDS 20

// How bright the LEDs shine (0-255)
#define LED_BRIGHTNESS 80

// LED PIN
#define LEDS_PIN 11

// Enable Serial Debugging
#define DEBUG true

// Amount of starting hit points
#define STARTING_HIT_POINTS 100

// Amount of damage each hit to the front plate does
#define FRONT_PLATE_DAMAGE 10

// Amount of damage each hit to the back plate does
#define BACK_PLATE_DAMAGE 15

// Mic input on pin 15
#define FRONT_PLATE_PIN A1

// Mic input on pin 14
#define BACK_PLATE_PIN A0

// Global Variables
volatile int currentHitPoints = 0;
volatile int plateReading = 0;
volatile int prevPlateReading = 0;
CRGB leds[NUM_LEDS];

void setup() {
  if(DEBUG) {
    // initialize serial communication at 9600 bits per second:
    Serial.begin(9600);
  }

  pinMode(FRONT_PLATE_PIN, INPUT);
  pinMode(BACK_PLATE_PIN, INPUT);
  pinMode(LED_BUILTIN, OUTPUT);

  FastLED.addLeds<WS2812, LEDS_PIN, GRB>(leds, NUM_LEDS);

  currentHitPoints = STARTING_HIT_POINTS;

  startupAnimation();
  showHpIndicator();
}

void loop() {
  checkForHit(FRONT_PLATE_PIN, FRONT_PLATE_DAMAGE);
  checkForHit(BACK_PLATE_PIN, BACK_PLATE_DAMAGE);
}

void checkForHit(uint8_t platePin, int damage){
  plateReading = analogRead(platePin);

  if((plateReading > HIT_THRESHOLD)&&(prevPlateReading < HIT_THRESHOLD)){
    if(DEBUG){
      Serial.print("Hit detected on pin: ");
      Serial.print(platePin);
      Serial.print(" | Reading: ");
      Serial.print(plateReading);
      Serial.print(" | Hit Points: ");
      Serial.println(currentHitPoints);
    }
    currentHitPoints -= damage;
    if (currentHitPoints <= 0) {
      deadAnimation();
    }
    else{
      hitAnimation();
    }
  }
  prevPlateReading = plateReading;
}

void showHpIndicator(){
  double health_percent = double(currentHitPoints)/double(STARTING_HIT_POINTS);
  double led_count = double(NUM_LEDS/2);
  int greenLedCount = floor(health_percent * led_count);
    
  setLEDs("green", 0, NUM_LEDS, 1);
  setLEDs("green", 0, greenLedCount, LED_BRIGHTNESS);
  //setLEDs("green", NUM_LEDS/2, NUM_LEDS/2+greenLedCount, LED_BRIGHTNESS);//Using both strips in same orientation
  setLEDs("green", (NUM_LEDS/2)+(NUM_LEDS/2-greenLedCount), NUM_LEDS, LED_BRIGHTNESS);//Using second strip upside down
}

void startupAnimation(){
  // Flash 3 times to indicate it has been reset
  for (int i = 0; i < 3; i++){
    setLEDs("red", 0, NUM_LEDS, LED_BRIGHTNESS);
    beep(HIT_ALERT_TIME_MS);
    clearLeds();
    delay(HIT_ALERT_TIME_MS);
  }
}

void deadAnimation(){
  // When dead don't leave this conditional
  while(true){
    // flash and beep
    setLEDs("red", 0, NUM_LEDS, LED_BRIGHTNESS);
    beep(HIT_ALERT_TIME_MS*2);
      
    // clear and silence for 300ms
    clearLeds();
    delay(HIT_ALERT_TIME_MS*2);
  }
}

void hitAnimation(){
  setLEDs("orange", 0, NUM_LEDS, LED_BRIGHTNESS);
  beep(HIT_ALERT_TIME_MS);
  showHpIndicator();
}

void clearLeds(){
  setLEDs("black", 0, NUM_LEDS, 0);
}

void setLEDs(String color, int startLED, int endLED, int brightLevel){
  int hue = 0;
  int saturation = 255;
  int brightness =  brightLevel;

  if(color.equals("red")){
    hue = 0;
  } else if(color.equals("orange")){
    hue = 22;
  } else if(color.equals("green")){
    hue = 85;
  } else if(color.equals("black")){
    brightness = 0;
  }
  
  for(int i = startLED; i < endLED; i++){
    leds[i].setHSV( hue, saturation, brightness);
  }
  FastLED.show();
}

void beep(int duration){
  digitalWrite(LED_BUILTIN, HIGH);
  delay(duration);
  digitalWrite(LED_BUILTIN, LOW);
}