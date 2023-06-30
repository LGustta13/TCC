/*
  Services.ino - exemplo de uso das funções
  Criado pelo Luis Gustavo, 29 de Novembro, 2022.
*/

/*
  Orientações:
  ESP32 PINOUT(32, 13, 16, 34, 33, 19, 18, 17)
  ARDUINO PINOUT(4, 5, 2, 3, 6, 7, 8, 9)
*/

/*
  Parâmetros de AtuadorServices:
  1. bomba_gatilho_0        \ RELÉ DE BLOQUEIO
  2. bomba_gatilho_1        /
  3. pino_sensor_0          \ PULSOS
  4. pino_sensor_1          /
  5. pino_buzzer            \
  5. pino_led_verde         | BUZZER E LEDS
  7. pino_led_vermelho      |
  8. pino_led_azul          /
*/

#include "AtuadorServices.h"

AtuadorServices esp32(13, -1, 4, -1, 32, 33, 27, 26);

void timeout(void)
{
}

void setup()
{
}

void loop()
{
}

/** Funcao que pega os pulsos do gatilho ZERO
 *  @param - int pino: pino reservado para conectar o sinal de controle
 *  @return - int value: valor entre 0 e 255
 */
int receberPulsosDoGatilhoZero(void);

/** Funcao que pega os pulsos do gatilho UM
 *  @param - int pino: pino reservado para conectar o sinal de controle
 *  @return - int value: valor entre 0 e 255
 */
int receberPulsosDoGatilhoUm(void);

int AtuadorServices::receberPulsosDoGatilhoZero(void)
{
  int valor_porta_analogica = analogRead(_pino_sensor_0);
  return valor_porta_analogica;
}

int AtuadorServices::receberPulsosDoGatilhoUm(void)
{
  int valor_porta_analogica = analogRead(_pino_sensor_1);
  return valor_porta_analogica;
}

/** Função que realiza o processo de bastecimento
 *  As portas de entrada são monitoradas por Threads
 *  @param - float volume_a_ser_abastecido, volume limite
 *  @return - float volume_dos_pulsos, volume calculado
 */
float getPulsosDaBombaPorParalelismo(float volume_a_ser_abastecido);

/** Função que realiza o processo de abastecimento
 *  As portas de entrada são monitoradas por Interrupção
 * @param - float volume_a_ser_abastecido, volume limite
 * @return - float volume_dos_pulsos, volume calculado
 */
float getPulsosDaBombaPorInterrupcao(float volume_a_ser_abastecido);

/** Versão 2*/
float getPulsosDaBombaPorInterrupcao_v2(float volume_a_ser_abastecido);

/** Versão 3*/
float getPulsosDaBombaPorInterrupcao_v3(float volume_a_ser_abastecido);

/** Função que inicializa o processo de abastecimento liberando os relés
 * @param - float volume_a_ser_abastecido, volume limite
 * @return - float volume_dos_pulsos, volume calculado
 */
float realizarAbastecimento(float volume_a_ser_abastecido);

float AtuadorServices::getPulsosDaBombaPorParalelismo(float volume_a_ser_abastecido)
{
  float qtd_1 = 0, in1 = 0, qtd_0 = 0, in0 = 0, qtd_t = 0;
  int timer = (40 * 4);
  volume_a_ser_abastecido -= 23;

  return volume_a_ser_abastecido;
}

float AtuadorServices::getPulsosDaBombaPorInterrupcao_v3(float volume_a_ser_abastecido)
{
  while (true)
  {
    Serial.println(_contador_pulsos_sensor_1);
    // Serial.println(analogRead(_pino_sensor_1));
  }
}

