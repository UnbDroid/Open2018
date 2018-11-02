#include <vector>
#include <iostream>
#include <fstream>
#include <math.h>

extern "C" {
    #include <stdio.h>
    #include <stdlib.h> // for atoi
	#include <getopt.h>
	#include <signal.h>
	#include <rc/mpu.h>
	#include <rc/math.h>
	#include <rc/button.h>
	#include <rc/encoder.h>
	#include <rc/time.h>
	#include <rc/motor.h>
	#include <rc/pthread.h>
	#include <rc/spi.h>
	#include <rc/servo.h>
	#include <rc/adc.h>
}

using namespace std;

#define PI 3.14159265f


/*--------------definicoes do SPI---------------*/
	#define CANAL_CHASSIS 0
	#define CANAL_GARRA 1

	#define BARRAMENTO_CHASSIS RC_BB_SPI1_SS1
	#define BARRAMENTO_GARRA RC_BB_SPI1_SS2

	#define SLAVE_MODE	SPI_SLAVE_MODE_AUTO
	#define BUS_MODE	SPI_MODE_0
	#define SPI_SPEED	100000
	string SpiComm(int canal, char* dado, int tamanho_resposta);
/*-----------definicoes dos sensores------------*/
	#define QUANTIDADE_LDR 8
	#define QUANTIDADE_COR 4
	#define TAMANHO_RESPOSTA_CHASSIS 16
	#define SENSORES_LDR 1
	#define SENSORES_COR 2
	
	#define LDRS_FRENTE 0,1
	#define LDRS_DIREITA 2,3
	#define LDRS_ESQUERDA 4,5
	#define LDRS_TRAS 6,7

	#define BRANCO 1 
	#define PRETO 2 
	#define VERDE 3 
	#define AZUL 4

	vector<uint8_t> sensoresLDR(8);
	vector<uint8_t> sensoresCOR(4);

	void imprimeLeituras(int sensores);
	void lerSensoresChassis();
/*----------definicoes signal handling----------*/
	timespec ts_geral;
	static int running;
	[[gnu::unused]] static void __signal_handler( [[gnu::unused]] int dummy);
/*------------definicoes motores----------------*/

	#define MOTOR_FRENTE 1
	#define MOTOR_ESQUERDA 2
	#define MOTOR_DIREITA 3
	#define MOTOR_TRAS 4

	#define TODOS_OS_MOTORES 0

	#define DUTY_CYCLE_MAXIMO 0.725f//normaliza potencia
	#define NORMALIZA_POTENCIA DUTY_CYCLE_MAXIMO/100

	#define DIRECAO_Y 1
	#define DIRECAO_X 2

	#define QUANTIDADE_PULSOS_POR_REV 350
	#define QUANTIDADE_PULSOS_PRECISAO 5
	#define DIAMETRO_DA_RODA 11

	#define POTENCIA_NORMAL 50

	#define SERVO_1 1
	#define SERVO_2 2
	#define SERVO_3 3
	#define SERVO_4 4
	#define POS_MAX_SERVO 0.75f
/*------------definicoes locomocao--------------*/
	#define X_POS 1
	#define X_NEG 2
	#define Y_POS 3
	#define Y_NEG 4

	#define INDICE_FRENTE 0
	#define INDIC_ESQUERDA 1
	#define INDICE_DIREITA 2
	#define INDICE_TRAS 3

	void andaMotores(int direcao);
	int inicializa();
	void andaDistancia(float dist, int eixo);
	bool acabouDeAndar(long long int cont1, long long int cont2, float dist);
	float velocidade_ref[4];
	float pot_ref[4];
	float orientacao_z;
	float Ki, Kp;
/*------------definicoes mpu--------------------*/
	#define I2C_BUS 2
	#define GPIO_INT_PIN_CHIP 3
	#define GPIO_INT_PIN_PIN  21
	static rc_mpu_data_t data;
	void __atualizaOrientacao();
/*----------------------------------------------*/




