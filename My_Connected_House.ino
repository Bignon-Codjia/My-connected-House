#include <WiFi.h>
#include <WebServer.h>
#include <DHT.h>

// =========================================================================
// 1. WI-FI NETWORK CONFIGURATION (AP MODE - ACCESS POINT)
// =========================================================================
// Definition of the credentials for the autonomous network created by the ESP32.
// The SSID is the public name that will be visible when searching for networks on your smartphone.
const char* ssidAP = "My_ESP32_Dashboard"; 
// WPA2 security key (Must absolutely contain at least 8 characters to be valid).
const char* passwordAP = ""; // Put the password you want to use for your network here.

// Instantiation of the WebServer object. Port 80 is the standard port for unsecure HTTP traffic.
WebServer server(80);

// =========================================================================
// 2. HARDWARE CONFIGURATION (GPIO Assignment)
// =========================================================================
// Array containing the physical pin numbers (GPIO) of the ESP32 connected to the relay modules.
const int relayPins[4] = {26, 27, 14, 12}; 

// Configuration of the temperature and humidity sensor.
#define DHTPIN 4       // The sensor's data signal is connected to GPIO 4.
#define DHTTYPE DHT22  // Specification of the sensor model (DHT22 is more precise than DHT11).
DHT dht(DHTPIN, DHTTYPE); // Initialization of the sensor instance with the defined pin and type.

