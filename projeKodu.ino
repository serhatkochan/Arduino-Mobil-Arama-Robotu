#include <SoftwareSerial.h>                                   //SoftwareSerial kütüphanimizi ekliyoruz.

String agAdi = "KOCHAN";                 //Ağımızın adını buraya yazıyoruz.    
String agSifresi = "2444666668888888";         //Ağımızın şifresini buraya yazıyoruz.


#define rxPin 7                                               //ESP8266 RX pini
#define txPin 13                                               //ESP8266 TX pini


String ip = "184.106.153.149";                                //Thingspeak ip adresi



#define sensor_pin A0 // sicaklik sensoru
int sensor_deger=0;
float voltaj_deger=0;
float sicaklik_deger=0;



#define s0 2  // Renk sensörünün pinlerini atadik
#define s1 3
#define s2 4
#define s3 5
#define out 6
int kRenk=0; // Ölçülen degerleri atayacagimiz integer degerler
int mRenk=0;
int yRenk=0;

#define sol_g 12 // motor sürücü pinleri tanımlandı
#define sol_i 11
#define sag_g 10 //10
#define sag_i 9

#define trigger_pin 8 // mesafe sensoru
#define echo_pin A1
long sure=0;
long mesafe=0;

//#define sensor1 A5 // Çizgi izleyen sensör pinleri
#define sensor2 A4
#define sensor3 A3
#define sensor4 A2 //
//inr sensor1Deger;
int sensor2Deger,sensor3Deger,sensor4Deger;
int sensorOrtalama=0;

#define led A5
int zaman=0; // Topu bulana kadarki zaman zamanı bulacağız. // unsigned long olabilir duruma göre.


SoftwareSerial esp(rxPin, txPin);                             //Seri haberleşme pin ayarlarını yapıyoruzA1



// SETUP FONKSİYONU
void setup() {  
  Serial.begin(9600);
  
   esp.begin(115200);                                          //ESP8266 ile seri haberleşmeyi başlatıyoruz.
  esp.println("AT");                                          //AT komutu ile modül kontrolünü yapıyoruz.
  Serial.println("AT Yollandı");
  while(!esp.find("OK")){                                     //Modül hazır olana kadar bekliyoruz.
    esp.println("AT");
    Serial.println("ESP8266 Bulunamadı.");
  }
  Serial.println("OK Komutu Alındı");
  esp.println("AT+CWMODE=1");                                 //ESP8266 modülünü client olarak ayarlıyoruz.
  while(!esp.find("OK")){                                     //Ayar yapılana kadar bekliyoruz.
    esp.println("AT+CWMODE=1");
    Serial.println("Ayar Yapılıyor....");
  }
  Serial.println("Client olarak ayarlandı");
  Serial.println("Aga Baglaniliyor...");
  esp.println("AT+CWJAP=\""+agAdi+"\",\""+agSifresi+"\"");    //Ağımıza bağlanıyoruz.
  while(!esp.find("OK"));                                     //Ağa bağlanana kadar bekliyoruz.
  Serial.println("Aga Baglandi.");
  delay(1000);

  pinMode(trigger_pin,OUTPUT); // Mesafe sensörü
  pinMode(echo_pin,INPUT);
  
  pinMode(s0,OUTPUT); // Renk sensörü
  pinMode(s1,OUTPUT);
  pinMode(s2,OUTPUT);
  pinMode(s3,OUTPUT);
  pinMode(out,INPUT);
  digitalWrite(s0,HIGH);
  digitalWrite(s1,LOW);

  pinMode(sol_i, OUTPUT); // motor sürücü pinleri tanımlandı
  pinMode(sol_g, OUTPUT);
  pinMode(sag_i, OUTPUT);
  pinMode(sag_g, OUTPUT);
  
  //pinMode(sensor1,INPUT); // çizgi izleyen sensor pinleri tanımlandı
  pinMode(sensor2,INPUT);
  pinMode(sensor3,INPUT);
  pinMode(sensor4,INPUT);
  pinMode(13,OUTPUT);

  pinMode(led,OUTPUT);
  digitalWrite(led,LOW);

} // void setup bitisi

