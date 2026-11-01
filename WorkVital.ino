#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_MLX90614.h>

#include "MAX30105.h"
#include "heartRate.h"

// =============================
// PINS
// =============================

#define SDA_PIN       4
#define SCL_PIN       5

#define MQ135_PIN     1
#define SOS_PIN       6
#define MOTOR_PIN     7

// =============================
// OLED
// =============================

#define SCREEN_WIDTH  128
#define SCREEN_HEIGHT 64
#define OLED_ADDR     0x3C

Adafruit_SSD1306 display(
  SCREEN_WIDTH,
  SCREEN_HEIGHT,
  &Wire,
  -1
);

// =============================
// SENSORS
// =============================

MAX30105 maxSensor;

Adafruit_MLX90614 mlx =
  Adafruit_MLX90614();

// =============================
// BPM
// =============================

long lastBeat = 0;

float bpm = 0;

const byte RATE_SIZE = 4;

byte rates[RATE_SIZE];

byte rateSpot = 0;

// =============================
// SETUP
// =============================

void setup()
{
  Serial.begin(115200);

  delay(1500);

  Serial.println();
  Serial.println("==========================");
  Serial.println("     WORKVITAL START");
  Serial.println("==========================");

  // -------------------------
  // MOTOR
  // -------------------------

  pinMode(MOTOR_PIN, OUTPUT);

  digitalWrite(
    MOTOR_PIN,
    LOW
  );

  // -------------------------
  // SOS
  // -------------------------

  pinMode(
    SOS_PIN,
    INPUT_PULLUP
  );

  // -------------------------
  // MQ135
  // -------------------------

  pinMode(
    MQ135_PIN,
    INPUT
  );

  // -------------------------
  // I2C
  // -------------------------

  Wire.begin(
    SDA_PIN,
    SCL_PIN
  );

  Wire.setClock(100000);

  delay(500);

  // -------------------------
  // OLED
  // -------------------------

  Serial.println(
    "Checking OLED..."
  );

  if (!display.begin(
        SSD1306_SWITCHCAPVCC,
        OLED_ADDR))
  {
    Serial.println(
      "OLED NOT FOUND!"
    );

    while (1)
    {
      delay(1000);
    }
  }

  Serial.println(
    "OLED OK"
  );

  display.clearDisplay();

  display.setTextColor(
    SSD1306_WHITE
  );

  display.setTextSize(2);

  display.setCursor(8, 10);

  display.println(
    "WORKVITAL"
  );

  display.setTextSize(1);

  display.setCursor(35, 40);

  display.println(
    "Starting..."
  );

  display.display();

  delay(1500);

  // -------------------------
  // MLX90614
  // -------------------------

  Serial.println(
    "Checking MLX90614..."
  );

  if (!mlx.begin())
  {
    Serial.println(
      "MLX90614 NOT FOUND!"
    );
  }
  else
  {
    Serial.println(
      "MLX90614 OK"
    );
  }

  // -------------------------
  // MAX30102
  // -------------------------

  Serial.println(
    "Checking MAX30102..."
  );

  if (!maxSensor.begin(
        Wire,
        I2C_SPEED_STANDARD))
  {
    Serial.println(
      "MAX30102 NOT FOUND!"
    );

    while (1)
    {
      delay(1000);
    }
  }

  Serial.println(
    "MAX30102 OK"
  );

  // -------------------------
  // MAX30102 SETUP
  // -------------------------

  maxSensor.setup(
    60,     // LED brightness
    4,      // sample average
    2,      // Red + IR
    100,    // sample rate
    411,    // pulse width
    4096    // ADC range
  );

  maxSensor.setPulseAmplitudeRed(
    0x3F
  );

  maxSensor.setPulseAmplitudeIR(
    0x3F
  );

  maxSensor.setPulseAmplitudeGreen(
    0
  );

  // -------------------------
  // READY
  // -------------------------

  display.clearDisplay();

  display.setTextSize(2);

  display.setCursor(20, 8);

  display.println(
    "READY"
  );

  display.setTextSize(1);

  display.setCursor(15, 38);

  display.println(
    "Place finger on MAX"
  );

  display.display();

  Serial.println();
  Serial.println(
    "SYSTEM READY"
  );
}

// =============================
// LOOP
// =============================

