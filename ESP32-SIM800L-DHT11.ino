// TTGO T-Call pin definitions
#define MODEM_RST            5
#define MODEM_PWKEY          4
#define MODEM_POWER_ON       23
#define MODEM_TX             27
#define MODEM_RX             26
#define I2C_SDA              21
#define I2C_SCL              22

#define BLYNK_HEARTBEAT 30
#define TINY_GSM_MODEM_SIM800

#include <TinyGsmClient.h>
#include <Wire.h>

// Set serial for debug console (to the Serial Monitor, default speed 115200)
#define SerialMon Serial

// Hardware Serial on Mega, Leonardo, Micro
#define SerialAT Serial1

const char apn[]  = "internet";
const char user[] = "";
const char pass[] = "";
// INFO SERVIDOR
const char server [] = "us-central1.cloudfunctions.net"; // nombre de dominio: example.com, maker.ifttt.com, etc.
const char resource [] = "/addData";
//const char server[] = "vsh.pp.ua";
//const char resource[] = "/TinyGSM/logo.txt";
const int port = 80; 

TinyGsm modem(SerialAT);
// Cliente TinyGSM para conexión a Internet
TinyGsmClient client(modem);

void setup(){
  // Set console baud rate
  SerialMon.begin(115200);
  delay(100);
  init000();
  // Set-up modem reset, enable, power pins
  pinMode(MODEM_PWKEY, OUTPUT);
  pinMode(MODEM_RST, OUTPUT);
  pinMode(MODEM_POWER_ON, OUTPUT);

  digitalWrite(MODEM_PWKEY, LOW);
  digitalWrite(MODEM_RST, HIGH);
  digitalWrite(MODEM_POWER_ON, HIGH);

  // Set GSM module baud rate and UART pins
  SerialAT.begin(115200, SERIAL_8N1, MODEM_RX, MODEM_TX);
  delay(3000);

  // Restart takes quite some time
  // To skip it, call init() instead of restart()
  SerialMon.println("Initializing modem...");
  modem.init();

  String modemInfo = modem.getModemInfo();
  SerialMon.print("Modem: ");
  SerialMon.println(modemInfo);

  // Unlock your SIM card with a PIN
  //modem.simUnlock("1234");

  SerialMon.print("Waiting for network...");
  if (!modem.waitForNetwork(40000L)) {
    SerialMon.println(" fail");
    delay(10000);
    return;
  }
  SerialMon.println(" OK");
  if (modem.isNetworkConnected()) {
    SerialMon.println("Network connected");
  }

  SerialMon.print(F("Connecting to APN: "));
  SerialMon.print(apn);
  if (!modem.gprsConnect(apn)) {
    SerialMon.println(" Fail Connect ANP");
    delay(10000);
    return;
  }
  SerialMon.println(" OK Connect ANP!! ......");

}


//-------
const int   watchdog = 1000;        // Fréquence du watchdog - Watchdog frequency
unsigned long previousMillis = millis();
volatile int  flow_frequency; // Mide los pulsos del medidor de flujo
unsigned int  l_hour;         // Calculo litros/horas
unsigned long currentMillis = millis();

void ICACHE_RAM_ATTR flow () { // Función de interrupción
  flow_frequency++;
}

void init000(){
  pinMode(I2C_SCL, INPUT);
  attachInterrupt(digitalPinToInterrupt(I2C_SCL), flow, RISING); // Setup Interrupt
  sei(); // Habilitar interrupciones
  currentMillis = millis();
}

void loop (){
  currentMillis = millis();
  if ( currentMillis - previousMillis > watchdog ) {
    previousMillis = currentMillis;
    // Frecuencia de pulso (Hz) = 7.5Q, Q es el caudal en L / min. (Resultados en un rango de +/- 3%)
    l_hour = (flow_frequency * 60 / 7.5); // (Frecuencia de pulso x 60 min) / 7.5Q = caudal en L/Hora
    flow_frequency = 0;                   // Reinicia Counter
    String caudal = String(l_hour, DEC);            // Imprime litros por horas
    Serial.print(caudal);
    Serial.println(" Litro/Hora");

    String requestData = "?id_sensor=";
    requestData += String(modem.getIMEI());
    requestData += "&id_client=";
    requestData += String("3450");
    requestData += "&liters=";
    requestData += String(l_hour, DEC);
    Serial.println(requestData);
    
    if (!client.connect(server, port)) {
      SerialMon.println(" fail");
      delay(10000);
      return;
    }
    SerialMon.println(" OK Connect Server!! .......");
    // Make a HTTP GET request:
    SerialMon.println("Performing HTTP GET request...");
    client.print(String("GET ") + resource + requestData +" HTTP/1.1\r\n");
    client.print(String("Host: ") + server + "\r\n");
    client.print("Connection: close\r\n\r\n");
    client.println();
  
    uint32_t timeout = millis();
    while (client.connected() && millis() - timeout < 10000L) {
      // Print available data
      while (client.available()) {
        char c = client.read();
        SerialMon.print(c);
        timeout = millis();
      }
    }
    SerialMon.println();
    // Shutdown
    client.stop();
    
  }
}
