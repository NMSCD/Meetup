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

ESP8266WebServer httpServer(80);
const char *ssid = "NMS-Meetup-Trophy-light-1";
const char *password = "nmscdmeetup";
IPAddress local_ip(192, 168, 1, 1);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);

uint32_t red = strip.Color(255, 0, 0);
uint32_t orange = strip.Color(255, 165, 0);
uint32_t yellow = strip.Color(255, 255, 0);
uint32_t green = strip.Color(0, 128, 0);
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

String redirectHomeHtml = "<!DOCTYPE HTML><html lang=\"en\"><head></head><body><h1>Redirecting</h1></body><script> window.location = '/'; setTimeout(function () { window.location = '/';}, 1000)</script></html>";

void setup(void)
{
  Serial.begin(115200);
  pinMode(BUTTONPIN, INPUT_PULLUP);

  setupHttpServer();
  setupHttpServerRoutes();

  httpServer.begin();
  Serial.println("HTTP server started");

  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
}

void loop(void)
{
  httpServer.handleClient();
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
  case 99:
    step = 0;
    break;
  default:
    choice = 0;
    break;
  }
}

void setupHttpServer()
{
  WiFi.softAP(ssid, password);
  WiFi.softAPConfig(local_ip, gateway, subnet);
  delay(100);
}

