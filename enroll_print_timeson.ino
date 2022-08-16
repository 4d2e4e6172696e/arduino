#include <Adafruit_Fingerprint.h>
#include <EEPROM.h>
#if (defined(__AVR__) || defined(ESP8266)) && !defined(__AVR_ATmega2560__)
SoftwareSerial mySerial(2, 3);
#else
#define mySerial Serial1
#endif
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);
uint16_t id;
unsigned long simdikizaman = 0;
unsigned long zaman = 0;
unsigned long ms;
int kimlik = 0 ;
bool touch;
int dk;
bool yanlis = 1;
int mode;
int acik = 0;
bool modee;
void(* resetFunc) (void) = 0;
const int ledPin =  6;//7  63=7-6-8//61=6-7-8//42=6-7-8//46=6-7-8
const int ledPin2 =  7;//6
const int ledPin3 =  8;//8

int buttonState = 0;
int kisi = 0;
void setup()
{
 dk= EEPROM.read(1);
  Serial.println(dk);
  pinMode(ledPin, OUTPUT);
  pinMode(ledPin2, OUTPUT);
  pinMode(ledPin3, OUTPUT);
  digitalWrite(ledPin2, LOW);
  digitalWrite(ledPin, HIGH);
  Serial.begin(9600);
  finger.begin(57600);
  pinMode(13, OUTPUT);
  if (finger.verifyPassword()) {
    Serial.println("sensör bağlı!");
  } else {
    Serial.println("sensör bağlı değil :(");
    while (1) {
      delay(1);
    }
  }
  finger.getTemplateCount();

  if (finger.templateCount == 0) {
    Serial.print("Sensor doesn't contain any fingerprint data. Please run the 'enroll' example.");
  }
  else {
    Serial.println("kayıtlı parmak bekleniyor...");
    Serial.print("Sensorde kayıtlı "); Serial.print(finger.templateCount); Serial.println(" parmak var");
  Serial.print("Araç açıldıktan ");  Serial.print(dk); Serial.println(" dk sonra kapanacak. ");   
 Serial.println("Yeni parmak kaydetmek için --> a");
 Serial.println("Kapanma süresini değiştirmek için --> t");
 Serial.print("Cihazı yeniden başlatmak için --> R ");
  Serial.println("giriniz.");
  }
}
void loop() {
  mode = Serial.read();
 
  if (mode == 'a')
    modee = 1;
  if (mode == 't') {
    dk = -1;
    timer();
  }
  mstodk();
  if (mode == 99) {
    kimlik = 0 ;
    modee == 0;
  }
  if (modee) {
    enr();
  }
  else {
    faruk();
  }
  if (millis() > 4000000000)
  { resetFunc();
  }
  if (mode == 'R') {
    resetFunc();
  }
  simdikizaman = millis();
  //600000=10 dk
  if (simdikizaman - zaman > ms) {
    digitalWrite(ledPin2, LOW);
    digitalWrite(ledPin, HIGH);
    digitalWrite(ledPin3, LOW);

  }


}

