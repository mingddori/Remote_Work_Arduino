#include <ESP8266WiFi.h>
#include <FirebaseESP8266.h>
#include <Servo.h>

// 🔹 Wi-Fi 설정
#define WIFI_SSID "SSID 입력"
#define WIFI_PASSWORD "WIFI 비밀번호 입력"

// 🔹 Firebase 설정
#define API_KEY "Fireabse API Key 입력"
#define DATABASE_URL "Firebase Database URL 입력"

// 🔹 Firebase 인증 토큰 (필수!)
#define FIREBASE_AUTH "Firebase Auth Token 입력"

#define SERVO_PIN 2  // GPIO2 (D4 핀)

FirebaseData firebaseData;
FirebaseAuth auth;
FirebaseConfig config;
Servo myServo;

const char* STATE_PATH = "/switch/사원/state";
const char* ANGLE_PATH = "/switch/사원/angle";
const char* TIME_PATH = "/switch/사원원/time";

// 🔹 기본값 설정
int servoAngle = 36;   // 서보모터 기본 회전 각도
int servoTime = 100;   // 서보모터 기본 머무는 시간 (ms)

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

// 🔹 Firebase에서 초기 설정값 가져오기
void fetchInitialValues() {
    Serial.println("🔹 Firebase에서 초기 값 가져오는 중...");

    // 🔹 angle 값 가져오기
    if (Firebase.getInt(firebaseData, ANGLE_PATH)) {
        servoAngle = firebaseData.intData();
        Serial.print("✅ Firebase angle 값: ");
        Serial.println(servoAngle);
    } else {
        Serial.print("❌ Firebase angle 읽기 실패: ");
        Serial.println(firebaseData.errorReason());
    }

    // 🔹 time 값 가져오기
    if (Firebase.getInt(firebaseData, TIME_PATH)) {
        servoTime = firebaseData.intData();
        Serial.print("✅ Firebase time 값: ");
        Serial.println(servoTime);
    } else {
        Serial.print("❌ Firebase time 읽기 실패: ");
        Serial.println(firebaseData.errorReason());
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

    // 🔹 Firebase에서 초기 값 가져오기
    fetchInitialValues();

    // 🔹 서보 모터 설정
    myServo.attach(SERVO_PIN);
    Serial.println("모터 연결 완료");
    myServo.write(0);
    Serial.println("모터 초기 세팅");
}

void loop() {
    // 🔹 Firebase에서 값 읽기
    if (Firebase.getBool(firebaseData, STATE_PATH)) {
        bool switchState = firebaseData.boolData();

        Serial.print("Firebase 값: ");
        Serial.println(switchState ? "true" : "false");

        if (switchState) {
            myServo.write(servoAngle);  
            delay(servoTime);
            myServo.write(0);  
            Firebase.setBool(firebaseData, STATE_PATH, false);  
        }
    } else {
        Serial.print("❌ Firebase 읽기 실패: ");
        Serial.println(firebaseData.errorReason());  // 🔹 Firebase 오류 메시지 출력
    }

    delay(5000);  // 🔹 5초마다 Firebase 확인
}