// =========================================================================
// 3. THE WEB PAGE (STORED IN FLASH MEMORY - PROGMEM)
// =========================================================================
// Using PROGMEM allows storing this long character string directly 
// in the ESP32's Flash memory instead of cluttering the much more limited RAM (SRAM).
const char index_html[] PROGMEM = R"=====(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="utf-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>My ESP32 Dashboard</title>
    <style>
        body {
            background: linear-gradient(to bottom right, #e2f8e9, #b3d4ff);
            font-family: Arial, sans-serif;
            display: flex;
            flex-direction: column;
            align-items: center;
            justify-content: center;
            min-height: 100vh;
            margin: 0;
            padding: 20px;
            box-sizing: border-box;
        }
        .dashboard {
            display: flex;
            flex-direction: column;
            gap: 20px;
            max-width: 800px;
        }
        .top-section {
            display: flex;
            flex-direction: row;
            gap: 20px;
            flex-wrap: nowrap;
            justify-content: center;
        }
        .panel {
            background-color: #3bb3ff;
            border-radius: 20px;
            padding: 20px;
            box-shadow: 0px 8px 15px rgba(0, 0, 0, 0.1);
        }
        .button-row {
            display: flex;
            gap: 15px;
            margin-bottom: 10px;
        }
        .button-row:last-child {
            margin-bottom: 0;
        }
        .btn {
            border: none;
            color: white;
            padding: 10px 20px;
            font-size: 16px;
            border-radius: 10px;
            cursor: pointer;
            width: 120px;
            font-weight: bold;
            display: flex;
            flex-direction: column;
            align-items: center;
            transition: 0.2s;
        }
        .btn:active {
            transform: scale(0.95);
        }
        .btn-on {
            background-color: #81d742;
        }
        .btn-off {
            background-color: #ff0000;
        }
        .sensor-panel {
            display: flex;
            align-items: center;
            justify-content: space-around;
            gap: 20px;
            min-width: 250px;
        }
        .sensor-display {
            display: flex;
            flex-direction: column;
            gap: 15px;
        }
        .sensor-value {
            background-color: #2c2c2c;
            color: white;
            padding: 10px 20px;
            font-size: 22px;
            border-radius: 5px;
            font-weight: bold;
        }
        .activity-title {
            color: white;
            text-align: center;
            margin: 0 0 10px 0;
            font-size: 18px;
        }
        .activity-log {
            background-color: white;
            color: black;
            height: 120px;
            overflow-y: auto;
            padding: 10px;
            border-radius: 5px;
            font-family: monospace;
            font-size: 14px;
            line-height: 1.5;
        }
        @media (max-width: 600px) {
            .top-section {
                flex-direction: column;
                gap: 10px;
            }
            .dashboard {
                padding: 10px;
            }
            .btn {
                width: 100%;
                padding: 15px;
                font-size: 18px;
            }
            .sensor-panel {
                flex-direction: column;
                gap: 10px;
            }
            .activity-log {
                height: 100px;
            }
        }
        
        /* Targets the signature area which will be located at the very bottom of the page.
           Since the body uses "display: flex" and "flex-direction: column", 
           this element will naturally stack under your dashboard.
        */
        .signature {
            /* Centers the text horizontally for an aesthetic and symmetrical rendering */
            text-align: center; 
            /* Uses a very dark grey (#2c2c2c) to ensure good contrast on the light blue background, while being less aggressive than pure black (#000000) */
            color: #2c2c2c; 
            /* Sets a font size of 14px, slightly smaller than the rest to indicate it's secondary information (credits) */
            font-size: 14px; 
            /* Forces a 30-pixel empty space above the signature to clearly separate it from the ESP32 control panels */
            margin-top: 30px; 
            /* Adds an internal spacing of 10 pixels to prevent text from touching the edges of the screen on very small phones */
            padding: 10px; 
            /* Slightly tilts the text (italic) to visually emphasize that it's a signature or legal notice */
            font-style: italic; 
        }

        /*
           Specifically targets a hyperlink (<a> tag) if there is one inside the ".signature" div (e.g. link to your GitHub).
        */
        .signature a {
            /* Applies a blue color (#0056b3) typical of clickable links to indicate interactivity to the user */
            color: #0056b3; 
            /* Removes the default underline imposed by browsers to achieve a cleaner and more modern design */
            text-decoration: none; 
            /* Makes the link text thicker (bold) to make it subtly stand out from the rest of the sentence */
            font-weight: bold; 
            /* Adds a smooth 0.3-second transition so the state change on mouse hover isn't abrupt */
            transition: color 0.3s ease;
        }

        /*
           Defines the visual behavior of the link exclusively when the user hovers their mouse cursor over it (pseudo-class state :hover).
        */
        .signature a:hover {
            /* Reactivates the underline only on hover to visually confirm to the user that the element is indeed clickable */
            text-decoration: underline; 
            /* Slightly darkens the blue on hover to accentuate the interactive effect */
            color: #003d82;
        }
    </style>
</head>
<body>
    <div class="dashboard">
        <div class="top-section">
            <div class="panel">
                <div class="button-row">
                    <button class="btn btn-on" onclick="lampAction(1, 'ON')"><span>Lamp 1</span><span>ON</span></button>
                    <button class="btn btn-off" onclick="lampAction(1, 'OFF')"><span>Lamp 1</span><span>OFF</span></button>
                </div>
                <div class="button-row">
                    <button class="btn btn-on" onclick="lampAction(2, 'ON')"><span>Lamp 2</span><span>ON</span></button>
                    <button class="btn btn-off" onclick="lampAction(2, 'OFF')"><span>Lamp 2</span><span>OFF</span></button>
                </div>
                <div class="button-row">
                    <button class="btn btn-on" onclick="lampAction(3, 'ON')"><span>Lamp 3</span><span>ON</span></button>
                    <button class="btn btn-off" onclick="lampAction(3, 'OFF')"><span>Lamp 3</span><span>OFF</span></button>
                </div>
                <div class="button-row">
                    <button class="btn btn-on" onclick="lampAction(4, 'ON')"><span>Lamp 4</span><span>ON</span></button>
                    <button class="btn btn-off" onclick="lampAction(4, 'OFF')"><span>Lamp 4</span><span>OFF</span></button>
                </div>
            </div>
            <div class="panel sensor-panel">
                <svg width="80" height="120" viewBox="0 0 100 150" fill="none" xmlns="http://www.w3.org/2000/svg">
                    <path d="M50 10C42 10 35 17 35 25V90C25 96 20 107 22 119C25 133 37 142 50 142C63 142 75 133 78 119C80 107 75 96 65 90V25C65 17 58 10 50 10Z" stroke="#333" stroke-width="6" fill="#e6f2ff"/>
                    <circle cx="50" cy="115" r="15" fill="#38b6ff" stroke="#333" stroke-width="4"/>
                    <rect x="44" y="40" width="12" height="65" fill="#38b6ff"/>
                    <line x1="25" y1="30" x2="35" y2="30" stroke="#333" stroke-width="4" stroke-linecap="round"/>
                    <line x1="20" y1="50" x2="35" y2="50" stroke="#333" stroke-width="4" stroke-linecap="round"/>
                    <line x1="25" y1="70" x2="35" y2="70" stroke="#333" stroke-width="4" stroke-linecap="round"/>
                </svg>
                <div class="sensor-display">
                    <div class="sensor-value" id="temp-value">TEMP: --°C</div>
                    <div class="sensor-value" id="hum-value">HUM: --%</div>
                </div>
            </div>
        </div>
        <div class="panel">
            <h3 class="activity-title">Activity</h3>
            <div class="activity-log" id="log-box"></div>
        </div>
    </div>
    <div class="signature">
        ESP32 Dashboard &copy; 2026 - Designed by 
        <a href="https://github.com/Bignon-Codjia" target="_blank">Bignon Codjia</a>
    </div>

    <script>
        function lampAction(lampNumber, state) {
            const actionText = "-Lamp " + lampNumber + ": " + state;
            const activityBox = document.getElementById("log-box");
            
            activityBox.innerHTML += actionText + "<br>";
            activityBox.scrollTop = activityBox.scrollHeight;

            fetch('/api/lamp?id=' + lampNumber + '&state=' + state)
                .then(response => {
                    if(!response.ok) {
                        activityBox.innerHTML += "<i>Error: The ESP32 did not respond correctly.</i><br>";
                    }
                })
                .catch(error => {
                    activityBox.innerHTML += "<i>Network error: Connection lost.</i><br>";
                });
        }

        function fetchSensors() {
            fetch('/api/sensors')
                .then(response => response.json())
                .then(data => {
                    if(data.error) {
                        console.error("Problem reading the DHT sensor.");
                        return;
                    }
                    document.getElementById("temp-value").innerText = "TEMP: " + data.temperature + "°C";
                    document.getElementById("hum-value").innerText = "HUM: " + data.humidity + "%";
                })
                .catch(err => console.error("Error fetching data:", err));
        }

        setInterval(fetchSensors, 5000);
        fetchSensors();
    </script>
</body>
</html>
)=====";