float AtuadorServices::getPulsosDaBombaPorInterrupcao_v2(float volume_a_ser_abastecido)
{
  float qtd_1 = 0;
  float in1 = 0;
  float qtd_0 = 0;
  float in0 = 0;
  float qtd_t = 0;
  float timer = (40 * 4);

  volume_a_ser_abastecido = ((volume_a_ser_abastecido)-23);

  for (int i = 0; i < timer; ++i)
  {
    Serial.println(_contador_pulsos_sensor_0);
    if (_contador_pulsos_sensor_0 > 0 or _contador_pulsos_sensor_1 > 0)
    {
      // Serial.println(_contador_pulsos_sensor_0);
      i = 0;
      //
      //            if (_contador_pulsos_sensor_1 >= in1)
      //            {
      //                in1 = _contador_pulsos_sensor_1;
      //            }
      //            else
      //            {
      //                qtd_1 = (qtd_1 + in1);
      //                in1 = _contador_pulsos_sensor_1;
      //            }
      //
      //            if (_contador_pulsos_sensor_0 >= in0)
      //            {
      //                in0 = _contador_pulsos_sensor_0;
      //            }
      //            else
      //            {
      //                qtd_0 = (qtd_0 + in0);
      //                in0 = _contador_pulsos_sensor_0;
      //            }
      //
      //            qtd_t = (qtd_0 + qtd_1 + in0 + in1);
      //
      //            if (qtd_t >= volume_a_ser_abastecido)
      //            {
      //                i = timer;
      //            }
      //
      //            Serial.println(qtd_t);
    }
    else
    {
      //            Serial.println(qtd_t);
    }
    delay(250);
  }

  return qtd_t;
}

float AtuadorServices::getPulsosDaBombaPorInterrupcao(float volume_a_ser_abastecido)
{
  int timer_ate_desativar_a_bomba = 160;
  int timer_contagem_atual = 0;
  unsigned long millis_do_loop_anterior = 0;
  float fluxo_combustivel_em_Lmin = 0;
  float fluxo_combustivel_em_Lseg = 0;
  float volume_total = 0;

  volume_a_ser_abastecido = ((volume_a_ser_abastecido)-23);

  while (timer_contagem_atual < timer_ate_desativar_a_bomba and volume_total < volume_a_ser_abastecido)
  {
    if (((millis() - millis_do_loop_anterior) >= _tempo_debounce)) //
    {
      fluxo_combustivel_em_Lmin = ((1000.0 / (millis() - millis_do_loop_anterior)) * _contador_pulsos_sensor_0) / _fator_calibracao; // Conversao do valor de pulsos para L/min

      fluxo_combustivel_em_Lseg = fluxo_combustivel_em_Lmin / 60; // Cálculo do volume em L passado pelo sensor
      volume_total += fluxo_combustivel_em_Lseg;                  // Armazenamento do volume

      _contador_pulsos_sensor_0 = 0;      // Reinicializacao do contador de pulsos
      millis_do_loop_anterior = millis(); // Atualizacao da variável tempo_antes
    }

    if (_contador_pulsos_sensor_0 == 0)
    {
      timer_contagem_atual++;
      //            delay(250);
    }
    else
    {
      Serial.println((String) "Volume: " + volume_total + " L");
      timer_contagem_atual = 0;
    }
  }

  _contador_pulsos_sensor_0 = 0;
  return volume_total;
}

float AtuadorServices::realizarAbastecimento(float volume_a_ser_abastecido)
{
  float volume_dos_pulsos;

  attachInterrupt(digitalPinToInterrupt(_pino_sensor_0), subrotina_do_sensor_0, FALLING);
  attachInterrupt(digitalPinToInterrupt(_pino_sensor_1), subrotina_do_sensor_1, FALLING);

  atuarNoReleDeBloqueioUm(HIGH);
  atuarNoReleDeBloqueioZero(HIGH);

  volume_dos_pulsos = getPulsosDaBombaPorInterrupcao_v3(volume_a_ser_abastecido);

  detachInterrupt(digitalPinToInterrupt(_pino_sensor_0));
  detachInterrupt(digitalPinToInterrupt(_pino_sensor_1));

  atuarNoReleDeBloqueioUm(LOW);
  atuarNoReleDeBloqueioZero(LOW);

  return volume_dos_pulsos;
}