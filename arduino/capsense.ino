// Capacitive sensor inputs
// Three channels: any one of them will trigger the sequence

// This is modification of:
// http://wordpress.codewrite.co.uk/pic/2014/01/21/cap-meter-with-arduino-uno/

const int CAP_PIN_1 = A2;
const int CAP_PIN_2 = A3;
const int CAP_PIN_3 = A4;
const int CAP_COMMON_PIN = A0; // Common

// Relay outputs

const int SEQ1 = 2;
const int SEQ2 = 4;
const int SEQ3 = 8;
const int SEQ4 = 12;

const float IN_CAP_TO_GND  = 1000; // Reference capacitance

const float WEIGHT = 0.1; // Weight of new sample

const float TRIGGER = 20.0; // Trigger percentage

const int MAX_ADC_VALUE = 1024;

void setup()
{
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);

  pinMode(CAP_COMMON_PIN, OUTPUT);
  //digitalWrite(CAP_COMMON_PIN, LOW);  // This is the default state for outputs

  pinMode(CAP_PIN_1, OUTPUT);
  pinMode(CAP_PIN_2, OUTPUT);
  pinMode(CAP_PIN_3, OUTPUT);
  //digitalWrite(CAP_PIN_1, LOW);
  //digitalWrite(CAP_PIN_2, LOW);
  //digitalWrite(CAP_PIN_3, LOW);

  pinMode(SEQ1, OUTPUT);
  pinMode(SEQ2, OUTPUT);
  pinMode(SEQ3, OUTPUT);
  pinMode(SEQ4, OUTPUT);

  Serial.begin(115200);
}

int skip_count = 10; // Don't trigger on first few measurements
float avg_1 = 0.0;
float avg_2 = 0.0;
float avg_3 = 0.0;

// Halloween sequence

void sequence()
{
  digitalWrite(SEQ1, 1); // Lights
  delay(500);
  digitalWrite(SEQ3, 1);
  delay(100);
  digitalWrite(SEQ2, 1); // Effects
  delay(500);
  digitalWrite(SEQ2, 0);
  digitalWrite(SEQ3, 0);
  delay(52000);
  digitalWrite(SEQ1, 0); // Back to normal
}

void loop()
{
  // Capacitor under test between CAP_PIN_* and CAP_COMMON_PIN
  pinMode(CAP_PIN_1, INPUT);
  pinMode(CAP_PIN_2, INPUT);
  pinMode(CAP_PIN_3, INPUT);

  // Charge caps
  digitalWrite(CAP_COMMON_PIN, HIGH);

  int val_1 = analogRead(CAP_PIN_1);
  int val_2 = analogRead(CAP_PIN_2);
  int val_3 = analogRead(CAP_PIN_3);

  // Clear everything for next measurement
  digitalWrite(CAP_COMMON_PIN, LOW);
  pinMode(CAP_PIN_1, OUTPUT);
  pinMode(CAP_PIN_2, OUTPUT);
  pinMode(CAP_PIN_3, OUTPUT);

  // Calculate capacitance

  float capacitance_1 = (float)val_1 * IN_CAP_TO_GND / (float)(MAX_ADC_VALUE - val_1);
  float capacitance_2 = (float)val_2 * IN_CAP_TO_GND / (float)(MAX_ADC_VALUE - val_2);
  float capacitance_3 = (float)val_3 * IN_CAP_TO_GND / (float)(MAX_ADC_VALUE - val_3);

  // Ignore first NN measurements so that avg learns idle capacitance

  if (0 == skip_count) {
    // Trigger if capacitane is > 20% above normal
    // and if capacitance is at least 10% of reference capacitance (so that unconnected inputs are ignored)
    if ((capacitance_1 > avg_1 + avg_1 * (TRIGGER / 100.0) && capacitance_1 > IN_CAP_TO_GND / 10) ||
        (capacitance_2 > avg_2 + avg_2 * (TRIGGER / 100.0) && capacitance_2 > IN_CAP_TO_GND / 10) ||
        (capacitance_3 > avg_3 + avg_3 * (TRIGGER / 100.0) && capacitance_3 > IN_CAP_TO_GND / 10)) {
      Serial.println("Touch detected!");
      digitalWrite(LED_BUILTIN, HIGH);
      skip_count = 20;
      sequence();
    } else {
      digitalWrite(LED_BUILTIN, LOW);
    }
  } else {
    skip_count--;
  }

  // Update running average of capacitance
  
  if (0.0 == avg_1) {
    // Use first reading as average
    avg_1 = capacitance_1;
    avg_2 = capacitance_2;
    avg_3 = capacitance_3;
  } else {
    // Update average
    avg_1 = avg_1 * (1.0 - WEIGHT) + capacitance_1 * WEIGHT;
    avg_2 = avg_2 * (1.0 - WEIGHT) + capacitance_2 * WEIGHT;
    avg_3 = avg_3 * (1.0 - WEIGHT) + capacitance_3 * WEIGHT;
  }
    
#if 0
  Serial.print("avg_1 = ");
  Serial.print(avg_1);
  Serial.print(" pF, capacitance = ");
  Serial.print(capacitance_1, 3);
  Serial.print(" pF, adc = ");
  Serial.println(val_1);

  Serial.print("avg_2 = ");
  Serial.print(avg_2);
  Serial.print(" pF, capacitance = ");
  Serial.print(capacitance_2, 3);
  Serial.print(" pF, adc = ");
  Serial.println(val_2);

  Serial.print("avg_3 = ");
  Serial.print(avg_3);
  Serial.print(" pF, capacitance = ");
  Serial.print(capacitance_3, 3);
  Serial.print(" pF, adc = ");
  Serial.println(val_3);
#endif

  delay(100);
}

