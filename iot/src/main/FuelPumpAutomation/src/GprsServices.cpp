/*
 GprsServices.cpp - Biblioteca para trabalhar com o GPRS (SIM800L).
 Criado pelo Luis Gustavo, 24 de Fevereiro, 2023.
 Atualizada, 03 de Abril, 2023.
*/

#include "GprsServices.h"

GprsServices::GprsServices(void)
{
  _apn = "simplepm.algar.br";
  _gprsUser = "";
  _gprsPass = "";
  _server = "darwin-gps.com.br";
  _resource = "/api/abastecimentos/getAbastecimentosInternoBomba.php?chave=4eef24c6b8248c2271f6663f44ec0de3c2535ca396a22cf60051137d71721309";

  setupGprs();
}

void GprsServices::setupGprs(void)
{

#define SerialMon Serial
#define SerialAT Serial2
#define TINY_GSM_RX_BUFFER 10000
#define GSM_BAUD 9600
#define TINY_GSM_USE_GPRS true
#define DUMP_AT_COMMANDS

#ifdef DUMP_AT_COMMANDS
#include <StreamDebugger.h>
  StreamDebugger debugger(SerialAT, SerialMon);
  TinyGsm modem(debugger);
#else
  TinyGsm modem(SerialAT);
#endif

#ifdef USE_SSL &&defined TINY_GSM_MODEM_HAS_SSL
  TinyGsmClientSecure client(modem);
  const int port = 443;
#else
  TinyGsmClient client(modem);
  const int port = 80;
#endif

  HttpClient http(client, server, port);
  SerialMon.begin(115200);
  delay(10);
  SerialAT.begin(115200);
  delay(6000);
}

void GprsServices::inicializarGprs(void)
{
  modem.restart();

  String modemInfo = modem.getModemInfo();
  SerialMon.print("Modem Info: ");
  SerialMon.println(modemInfo);

#if TINY_GSM_USE_GPRS
  // Unlock your SIM card with a PIN if needed
  if (GSM_PIN && modem.getSimStatus() != 3)
  {
    modem.simUnlock(GSM_PIN);
  }
#endif
}

bool GprsServices::conectarNaRede(void)
{
  SerialMon.print("Waiting for network...");
  if (!modem.waitForNetwork())
  {
    SerialMon.println(" fail");
    delay(10000);
    return false;
  }
  SerialMon.println(" success");

  if (modem.isNetworkConnected())
  {
    SerialMon.println("Network connected");
  }

#if TINY_GSM_USE_GPRS
  // GPRS connection parameters are usually set after network registration
  SerialMon.print(F("Connecting to "));
  SerialMon.print(apn);
  if (!modem.gprsConnect(apn, gprsUser, gprsPass))
  {
    SerialMon.println(" fail");
    delay(10000);
    return false;
  }
  SerialMon.println(" success");

  if (modem.isGprsConnected())
  {
    SerialMon.println("GPRS connected");
  }
#endif

  return true;
}

void GprsServices::requisicaoHttp(String token, String abastecimentosJson)
{
  SerialMon.print(F("Performing HTTPS GET request... "));
  http.connectionKeepAlive(); // Currently, this is needed for HTTPS
  int err = http.get(resource);
  if (err != 0)
  {
    SerialMon.println(F("failed to connect"));
    delay(10000);
    return;
  }

  int status = http.responseStatusCode();
  SerialMon.print(F("Response status code: "));
  SerialMon.println(status);
  if (!status)
  {
    delay(10000);
    return;
  }

  SerialMon.println(F("Response Headers:"));
  while (http.headerAvailable())
  {
    String headerName = http.readHeaderName();
    String headerValue = http.readHeaderValue();
    SerialMon.println("    " + headerName + " : " + headerValue);
  }

  int length = http.contentLength();
  if (length >= 0)
  {
    SerialMon.print(F("Content length is: "));
    SerialMon.println(length);
  }
  if (http.isResponseChunked())
  {
    SerialMon.println(F("The response is chunked"));
  }

  setBody(http.responseBody());
  SerialMon.println(F("Response:"));
  SerialMon.println(getBody());

  SerialMon.print(F("Body length is: "));
  SerialMon.println(getBody().length());

  http.stop();
  SerialMon.println(F("Server disconnected"));
}

bool GprsServices::desconectarGprs(void)
{
#if TINY_GSM_USE_GPRS
  modem.gprsDisconnect();
  SerialMon.println(F("GPRS disconnected"));
  setBody("");
#endif

  return true;
}

void GprsServices::updateDadosSerial(void)
{
  if (Serial2.available())
  {
    Serial.write(Serial2.read());
  }
  if (Serial.available())
  {
    Serial2.write(Serial.read());
  }
}

String GprsServices::getBody(void)
{
  return _body;
}

void GprsServices::setBody(String body)
{
  _body = body;
}

String GprsServices::getToken(void)
{
  return _token;
}

void GprsServices::setToken(String token)
{
  _token = token;
}