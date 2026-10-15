
#include <WiFi.h>
#include <WebServer.h>

// ==========================================
// MOBILE HOTSPOT SETTINGS
// ==========================================
const char* ssid = "iQOO z9x";
const char* password = "vishnu22";

WebServer server(80);

// ==========================================
// ESP32 PIN SETTINGS
// ==========================================
const int SOUND_PIN = 27;
const int RELAY1_PIN = 26;
const int RELAY2_PIN = 25;

// Typical active-LOW relay module.
// If your relay operates in reverse, try HIGH/LOW.
const int RELAY_ON = LOW;
const int RELAY_OFF = HIGH;

// Typical KY-038/KY-037 sound trigger.
// Change to HIGH if your module triggers HIGH.
const int SOUND_TRIGGER = LOW;

// ==========================================
// PROJECT STATES
// ==========================================
bool led1 = false;
bool led2 = false;
bool soundEnabled = true;

int soundEvents = 0;
int previousSound = HIGH;

unsigned long lastSoundTime = 0;
const unsigned long soundDelay = 800;

// ==========================================
// LED CONTROL FUNCTIONS
// ==========================================
void controlLED1(bool state) {
  led1 = state;
  digitalWrite(RELAY1_PIN, state ? RELAY_ON : RELAY_OFF);

  Serial.print("LED 1 state: ");
  Serial.println(state ? "ON" : "OFF");
}

void controlLED2(bool state) {
  led2 = state;
  digitalWrite(RELAY2_PIN, state ? RELAY_ON : RELAY_OFF);

  Serial.print("LED 2 state: ");
  Serial.println(state ? "ON" : "OFF");
}

// ==========================================
// DASHBOARD HTML
// ==========================================
const char webpage[] PROGMEM = R"HTML(
<!DOCTYPE html>
<html>
<head>
<meta name="viewport" content="width=device-width, initial-scale=1">
<title>Smart LED Control</title>

<style>
* { box-sizing: border-box; }

body {
  margin: 0;
  font-family: Arial, sans-serif;
  background: #101827;
  color: white;
}

header {
  text-align: center;
  background: #1b2940;
  padding: 22px 12px;
  border-bottom: 1px solid #35445c;
}