int main () 
{
	if (inicializa())
	{
		cout << "deu ruim na inicializacao"<<endl;
		return 0;
	}
	cout<<"tentando andar "<<endl;
	andaDistancia(20, X_POS);	
	//string retorno = string(SpiComm(0,(char *)"s",16));
	//cout << retorno <<endl<<retorno.length()<<endl;
	// float pot = 50;
	// rc_motor_set(MOTOR_DIREITA,pot*NORMALIZA_POTENCIA);
	 rc_motor_set(MOTOR_FRENTE,-POTENCIA_NORMAL*NORMALIZA_POTENCIA);
	 rc_motor_set(MOTOR_TRAS,POTENCIA_NORMAL*NORMALIZA_POTENCIA);
	// rc_motor_set(MOTOR_ESQUERDA,-pot*NORMALIZA_POTENCIA);

	 while(running)
	 {
	 	rc_usleep(500000);
		cout<<orientacao_z<<endl;
	 }
	 rc_motor_brake(TODOS_OS_MOTORES);

	//lerSensoresChassis();
	//imprimeLeituras(SENSORES_LDR);
	//imprimeLeituras(SENSORES_COR);
	return 0;
}

int inicializa()
{
	signal(SIGINT, __signal_handler);
	running = 1;
	system("config-pin p9.30 spi");
	system("config-pin p9.31 spi_sclk");
	system("config-pin p9.28 spi_cs");
	system("config-pin p9.29 spi");
	cout << "Spi pins activated" << endl;
	
	rc_mpu_config_t config = rc_mpu_default_config();
	config.i2c_bus = I2C_BUS;

	if(rc_mpu_calibrate_gyro_routine(config)<0){
		cout << "Failed to complete gyro calibration\n";
		return -1;
	}
	cout << "Gyro Calibrated." << endl;
	cout << "\033[1;31m"<<"RODOU COM SUDO MERMAO ? SE NAO VOLTA E RODA COM SUDO"<<"\033[0m"<< endl;
	if(rc_servo_init()) return -1;
	
	config.gpio_interrupt_pin_chip = GPIO_INT_PIN_CHIP;
	config.gpio_interrupt_pin = GPIO_INT_PIN_PIN;
	
	if(rc_mpu_initialize_dmp(&data, config)){
		printf("rc_mpu_initialize_failed\n");
		return -1;
	}
	
	rc_mpu_set_dmp_callback(&__atualizaOrientacao);

	rc_motor_init_freq(RC_MOTOR_DEFAULT_PWM_FREQ);
	rc_encoder_init();
	cout<<"terminou as inicializacoes"<<endl;
	Kp = 3/20;
	Ki = 3/20;
	return 0;
}
/*
void controleVel(int motor)
{
	//Measuring motor speed
	mspeed = (float)(rc_encoder_read(motor) - prevencoderval);
	prevencoderval = rc_encoder_read(motor) ;
	mspeed = mspeed * 1000 / (2 * Ts);

	error = velRef - mspeed;

	integrat = previntegrat + (preverror / 1000000) * Ts;
	previntegrat = integrat;
	preverror = error;
	controllaw = error * Kp + Ki * integrat;

	if(abs(controllaw) > DUTY_CYCLE_MAXIMO)
	{
		controllaw = (abs(controllaw)/controllaw)*DUTY_CYCLE_MAXIMO;
	}

	//Updating motor speed
	Serial.println(mspeed);
	controllaw = abs(controllaw);
	pwm = (controllaw / 12) * 255;
	analogWrite(en, pwm);

	//Measuring sample time
	Ts = micros() - prevtime;
	prevtime = micros();
}
*/
/*
void andaDoInicioAteLinhaVerde()
{
	andaDistancia(10, 50, Y_POS);

}
*/
void __atualizaOrientacao()
{
	orientacao_z = data.dmp_TaitBryan[TB_YAW_Z]*RAD_TO_DEG;
}


void fechaServos()
{
	rc_servo_send_pulse_normalized(SERVO_1,0);
	rc_servo_send_pulse_normalized(SERVO_2,0);
	rc_servo_send_pulse_normalized(SERVO_3,0);
	rc_servo_send_pulse_normalized(SERVO_4,0);
}


void abreServos()
{
	rc_servo_send_pulse_normalized(SERVO_1,POS_MAX_SERVO);
	rc_servo_send_pulse_normalized(SERVO_2,POS_MAX_SERVO);
	rc_servo_send_pulse_normalized(SERVO_3,POS_MAX_SERVO);
	rc_servo_send_pulse_normalized(SERVO_4,POS_MAX_SERVO);
}



