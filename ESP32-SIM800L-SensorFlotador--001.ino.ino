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
const char server [] = "us-central1-elemenko-103eb.cloudfunctions.net"; // nombre de dominio: example.com, maker.ifttt.com, etc. -- https://us-central1-elemenko-103eb.cloudfunctions.net/app/api/read/cliente1
const char resource [] = "/addData";
//const char server[] = "vsh.pp.ua";
//const char resource[] = "/TinyGSM/logo.txt";
const int port = 80; 

TinyGsm modem(SerialAT);
// Cliente TinyGSM para conexión a Internet
TinyGsmClient client(modem);


void setup() {
  SerialMon.begin(115200);
  delay(100);
  pinMode(I2C_SCL, INPUT);

}

void loop() {
  SerialMon.println("Estado del switch: ");
  SerialMon.println(digitalRead(I2C_SCL));
  if (digitalRead(I2C_SCL) == HIGH) {
    SerialMon.print("OFF");
  }
  else {
    SerialMon.print("ON");
  }
  delay(5000);
}
