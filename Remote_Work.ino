#include <ESP8266WiFi.h>
#include <FirebaseESP8266.h>
#include <Servo.h>

// 🔹 Wi-Fi 설정
#define WIFI_SSID "SSID 입력하기"
#define WIFI_PASSWORD "비밀번호 입력하기"

// 🔹 Firebase 설정
#define API_KEY "Firebase API"
#define DATABASE_URL "Firebase DB 주소"

// 🔹 Firebase 인증 토큰 (필수!)
#define FIREBASE_AUTH "Firebase Auth"

#define SERVO_PIN 2  // GPIO2 (D4 핀)

FirebaseData firebaseData;
FirebaseAuth auth;
FirebaseConfig config;
Servo myServo;

const char* FIREBASE_PATH = "/switch/직원 ID";

// 🔹 Wi-Fi 연결
void connectToWiFi() {
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    Serial.print("WiFi 연결중 ...!!!");
    int count = 0;

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
        count++;

        Serial.print(" count: ");
        Serial.println(count);

        if (count > 30) {  // 15초 동안 연결되지 않으면 실패 메시지 출력
            Serial.println("\n❌ Wi-Fi 연결 실패! SSID와 비밀번호 확인하세요.");
            return;
        }
    }
    Serial.println("\n✅ Wi-Fi 연결 완료!");
}

// 🔹 Firebase 연결 상태 확인 함수
void checkFirebaseConnection() {
    Serial.print("Firebase 연결 테스트... ");

    if (Firebase.ready()) {
        Serial.println("✅ Firebase 정상 연결됨!");
    } else {
        Serial.println("❌ Firebase 연결 실패! API Key 및 Database URL 확인 필요");
    }
}

void setup() {
    Serial.begin(115200);

    // 🔹 Wi-Fi 연결
    connectToWiFi();
    Serial.println("와이파이 연결까지는 완료");

    // 🔹 Firebase 설정
    config.api_key = API_KEY;
    config.database_url = DATABASE_URL;
    config.signer.tokens.legacy_token = FIREBASE_AUTH;  // 🔹 인증 정보 추가
    Firebase.begin(&config, &auth);
    Firebase.reconnectWiFi(true);

    Serial.println("파이어베이스 연결 설정 완료");

    // 🔹 Firebase 연결 테스트
    checkFirebaseConnection();

    // 🔹 서보 모터 설정
    myServo.attach(SERVO_PIN);
    Serial.println("모터 연결 완료");
}

void loop() {
    String path = String(FIREBASE_PATH);
    Serial.println("path값");
    Serial.println(path);

    // 🔹 Firebase에서 값 읽기
    if (Firebase.getBool(firebaseData, path)) {
        bool switchState = firebaseData.boolData();

        Serial.print("Firebase 값: ");
        Serial.println(switchState ? "true" : "false");

        if (switchState) {
            myServo.write(180);  
            delay(1500);
            Firebase.setBool(firebaseData, path, false);  
            myServo.write(0);  
        }
    } else {
        Serial.print("❌ Firebase 읽기 실패: ");
        Serial.println(firebaseData.errorReason());  // 🔹 Firebase 오류 메시지 출력
    }

    delay(5000);  // 🔹 5초마다 Firebase 확인
}