void controleAndarReto(int motor_a, int motor_b, bool inicio, float pot)
{
	static float d_enc, integral, integral_passado, erro_passado;
	static long long int tempo_passado;
	float  correcao;
	long long int  dt;
	if(inicio){
		d_enc = 0;
		tempo_passado = 0;
		integral = 0;
		integral_passado = 0;
		erro_passado = 0;
		pot_ref[motor_a-1] = pot;
		pot_ref[motor_b-1] = -pot;
	}
	d_enc = (float)(rc_encoder_read(motor_a) - rc_encoder_read(motor_b));
	dt = rc_timespec_to_micros(ts_geral) - tempo_passado;
	integral = integral_passado + (erro_passado / 1000000) * dt;
	integral_passado = integral;
	erro_passado = d_enc;
	correcao = d_enc * Kp + Ki * integral;
	pot_ref[motor_a-1] = pot_ref[motor_a-1] - correcao;
	pot_ref[motor_b-1] = pot_ref[motor_b-1] + correcao;
	tempo_passado = rc_timespec_to_micros(ts_geral);
}


bool acabouDeAndar(long long int cont1, long long int cont2, float dist)
{
	return ((abs(cont1-cont2)< QUANTIDADE_PULSOS_PRECISAO) && cont1>=round(dist*QUANTIDADE_PULSOS_POR_REV/DIAMETRO_DA_RODA))? true : false;
}

void andaDistancia(float dist,int eixo)
{
	long long int cont_inicial_1 = 0;
	long long int cont_inicial_2 = 0;
	switch(eixo)
	{
		case X_POS:
			cont_inicial_1 = rc_encoder_read(MOTOR_FRENTE);
			cont_inicial_2 = rc_encoder_read(MOTOR_TRAS);
			controleAndarReto(MOTOR_FRENTE,MOTOR_TRAS, true, POTENCIA_NORMAL);
			andaMotores(DIRECAO_X);
			while(!acabouDeAndar(cont_inicial_1,cont_inicial_2,dist) && running)
			{
				cont_inicial_1 = rc_encoder_read(MOTOR_FRENTE);
				cont_inicial_2 = rc_encoder_read(MOTOR_TRAS);
				//cout<<"c1 "<<cont_inicial_1<<" c2 "<<cont_inicial_2<<endl;
				controleAndarReto(MOTOR_FRENTE,MOTOR_TRAS, false, POTENCIA_NORMAL);
			}
			break;
		case X_NEG:
			cont_inicial_1 = rc_encoder_read(MOTOR_FRENTE);
			cont_inicial_2 = rc_encoder_read(MOTOR_TRAS);
			controleAndarReto(MOTOR_FRENTE,MOTOR_TRAS, true, -POTENCIA_NORMAL);
			andaMotores(DIRECAO_X);
			while(!acabouDeAndar(cont_inicial_1,cont_inicial_2,dist) && running)
			{
				cont_inicial_1 = rc_encoder_read(MOTOR_FRENTE);
				cont_inicial_2 = rc_encoder_read(MOTOR_TRAS);
				controleAndarReto(MOTOR_FRENTE,MOTOR_TRAS, false, POTENCIA_NORMAL);

			}

			break;
		case Y_POS:
			cont_inicial_1 = rc_encoder_read(MOTOR_DIREITA);
			cont_inicial_2 = rc_encoder_read(MOTOR_ESQUERDA);
			controleAndarReto(MOTOR_DIREITA,MOTOR_ESQUERDA, true, POTENCIA_NORMAL);
			andaMotores(DIRECAO_Y);
			while(!acabouDeAndar(cont_inicial_1,cont_inicial_2,dist) && running)
			{
				cont_inicial_1 = rc_encoder_read(MOTOR_DIREITA);
				cont_inicial_2 = rc_encoder_read(MOTOR_ESQUERDA);
				controleAndarReto(MOTOR_DIREITA,MOTOR_ESQUERDA, false, POTENCIA_NORMAL);
			}
			break;
		case Y_NEG:
			cont_inicial_1 = rc_encoder_read(MOTOR_DIREITA);
			cont_inicial_2 = rc_encoder_read(MOTOR_ESQUERDA);
			controleAndarReto(MOTOR_DIREITA,MOTOR_ESQUERDA, true, -POTENCIA_NORMAL);
			andaMotores(DIRECAO_Y);
			while(!acabouDeAndar(cont_inicial_1,cont_inicial_2,dist) && running)
			{
				cont_inicial_1 = rc_encoder_read(MOTOR_DIREITA);
				cont_inicial_2 = rc_encoder_read(MOTOR_ESQUERDA);
				controleAndarReto(MOTOR_DIREITA,MOTOR_ESQUERDA, false, -POTENCIA_NORMAL);
			}
			break;
		default:
			break;

	}
	rc_motor_brake(TODOS_OS_MOTORES);
}