// LOOP DÖNGÜSÜ
void loop() {
  zaman=(millis()/1000); // gecen zamanı bulacaz

        sensor_deger=analogRead(sensor_pin);
        voltaj_deger=(sensor_deger/1023.0)*5000;
        sicaklik_deger=(voltaj_deger/10.0);
        Serial.print("Sicaklik deger: ");
        Serial.println(sicaklik_deger);
        
        esp.println("AT+CIPSTART=\"TCP\",\""+ip+"\",80");           //Thingspeak'e bağlanıyoruz.
        if(esp.find("Error")){                                      //Bağlantı hatası kontrolü yapıyoruz.
          Serial.println("AT+CIPSTART Error");
        }
        String veri = "GET https://api.thingspeak.com/update?api_key=QGKETYZHU6DVYO3Z";   //Thingspeak komutu. Key kısmına kendi api keyimizi yazıyoruz.                                   //Göndereceğimiz sıcaklık değişkeni
        veri += "&field1=";
        veri += String(sicaklik_deger);       
        veri += "&field2=";
        veri += String(zaman);                                      //Göndereceğimiz zaman değişkeni                             
        veri += "\r\n\r\n"; 
        esp.print("AT+CIPSEND=");                                   //ESP'ye göndereceğimiz veri uzunluğunu veriyoruz.
        esp.println(veri.length()+3);
        delay(2000);
        if(esp.find(">")){                                          //ESP8266 hazır olduğunda içindeki komutlar çalışıyor.
          esp.print(veri);                                          //Veriyi gönderiyoruz.
          Serial.println(veri);
          Serial.println("Veri gonderildi.");
          delay(1000);
        }
        Serial.println("Baglantı Kapatildi.");
        esp.println("AT+CIPCLOSE");                                //Bağlantıyı kapatıyoruz
        delay(1000);                                               //Yeni veri gönderimi için 1 dakika bekliyoruz.


  
  /*
  //  CİZGİ
  // en son sensor1==1 diye kontrol edelim deniyelim.
  //sensor1Deger=digitalRead(sensor1); // cizgi sensoru
  sensor2Deger=digitalRead(sensor2);
  sensor3Deger=digitalRead(sensor3); 
  sensor4Deger=digitalRead(sensor4);
  if(sensor2Deger==HIGH || sensor3Deger==HIGH || sensor4Deger==HIGH){
    sensorOrtalama=1;
  }
  else{
    sensorOrtalama=0;
  }
  
  
  digitalWrite(trigger_pin,HIGH); // mesafe sensoru komutlari
  delay(1);
  digitalWrite(trigger_pin,LOW);
  sure=pulseIn(echo_pin,HIGH);
  mesafe=(sure/2)/28.5; // (sure/2)/29.1;
  //kontrolEt();

  
  
  if(sensorOrtalama==1 || mesafe<19 || mesafe>400){
    if(mesafe==0){
      ilerle();
    }
    else if(sensorOrtalama==1){
      dur();
      delay(100);
      geriDon();
      delay(150);
      solYap();
      delay(250); // delay(600);
    }
    
    else if(mesafe<19 && mesafe>9){
      ilerle();
      delay(100);
      dur();
    }
    else if(mesafe>400 || mesafe<10){
      
      kontrolEt();
      if(kontrolEt()==1){
        sensor_deger=analogRead(sensor_pin);
        voltaj_deger=(sensor_deger/1023.0)*5000;
        sicaklik_deger=voltaj_deger/10.0;
        
        esp.println("AT+CIPSTART=\"TCP\",\""+ip+"\",80");           //Thingspeak'e bağlanıyoruz.
        if(esp.find("Error")){                                      //Bağlantı hatası kontrolü yapıyoruz.
          Serial.println("AT+CIPSTART Error");
        }
        String veri = "GET https://api.thingspeak.com/update?api_key=F7WS1SCZNOWTY437";   //Thingspeak komutu. Key kısmına kendi api keyimizi yazıyoruz.                                   //Göndereceğimiz sıcaklık değişkeni
        veri += "&field1=";
        veri += String(sicaklik_deger);       
        veri += "&field2=";
        veri += String(zaman);                                      //Göndereceğimiz zaman değişkeni                             
        veri += "\r\n\r\n"; 
        esp.print("AT+CIPSEND=");                                   //ESP'ye göndereceğimiz veri uzunluğunu veriyoruz.
        esp.println(veri.length()+3);
        delay(2000);
        if(esp.find(">")){                                          //ESP8266 hazır olduğunda içindeki komutlar çalışıyor.
          esp.print(veri);                                          //Veriyi gönderiyoruz.
          Serial.println(veri);
          Serial.println("Veri gonderildi.");
          delay(1000);
        }
        Serial.println("Baglantı Kapatildi.");
        esp.println("AT+CIPCLOSE");                                //Bağlantıyı kapatıyoruz
        delay(1000);                                               //Yeni veri gönderimi için 1 dakika bekliyoruz.
       digitalWrite(led,HIGH);
        while(1);
      
        
      }
      else{
        geriDon();
        delay(200);
        solYap();
        delay(250);
      }
    }
  }
  else{
    ilerle();
  }
  */
}// void loop bitisi

