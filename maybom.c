const int trigPin = 7;
const int echoPin = 8;
const int IN1_PUMP1 = 9;
const int IN2_PUMP1 = 10;
const int ENA_PUMP1 = 11;
const int IN3_PUMP2 = 5;
const int IN4_PUMP2 = 6;
const int ENB_PUMP2 = 3;
const float LEVEL_LOW = 17.0;  
const float LEVEL_HIGH = 13.0;
const float HYSTERESIS = 0.5;
const int PUMP_SPEED = 200;
bool pump1_running = false;
bool pump2_running = false;

void setup() {
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  pinMode(IN1_PUMP1, OUTPUT);
  pinMode(IN2_PUMP1, OUTPUT);
  pinMode(ENA_PUMP1, OUTPUT);

  pinMode(IN3_PUMP2, OUTPUT);
  pinMode(IN4_PUMP2, OUTPUT);
  pinMode(ENB_PUMP2, OUTPUT);

  Serial.begin(9600);
  Serial.println("=== HỆ THỐNG ĐIỀU KHIỂN BƠM NƯỚC ===");
  Serial.println("Mực nước THẤP (>17cm): BƠM 2 BƠM VÀO");
  Serial.println("Mực nước CAO (<13cm): BƠM 1 BƠM RA");
  Serial.println("====================================");
}

float getDistance() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long duration = pulseIn(echoPin, HIGH, 30000);
  
  if (duration == 0) {
    return -1;
  }
  
  return duration * 0.034 / 2;
}

void setPump1State(bool state) {
  if (state) {
    digitalWrite(IN1_PUMP1, HIGH);
    digitalWrite(IN2_PUMP1, LOW);
    analogWrite(ENA_PUMP1, PUMP_SPEED);
    if (!pump1_running) {
      Serial.println(">>> BƠM 1 BẬT (Bơm ra ngoài)");
      pump1_running = true;
    }
  } else {
    analogWrite(ENA_PUMP1, 0);
    digitalWrite(IN1_PUMP1, LOW);
    digitalWrite(IN2_PUMP1, LOW);
    if (pump1_running) {
      Serial.println(">>> BƠM 1 TẮT");
      pump1_running = false;
    }
  }
}

void setPump2State(bool state) {
  if (state) {
    digitalWrite(IN3_PUMP2, HIGH);
    digitalWrite(IN4_PUMP2, LOW);
    analogWrite(ENB_PUMP2, PUMP_SPEED);
    if (!pump2_running) {
      Serial.println(">>> BƠM 2 BẬT (Bơm vào)");
      pump2_running = true;
    }
  } else {
    analogWrite(ENB_PUMP2, 0);
    digitalWrite(IN3_PUMP2, LOW);
    digitalWrite(IN4_PUMP2, LOW);
    if (pump2_running) {
      Serial.println(">>> BƠM 2 TẮT");
      pump2_running = false;
    }
  }
}

void loop() {
  float distanceCm = getDistance();
  if (distanceCm < 0) {
    Serial.println("LỖI: Không đọc được cảm biến!");
    delay(1000);
    return;
  }
  Serial.print("Khoảng cách: ");
  Serial.print(distanceCm, 1);
  Serial.print(" cm | ");
  if (!pump2_running && distanceCm > LEVEL_LOW) {
    Serial.print("Nước THẤP! ");
    setPump2State(true);
  } else if (pump2_running && distanceCm < (LEVEL_LOW - HYSTERESIS)) {
    Serial.print("Nước đã đủ. ");
    setPump2State(false);
  }
  if (!pump1_running && distanceCm < LEVEL_HIGH) {
    Serial.print("Nước CAO! ");
    setPump1State(true);
  } else if (pump1_running && distanceCm > (LEVEL_HIGH + HYSTERESIS)) {
    Serial.print("Nước đã hạ. ");
    setPump1State(false);
  }
  Serial.print("| BƠM 1: ");
  Serial.print(pump1_running ? "BẬT" : "TẮT");
  Serial.print(" | BƠM 2: ");
  Serial.println(pump2_running ? "BẬT" : "TẮT");

  delay(500);
}