void andaMotores(int direcao)
{
	if (direcao == DIRECAO_X)
	{
		(abs(pot_ref[MOTOR_FRENTE-1]*NORMALIZA_POTENCIA) <= DUTY_CYCLE_MAXIMO) ? rc_motor_set(MOTOR_FRENTE,pot_ref[MOTOR_FRENTE-1]*NORMALIZA_POTENCIA) : rc_motor_set(MOTOR_FRENTE,DUTY_CYCLE_MAXIMO);
		(abs(pot_ref[MOTOR_TRAS-1]*NORMALIZA_POTENCIA) <= DUTY_CYCLE_MAXIMO) ? rc_motor_set(MOTOR_TRAS,pot_ref[MOTOR_TRAS-1]*NORMALIZA_POTENCIA) : rc_motor_set(MOTOR_TRAS,DUTY_CYCLE_MAXIMO);
	}else if(direcao == DIRECAO_Y)
	{
		(abs(pot_ref[MOTOR_DIREITA-1]*NORMALIZA_POTENCIA) <= DUTY_CYCLE_MAXIMO) ? rc_motor_set(MOTOR_DIREITA,pot_ref[MOTOR_DIREITA-1]*NORMALIZA_POTENCIA) : rc_motor_set(MOTOR_DIREITA,DUTY_CYCLE_MAXIMO);
		(abs(pot_ref[MOTOR_ESQUERDA-1]*NORMALIZA_POTENCIA) <= DUTY_CYCLE_MAXIMO) ? rc_motor_set(MOTOR_ESQUERDA,pot_ref[MOTOR_ESQUERDA-1]*NORMALIZA_POTENCIA) : rc_motor_set(MOTOR_ESQUERDA,DUTY_CYCLE_MAXIMO);
	}
}

string SpiComm(int canal, char* dado, int tamanho_resposta)
{
	char* test_str = dado;
	int bytes = tamanho_resposta;	// get number of bytes in test string
	uint8_t buf[bytes];		// read buffer
	int ret;			// return value
	// attempt a string send/receive test
	printf("Sending  %d bytes: %s\n", bytes, test_str);
	if(canal == CANAL_CHASSIS){
		if(rc_spi_init_auto_slave(BARRAMENTO_CHASSIS, BUS_MODE, SPI_SPEED)){
			string vec = {'F'};
			return vec;
		}
	
		ret = rc_spi_transfer(BARRAMENTO_CHASSIS, (uint8_t*)test_str, bytes, buf);
	}else if(canal == CANAL_GARRA){
		if(rc_spi_init_auto_slave(BARRAMENTO_GARRA, BUS_MODE, SPI_SPEED)){
			string vec = {'F'};
			return vec;
		}
		ret = rc_spi_transfer(BARRAMENTO_GARRA, (uint8_t*)test_str, bytes, buf);
	}
	if(ret<0){
		printf("send failed\n");
		rc_spi_close(canal);
		string vec = {'F'};
		return vec;
	}
	else printf("Received %d bytes: %s\n",ret, buf);
	string vec = string((char *)buf);
	rc_spi_close(1);
	return vec;
}

void lerSensoresChassis()
{
	string aux = SpiComm(CANAL_CHASSIS,(char *)"s",TAMANHO_RESPOSTA_CHASSIS);
	int j = 0;
	for (int i = 0; (i < signed(aux.length())) && (j < (QUANTIDADE_LDR + QUANTIDADE_COR)); ++i)
	{
		if (aux[i] >= '1' && aux[i] <= ('1'+2))
		{
			if(j<QUANTIDADE_LDR){
				sensoresLDR[j] = aux[i] - '0';
				//printf("%c %d \n",aux[i],sensoresLDR[j]);
			}
			else{
				sensoresCOR[j- QUANTIDADE_LDR] = aux[i] - '0';
				//printf("%c %d j=%d \n",aux[i],sensoresCOR[j- QUANTIDADE_LDR],j);
			}
			j++;
		}
	}
}


void imprimeLeituras(int sensores)
{
	if (sensores == SENSORES_LDR)
	{
		for (int i = 0; i < signed(sensoresLDR.size()); ++i)
		{
			cout<<"sensorLDR "<<i<<"= "<<int(sensoresLDR[i])<<endl;
		}
	}else if(sensores == SENSORES_COR){
		for (int i = 0; i < signed(sensoresCOR.size()); ++i)
		{
			cout<<"sensorCOR "<<i<<"= "<<int(sensoresCOR[i])<<endl;
		}
	}
}

[[gnu::unused]] static void __signal_handler([[gnu::unused]] int dummy)//__attribute__ ((unused)) int dummy)
{
	running=0;
	return;
}