header h2 { margin: 0 0 8px; }
header p { margin: 0; color: #bac8dc; }

main {
  max-width: 650px;
  margin: auto;
  padding: 15px;
}

.grid {
  display: grid;
  grid-template-columns: repeat(2, minmax(0, 1fr));
  gap: 12px;
}

.card {
  background: #1b2940;
  padding: 20px;
  border-radius: 14px;
  border: 1px solid #35445c;
  margin-bottom: 15px;
}

.label {
  font-size: 13px;
  color: #bac8dc;
}

.state {
  font-size: 25px;
  font-weight: bold;
  margin: 15px 0;
}

.on { color: #4ade80; }
.off { color: #fb7185; }

button {
  width: 100%;
  padding: 13px 8px;
  margin-top: 8px;
  border: none;
  border-radius: 9px;
  background: #38bdf8;
  color: #071827;
  font-size: 15px;
  font-weight: bold;
  cursor: pointer;
}

.secondary {
  background: #35445c;
  color: white;
}

small {
  color: #bac8dc;
  line-height: 1.6;
}

@media(max-width: 380px) {
  .grid { grid-template-columns: 1fr; }
  .card { padding: 15px; }
}
</style>
</head>

<body>
<header>
  <h2>Smart LED Control</h2>
  <p>ESP32 | Dual Relay | Sound Automation</p>
</header>

<main>

  <div class="grid">
    <div class="card">
      <div class="label">LED 1 - RELAY 1</div>
      <div id="state1" class="state off">OFF</div>

      <button onclick="toggleLED(1)">
        Turn LED 1 ON / OFF
      </button>

      <button class="secondary" onclick="testRelay(1)">
        Test Relay 1
      </button>

      <p><small>Sound sensor controls LED 1.</small></p>
    </div>

    <div class="card">
      <div class="label">LED 2 - RELAY 2</div>
      <div id="state2" class="state off">OFF</div>

      <button onclick="toggleLED(2)">
        Turn LED 2 ON / OFF
      </button>

      <button class="secondary" onclick="testRelay(2)">
        Test Relay 2
      </button>

      <p><small>LED 2 is controlled independently.</small></p>
    </div>
  </div>

  <div class="card">
    <div class="label">SOUND AUTOMATION</div>
    <div id="sound" class="state">Enabled</div>

    <button class="secondary" onclick="toggleSound()">
      Enable / Disable Sound Control
    </button>

    <p><small>
      A detected sound toggles LED 1.
      LED 2 remains independent.
    </small></p>
  </div>

  <div class="grid">
    <div class="card">
      <div class="label">SOUND EVENTS</div>
      <div id="events" class="state">0</div>
    </div>

    <div class="card">
      <div class="label">CONNECTION</div>
      <div id="connection" class="state on">Connected</div>
    </div>
  </div>

  <div class="card">
    <div class="label">DEBUG MESSAGE</div>
    <p id="message">Dashboard ready.</p>
  </div>

</main>

<script>
async function request(path) {
  const response = await fetch(path, {
    method: 'GET',
    cache: 'no-store'
  });

  if (!response.ok) {
    throw new Error('HTTP ' + response.status);
  }

  return response;
}

async function toggleLED(number) {
  const message = document.getElementById('message');

  try {
    message.textContent = 'Sending command to LED ' + number;

    await request('/led' + number);
    await refresh();

    message.textContent = 'LED ' + number + ' command sent.';
  } catch (error) {
    message.textContent = 'Button error: ' + error.message;
    document.getElementById('connection').textContent = 'Error';
  }
}

async function testRelay(number) {
  const message = document.getElementById('message');

  try {
    message.textContent = 'Testing relay ' + number;

    await request('/test' + number);
    await refresh();

    message.textContent = 'Relay ' + number + ' test started.';
  } catch (error) {
    message.textContent = 'Test error: ' + error.message;
  }
}

async function toggleSound() {
  try {
    await request('/sound');
    await refresh();

    document.getElementById('message').textContent =
      'Sound control setting updated.';
  } catch (error) {
    document.getElementById('message').textContent =
      'Sound control request failed.';
  }
}

async function refresh() {
  try {
    const response = await request('/status');
    const data = await response.json();

    document.getElementById('state1').textContent =
      data.led1 ? 'ON' : 'OFF';

    document.getElementById('state1').className =
      'state ' + (data.led1 ? 'on' : 'off');

    document.getElementById('state2').textContent =
      data.led2 ? 'ON' : 'OFF';

    document.getElementById('state2').className =
      'state ' + (data.led2 ? 'on' : 'off');

    document.getElementById('sound').textContent =
      data.sound ? 'Enabled' : 'Disabled';

    document.getElementById('events').textContent =
      data.events;

    document.getElementById('connection').textContent =
      'Connected';

    document.getElementById('connection').className =
      'state on';
  } catch (error) {
    document.getElementById('connection').textContent =
      'Offline';

    document.getElementById('connection').className =
      'state off';
  }
}

refresh();
setInterval(refresh, 1000);
</script>
</body>
</html>
)HTML";

// ==========================================
// HTTP HANDLERS
// ==========================================
void handleRoot() {
  server.send_P(200, "text/html", webpage);
}

void handleStatus() {
  String json = "{";

  json += "\"led1\":";
  json += (led1 ? "true" : "false");

  json += ",\"led2\":";
  json += (led2 ? "true" : "false");

  json += ",\"sound\":";
  json += (soundEnabled ? "true" : "false");

  json += ",\"events\":";
  json += String(soundEvents);

  json += "}";

  server.sendHeader("Cache-Control", "no-store");
  server.send(200, "application/json", json);
}

void handleLED1() {
  Serial.println("Dashboard button: LED 1");

  controlLED1(!led1);

  server.send(200, "text/plain", "LED 1 updated");
}

void handleLED2() {
  Serial.println("Dashboard button: LED 2");

  controlLED2(!led2);

  server.send(200, "text/plain", "LED 2 updated");
}

void handleSound() {
  soundEnabled = !soundEnabled;

  Serial.print("Sound automation: ");
  Serial.println(soundEnabled ? "ENABLED" : "DISABLED");

  server.send(200, "text/plain", "Sound setting updated");
}

// ==========================================
// RELAY TEST TIMERS
// ==========================================
bool test1Active = false;
bool test2Active = false;

unsigned long test1Started = 0;
unsigned long test2Started = 0;

void handleTest1() {
  Serial.println("Testing relay 1");

  controlLED1(true);

  test1Started = millis();
  test1Active = true;

  server.send(200, "text/plain", "Relay 1 test started");
}

void handleTest2() {
  Serial.println("Testing relay 2");

  controlLED2(true);

  test2Started = millis();
  test2Active = true;

  server.send(200, "text/plain", "Relay 2 test started");
}

// ==========================================
// SETUP
// ==========================================
void setup() {
  Serial.begin(115200);

  // Initialize relay outputs.
  digitalWrite(RELAY1_PIN, RELAY_OFF);
  digitalWrite(RELAY2_PIN, RELAY_OFF);

  pinMode(RELAY1_PIN, OUTPUT);
  pinMode(RELAY2_PIN, OUTPUT);
  pinMode(SOUND_PIN, INPUT);

  led1 = false;
  led2 = false;

  previousSound = digitalRead(SOUND_PIN);

  // Connect to mobile hotspot.
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  Serial.println();
  Serial.print("Connecting to mobile hotspot");

  unsigned long start = millis();

  while (WiFi.status() != WL_CONNECTED &&
         millis() - start < 20000) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("Wi-Fi connected!");
    Serial.print("Network: ");
    Serial.println(WiFi.SSID());

    Serial.print("Dashboard URL: http://");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("Wi-Fi connection failed!");
    Serial.println("Check hotspot credentials and 2.4 GHz Wi-Fi.");
  }

  // Register all dashboard routes.
  server.on("/", handleRoot);
  server.on("/status", handleStatus);
  server.on("/led1", handleLED1);
  server.on("/led2", handleLED2);
  server.on("/sound", handleSound);
  server.on("/test1", handleTest1);
  server.on("/test2", handleTest2);

  server.begin();

  Serial.println("Web server started");
}

// ==========================================
// MAIN LOOP
// ==========================================
void loop() {
  server.handleClient();

  unsigned long now = millis();

  // End relay tests after 2 seconds.
  // Do not turn off a relay if a newer command changed its state.
  if (test1Active && now - test1Started >= 2000) {
    test1Active = false;
    controlLED1(false);
  }

  if (test2Active && now - test2Started >= 2000) {
    test2Active = false;
    controlLED2(false);
  }

  // Sound control toggles LED 1.
  int reading = digitalRead(SOUND_PIN);

  if (WiFi.status() == WL_CONNECTED &&
      soundEnabled &&
      reading == SOUND_TRIGGER &&
      previousSound != SOUND_TRIGGER &&
      now - lastSoundTime >= soundDelay) {

    controlLED1(!led1);

    soundEvents++;
    lastSoundTime = now;

    Serial.println("Sound event detected");
  }

  previousSound = reading;

  delay(2);
}