void setupHttpServerRoutes()
{
  httpServer.on("/", []()
                {
                  String htmlBody = "<body><main class=\"container\"> <h1 style=\"text-align: center;\">Control Trophy lights</h1> <h3>Select light mode</h3> <div id=\"actions\"></div><hr> <br><h4>Custom light mode</h4> <input type=\"color\" id=\"colourInput\" onchange=\"customColour()\" value=\"#0000ff\"> <input type=\"range\" id=\"lightPos\" oninput=\"customColour()\" min=\"0\" max=\"7\" value=\"4\"> <div style=\"margin-bottom: 10em\"></div></main></body>";

                  String modeOptions = "{";
                  modeOptions = modeOptions + "'Red': 1,";
                  modeOptions = modeOptions + "'Orange ~ish': 2,";
                  modeOptions = modeOptions + "'Yellow ~ish': 3,";
                  modeOptions = modeOptions + "'Green': 4,";
                  modeOptions = modeOptions + "'Blue': 5,";
                  modeOptions = modeOptions + "'Purple': 6,";
                  modeOptions = modeOptions + "'Indigo': 7,";
                  modeOptions = modeOptions + "'Pink': 8,";
                  modeOptions = modeOptions + "'Rainbow': 9,";
                  modeOptions = modeOptions + "}";

                  String htmlScriptTag = "<script>var options=" + modeOptions + "; const customColour=()=>{const colourInputElem=document.getElementById('colourInput'); const lightPosElem=document.getElementById('lightPos'); const rgb=/^#?([a-f\\d]{2})([a-f\\d]{2})([a-f\\d]{2})$/i.exec(colourInputElem.value); const r=parseInt(rgb[1], 16); const g=parseInt(rgb[2], 16); const b=parseInt(rgb[3], 16); const request=new XMLHttpRequest(); request.open('GET', `/drag-colour?r=${r}&g=${g}&b=${b}&pos=${lightPosElem.value}`); request.send();}; const changeMode=(id)=> ()=>{const request=new XMLHttpRequest(); request.open('GET', `/select-mode?mode=${id}`); request.send();}; const actionsElem=document.getElementById('actions'); for (const opt of Object.keys(options)){const btn=document.createElement('button'); btn.innerHTML=opt; btn.onclick=changeMode(options[opt]); actionsElem.append(btn);}</script>";

                  String welcomeMessage = "<html><head><link rel=\"stylesheet\" href=\"/main.css\"></head>" + htmlBody + htmlScriptTag + "</html>";
                  httpServer.send(200, "text/html", welcomeMessage); //
                }                                                    //
  );

  httpServer.on("/main.css", []()
                { String cssText = ".container,body>main{margin-right:auto;margin-left:auto;width:100%}button,input{overflow:visible}button,select{text-transform:none}[type=checkbox][role=switch],[type=checkbox][role=switch]:focus{--background-color:var(--switch-background-color);--border-color:var(--switch-background-color)}button,main{display:block}:root{--font-family:system-ui,-apple-system,'Segoe UI','Roboto'--line-height: 1.5;--font-weight:400;--font-size:16px;--border-radius:0.25rem;--border-width:1px;--outline-width:3px;--spacing:1rem;--typography-spacing-vertical:1.5rem;--block-spacing-vertical:calc(var(--spacing) * 2);--block-spacing-horizontal:var(--spacing);--grid-spacing-vertical:0;--grid-spacing-horizontal:var(--spacing);--form-element-spacing-vertical:0.75rem;--form-element-spacing-horizontal:1rem;--nav-element-spacing-vertical:1rem;--nav-element-spacing-horizontal:0.5rem;--nav-link-spacing-vertical:0.5rem;--nav-link-spacing-horizontal:0.5rem;--form-label-font-weight:var(--font-weight);--transition:0.2s ease-in-out;--modal-overlay-backdrop-filter:blur(0.25rem);--background-color:#11191f;--color:hsl(205, 16%, 77%);--h1-color:hsl(205, 20%, 94%);--h2-color:#e1e6eb;--h3-color:hsl(205, 18%, 86%);--h4-color:#c8d1d8;--h5-color:hsl(205, 16%, 77%);--h6-color:#afbbc4;--muted-color:hsl(205, 10%, 50%);--muted-border-color:#1f2d38;--primary:hsl(195, 85%, 41%);--primary-hover:hsl(195, 80%, 50%);--primary-focus:rgba(16, 149, 193, 0.25);--primary-inverse:#fff;--secondary:hsl(205, 15%, 41%);--secondary-hover:hsl(205, 10%, 50%);--secondary-focus:rgba(115, 130, 140, 0.25);--secondary-inverse:#fff;--contrast:hsl(205, 20%, 94%);--contrast-hover:#fff;--contrast-focus:rgba(115, 130, 140, 0.25);--contrast-inverse:#000;--mark-background-color:#d1c284;--mark-color:#11191f;--ins-color:#388e3c;--del-color:#c62828;--blockquote-border-color:var(--muted-border-color);--blockquote-footer-color:var(--muted-color);--button-box-shadow:0 0 0 rgba(0, 0, 0, 0);--button-hover-box-shadow:0 0 0 rgba(0, 0, 0, 0);--form-element-background-color:#11191f;--form-element-border-color:#374956;--form-element-color:var(--color);--form-element-placeholder-color:var(--muted-color);--form-element-active-background-color:var(--form-element-background-color);--form-element-active-border-color:var(--primary);--form-element-focus-color:var(--primary-focus);--form-element-disabled-background-color:hsl(205, 25%, 23%);--form-element-disabled-border-color:hsl(205, 20%, 32%);--form-element-disabled-opacity:0.5;--form-element-invalid-border-color:#b71c1c;--form-element-invalid-active-border-color:#c62828;--form-element-invalid-focus-color:rgba(198, 40, 40, 0.25);--form-element-valid-border-color:#2e7d32;--form-element-valid-active-border-color:#388e3c;--form-element-valid-focus-color:rgba(56, 142, 60, 0.25);--switch-background-color:#374956;--switch-color:var(--primary-inverse);--switch-checked-background-color:var(--primary);--range-border-color:#24333e;--range-active-border-color:hsl(205, 25%, 23%);--range-thumb-border-color:var(--background-color);--range-thumb-color:var(--secondary);--range-thumb-hover-color:var(--secondary-hover);--range-thumb-active-color:var(--primary);--table-border-color:var(--muted-border-color);--table-row-stripped-background-color:rgba(115, 130, 140, 0.05);--code-background-color:#18232c;--code-color:var(--muted-color);--code-kbd-background-color:var(--contrast);--code-kbd-color:var(--contrast-inverse);--code-tag-color:hsl(330, 30%, 50%);--code-property-color:hsl(185, 30%, 50%);--code-value-color:hsl(40, 10%, 50%);--code-comment-color:#4d606d;--accordion-border-color:var(--muted-border-color);--accordion-active-summary-color:var(--primary);--accordion-close-summary-color:var(--color);--accordion-open-summary-color:var(--muted-color);--card-background-color:#141e26;--card-border-color:var(--card-background-color);--card-box-shadow:0.0145rem 0.029rem 0.174rem rgba(0, 0, 0, 0.01698),0.0335rem 0.067rem 0.402rem rgba(0, 0, 0, 0.024),0.0625rem 0.125rem 0.75rem rgba(0, 0, 0, 0.03),0.1125rem 0.225rem 1.35rem rgba(0, 0, 0, 0.036),0.2085rem 0.417rem 2.502rem rgba(0, 0, 0, 0.04302),0.5rem 1rem 6rem rgba(0, 0, 0, 0.06),0 0 0 0.0625rem rgba(0, 0, 0, 0.015);--card-sectionning-background-color:#18232c;--dropdown-background-color:hsl(205, 30%, 15%);--dropdown-border-color:#24333e;--dropdown-box-shadow:var(--card-box-shadow);--dropdown-color:var(--color);--dropdown-hover-background-color:rgba(36, 51, 62, 0.75);--modal-overlay-background-color:rgba(36, 51, 62, 0.8);--progress-background-color:#24333e;--progress-color:var(--primary);--loading-spinner-opacity:0.5;--tooltip-background-color:var(--contrast);--tooltip-color:var(--contrast-inverse);color-scheme:dark}h1,h2,h3,h4,h5,h6{--font-weight:700;margin-top:0;margin-bottom:var(--typography-spacing-vertical);color:var(--color);font-weight:var(--font-weight);font-size:var(--font-size);font-family:var(--font-family)}button,input,select{margin:0;font-family:inherit}h1{--font-size:2rem;--typography-spacing-vertical:3rem;--color:var(--h1-color)}h2{--font-size:1.75rem;--typography-spacing-vertical:2.625rem;--color:var(--h2-color)}h3{--font-size:1.5rem;--typography-spacing-vertical:2.25rem;--color:var(--h3-color)}h4{--font-size:1.25rem;--typography-spacing-vertical:1.874rem;--color:var(--h4-color)}h5{--font-size:1.125rem;--typography-spacing-vertical:1.6875rem;--color:var(--h5-color)}[type=checkbox],[type=radio],[type=range]{accent-color:var(--primary)}*,::after,::before{box-sizing:border-box;background-repeat:no-repeat}::after,::before{text-decoration:inherit;vertical-align:inherit}:where(:root){-webkit-tap-highlight-color:transparent;-webkit-text-size-adjust:100%;-moz-text-size-adjust:100%;text-size-adjust:100%;background-color:var(--background-color);color:var(--color);font-weight:var(--font-weight);font-size:var(--font-size);line-height:var(--line-height);font-family:var(--font-family);text-rendering:optimizeLegibility;overflow-wrap:break-word;cursor:default;-moz-tab-size:4;-o-tab-size:4;tab-size:4}[role=button],button,input,input[type=button],input[type=reset],input[type=submit],select{font-size:1rem;line-height:var(--line-height);box-shadow:var(--box-shadow);font-weight:var(--font-weight);transition:background-color var(--transition),border-color var(--transition),color var(--transition),box-shadow var(--transition);outline:0;background-color:var(--background-color)}body{width:100%;margin:0}body>main{padding:var(--block-spacing-vertical) 0}.container{padding-right:var(--spacing);padding-left:var(--spacing)}@media (min-width:576px){:root{--font-size:17px}.container{max-width:510px;padding-right:0;padding-left:0}}@media (min-width:768px){:root{--font-size:18px}.container{max-width:700px}}@media (min-width:992px){:root{--font-size:19px}.container{max-width:920px}}@media (min-width:1200px){:root{--font-size:20px}.container{max-width:1130px}}h6{--color:var(--h6-color)}::-moz-selection{background-color:var(--primary-focus)}::selection{background-color:var(--primary-focus)}button,input:not([type=checkbox],[type=radio]),select{width:100%;margin-bottom:var(--spacing)}[type=button],[type=reset],[type=submit],button{-webkit-appearance:button}[role=button]{display:inline-block;text-decoration:none}[role=button],button,input[type=button],input[type=reset],input[type=submit]{--background-color:var(--primary);--border-color:var(--primary);--color:var(--primary-inverse);--box-shadow:var(--button-box-shadow, 0 0 0 rgba(0, 0, 0, 0));padding:var(--form-element-spacing-vertical) var(--form-element-spacing-horizontal);border:var(--border-width) solid var(--border-color);border-radius:var(--border-radius);color:var(--color);text-align:center;cursor:pointer}[role=button]:is([aria-current],:hover,:active,:focus),button:is([aria-current],:hover,:active,:focus),input[type=button]:is([aria-current],:hover,:active,:focus),input[type=reset]:is([aria-current],:hover,:active,:focus),input[type=submit]:is([aria-current],:hover,:active,:focus){--background-color:var(--primary-hover);--border-color:var(--primary-hover);--box-shadow:var(--button-hover-box-shadow, 0 0 0 rgba(0, 0, 0, 0));--color:var(--primary-inverse)}[role=button]:focus,button:focus,input[type=button]:focus,input[type=reset]:focus,input[type=submit]:focus{--box-shadow:var(--button-hover-box-shadow, 0 0 0 rgba(0, 0, 0, 0)),0 0 0 var(--outline-width) var(--primary-focus)}input[type=reset]{--background-color:var(--secondary);--border-color:var(--secondary);--color:var(--secondary-inverse);cursor:pointer}input[type=reset]:is([aria-current],:hover,:active,:focus){--background-color:var(--secondary-hover);--border-color:var(--secondary-hover);--color:var(--secondary-inverse)}input[type=reset]:focus{--box-shadow:var(--button-hover-box-shadow, 0 0 0 rgba(0, 0, 0, 0)),0 0 0 var(--outline-width) var(--secondary-focus)}:where(button,[type=submit],[type=button],[type=reset],[role=button])[disabled],:where(fieldset[disabled]) :is(button,[type=submit],[type=button],[type=reset],[role=button]),a[role=button]:not([href]){opacity:.5;pointer-events:none}input,select{letter-spacing:inherit;--background-color:var(--form-element-background-color);--border-color:var(--form-element-border-color);--color:var(--form-element-color);--box-shadow:none;border:var(--border-width) solid var(--border-color);border-radius:var(--border-radius);color:var(--color)}::-webkit-inner-spin-button,::-webkit-outer-spin-button{height:auto}[type=search]{-webkit-appearance:textfield;outline-offset:-2px}[type=search]::-webkit-search-decoration{-webkit-appearance:none}::-webkit-file-upload-button{-webkit-appearance:button;font:inherit}::-moz-focus-inner{padding:0;border-style:none}:-moz-focusring{outline:0}:-moz-ui-invalid{box-shadow:none}::-ms-expand{display:none}[type=file],[type=range]{padding:0;border-width:0}input:not([type=checkbox],[type=radio],[type=range]){height:4em}input:not([type=checkbox],[type=radio],[type=range],[type=file]),select{-webkit-appearance:none;-moz-appearance:none;appearance:none;padding:var(--form-element-spacing-vertical) var(--form-element-spacing-horizontal)}input:not([type=submit],[type=button],[type=reset],[type=checkbox],[type=radio],[readonly]):is(:active,:focus){--background-color:var(--form-element-active-background-color)}input:not([type=submit],[type=button],[type=reset],[role=switch],[readonly]):is(:active,:focus){--border-color:var(--form-element-active-border-color)}input:not([type=submit],[type=button],[type=reset],[type=range],[type=file],[readonly]):focus,select:focus{--box-shadow:0 0 0 var(--outline-width) var(--form-element-focus-color)}input:not([type=submit],[type=button],[type=reset])[disabled],select[disabled]{--background-color:var(--form-element-disabled-background-color);--border-color:var(--form-element-disabled-border-color);opacity:var(--form-element-disabled-opacity);pointer-events:none}input::-webkit-input-placeholder,input::placeholder,select:invalid{color:var(--form-element-placeholder-color);opacity:1}select::-ms-expand{border:0;background-color:transparent}select:not([multiple],[size]){padding-right:calc(var(--form-element-spacing-horizontal) + 1.5rem);padding-left:var(--form-element-spacing-horizontal);-webkit-padding-start:var(--form-element-spacing-horizontal);padding-inline-start:var(--form-element-spacing-horizontal);-webkit-padding-end:calc(var(--form-element-spacing-horizontal) + 1.5rem);padding-inline-end:calc(var(--form-element-spacing-horizontal) + 1.5rem);background-image:var(--icon-chevron);background-position:center right .75rem;background-size:1rem auto;background-repeat:no-repeat}[dir=rtl] select:not([multiple],[size]){background-position:center left .75rem}[type=checkbox],[type=radio]{--border-width:2px;padding:0;-webkit-appearance:none;-moz-appearance:none;appearance:none;width:1.25em;height:1.25em;margin-top:-.125em;margin-right:.375em;margin-left:0;-webkit-margin-start:0;margin-inline-start:0;-webkit-margin-end:.375em;margin-inline-end:.375em;border-width:var(--border-width);font-size:inherit;vertical-align:middle;cursor:pointer}[type=checkbox]::-ms-check,[type=radio]::-ms-check{display:none}[type=checkbox]:checked,[type=checkbox]:checked:active,[type=checkbox]:checked:focus,[type=radio]:checked,[type=radio]:checked:active,[type=radio]:checked:focus{--background-color:var(--primary);--border-color:var(--primary);background-image:var(--icon-checkbox);background-position:center;background-size:.75em auto;background-repeat:no-repeat}[type=checkbox]:indeterminate{--background-color:var(--primary);--border-color:var(--primary);background-image:var(--icon-minus);background-position:center;background-size:.75em auto;background-repeat:no-repeat}[type=radio]{border-radius:50%}[type=radio]:checked,[type=radio]:checked:active,[type=radio]:checked:focus{--background-color:var(--primary-inverse);border-width:.35em;background-image:none}[type=checkbox][role=switch]{--border-width:3px;--color:var(--switch-color);width:2.25em;height:1.25em;border:var(--border-width) solid var(--border-color);border-radius:1.25em;background-color:var(--background-color);line-height:1.25em}[type=checkbox][role=switch]:checked{--background-color:var(--switch-checked-background-color);--border-color:var(--switch-checked-background-color);background-image:none}[type=checkbox][role=switch]:before{display:block;width:calc(1.25em - (var(--border-width) * 2));height:100%;border-radius:50%;background-color:var(--color);content:'';transition:margin .1s ease-in-out}[type=checkbox][role=switch]:checked::before{margin-left:calc(1.125em - var(--border-width));-webkit-margin-start:calc(1.125em - var(--border-width));margin-inline-start:calc(1.125em - var(--border-width))}[type=checkbox]:checked[aria-invalid=false],[type=checkbox][aria-invalid=false],[type=checkbox][role=switch]:checked[aria-invalid=false],[type=checkbox][role=switch][aria-invalid=false],[type=radio]:checked[aria-invalid=false],[type=radio][aria-invalid=false]{--border-color:var(--form-element-valid-border-color)}[type=checkbox]:checked[aria-invalid=true],[type=checkbox][aria-invalid=true],[type=checkbox][role=switch]:checked[aria-invalid=true],[type=checkbox][role=switch][aria-invalid=true],[type=radio]:checked[aria-invalid=true],[type=radio][aria-invalid=true]{--border-color:var(--form-element-invalid-border-color)}[type=color]::-webkit-color-swatch-wrapper{padding:0}[type=color]::-moz-focus-inner{padding:0}[type=color]::-webkit-color-swatch{border:0;border-radius:calc(var(--border-radius) * .5)}[type=color]::-moz-color-swatch{border:0;border-radius:calc(var(--border-radius) * .5)}input:not([type=checkbox],[type=radio],[type=range],[type=file]):is([type=date],[type=datetime-local],[type=month],[type=time],[type=week]){--icon-position:0.75rem;--icon-width:1rem;padding-right:calc(var(--icon-width) + var(--icon-position));background-image:var(--icon-date);background-position:center right var(--icon-position);background-size:var(--icon-width) auto;background-repeat:no-repeat}input:not([type=checkbox],[type=radio],[type=range],[type=file])[type=time]{background-image:var(--icon-time)}[type=range]{-webkit-appearance:none;-moz-appearance:none;appearance:none;width:100%;height:2rem;background:0 0}[type=range]::-webkit-slider-runnable-track{width:100%;height:1.25rem;border-radius:var(--border-radius);background-color:var(--range-border-color);-webkit-transition:background-color var(--transition),box-shadow var(--transition);transition:background-color var(--transition),box-shadow var(--transition)}[type=range]::-moz-range-track{width:100%;height:1.25rem;border-radius:var(--border-radius);background-color:var(--range-border-color);-moz-transition:background-color var(--transition),box-shadow var(--transition);transition:background-color var(--transition),box-shadow var(--transition)}[type=range]::-ms-track{width:100%;height:1.25rem;border-radius:var(--border-radius);background-color:var(--range-border-color);-ms-transition:background-color var(--transition),box-shadow var(--transition);transition:background-color var(--transition),box-shadow var(--transition)}[type=range]::-webkit-slider-thumb{-webkit-appearance:none;width:2.5rem;height:2.5rem;margin-top:-.5rem;border:2px solid var(--range-thumb-border-color);border-radius:50%;background-color:var(--range-thumb-color);cursor:pointer;-webkit-transition:background-color var(--transition),transform var(--transition);transition:background-color var(--transition),transform var(--transition)}[type=range]::-moz-range-thumb{-webkit-appearance:none;width:2.5rem;height:2.5rem;margin-top:-.5rem;border:2px solid var(--range-thumb-border-color);border-radius:50%;background-color:var(--range-thumb-color);cursor:pointer;-moz-transition:background-color var(--transition),transform var(--transition);transition:background-color var(--transition),transform var(--transition)}[type=range]::-ms-thumb{-webkit-appearance:none;width:2.5rem;height:2.5rem;margin-top:-.5rem;border:2px solid var(--range-thumb-border-color);border-radius:50%;background-color:var(--range-thumb-color);cursor:pointer;-ms-transition:background-color var(--transition),transform var(--transition);transition:background-color var(--transition),transform var(--transition)}[type=range]:focus,[type=range]:hover{--range-border-color:var(--range-active-border-color);--range-thumb-color:var(--range-thumb-hover-color)}[type=range]:active{--range-thumb-color:var(--range-thumb-active-color)}[type=range]:active::-webkit-slider-thumb{transform:scale(5)}[type=range]:active::-moz-range-thumb{transform:scale(5)}[type=range]:active::-ms-thumb{transform:scale(5)}input:not([type=checkbox],[type=radio],[type=range],[type=file])[type=search]{-webkit-padding-start:calc(var(--form-element-spacing-horizontal) + 1.75rem);padding-inline-start:calc(var(--form-element-spacing-horizontal) + 1.75rem);border-radius:5rem;background-image:var(--icon-search);background-position:center left 1.125rem;background-size:1rem auto;background-repeat:no-repeat}input:not([type=checkbox],[type=radio],[type=range],[type=file])[type=search][aria-invalid]{-webkit-padding-start:calc(var(--form-element-spacing-horizontal) + 1.75rem)!important;padding-inline-start:calc(var(--form-element-spacing-horizontal) + 1.75rem)!important;background-position:center left 1.125rem,center right .75rem}input:not([type=checkbox],[type=radio],[type=range],[type=file])[type=search][aria-invalid=false]{background-image:var(--icon-search),var(--icon-valid)}input:not([type=checkbox],[type=radio],[type=range],[type=file])[type=search][aria-invalid=true]{background-image:var(--icon-search),var(--icon-invalid)}[type=search]::-webkit-search-cancel-button{-webkit-appearance:none;display:none}hr{height:0;border:0;border-top:1px solid var(--muted-border-color);color:inherit}";
                
                  httpServer.send(200, "text/css", cssText); });

  httpServer.on("/select-mode", []()
                {
                  for (uint8_t i = 0; i < httpServer.args(); i++)
                  {
                    if (httpServer.argName(i) == "mode")
                    {
                      choice = httpServer.arg(i).toFloat();
                      step = 0;
                    }
                    else
                    {
                      Serial.println("unknown argument! ");
                    }
                  }
                  httpServer.send(200, "text/html", redirectHomeHtml); });

  httpServer.on("/drag-colour", []()
                { 
                  int r = 0;
                  int g = 0;
                  int b = 0;
                  int lightIndex = 0;
                  for (uint8_t i = 0; i < httpServer.args(); i++)
                  {
                    if (httpServer.argName(i) == "r")
                    {
                      r = httpServer.arg(i).toFloat();
                    }
                    else if (httpServer.argName(i) == "g")
                    {
                      g = httpServer.arg(i).toFloat();
                    }
                    else if (httpServer.argName(i) == "b")
                    {
                      b = httpServer.arg(i).toFloat();
                    }
                    else if (httpServer.argName(i) == "pos")
                    {
                      lightIndex = httpServer.arg(i).toFloat();
                    }
                    else
                    {
                      Serial.println("unknown argument! ");
                    }
                  }
                  choice = 99;    
                  uint16_t i;              
                  for (i = 0; i < NUMPIXELS; i++)
                  {
                    if (i == lightIndex) {
                      strip.setPixelColor(i, strip.Color(r, g, b));
                    }
                    else {
                      strip.setPixelColor(i, black);
                    }
                  }
                  strip.setBrightness(lightBrightness);
                  strip.show();
                  httpServer.send(200, "text/html", redirectHomeHtml); });

  httpServer.onNotFound(handleNotFound);
}

void handleNotFound()
{
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += httpServer.uri();
  message += "\nMethod: ";
  message += (httpServer.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += httpServer.args();
  message += "\n";
  for (uint8_t i = 0; i < httpServer.args(); i++)
  {
    message += " " + httpServer.argName(i) + ": " + httpServer.arg(i) + "\n";
  }
  httpServer.send(404, "text/plain", message);
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