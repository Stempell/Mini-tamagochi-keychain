
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Preferences.h>
#include <esp_sleep.h>
#include <sys/time.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_ADDR 0x3C

#define BTN_FEED  3
#define BTN_PLAY  4
#define BTN_POWER 5
#define BATTERY_ADC_PIN 2


#define BATTERY_DIVIDER_RATIO 2.0f

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
Preferences prefs;


//Stats
int hunger = 80;
int happiness = 80;
int energy = 100;
bool dead = false;


float hungerAccumulator = 0.0f;
float happinessAccumulator = 0.0f;

uint32_t lastUpdate = 0;
uint32_t lastSave = 0;
uint32_t lastRender = 0;

const float HUNGER_SECONDS_PER_POINT = 180.0f;
const float HAPPY_SECONDS_PER_POINT = 240.0f;

const uint32_t UPDATE_INTERVAL_MS = 1000;
const uint32_t SAVE_INTERVAL_MS = 30000;
const uint32_t RENDER_INTERVAL_MS = 250;


//RTC
RTC_DATA_ATTR uint32_t rtcMagic = 0;
RTC_DATA_ATTR int64_t sleepStartUs = 0;
const uint32_t RTC_MAGIC_VALUE = 0x54414D41;

struct Button {
  uint8_t pin;
  bool stableState;
  bool lastReading;
  uint32_t changedAt;
};


//Buttons
Button feedButton  = {BTN_FEED, HIGH, HIGH, 0};
Button playButton  = {BTN_PLAY, HIGH, HIGH, 0};
Button powerButton = {BTN_POWER, HIGH, HIGH, 0};


bool justPressed(Button &b) {
  bool reading = digitalRead(b.pin);
  uint32_t now = millis();

    if (reading != b.lastReading) {
    b.lastReading = reading;
    b.changedAt = now;
  }

    if (now - b.changedAt >= 30) {
    if (reading != b.stableState) {
      b.stableState = reading;
      if (b.stableState == LOW) return true;
    }
  }

  return false;
}


//Battery
float readBatteryVoltage() {
  uint32_t measuredMv = analogReadMilliVolts(BATTERY_ADC_PIN);
  return (measuredMv / 1000.0f) * BATTERY_DIVIDER_RATIO;
}

int readBatteryPercent() {
  float voltage = readBatteryVoltage();

  // Approximate LiPo 1S mapping; voltage is not linear with charge.
  const float EMPTY_VOLTAGE = 3.0f;
  const float FULL_VOLTAGE = 4.2f;

  int percent = (int)(
    (voltage - EMPTY_VOLTAGE) /
    (FULL_VOLTAGE - EMPTY_VOLTAGE) * 100.0f
  );

  return constrain(percent, 0, 100);
}

int getEnergyLevel() {
  energy = readBatteryPercent();
  return energy;
}


//Flash storage
void savePet() {
  prefs.putInt("hunger", hunger);
  prefs.putInt("happy", happiness);
  prefs.putBool("dead", dead);
  prefs.putFloat("hAcc", hungerAccumulator);
  prefs.putFloat("pAcc", happinessAccumulator);
}

void loadPet() {
  hunger = constrain(prefs.getInt("hunger", 80), 0, 100);
  happiness = constrain(prefs.getInt("happy", 80), 0, 100);
  dead = prefs.getBool("dead", false);

  hungerAccumulator = prefs.getFloat("hAcc", 0.0f);
  happinessAccumulator = prefs.getFloat("pAcc", 0.0f);
}



//Actions
void feedPet() {
  if (dead) return;

  hunger = min(100, hunger + 25);

  savePet();
}


void playWithPet() {
  if (dead) return;

  int battery = getEnergyLevel();
  float effectiveness;

  if (battery >= 60) {
    effectiveness = 1.0f;
  } else if (battery >= 30) {
    effectiveness = 0.65f;
  } else if (battery >= 10) {
    effectiveness = 0.35f;
  } else {
    effectiveness = 0.15f;
  }

  int happinessGain = (int)(20.0f * effectiveness + 0.5f);
  happiness = min(100, happiness + happinessGain);

  hunger = max(0, hunger - 5);

  savePet();
}


int64_t getTimeUs() {
  struct timeval tv;
  gettimeofday(&tv, nullptr);
  return (int64_t)tv.tv_sec * 1000000LL + tv.tv_usec;
}

void applyElapsedTime(float seconds) {
  if (dead || seconds <= 0) return;

  // Hunger and happiness drain over time.
  hungerAccumulator += seconds / HUNGER_SECONDS_PER_POINT;
  happinessAccumulator += seconds / HAPPY_SECONDS_PER_POINT;

  int hungerLoss = (int)hungerAccumulator;
  int happinessLoss = (int)happinessAccumulator;

  hungerAccumulator -= hungerLoss;
  happinessAccumulator -= happinessLoss;

  hunger = max(0, hunger - hungerLoss);
  happiness = max(0, happiness - happinessLoss);


  if (hunger <= 0) {
    dead = true;
  }

  savePet();
}