void dur(){
  digitalWrite(sag_i,LOW);
  digitalWrite(sag_g,LOW);
  digitalWrite(sol_i,LOW);
  digitalWrite(sol_g,LOW);  
}
void geriDon(){
  digitalWrite(sag_i,LOW);
  digitalWrite(sag_g,HIGH);
  digitalWrite(sol_i,LOW);
  digitalWrite(sol_g,HIGH);
}
void ilerle(){
  digitalWrite(sag_i,HIGH);
  digitalWrite(sag_g,LOW);
  digitalWrite(sol_i,HIGH);
  digitalWrite(sol_g,LOW);
}
void solYap(){
  digitalWrite(sag_i,HIGH);
  digitalWrite(sag_g,LOW);
  digitalWrite(sol_i,LOW);
  digitalWrite(sol_g,HIGH);
}
void sagYap(){
  digitalWrite(sag_i,LOW);
  digitalWrite(sag_g,HIGH);
  digitalWrite(sol_i,HIGH);
  digitalWrite(sol_g,LOW);
}
int kontrolEt(){
  float ortalamaKirmizi=0;
  float ortalamaYesil=0;
  float ortalamaMavi=0;
  for(int i=0; i<5; i++){
    digitalWrite(s2,LOW); // renk sensoru komutları
    digitalWrite(s3,LOW);
    kRenk=pulseIn(out,LOW); // izlemeye alıyoruz   / degerler dustukce renk yogunlugu artacak
    //kRenk=map(kRenk,100,280,100,0);
    delay(50);
    digitalWrite(s2,HIGH);
    digitalWrite(s3,HIGH);
    yRenk=pulseIn(out,LOW);
    delay(50);
    digitalWrite(s2,LOW);
    digitalWrite(s3,HIGH);
    mRenk=pulseIn(out,LOW);
    delay(50);
   
   
    ortalamaKirmizi+=kRenk/5;
    ortalamaYesil+=yRenk/5;
    ortalamaMavi+=mRenk/5;
    /*
    Serial.print(" Kirmizi= ");
    Serial.print(kRenk);
    Serial.print(" Yesil= ");
    Serial.print(yRenk);
    Serial.print(" Mavi= ");
    Serial.println(mRenk); // renk sensorleri komutları bitisi
    */
  }
  /*
   Serial.print(" ortalamaKirmizi= ");
   Serial.print(ortalamaKirmizi);
   Serial.print(" ortalamaYesil= ");
   Serial.print(ortalamaYesil);
   Serial.print(" ortalamaMavi= ");
   Serial.println(ortalamaMavi); // renk sensorleri komutları bitisi
    */
  /*
   if(ortalamaKirmizi>800 && ortalamaKirmizi<1200 && ortalamaYesil<1700 && ortalamaYesil>1200 && ortalamaMavi<1700 && ortalamaMavi>1300){
    return 1;
   }
   else if(ortalamaKirmizi>500 && ortalamaKirmizi<800 && ortalamaYesil<1600 && ortalamaYesil>1100 && ortalamaMavi>1380 && ortalamaMavi<1800){
    return 1;
   }
   else if(ortalamaKirmizi>700 && ortalamaKirmizi<1100 && ortalamaYesil<2000 && ortalamaYesil>1350 && ortalamaMavi>1500 && ortalamaMavi<2000){
    return 1;
   }
   else if(renklerin aldigi deger araliklari girilecek){
    return 1;
   }
   else{
    return 0; 
   }*/
   /*
   //ortalamaKirmizi<ortalamMavi<ortalamaYesil
   if(ortalamaKirmizi>ortalamaMavi || ortalamaMavi>ortalamaYesil){
    return 0;
   }
   else{
    return 1;
   }
   */
}