// =========================================================================
// 4. WEB SERVER ROUTES (Processing incoming requests)
// =========================================================================

// Function called when the user types the ESP32 IP address (Root route "/").
void handleRoot() {
  // The server responds with a success code (200), specifies it sends HTML format, 
  // and transmits the entire 'index_html' string stored in Flash memory.
  server.send(200, "text/html", index_html);
}

// Function called when JavaScript executes: fetch('/api/lamp?id=X&state=Y')
void handleLamp() {
  // Security validation: Checks that the URL actually contains both mandatory parameters "id" and "state".
  if (server.hasArg("id") && server.hasArg("state")) {
    
    // Extraction and conversion of the lamp number transmitted in the URL (String to Integer).
    int lampId = server.arg("id").toInt();
    // Extraction of the state command ("ON" or "OFF").
    String state = server.arg("state");
    
    // Index adjustment: The HTML requests lamps 1 to 4, but the C++ array 'relayPins' is indexed from 0 to 3.
    int arrayIndex = lampId - 1;

    // Protection against memory overflows: Checks that the requested index corresponds to an existing lamp.
    if (arrayIndex >= 0 && arrayIndex < 4) {
      
      // Relay activation logic.
      // Warning: Many relay modules activate when they receive a LOW state and turn off on HIGH.
      if (state == "ON") {
        digitalWrite(relayPins[arrayIndex], LOW); // Turns on the lamp.
      } else if (state == "OFF") {
        digitalWrite(relayPins[arrayIndex], HIGH); // Turns off the lamp.
      }
      
      // Confirms to JavaScript that the hardware order has been executed.
      server.send(200, "text/plain", "Command executed");
    } else {
      // Returns an HTTP 400 error (Bad Request) if the lamp number is outside the 1-4 range.
      server.send(400, "text/plain", "Error: Invalid lamp number");
    }
  } else {
    // Returns an HTTP 400 error if the URL is badly formatted.
    server.send(400, "text/plain", "Error: Missing parameters");
  }
}

