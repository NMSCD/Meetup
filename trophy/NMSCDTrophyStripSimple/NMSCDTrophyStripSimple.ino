#include <WiFiClient.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <ESP8266WebServer.h>
#include <Adafruit_NeoPixel.h>

#define NUMPIXELS 8
#define PIXELPIN D1
#define BUTTONPIN D2

// Parameter 1 = number of pixels in strip
// Parameter 2 = Arduino pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUMPIXELS, PIXELPIN, NEO_GRB + NEO_KHZ800);

uint32_t red = strip.Color(255, 0, 0);
uint32_t orange = strip.Color(255, 165, 0);
uint32_t yellow = strip.Color(255, 255, 0);
uint32_t green = strip.Color(0, 255, 100);
uint32_t blue = strip.Color(0, 0, 255);
uint32_t purple = strip.Color(100, 0, 255);
uint32_t indigo = strip.Color(75, 0, 130);
uint32_t pink = strip.Color(255, 0, 255);
uint32_t black = strip.Color(0, 0, 0);

int lightBrightness = 255;
int buttonState = 0;
int choice = 0;
int step = 0;
int solidColourTimeout = 100;

void setup(void)
{
  Serial.begin(115200);
  pinMode(BUTTONPIN, INPUT_PULLUP);

  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
}

void loop(void)
{
  int localButtonState = digitalRead(BUTTONPIN);
  if (buttonState != localButtonState)
  {
    buttonState = localButtonState;
    if (localButtonState == LOW)
    {
      Serial.print("click");
      choice++;
      step = 0;
    }
  }

  switch (choice)
  {
  case 0:
    step = colorWipeStep(step, red, solidColourTimeout);
    break;
  case 1:
    step = solidColorWipeStep(step, red, solidColourTimeout);
    break;
  case 2:
    step = solidColorWipeStep(step, orange, solidColourTimeout);
    break;
  case 3:
    step = solidColorWipeStep(step, yellow, solidColourTimeout);
    break;
  case 4:
    step = solidColorWipeStep(step, green, solidColourTimeout);
    break;
  case 5:
    step = solidColorWipeStep(step, blue, solidColourTimeout);
    break;
  case 6:
    step = solidColorWipeStep(step, purple, solidColourTimeout);
    break;
  case 7:
    step = solidColorWipeStep(step, indigo, solidColourTimeout);
    break;
  case 8:
    step = solidColorWipeStep(step, pink, solidColourTimeout);
    break;
  case 9:
    step = rainbowStep(step, 20);
    break;
  default:
    choice = 0;
    break;
  }
}

void blankStep(int step)
{
  uint16_t i;
  for (i = 0; i < NUMPIXELS; i++)
  {
    strip.setPixelColor(i, black);
  }
  strip.show();
  step = 0;
  delay(1000);
}

int colorWipeStep(int step, uint32_t c, uint8_t wait)
{
  if (step > (NUMPIXELS + NUMPIXELS))
  {
    return 0;
  }
  if (step < NUMPIXELS)
  {
    strip.setPixelColor(step, c);
  }
  if (step >= NUMPIXELS)
  {
    strip.setPixelColor(step - NUMPIXELS, black);
  }
  strip.setBrightness(lightBrightness);
  strip.show();
  delay(wait);
  return step + 1;
}

int solidColorWipeStep(int step, uint32_t c, uint8_t wait)
{
  if (step < NUMPIXELS)
  {
    strip.setPixelColor(step, c);
  }

  strip.setBrightness(lightBrightness);
  strip.show();
  delay(wait);

  if (step >= NUMPIXELS)
  {
    return step;
  }
  return step + 1;
}

int rainbowStep(int step, uint8_t wait)
{
  uint16_t i;

  if (step > 256)
  {
    return 0;
  }

  for (i = 0; i < NUMPIXELS; i++)
  {
    strip.setPixelColor(i, Wheel((i + step) & 255));
  }
  strip.setBrightness(lightBrightness);
  strip.show();
  delay(wait);
  return step + 1;
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos)
{
  WheelPos = 255 - WheelPos;
  if (WheelPos < 85)
  {
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if (WheelPos < 170)
  {
    WheelPos -= 85;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}