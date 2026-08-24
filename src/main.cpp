#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <OneWire.h>
#include <DallasTemperature.h>

// ==========================================
// Configurações de Rede e MQTT
// ==========================================
const char* ssid = "Romu";
const char* password = "manugay1";
const char* mqtt_server = "10.70.150.187";

// ==========================================
// Definição dos Pinos
// ==========================================
const int pinoPeltier = 5; 
const int pinoFan = 25;
const int pinoSensor = 4;

OneWire oneWire(pinoSensor);
DallasTemperature sensorTemp(&oneWire);

WiFiClient espClient;
PubSubClient client(espClient);

// ==========================================
// Variáveis Globais do Sistema e PID
// ==========================================
double Setpoint = 20.0; 
double Input = 24.0;    
double Output = 0.0;
double erroAtual = 0.0;    

double Kp = 25.0;
double Ki = 1.0;
double Kd = 15.0;

double erroAnterior = 0;
double somaErro = 0;
unsigned long ultimoTempoPID = 0;
const unsigned long tempoAmostragemPID = 200; 

unsigned long tempoAnteriorEnvio = 0;
const long intervaloEnvio = 2000; 

// ==========================================
// Função Manual do PID (Refrigeração)
// ==========================================
void calcularPID() {
  unsigned long tempoAtual = millis();
  unsigned long dt_ms = tempoAtual - ultimoTempoPID;

  if (dt_ms >= tempoAmostragemPID) {
    double dt = (double)dt_ms / 1000.0;
    
    erroAtual = Input - Setpoint; 

    double P = Kp * erroAtual;

    somaErro += erroAtual * dt;
    double I = Ki * somaErro;

    double D = Kd * ((erroAtual - erroAnterior) / dt);

    Output = P + I + D;

    if (Output > 255.0) {
      Output = 255.0;
      somaErro -= erroAtual * dt; 
    } else if (Output < 0.0) {
      Output = 0.0;
      if (erroAtual < 0) somaErro -= erroAtual * dt; 
    }

    erroAnterior = erroAtual;
    ultimoTempoPID = tempoAtual;
  }
}

// ==========================================
// Leitura do Sensor
// ==========================================
double lerTemperatura() {
  sensorTemp.requestTemperatures();
  float temp = sensorTemp.getTempCByIndex(0);
  
  if (temp == DEVICE_DISCONNECTED_C) {
    Serial.println("ALERTA: Falha na leitura do sensor!");
    return Input; 
  }
  return temp;
}

// ==========================================
// Callback MQTT
// ==========================================
void callback(char* topic, byte* payload, unsigned int length) {
  String mensagem = "";
  for (int i = 0; i < length; i++) {
    mensagem += (char)payload[i];
  }
  
  float valorRecebido = mensagem.toFloat();
  String strTopic = String(topic);

  if (strTopic == "geladeira/controle/setpoint") {
    Setpoint = valorRecebido;
  } 
  else if (strTopic == "geladeira/controle/kp") {
    Kp = valorRecebido;
  } 
  else if (strTopic == "geladeira/controle/ki") {
    Ki = valorRecebido;
    somaErro = 0; 
  } 
  else if (strTopic == "geladeira/controle/kd") {
    Kd = valorRecebido;
  }
}

// ==========================================
// Conexão MQTT
// ==========================================
void reconnect() {
  while (!client.connected()) {
    if (client.connect("ESP32_Controlador_Geladeira")) {
      client.subscribe("geladeira/controle/setpoint");
      client.subscribe("geladeira/controle/kp");
      client.subscribe("geladeira/controle/ki");
      client.subscribe("geladeira/controle/kd");
    } else {
      delay(5000);
    }
  }
}

// ==========================================
// Setup
// ==========================================
void setup() {
  Serial.begin(115200);
  
  pinMode(pinoPeltier, OUTPUT);
  pinMode(pinoFan, OUTPUT);

  sensorTemp.begin();

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }

  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

// ==========================================
// Loop Principal
// ==========================================
void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop(); 

  Input = lerTemperatura(); 
  calcularPID();

  int pwmPeltier = (int)Output;
  if (pwmPeltier <= 0) {
    digitalWrite(pinoPeltier, LOW); 
  } else if (pwmPeltier >= 255) {
    digitalWrite(pinoPeltier, HIGH); 
  } else {
    analogWrite(pinoPeltier, pwmPeltier); 
  }
  
  int pwmFan = (pwmPeltier > 0) ? 255 : 0; 
  if (pwmFan > 0) {
    digitalWrite(pinoFan, HIGH);
  } else {
    digitalWrite(pinoFan, LOW);
  }

  unsigned long tempoAtual = millis();
  if (tempoAtual - tempoAnteriorEnvio >= intervaloEnvio) {
    tempoAnteriorEnvio = tempoAtual;

    client.publish("geladeira/sensores/temperatura", String(Input).c_str());
    client.publish("geladeira/controle/pwm", String(Output).c_str());
    client.publish("geladeira/sensores/setpoint_atual", String(Setpoint).c_str());
    client.publish("geladeira/controle/fan_pwm", String(pwmFan).c_str());
    client.publish("geladeira/sensores/erro", String(erroAtual).c_str());
    client.publish("geladeira/sensores/kp_atual", String(Kp).c_str());
    client.publish("geladeira/sensores/ki_atual", String(Ki).c_str());
    client.publish("geladeira/sensores/kd_atual", String(Kd).c_str());
  }
}