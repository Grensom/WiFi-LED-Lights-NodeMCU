// Load Wi-Fi library
#include <ESP8266WiFi.h>
#include <FastLED.h>

#define LED_PIN     5
#define NUM_LEDS    50
#define BRIGHTNESS  30
#define LED_TYPE    WS2812
#define COLOR_ORDER GRB
CRGB leds[NUM_LEDS];

int UPDATES_PER_SECOND = 100;

CRGBPalette16 currentPalette;
TBlendType    currentBlending;

//==============================================================
//                  WIFI CREDENTIALS
//==============================================================
// Replace with your network credentials
const char* ssid     = "GregsWiFi";
const char* password = "stinkyhamster5";

// Set web server port number to 80
WiFiServer server(80);

// Variable to store the HTTP request
String header;

// Current time
unsigned long currentTime = millis();
// Previous time
unsigned long previousTime = 0; 
// Define timeout time in milliseconds (example: 2000ms = 2s)
const long timeoutTime = 2000;
//==============================================================
//                  SETUP
//==============================================================
void setup() {
  Serial.begin(115200);
// Connect to Wi-Fi network with SSID and password
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  // Print local IP address and start web server
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  server.begin();
  delay( 3000 ); // power-up safety delay
  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
  FastLED.setBrightness(  BRIGHTNESS );
  currentPalette = RainbowColors_p;
  currentBlending = LINEARBLEND;
}
//==============================================================
//                  LOOP
//==============================================================
void loop(){
  WiFiClient client = server.available();   // Listen for incoming clients
  displayColors();
  if (client) {                             // If a new client connects,
    Serial.println("New Client.");          // print a message out in the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    currentTime = millis();
    previousTime = currentTime;
    while (client.connected() && currentTime - previousTime <= timeoutTime) { // loop while the client's connected
      currentTime = millis();         
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        header += c;
        if (c == '\n') {                    // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();
            // turns the GPIOs on and off
            if (header.indexOf("GET /?mode=SolidColor") >= 0) {
              String color = header.substring(41,47);
              Serial.println("THIS COLOR IS: " + color);
              char colorHolder[7];
              uint32_t hex = 0;
              color.toCharArray(colorHolder, 7);
              hex = strtol(colorHolder, NULL, 16);
              fill_solid( currentPalette, 16, hex);
                displayColors();
            } else if (header.indexOf("GET /?mode=Rainbow") >= 0) {
              Serial.println("ACTIVATE Rainbow");
                getSpeed();
                currentPalette = RainbowColors_p;
                displayColors();
            } else if (header.indexOf("GET /?mode=Audio") >= 0) {
              getSpeed();
              currentPalette = RainbowStripeColors_p;   currentBlending = NOBLEND;
              displayColors();
            } else if (header.indexOf("GET /?mode=Wave") >= 0) {
              getSpeed();
              currentPalette = PartyColors_p;
              displayColors();
            }
//==============================================================
//                  HTML Page
//==============================================================  
client.println("<html lang=\"en\"><head> <meta charset=\"utf-8\"> <meta name=\"viewport\" content=\"width=device-width, initial-scale=1, shrink-to-fit=no\"> <meta name=\"description\" content=\"\"> <meta name=\"author\" content=\"Mark Otto, Jacob Thornton, and Bootstrap contributors\"> <meta name=\"generator\" content=\"Jekyll v3.8.5\"> <title>Signin Template · Bootstrap</title> <link rel=\"stylesheet\" href=\"https://stackpath.bootstrapcdn.com/bootstrap/4.3.1/css/bootstrap.min.css\" integrity=\"sha384-ggOyR0iXCbMQv3Xipma34MD+dH/1fQ784/j6cY/iJTQUOhcWr7x9JvoRxT2MZw1T\" crossorigin=\"anonymous\"> <style> .bd-placeholder-img { font-size: 1.125rem; text-anchor: middle; -webkit-user-select: none; -moz-user-select: none; -ms-user-select: none; user-select: none; } @media (min-width: 768px) { .bd-placeholder-img-lg { font-size: 3.5rem; } } html,body { height: 100%;}body { display: -ms-flexbox; display: flex; -ms-flex-align: center; align-items: center; padding-top: 40px; padding-bottom: 40px; background-color: #f5f5f5;}.form-signin { width: 100%; max-width: 330px; padding: 15px; margin: auto;}.form-signin .checkbox { font-weight: 400;}.form-signin .form-control { position: relative; box-sizing: border-box; height: auto; padding: 10px; font-size: 16px;}.form-signin .form-control:focus { z-index: 2;}.form-signin input[type=\"email\"] { margin-bottom: -1px; border-bottom-right-radius: 0; border-bottom-left-radius: 0;}.form-signin input[type=\"password\"] { margin-bottom: 10px; border-top-left-radius: 0; border-top-right-radius: 0;}.slidecontainer { width: 100%;}.slider { -webkit-appearance: none; width: 100%; height: 2px; background: #d3d3d3; outline: none; opacity: 0.7; -webkit-transition: .2s; transition: opacity .2s;}.slider:hover { opacity: 1;}.slider::-webkit-slider-thumb { -webkit-appearance: none; appearance: none; width: 25px; height: 25px; background: #007bff; cursor: pointer; border-radius: 100%;}.slider::-moz-range-thumb { width: 25px; height: 25px; background: #007bff; c </style> <!-- Custom styles for this template --> </head> <body class=\"text-center\"> <?php if(isset($_POST['mode'])){echo $_POST['mode'];} ?> <form class=\"form-signin\"> <img class=\"mb-4\" src=\"https://i.imgur.com/41I9ccK.png\" alt=\"\" width=\"72\" height=\"72\"> <h1 class=\"h3 mb-3 font-weight-normal\">Greg's LEDs</h1> <div class=\"form-group row\"> <label style=\"padding-top:20px;\" for=\"example-color-input\" class=\"col-2 col-form-label\">Mode</label> <div class=\"col-10\"> <select style=\"margin:10px;\" class=\"form-control\" id=\"mode\" name=\"mode\" value=\"Rainbow...\" ><option value=\"Rainbow...\">Rainbow</option><option value=\"SolidColor\">Solid Color</option><option value=\"Audio.....\">Audio</option><option value=\"Wave......\">Wave</option></select> </div> <label style=\"padding-top:3px;\" for=\"example-color-input\" class=\"col-2 col-form-label\">Speed</label> <div class=\"col-10\"> <input style=\"margin:10px;height:1px;padding:5px\" class=\"slider form-control\" type=\"range\" min=\"100\" max=\"999\" value=\"550\" class=\"slider\" id=\"speed\" name=\"speed\"> </div> <label style=\"padding-top:20px;\" for=\"example-color-input\" class=\"col-2 col-form-label\">Color</label> <div class=\"col-10\"> <input style=\"margin:10px; height: 50px;padding:5px\" class=\"form-control\" type=\"color\" value=\"#a500f4\" id=\"color\" name=\"color\"> <button style=\"margin:10px;margin-top:20px;\" class=\"btn btn-lg btn-primary btn-block\" type=\"submit\">Submit</button> </div></div> <p class=\"mt-5 mb-3 text-muted\">© Designed by Greg Ensom</p></form></body></html>");
            
            // The HTTP response ends with another blank line
            client.println();
            // Break out of the while loop
            break;
          } else { // if you got a newline, then clear currentLine
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
      }
    }
    // Clear the header variable
    header = "";
    // Close the connection
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
  }
}

void getSpeed(){
  String animationSpeed = header.substring(28,31);
  UPDATES_PER_SECOND = animationSpeed.toInt();
}

void displayColors(){
  static uint8_t startIndex = 0;
              startIndex = startIndex + 1; /* motion speed */
              FillLEDsFromPaletteColors( startIndex);
              FastLED.show();
              FastLED.delay(1000 / (UPDATES_PER_SECOND/10));
  }

void FillLEDsFromPaletteColors( uint8_t colorIndex)
{
    uint8_t brightness = 255;
    
    for( int i = 0; i < NUM_LEDS; i++) {
        leds[i] = ColorFromPalette( currentPalette, colorIndex, brightness, currentBlending);
        colorIndex += 3;
    }
}