void updatePetWhileAwake() {
  uint32_t now = millis();

  if (now - lastUpdate >= UPDATE_INTERVAL_MS) {
    float seconds = (now - lastUpdate) / 1000.0f;
    lastUpdate = now;
    applyElapsedTime(seconds);
  }

  if (now - lastSave >= SAVE_INTERVAL_MS) {
    lastSave = now;
    savePet();
  }
}


//Deep Sleep


void enterDeepSleep(bool oledReady) {
  savePet();

  sleepStartUs = getTimeUs();
  rtcMagic = RTC_MAGIC_VALUE;

  if (oledReady) {
    display.clearDisplay();
    display.display();
    display.ssd1306_command(SSD1306_DISPLAYOFF);
  }

  // Wait until the power button is released.
  while (digitalRead(BTN_POWER) == LOW) {
    delay(10);
  }

  // Wake on the power button pulling GPIO5 LOW.
  esp_deep_sleep_enable_gpio_wakeup(
    1ULL << BTN_POWER,
    ESP_GPIO_WAKEUP_GPIO_LOW
  );

  esp_deep_sleep_start();
}


//Oled (initial)

void drawBar(int x, int y, int width, int height, int value) {
  display.drawRect(x, y, width, height, SSD1306_WHITE);

  int fillWidth = (width - 2) * constrain(value, 0, 100) / 100;

  if (fillWidth > 0) {
    display.fillRect(
      x + 1, y + 1, fillWidth, height - 2, SSD1306_WHITE
    );
  }
}

void drawPet() {
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);

  if (dead) {
    display.setTextSize(2);
    display.setCursor(22, 10);
    display.println("GAME OVER");

    display.setTextSize(1);
    display.setCursor(15, 40);
    display.println("Starved :(");

    display.display();
    return;
  }

  int battery = getEnergyLevel();

  // Face reflects energy level.
  display.drawCircle(64, 17, 12, SSD1306_WHITE);

  if (battery >= 60) {
    display.fillCircle(60, 14, 2, SSD1306_WHITE);
    display.fillCircle(68, 14, 2, SSD1306_WHITE);
    display.drawLine(59, 21, 64, 24, SSD1306_WHITE);
    display.drawLine(64, 24, 69, 21, SSD1306_WHITE);
  } else if (battery >= 30) {
    display.drawLine(57, 14, 62, 14, SSD1306_WHITE);
    display.drawLine(66, 14, 71, 14, SSD1306_WHITE);
    display.drawLine(60, 22, 68, 22, SSD1306_WHITE);
  } else {
    display.drawLine(57, 15, 62, 15, SSD1306_WHITE);
    display.drawLine(66, 15, 71, 15, SSD1306_WHITE);
    display.drawCircle(64, 22, 2, SSD1306_WHITE);
  }

  display.setTextSize(1);

  display.setCursor(0, 33);
  display.print("Hunger ");
  display.print(hunger);
  display.print("%");

  drawBar(72, 33, 54, 8, hunger);

  display.setCursor(0, 44);
  display.print("Happy  ");
  display.print(happiness);
  display.print("%");

  drawBar(72, 44, 54, 8, happiness);

  display.setCursor(0, 55);
  display.print("Energy ");
  display.print(battery);
  display.print("%");

  display.display();
}



//////




void setup() {
  pinMode(BTN_FEED, INPUT_PULLUP);
  pinMode(BTN_PLAY, INPUT_PULLUP);
  pinMode(BTN_POWER, INPUT_PULLUP);

  analogReadResolution(12);

  prefs.begin("tamagotchi", false);
  loadPet();

  esp_sleep_wakeup_cause_t wakeCause = esp_sleep_get_wakeup_cause();

  if (wakeCause == ESP_SLEEP_WAKEUP_GPIO &&
      rtcMagic == RTC_MAGIC_VALUE) {
    int64_t nowUs = getTimeUs();
    int64_t elapsedUs = nowUs - sleepStartUs;

    if (elapsedUs > 0) {
      applyElapsedTime(elapsedUs / 1000000.0f);
    }
  }

  rtcMagic = 0;

  Wire.begin(9, 10);

  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {
    while (true) delay(1000);
  }

  display.clearDisplay();
  display.display();

  uint32_t now = millis();
  lastUpdate = now;
  lastSave = now;
  lastRender = now;
}




void loop() {
  if (justPressed(feedButton)) {
    feedPet();
  }

  if (justPressed(playButton)) {
    playWithPet();
  }

  if (justPressed(powerButton)) {
    enterDeepSleep(true);
  }

  updatePetWhileAwake();

  uint32_t now = millis();

  if (now - lastRender >= RENDER_INTERVAL_MS) {
    lastRender = now;
    drawPet();
  }

  delay(5);
}









