/*
Program Sistem Asuransi Jalan Kendaraan (SAJAK)
Tim Banaspati Hackathon Jasa Raharja 2018
*/
// Inisiasi Library yang dipakai
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
// Inisiasi Variabel
const int BUTTON = 4; // Tombol diletakan di D1
const int LED = 0;   // LED dipasang di D2
int BUTTONstate = 0; // Kondisi awal tombol
int Counter = 0;//inisialisasi counter
int premi = 0; // inisialisasi premi
int tarif = 150; // inisialisasi tarif
int d_dum = 250; // waktu untuk delay

// Inisiasi Koneksi ke Internet

const char* ssid = "Andromax"; // Nama Jaringan Wifi
const char* password = "Awasledakan"; // Password Wifi
const char* mqtt_server = "broker.mqtt-dashboard.com"; // Alamat Server MQTT yang digunakan

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[100];
int value = 0;
//Set-up koneksi Wifi
void setup_wifi() {

  delay(10);
  // Mencoba untuk koneksi ke Wifi 
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());
// Mendapatkan alamat pada jaringan
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
 
  if ((char)payload[0] == '1') {
    digitalWrite(BUILTIN_LED, LOW);   
  } else {
    digitalWrite(BUILTIN_LED, HIGH); 
  }

}

void reconnect() {
  // Jika gagal terkoneksi maka akan terus mencoba untuk melakukan koneksi ulang
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Membuat ID pada server MQTT
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Mencoba untuk koneksi ke topic MQTT
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      client.subscribe("banaspati");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup() {
  pinMode(LED, OUTPUT); // LED menjadi mode keluaran untuk tester
  pinMode (BUTTON, INPUT); // Tombol menjadi mode masukan
  
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void loop() {

  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  BUTTONstate = digitalRead(BUTTON);  // Baca status tombol
  if (BUTTONstate == HIGH)  // cek status tombol
    {
      Counter++;
      if (Counter > 100) // cek kondisi counter
      {
        // Jika kurang dari 100Km maka akan membayar tarif dasar contoh 100 ribu rupiah
        premi = Counter * tarif;
        premi = premi + 100000;
        delay(d_dum);
      }
      else
      {
        premi = 100000;
        delay(d_dum);
      }
      
      // kondisi saat tombol ditekan
      digitalWrite(LED, HIGH);
      
      snprintf (msg, 100, "Jarak tempuh : %d km | Biaya premi : Rp %d,00", Counter, premi);
      Serial.print("Publish message: ");
      Serial.println(msg);
      client.publish("banaspati", msg);
    }
}
