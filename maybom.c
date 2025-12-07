const int trigPin = 7;
const int echoPin = 8;


const int IN1 = 9;   
const int IN2 = 10;
const int IN3 = 5;   
const int IN4 = 6;


const float LEVEL_LOW = 13.0;   // Dưới 13cm -> Bơm 1 bật (bơm ra)
const float LEVEL_HIGH = 17.0;  // Trên 17cm -> Bơm 2 bật (hút vào)
const float HYSTERESIS = 0.5;   // Độ trễ tránh dao động

bool pump1_running = false;
bool pump2_running = false;

void setup() {
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  
  // Cấu hình chân L298N - chỉ cần IN1-4
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  
  Serial.begin(9600);
  Serial.println("=== HỆ THỐNG ĐIỀU KHIỂN BƠM NƯỚC ===");
  Serial.println("Sử dụng 1 module L298N - chỉ dùng IN1-4");
  Serial.println("Mực nước < 13cm: BƠM 1 BẬT (Bơm ra ngoài)");
  Serial.println("Mực nước > 17cm: BƠM 2 BẬT (Hút nước vào)");
  Serial.println("LƯU Ý: Giữ jumper ENA/ENB trên module L298N");
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
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
    if (!pump1_running) {
      Serial.println(">>> BƠM 1 BẬT (Motor A - Bơm ra ngoài)");
      pump1_running = true;
    }
  } else {
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, LOW);
    if (pump1_running) {
      Serial.println(">>> BƠM 1 TẮT (Motor A)");
      pump1_running = false;
    }
  }
}

void setPump2State(bool state) {
  if (state) {
    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);
    if (!pump2_running) {
      Serial.println(">>> BƠM 2 BẬT (Motor B - Hút nước vào)");
      pump2_running = true;
    }
  } else {
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, LOW);
    if (pump2_running) {
      Serial.println(">>> BƠM 2 TẮT (Motor B)");
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
  
  // Điều khiển BƠM 1: Bật khi mực nước < 13cm (bơm ra ngoài)
  if (!pump1_running && distanceCm < LEVEL_LOW) {
    Serial.print("Nước CAO! ");
    setPump1State(true);
    setPump2State(false); // Tắt bơm 2 nếu đang chạy
  } else if (pump1_running && distanceCm > (LEVEL_LOW + HYSTERESIS)) {
    Serial.print("Nước đã hạ. ");
    setPump1State(false);
  }
  
  // Điều khiển BƠM 2: Bật khi mực nước > 17cm (hút nước vào)
  if (!pump2_running && distanceCm > LEVEL_HIGH) {
    Serial.print("Nước THẤP! ");
    setPump2State(true);
    setPump1State(false); // Tắt bơm 1 nếu đang chạy
  } else if (pump2_running && distanceCm < (LEVEL_HIGH - HYSTERESIS)) {
    Serial.print("Nước đã đủ. ");
    setPump2State(false);
  }
  
  Serial.print("| BƠM 1: ");
  Serial.print(pump1_running ? "BẬT" : "TẮT");
  Serial.print(" | BƠM 2: ");
  Serial.println(pump2_running ? "BẬT" : "TẮT");
  
  delay(500);
}