uint16_t readnumber(void) {
  uint16_t num = 0;

  while (num == 0 ) {
    while (! Serial.available()) ;
    num = Serial.parseInt();
    mode = Serial.read();
    if (num == 99) {
     
      resetFunc();
    }
  }
  Serial.println(mode);
  return num;
}
void enr()                     // run over and over again
{
  if ( mode != 'b') {
    Serial.println("1 ve 127 arasında id numarası girin çıkmak için 99 girin");
    id = readnumber();
    if (id == 0 && mode != 'b') {// ID #0 not allowed, try again!
      return;
    }
    Serial.print("kayıt numarası #");
    Serial.println(id);
    while (!  getFingerprintEnroll() );
  }
}
uint16_t getFingerprintEnroll() {
  int p = -1;
  Serial.print(id); Serial.println(" numaraya kayıt için geçerli bir parmak bekleniyor #"); 
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
      case FINGERPRINT_OK:
        Serial.println("resim çekildi");
        break;
      case FINGERPRINT_NOFINGER:
       //Serial.println(".");
        break;
      case FINGERPRINT_PACKETRECIEVEERR:
        Serial.println("Communication error");
        break;
      case FINGERPRINT_IMAGEFAIL:
        Serial.println("resim hatalı");
        break;
      default:
        Serial.println("Unknown error");
        break;
    }
  }
  p = finger.image2Tz(1);
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("görüntü dönüştürüldü");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("görüntü çok karışık");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("haberleşme hatası");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("parmak izi özellikleri bulunamadı");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("parmak izi özellikleri bulunamadı");
      return p;
    default:
      Serial.println("bilinmeyen hata");
      return p;
  }
  Serial.println("parmağı çek");
  delay(2000);
  p = 0;
  while (p != FINGERPRINT_NOFINGER) {
    p = finger.getImage();
  }
  Serial.print("ID "); Serial.println(id);
  p = -1;
  Serial.println("ayni parmağı tekrar yerleştirin");
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
      case FINGERPRINT_OK:
        Serial.println("resim çekildi");
        break;
      case FINGERPRINT_NOFINGER:
        Serial.print(".");
        break;
      case FINGERPRINT_PACKETRECIEVEERR:
        Serial.println("haberleşme hatası");
        break;
      case FINGERPRINT_IMAGEFAIL:
        Serial.println("görüntüleme hatası");
        break;
      default:
        Serial.println("bilinmeyen hata");
        break;
    }
  }
  p = finger.image2Tz(2);
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("resim dönüştürüldü");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("resim çok karışık");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("parmak izi özellikleri bulunamadı");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("parmak izi özellikleri bulunamadı");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }
  Serial.print("id için model oluşturuluyor #");  Serial.println(id);
  p = finger.createModel();
  if (p == FINGERPRINT_OK) {
    Serial.println("parmak izi eşleşti!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    return p;
  } else if (p == FINGERPRINT_ENROLLMISMATCH) {
    Serial.println("parmak izi eşleşmedi");
    return p;
  } else {
    Serial.println("Unknown error");
    return p;
  }
  Serial.print("ID "); Serial.println(id);
  p = finger.storeModel(id);
  if (p == FINGERPRINT_OK) {
    Serial.println("parmak kaydedildi!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    return p;
  } else if (p == FINGERPRINT_BADLOCATION) {
    Serial.println("bu konuma kaydedilemedi");
    return p;
  } else if (p == FINGERPRINT_FLASHERR) {
    Serial.println("flash belleğe yazma hatası");
    return p;
  } else {
    Serial.println("Unknown error");
    return p;
  }
  return true;
}
void faruk() {
  getFingerprintIDez();
  delay(50); //don't ned to run this at full speed.
}
uint16_t getFingerprintID() {
  uint16_t p = finger.getImage();
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Resim Çekildi");
      break;
    case FINGERPRINT_NOFINGER:
      Serial.println("Parmak izi yok");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("İletişim hatası");
      return p;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Görüntüleme hatası");
      return p;
    default:
      Serial.println("Bilinmeyen hata");
      return p;
  }  // OK success!
  p = finger.image2Tz();
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("işlenmiş görüntü");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Çok dağınık görüntü");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("İletişim hatası");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Parmak izi özellikleri bulunamadı");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Parmak izi özellikleri bulunamadı");
      return p;
    default:
      //Serial.println("Bilinmeyen hata")";
      return p;
  }  // OK converted!
  p = finger.fingerFastSearch();
  if (p == FINGERPRINT_OK) {
    Serial.println("Bir parmak izi ile eşleşti!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("İletişim hatası");
    return p;
  } else if (p == FINGERPRINT_NOTFOUND) {
    Serial.println("Bir eşleşme bulunamadı");
    return p;
  } else {
    Serial.println("Bilinmeyen hata");
    return p;
  }  // found a match!
  Serial.print("Bulunan Kimlik #"); Serial.print(finger.fingerID);
  Serial.print(" güvenle"); Serial.println(finger.confidence);
  return finger.fingerID;
}// returns -1 if failed, otherwise returns ID #
int getFingerprintIDez() {
  uint16_t p = finger.getImage();
  if (p != FINGERPRINT_OK) return -1;
  p = finger.image2Tz();
  if (p != FINGERPRINT_OK) return -1;
  p = finger.fingerFastSearch();
  if (p != FINGERPRINT_OK) return -1;  // found a match!
  simdikizaman = millis();
  zaman = simdikizaman;
  buttonState = digitalRead(ledPin2);
  kisi = finger.fingerID;
  Serial.print(kisi);
  if (kisi <= 5) {
    Serial.println("  bayram"); //1-5
  } else  if (kisi <= 10) {
    Serial.println("  ilyas");  //6-10
  } else if (kisi <= 15) {
    Serial.println("  evren");  //11-15
  } else  if (kisi <= 20) {
    Serial.println("  metin");   //16-20
  } else if (kisi <= 25) {
    Serial.println("  altan");  //21-25
  } else  if (kisi <= 30) {
    Serial.println("  mustafa n");   //26-30
  } else if (kisi <= 35) {
    Serial.println("  bulent");  //31-35
  } else  if (kisi <= 40) {
    Serial.println("  fatih");   //36-40
  } else if (kisi <= 45) {
    Serial.println("  özgür");  //41-45
  } else  if (kisi <= 50) {
    Serial.println("  gökhan b.");   //46-50
  } else if (kisi <= 55) {
    Serial.println("  yasin");  //51-55
  } else  if (kisi <= 60) {
    Serial.println("  ahmet a.");   //56-60
  } else if (kisi <= 65) {
    Serial.println("  huseyin");  //61-65
  } else  if (kisi <= 70) {
    Serial.println("  fucuk");   //66-70
  } else if (kisi <= 75) {
    Serial.println("  zafer");  //71-75
  } else  if (kisi <= 80) {
    Serial.println( "  mehmet");   //76-80
  } else if (kisi <= 85) {
    Serial.println("  mustafa k.");  //81-85
  } else  if (kisi <= 90) {
    Serial.println("  emre");   //86-90
  } else if (kisi <= 95) {
    Serial.println("  metin");  //91-95
  } /*else  if (kisi <= 100) {
    //Serial.println("");   //96-100
  }*/ else if (kisi <= 105) {
    Serial.println("  gökhan k.");  //101-105
  } else  if (kisi <= 110) {
    Serial.println("  ibo");   //106-110
  } else if (kisi <= 115) {
    Serial.println("  isa");  //111-115
  } else  if (kisi <= 0) {
    Serial.println("  kemal a.");   //116-120
  } else if (kisi <= 5) {
    Serial.println("kagan-talha");  //121-125
  }
  if (buttonState == HIGH) {

    digitalWrite(ledPin2, LOW);
    digitalWrite(ledPin, HIGH);
    digitalWrite(ledPin3, LOW);
    //Serial.println(" LOW");

  } else {

    digitalWrite(ledPin2, HIGH);
    digitalWrite(ledPin, LOW);
    digitalWrite(ledPin3, HIGH);
    //Serial.println("HIGH");
    
  }
  delay(50);
  return finger.fingerID;

}
void timer() {

  Serial.print("dk cinsinden süre(min.5 dk)=");
  while (dk <5)
  {
    dk = Serial.parseInt();
  }
  EEPROM.write(1,dk);
  Serial.println(dk);
}
void mstodk() {
  ms=dk*60000;
  

  //dk = Serial.read();

}