// Function called when JavaScript executes: fetch('/api/sensors')
void handleSensors() {
  // Physically polls the DHT22 component to get the measurements.
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  // "isnan" (Is Not A Number) checks if the hardware read failed (e.g., bad wiring, interference).
  if (isnan(h) || isnan(t)) {
    // Formats an error response in JSON format so JavaScript understands it and doesn't crash.
    server.send(500, "application/json", "{\"error\": true}");
    return; // Stops the execution of the function here.
  }

  // Manual construction of the JSON string to package the two values in a structured way.
  // The final result will look exactly like this: {"temperature": 25.50, "humidity": 60.20}
  String json = "{";
  json += "\"temperature\": " + String(t) + ",";
  json += "\"humidity\": " + String(h);
  json += "}";

  // Dispatches the data packet to the browser with the correct MIME type for JSON.
  server.send(200, "application/json", json);
}

// =========================================================================
// 5. INITIALIZATION (Executed only once at board startup)
// =========================================================================
void setup() {
  // Opens the computer's serial port to read debugging messages (speed of 115200 bauds).
  Serial.begin(115200);
  delay(100);

  // Systematic initial configuration of the pins connected to the relays.
  for (int i = 0; i < 4; i++) {
    // Sets the pin as a current OUTPUT.
    pinMode(relayPins[i], OUTPUT);
    // Forces an immediate HIGH state to ensure all lamps are OFF at startup (inverted logic).
    digitalWrite(relayPins[i], HIGH); 
  }

  // Starts the library managing the physical DHT22 component.
  dht.begin();
  Serial.println("Environmental sensor initialized.");

  // Configuration of the ESP32 Wi-Fi network.
  // WIFI_AP stands for "Access Point" (The ESP32 creates its own internal router, without needing your local internet box).
  WiFi.mode(WIFI_AP); 
  
  // Starts broadcasting the wireless network using the name and password defined at the top of the code.
  WiFi.softAP(ssidAP, passwordAP);

  // Display of connection information in the Serial Monitor for the user.
  Serial.println("");
  Serial.println("Local Wi-Fi network successfully created!");
  Serial.print("Search for the SSID: ");
  Serial.println(ssidAP);
  
  // Displays the static IP address assigned by default to the ESP32 in AP mode (usually 192.168.4.1).
  Serial.print("Open the browser to this local address: http://");
  Serial.println(WiFi.softAPIP());

  // Mapping of web requests: "When the browser requests address X, execute C++ function Y".
  server.on("/", handleRoot);             // Route for displaying the web page.
  server.on("/api/lamp", handleLamp);     // API route for relay control.
  server.on("/api/sensors", handleSensors); // API route for sending DHT22 data.

  // Formally starts the web server component to make it listen for HTTP requests.
  server.begin();
  Serial.println("The Web Server is active and waiting for instructions!");
}

// =========================================================================
// 6. MAIN LOOP (Runs infinitely and at very high frequency)
// =========================================================================
void loop() {
  // Continually asks the network processor to check if a new device (like your smartphone)
  // has connected or sent a new HTTP request. This is the central engine of the server.
  // IP Address: http://192.168.4.1/
  server.handleClient();
}