void loop()
{
  // =========================
  // SOS
  // =========================

  bool sosPressed =
    (digitalRead(SOS_PIN) == LOW);

  // =========================
  // MAX30102
  // =========================

  long irValue =
    maxSensor.getIR();

  // =========================
  // BPM CALCULATION
  // =========================

  if (irValue > 30000)
  {
    if (checkForBeat(irValue))
    {
      long currentTime =
        millis();

      long delta =
        currentTime - lastBeat;

      lastBeat =
        currentTime;

      if (delta > 250 &&
          delta < 2000)
      {
        float newBPM =
          60000.0 / delta;

        if (newBPM >= 40 &&
            newBPM <= 180)
        {
          rates[rateSpot] =
            (byte)newBPM;

          rateSpot++;

          if (rateSpot >= RATE_SIZE)
          {
            rateSpot = 0;
          }

          int total = 0;
          int count = 0;

          for (byte i = 0;
               i < RATE_SIZE;
               i++)
          {
            if (rates[i] > 0)
            {
              total += rates[i];
              count++;
            }
          }

          if (count > 0)
          {
            bpm =
              (float)total /
              count;
          }
        }
      }
    }
  }
  else
  {
    bpm = 0;

    lastBeat = 0;

    rateSpot = 0;

    for (byte i = 0;
         i < RATE_SIZE;
         i++)
    {
      rates[i] = 0;
    }
  }

  // =========================
  // TEMPERATURE
  // =========================

  float temperature =
    mlx.readObjectTempC();

  // =========================
  // MQ135
  // =========================

  int mqValue =
    analogRead(MQ135_PIN);

  // =========================
  // ALERT LEVEL
  // =========================

  bool safetyAlert =
    (bpm > 100);

  bool ambulanceAlert =
    (bpm > 110);

  // =========================
  // VIBRATION MOTOR
  // =========================

  if (sosPressed ||
      ambulanceAlert)
  {
    digitalWrite(
      MOTOR_PIN,
      HIGH
    );
  }
  else
  {
    digitalWrite(
      MOTOR_PIN,
      LOW
    );
  }

  // =========================
  // OLED
  // =========================

  display.clearDisplay();

  display.setTextColor(
    SSD1306_WHITE
  );

  display.setTextSize(1);

  display.setCursor(0, 0);

  display.println(
    "WORKVITAL"
  );

  // BPM

  display.setCursor(0, 14);

  display.print(
    "BPM: "
  );

  if (bpm > 0)
  {
    display.println(
      bpm,
      0
    );
  }
  else
  {
    display.println(
      "---"
    );
  }

  // Temperature

  display.setCursor(0, 26);

  display.print(
    "TEMP: "
  );

  if (!isnan(temperature))
  {
    display.print(
      temperature,
      1
    );

    display.println(
      " C"
    );
  }
  else
  {
    display.println(
      "---"
    );
  }

  // MQ

  display.setCursor(0, 38);

  display.print(
    "AIR: "
  );

  display.println(
    mqValue
  );

  // STATUS

  display.setCursor(0, 50);

  if (sosPressed)
  {
    display.println(
      "SOS! EMERGENCY"
    );
  }
  else if (ambulanceAlert)
  {
    display.println(
      "AMBULANCE ALERT"
    );
  }
  else if (safetyAlert)
  {
    display.println(
      "SAFETY WARNING"
    );
  }
  else
  {
    display.println(
      "STATUS: NORMAL"
    );
  }

  display.display();

  // =========================
  // SERIAL MONITOR
  // =========================

  Serial.print(
    "IR: "
  );

  Serial.print(
    irValue
  );

  Serial.print(
    " | BPM: "
  );

  if (bpm > 0)
  {
    Serial.print(
      bpm,
      0
    );
  }
  else
  {
    Serial.print(
      "---"
    );
  }

  Serial.print(
    " | TEMP: "
  );

  if (!isnan(temperature))
  {
    Serial.print(
      temperature,
      1
    );
  }
  else
  {
    Serial.print(
      "---"
    );
  }

  Serial.print(
    " C | MQ135: "
  );

  Serial.print(
    mqValue
  );

  Serial.print(
    " | STATUS: "
  );

  if (sosPressed)
  {
    Serial.print(
      "SOS"
    );
  }
  else if (ambulanceAlert)
  {
    Serial.print(
      "AMBULANCE"
    );
  }
  else if (safetyAlert)
  {
    Serial.print(
      "SAFETY WARNING"
    );
  }
  else
  {
    Serial.print(
      "NORMAL"
    );
  }

  Serial.print(
    " | MOTOR: "
  );

  if (sosPressed ||
      ambulanceAlert)
  {
    Serial.println(
      "ON"
    );
  }
  else
  {
    Serial.println(
      "OFF"
    );
  }

  delay(50);
}
