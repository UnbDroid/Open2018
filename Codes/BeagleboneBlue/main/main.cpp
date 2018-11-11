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
	#include <rc/cpu.h>
	#include <rc/button.h>
	#include <rc/led.h>
	#include <rc/start_stop.h>
}

using namespace std;

/*--------------definicoes do SPI---------------*/
	#define CANAL_CHASSIS 0
	#define CANAL_GARRA 1

	#define BARRAMENTO_CHASSIS RC_BB_SPI1_SS1
	#define BARRAMENTO_GARRA RC_BB_SPI1_SS2

	#define SLAVE_MODE	SPI_SLAVE_MODE_AUTO
	#define BUS_MODE	SPI_MODE_0
	#define SPI_SPEED	100000
	
	#define ID_SENSORES_CHASSIS (char *)"s"
	#define ID_SENSORES_US (char *)"u"
	#define ID_SENSOR_COR_GARRA (char *)"p"
	#define ID_LIGA_ELETROIMA (char *)"e"
	#define ID_DESLIGA_ELETROIMA (char *)"d"
	#define ID_MEXE_GARRA_Z (char *)"z "
	#define ID_MEXE_GARRA_R (char *)"r "
	#define ID_CHECA_GARRA (char *)"c"
#define QUIT_TIMEOUT_US 1500000 // quit after 1.5 seconds holding pause button
#define QUIT_CHECK_US	100000	// check every 1/10 second
	#define HANDSHAKE_INICIO_US (unsigned char) 253
	#define HANDSHAKE_FIM_US (unsigned char) 254

	#define HANDSHAKE_INICIO_GARRA (unsigned char) 253
	#define HANDSHAKE_FIM_GARRA (unsigned char) 254


	#define TAMANHO_RESPOSTA_CHASSIS 16
	#define TAMANHO_RESPOSTA_US 6
	#define TAMANHO_RESPOSTA_COR_GARRA 5
	#define TAMANHO_RESPOSTA_ELETROIMA 5
	#define TAMANHO_RESPOSTA_MEXE_GARRA 5
	#define TAMANHO_RESPOSTA_CHECA_GARRA 5


	string SpiComm(int canal, char* dado, int tamanho_resposta);
/*-----------definicoes dos sensores------------*/
	#define QUANTIDADE_LDR 8
	#define QUANTIDADE_COR 4
	#define QUANTIDADE_US 2
	
	#define SENSORES_LDR 1
	#define SENSORES_COR 2
	#define SENSORES_US 3
	#define SENSOR_COR_GARRA 4
	
	#define SENSORES_CHASSIS 0

	#define SENSOR_US_DIREITA 0
	#define SENSOR_US_ESQUERDA 1
	
	#define MAXIMO_US 15

	#define NORMALIZA_US MAXIMO_US/256

	#define LDR_FRENTE_ESQ 0
	#define LDR_FRENTE_DIR 1

	#define LDR_DIREITA_BAIXO 2
	#define LDR_DIREITA_CIMA 3
	
	#define LDR_ESQUERDA_BAIXO 4
	#define LDR_ESQUERDA_CIMA 5
	
	#define LDR_TRAS_DIR 6
	#define LDR_TRAS_ESQ 7

	#define TCS_1 0
	#define TCS_2 1
	#define TCS_3 2
	#define TCS_4 3
	#define TCS_5 4
	

	#define BRANCO 1 
	#define PRETO 2 
	#define VERDE 3 
	#define AZUL 4
	#define VERMELHO 5
	
	#define DIST_CONTEINER 10
	#define DIST_NAVIOAZUL 8
	#define DIST_NAVIOVERDE 8
	#define DIST_CONTEINER  10

	

	void voltaInit();
	void imprimeLeituras(int sensores);
	void lerSensores(int sensores);

	vector<uint8_t> sensoresLDR(QUANTIDADE_LDR);
	vector<uint8_t> sensoresCOR(QUANTIDADE_COR);
	uint8_t sensor_cor_garra; 

	#define US_DIREITA 0
	#define US_ESQUERDA 1

	vector<float> sensoresUS(QUANTIDADE_US);

/*----------definicoes signal handling----------*/
	timespec ts_geral;
	static int running;
	[[gnu::unused]] static void __signal_handler( [[gnu::unused]] int dummy);
/*------------definicoes motores----------------*/
	#define MOTOR_FRENTE 2
	#define MOTOR_ESQUERDA 1
	#define MOTOR_DIREITA 3
	#define MOTOR_TRAS 4

	#define TODOS_OS_MOTORES 0

	#define DUTY_CYCLE_MAXIMO 0.425f //normaliza potencia
	#define NORMALIZA_POTENCIA DUTY_CYCLE_MAXIMO/100

	#define DIRECAO_Y 1
	#define DIRECAO_X 2

	#define QUANTIDADE_PULSOS_POR_REV 1400
	#define QUANTIDADE_PULSOS_PRECISAO 5
	#define DIAMETRO_DA_RODA 10.63f

	#define POTENCIA_MAXIMA 100
	#define POTENCIA_NORMAL 75
	#define POTENCIA_FRACA 50

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
	#define INDICE_ESQUERDA 1
	#define INDICE_DIREITA 2
	#define INDICE_TRAS 3
	
	#define DIFERENCA_PULSOS_MAXIMA 40
	#define GIRO_MAXIMO 15

	#define RAZAO_REAL 10/9

	#define PI 3,1415926

//	#define NORMALIZA_GIRO	DIFERENCA_PULSOS_MAXIMA/GIRO_MAXIMO
	
	#define PRIORIDADE_GIRO_FUSAO 1.0f

	#define KI 0.0f
	#define KP 0.25f

	void stateMachine();

	void andaAteAPilhaDeDireita();
	int pegaConteiner();
	void entregaAzul1();
	void entregaVerde1();


	long long int leEncoder(int motor);
	void andaMotores(int direcao);
	int inicializa();
	void andaDistancia(float dist, int eixo);
	bool acabouDeAndarDist(long long int cont1, long long int cont2, float dist);
	bool chegouNaLinhaPorLDR(int eixo, int cor_linha);
	void controleAndarReto(int motor_a, int motor_b, bool inicio, float pot);
	void andaAteALinha(int eixo, int cor_linha);
	float transformaAnguloEmErroEnc(float angulo);
	bool seAlinhou(int eixo,int cor_linha);//SKIRA
	bool seAlinhe(int eixo,int cor_linha);
	void setaPotencia(int motor_a, int pot);
	void andaMotor(int motor);
	void andaDistanciaSemControle(float dist,int eixo);
	float velocidade_ref[4];
	float pot_ref[4];
	float orientacao_z;
	int state = 0;
/*------------definicoes mpu--------------------*/
	#define I2C_BUS 2
	#define GPIO_INT_PIN_CHIP 3
	#define GPIO_INT_PIN_PIN  21
	static rc_mpu_data_t data;
	void __atualizaOrientacao();
/*------------definicoes garra-------------------*/

	bool ligaEletroIma(bool liga);
	int mexeGarra(int eixo, float distancia, bool espera);
	
	#define EIXO_HORIZONTAL 1
	#define EIXO_VERTICAL 2

	#define MAXIMO_GARRA_Z 118
	#define MAXIMO_GARRA_R 1

	#define NORMALIZA_GARRA_Z MAXIMO_GARRA_Z/256
	#define NORMALIZA_GARRA_R MAXIMO_GARRA_R/256

	#define DISTANCIA_ENTRE_US 10.8f

/*-----------definicoes de estrategia------------*/
	float anguloAlinhamentoPorUS(float us_dir, float us_esq);
	
/*------------------------------------------*/
// Set first 2 as INPUT
#define CHIP_input 1

#define color_1 25
#define color_2 17

// Chip number in the beaglebone
#define CHIP_output 3

#define eletro 20

static void __on_pause_release(void)
{

}

void clean_all(/*int chip1, int step, int end_switch, int chip2, int dir*/){
	rc_gpio_cleanup(CHIP_input, color_1);
	rc_gpio_cleanup(CHIP_input, color_2);
	rc_gpio_cleanup(CHIP_output, eletro);	
}

/**
* If the user holds the pause button for 2 seconds, set state to exiting which
* triggers the rest of the program to exit cleanly.
*/
static void __on_pause_press(void)
{
	// toggle betewen paused and running modes
	rc_set_state(RUNNING);
	return;
}


void stateMachine()
{
	andaAteAPilhaDeDireita();
			
}

void andaAteAPilhaDeDireita()
{
	andaDistanciaSemControle(50, Y_POS);//89
	//andaDistanciaSemControle(10, X_POS);
	//andaDistanciaSemControle(25, Y_POS);
	
	

	
}

void voltaInit()
{
	//andaDistanciaSemControle(DIST_CONTEINER , X_POS);
	//andaDistanciaSemControle(100, Y_NEG);
	//andaDistanciaSemControle(70,X_NEG);
}



void startOver()
{
	andaDistancia(DIST_NAVIOAZUL, X_NEG);
}

int pegaConteiner()
{
	andaDistanciaSemControle(DIST_CONTEINER , X_NEG);
	int cor = sensoresCOR[TCS_5];
	return cor;
}

void entregaAzul1()
{
	//giraAzul();
	andaDistancia(DIST_NAVIOAZUL, X_POS);
	//ligaEletroIma(false);
}

void entregaVerde1()
{
	//giraVerde();
	andaDistancia(DIST_NAVIOVERDE, X_NEG);
	//ligaEletroIma(false);
}

bool start_all(){
	// Start first input
	if(rc_gpio_init(CHIP_input, color_1, GPIOHANDLE_REQUEST_INPUT)){
		// Erro, nao foi possivel inicializar a leitura
		return 1;
	};	

	// Start second input	
	if(rc_gpio_init(CHIP_input, color_2, GPIOHANDLE_REQUEST_INPUT)){
		// Erro, nao foi possivel inicializar a leitura
		return 1;
	};	

	if(rc_gpio_init(CHIP_output, eletro, GPIOHANDLE_REQUEST_OUTPUT)){
		// Erro, nao foi possivel inicializar a leitura
		return 1;
	};	
}

bool turn_magnets(bool signal){
	if(rc_gpio_set_value(CHIP_output, eletro, signal))
		return 1;
}

int get_color(){
	int reading_1 = 0, reading_2 = 0;
	
	reading_1 = rc_gpio_get_value(CHIP_input, color_1);
	reading_2 = rc_gpio_get_value(CHIP_input, color_2);
	
	if(reading_1 && reading_2)
		return AZUL;
	else if(reading_1)
		return VERMELHO;
	else if(reading_2)
		return VERDE;
	else 
		return BRANCO;
}

int main () 
{
	if (inicializa())
	{
		cout << "deu ruim na inicializacao"<<endl;
		return 0;
	}
//////////////////////////////////////////////////////////////////////////////////
/*
	anda ate a linha de frente
	anda ate a linha da direita
	vira garra para a direcao dos containers e fica olhando ate pegar leitura deles
	anda ate o meio do containers
	alinha a garra com o container
	ou chega a garra pra frente ou anda o robo pra frente


*/
//////////////////////////////////////////////////////////////////////////////////
/* 	cout<<"tentando andar "<<endl;
	andaDistancia(20, X_POS);	
	string retorno = string(SpiComm(0,(char *)"s",16));
	cout << retorno <<endl<<retorno.length()<<endl;*/
	// float pot = 50;
	//pot_ref[MOTOR_ESQUERDA-1] = 50;
	// for(int i = 0; i < 4; i++)
	// 	pot_ref[i] =POTENCIA_MAXIMA;

	// andaMotores(X_POS);
	
	//andaDistancia(35,X_POS);
	//andaDistancia(15,Y_POS);
	

	//andaDistanciaSemControle(50*RAZAO_REAL, Y_POS);
	
	//rc_usleep(500000);

	if(start_all()){
		clean_all();
		//return 1;
	};	
	
	// Send eletromagnets positive signal
	if(turn_magnets(1)){
		clean_all();
		//return 1;
	};	
	
	stateMachine();
	
	while(running)
	{
		rc_usleep(500000);
	//	cout << orientacao_z << endl;
	}

 	rc_motor_brake(TODOS_OS_MOTORES);
	clean_all();
	/*
	lerSensores(SENSORES_US);
	imprimeLeituras(SENSORES_LDR);
	imprimeLeituras(SENSORES_COR); */
	return 0;
}

int inicializa()
{
	signal(SIGINT, __signal_handler);
	rc_cpu_set_governor(RC_GOV_PERFORMANCE);
	running = 1;	
	system("config-pin p9.30 spi");
	system("config-pin p9.31 spi_sclk");
	system("config-pin p9.28 spi_cs");
	system("config-pin p9.29 spi");
	cout << "Spi pins activated" << endl;
	
	rc_mpu_config_t config = rc_mpu_default_config();
	config.i2c_bus = I2C_BUS;
	config.dmp_auto_calibrate_gyro = 1;
	config.dmp_sample_rate = 200;
	//config.enable_magnetometer = 1;
	if(rc_mpu_calibrate_gyro_routine(config)<0){
		cout << "Failed to complete gyro calibration\n";
		return -1;
	}
	cout << "Gyro Calibrated." << endl;
	std::cout << "\033[1;41m"<<"RODOU COM SUDO MERMAO ? SE NAO VOLTA E RODA COM SUDO"<<"\033[0m"<< endl;
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
	
	for(int i = 0; i < 4; i++)
		pot_ref[i] = 0;
	cout<<"terminou as inicializacoes"<<endl;
	if(rc_button_init(RC_BTN_PIN_PAUSE, RC_BTN_POLARITY_NORM_HIGH,
						RC_BTN_DEBOUNCE_DEFAULT_US)){
		fprintf(stderr,"ERROR: failed to init buttons\n");
		return -1;
	}
	rc_button_set_callbacks(RC_BTN_PIN_PAUSE, __on_pause_press, __on_pause_release);
	rc_set_state(PAUSED);
	while(rc_get_state()!=RUNNING){
		// if the state is RUNNING (instead of PAUSED) then blink!
		rc_usleep(100000);
	}
	return 0;
}

long long int leEncoder(int motor)
{
	switch(motor)
	{
		case MOTOR_FRENTE:
			return rc_encoder_read(MOTOR_FRENTE);
			break;
		case MOTOR_ESQUERDA:
			return rc_encoder_read(MOTOR_ESQUERDA);
			break;
		case MOTOR_DIREITA:
			return -rc_encoder_read(MOTOR_DIREITA);
			break;
		case MOTOR_TRAS:
			return -rc_encoder_read(MOTOR_TRAS);
			break;
		default:
			return 0;		
	}
}
/*
void controleVel(int motor)
{
	//Measuring motor speed
	mspeed = (float)(leEncoder(motor) - prevencoderval);
	prevencoderval = leEncoder(motor) ;
	mspeed = mspeed * 1000 / (2 * Ts);

	error = velRef - mspeed;

	integrat = previntegrat + (preverror / 1000000) * Ts;
	previntegrat = integrat;
	preverror = error;
	controllaw = error * KP + KI * integrat;

	if(abs(controllaw) > DUTY_CYCLE_MAXIMO)
	{
		controllaw = (abs(controllaw)/controllaw)*DUTY_CYCLE_MAXIMO;
	}

	//Updating motor speed
	controllaw = abs(controllaw);
	pwm = (controllaw / 12) * 255;

	//Measuring sample time
	Ts = micros() - prevtime;
	prevtime = micros();
}
*/

void setaPotencia(int motor_a, int pot)//SKIRA
{
	pot_ref[motor_a-1] = pot;
}

void andaMotor(int motor)//SKIRA
{
	switch(motor)
	{
		case MOTOR_FRENTE:
			if(abs(pot_ref[MOTOR_FRENTE-1]*NORMALIZA_POTENCIA) <= DUTY_CYCLE_MAXIMO) 
				rc_motor_set(MOTOR_FRENTE,-pot_ref[MOTOR_FRENTE-1]*NORMALIZA_POTENCIA);
			else
				rc_motor_set(MOTOR_FRENTE,-(abs(pot_ref[MOTOR_FRENTE-1])/(pot_ref[MOTOR_FRENTE-1]))*DUTY_CYCLE_MAXIMO);
			break;
		case MOTOR_TRAS:
			if(abs(pot_ref[MOTOR_TRAS-1]*NORMALIZA_POTENCIA) <= DUTY_CYCLE_MAXIMO)
				rc_motor_set(MOTOR_TRAS,pot_ref[MOTOR_TRAS-1]*NORMALIZA_POTENCIA);
			else
				rc_motor_set(MOTOR_TRAS,(abs(pot_ref[MOTOR_TRAS-1])/(pot_ref[MOTOR_TRAS-1]))*DUTY_CYCLE_MAXIMO);
			break;
		case MOTOR_DIREITA:
				if(abs(pot_ref[MOTOR_DIREITA-1]*NORMALIZA_POTENCIA) <= DUTY_CYCLE_MAXIMO)
					rc_motor_set(MOTOR_DIREITA,pot_ref[MOTOR_DIREITA-1]*NORMALIZA_POTENCIA);
				else
					rc_motor_set(MOTOR_DIREITA,(abs(pot_ref[MOTOR_DIREITA-1])/(pot_ref[MOTOR_DIREITA-1]))*DUTY_CYCLE_MAXIMO);
			break;
		case MOTOR_ESQUERDA:
			if(abs(pot_ref[MOTOR_ESQUERDA-1]*NORMALIZA_POTENCIA) <= DUTY_CYCLE_MAXIMO)	
				rc_motor_set(MOTOR_ESQUERDA,-pot_ref[MOTOR_ESQUERDA-1]*NORMALIZA_POTENCIA);
			else
				rc_motor_set(MOTOR_ESQUERDA,-(abs(pot_ref[MOTOR_ESQUERDA-1])/(pot_ref[MOTOR_ESQUERDA-1]))*DUTY_CYCLE_MAXIMO);
			break;
	}
}

bool seAlinhou(int eixo,int cor_linha)//SKIRA
{
	bool cond = false;	
	switch(eixo){
		case X_POS:{
			cond = (sensoresLDR[LDR_DIREITA_BAIXO] == cor_linha) && (sensoresLDR[LDR_DIREITA_CIMA] == cor_linha);
			return cond;
			break;
		}case X_NEG:{
			cond = (sensoresLDR[LDR_ESQUERDA_BAIXO] == cor_linha) && (sensoresLDR[LDR_ESQUERDA_CIMA] == cor_linha);
			return cond;
			break;
		}case Y_POS:{
			cond = (sensoresLDR[LDR_FRENTE_DIR] == cor_linha) && (sensoresLDR[LDR_FRENTE_ESQ] == cor_linha);
			return cond;
			break;
		}case Y_NEG:{
			cond = (sensoresLDR[LDR_TRAS_DIR] == cor_linha) && (sensoresLDR[LDR_TRAS_ESQ] == cor_linha);
			return cond;
			break;
		}default:
			return false;
			break;
	}
}

bool seAlinhe(int eixo,int cor_linha)//SKIRA
{
	int mot_a=1; //random init values
	int mot_b=1; //random init values 
	int s1 = 0;  //random init values
	int s2 = 0;  //random init values
	int potA = 0, potB = 0;
	//int revEixo = 0;

	bool cond_S1_nS2 = false;
	bool cond_nS1_S2 = false;
	bool cond_nS1_nS2 = false;


	switch(eixo){
		case X_POS:
			mot_a = MOTOR_FRENTE;
			mot_b = MOTOR_TRAS;
			s1 = LDR_DIREITA_CIMA;
			s2 = LDR_DIREITA_BAIXO;
			//revEixo = X_NEG;
			break;
		case X_NEG:
			mot_a = MOTOR_TRAS;
			mot_b = MOTOR_FRENTE;
			s1 = LDR_ESQUERDA_BAIXO;
			s2 = LDR_ESQUERDA_CIMA;
			//revEixo = X_POS;
			break;
		case Y_POS:
			mot_a = MOTOR_ESQUERDA;
			mot_b = MOTOR_DIREITA;
			s1 = LDR_FRENTE_ESQ;
			s2 = LDR_FRENTE_DIR;
			//revEixo = Y_NEG;
			break;
		case Y_NEG:
			mot_a = MOTOR_DIREITA;
			mot_b = MOTOR_ESQUERDA;
			s1 = LDR_TRAS_DIR;
			s2 = LDR_TRAS_ESQ;
			//revEixo = Y_POS;
			break;
		default:
			break;
	}
	while(!seAlinhou(eixo, cor_linha) && running)
	{
		lerSensores(SENSORES_CHASSIS);

		cond_S1_nS2 = (sensoresLDR[s1] == cor_linha)&&(sensoresLDR[s2] != cor_linha);
		cond_nS1_S2 = (sensoresLDR[s2] == cor_linha)&&(sensoresLDR[s1] != cor_linha);
		cond_nS1_nS2 = (sensoresLDR[s2] != cor_linha)&&(sensoresLDR[s1] != cor_linha);

		if(cond_S1_nS2)
		{
			potA = 0;
			potB = POTENCIA_FRACA;
		}
		else if(cond_nS1_S2){
			potB = 0;
			potA = POTENCIA_FRACA;
		}
		else if(cond_nS1_nS2)
		{
			andaAteALinha(eixo, cor_linha);
		}
		setaPotencia(mot_a, potA);
		setaPotencia(mot_b, potB);
		andaMotor(mot_a);
		andaMotor(mot_b);
	}

	////Fine ajustments
	//andaAteALinha(eixo, cor_linha);
	//andaDistancia(5, revEixo);


	rc_motor_brake(TODOS_OS_MOTORES);
	return false;
}


void encaixaALinhaEntreSensores(int eixo,int cor_linha)
{
	//

	/////SKIRA (considera que pode tocar no barco
}

void andaAteAcharOCanto()
{
	///SKIRA
}

float anguloAlinhamentoPorUS(float us_dir, float us_esq)
{
	return atan2((us_dir - us_esq),DISTANCIA_ENTRE_US);
}

bool acabouDeAndarDist(long long int cont1, long long int cont2, float dist)
{
	return /*((abs(cont1-cont2)< QUANTIDADE_PULSOS_PRECISAO) && */ abs(cont1)>=abs(round(dist*QUANTIDADE_PULSOS_POR_REV/(PI*DIAMETRO_DA_RODA)))? true : false;
}

void __atualizaOrientacao()
{
	orientacao_z = data.dmp_TaitBryan[TB_YAW_Z]*RAD_TO_DEG;
}


void fechaServos()
{
	rc_servo_send_pulse_normalized(SERVO_1, 0);
	rc_servo_send_pulse_normalized(SERVO_2, 0);
	rc_servo_send_pulse_normalized(SERVO_3, 0);
	rc_servo_send_pulse_normalized(SERVO_4, 0);
}

void abreServos()
{ 
	rc_servo_send_pulse_normalized(SERVO_1, POS_MAX_SERVO);
	rc_servo_send_pulse_normalized(SERVO_2, POS_MAX_SERVO);
	rc_servo_send_pulse_normalized(SERVO_3, POS_MAX_SERVO);
	rc_servo_send_pulse_normalized(SERVO_4, POS_MAX_SERVO);
}

bool chegouNaLinhaPorLDR(int eixo, int cor_linha)
{
	switch (eixo)
	{
		case X_POS:
			return ((sensoresLDR[LDR_DIREITA_BAIXO] == cor_linha) || (sensoresLDR[LDR_DIREITA_CIMA] == cor_linha));
			break;
		case X_NEG:
			return ((sensoresLDR[LDR_ESQUERDA_BAIXO] == cor_linha) || (sensoresLDR[LDR_ESQUERDA_CIMA] == cor_linha));
			break;
		case Y_POS:
			return ((sensoresLDR[LDR_FRENTE_DIR] == cor_linha) || (sensoresLDR[LDR_FRENTE_ESQ] == cor_linha));
			break;
		case Y_NEG:
			return ((sensoresLDR[LDR_TRAS_DIR] == cor_linha) || (sensoresLDR[LDR_TRAS_ESQ] == cor_linha));
			break;
		default:
			return false;
			break;
	}
}

float transformaAnguloEmErroEnc(float angulo)
{
	return (float)((QUANTIDADE_PULSOS_POR_REV*4*sin((float)((float)RAD_TO_DEG*angulo)/2))/DIAMETRO_DA_RODA);
}

void controleAndarReto(int motor_a, int motor_b, bool inicio, float pot)
{
	static float erro, integral, integral_passado, erro_passado;
	static long long int tempo_passado;
	float  correcao;
	long long int  dt;
	if(inicio){
		erro = 0;
		tempo_passado = 0;
		integral = 0;
		integral_passado = 0;
		erro_passado = 0;
		pot_ref[motor_a-1] = pot;
		pot_ref[motor_b-1] = pot;
	}
	erro = (float)((1-PRIORIDADE_GIRO_FUSAO)*(leEncoder(motor_a) - leEncoder(motor_b))) + (PRIORIDADE_GIRO_FUSAO)*orientacao_z;//transformaAnguloEmErroEnc(orientacao_z)); //fusao entre encoder e giroscopio para andar reto com prioridade maior pro giroscopio
	dt = rc_timespec_to_micros(ts_geral) - tempo_passado;
	integral = integral_passado + (erro_passado / 1000000) * dt;
	integral_passado = integral;
	erro_passado = erro;
	correcao = erro * KP + KI * integral;
	pot_ref[motor_a-1] = pot_ref[motor_a-1] - correcao;
	pot_ref[motor_b-1] = pot_ref[motor_b-1] + correcao;
	tempo_passado = rc_timespec_to_micros(ts_geral);
}

void andaAteALinha(int eixo, int cor_linha)
{
	switch(eixo)
	{
		case X_POS:		
			controleAndarReto(MOTOR_FRENTE,MOTOR_TRAS, true, POTENCIA_NORMAL);
			andaMotores(DIRECAO_X);
			while(!chegouNaLinhaPorLDR(eixo, cor_linha) && running)
			{
				lerSensores(SENSORES_CHASSIS);
				controleAndarReto(MOTOR_FRENTE,MOTOR_TRAS, false, POTENCIA_NORMAL);
				andaMotores(DIRECAO_X);
			}
			rc_motor_brake(MOTOR_TRAS);
			rc_motor_brake(MOTOR_FRENTE);
			break;
		case X_NEG:
			controleAndarReto(MOTOR_FRENTE,MOTOR_TRAS, true, -POTENCIA_NORMAL);
			andaMotores(DIRECAO_X);
			while(!chegouNaLinhaPorLDR(eixo, cor_linha) && running)
			{
				lerSensores(SENSORES_CHASSIS);
				controleAndarReto(MOTOR_FRENTE,MOTOR_TRAS, false, POTENCIA_NORMAL);
				andaMotores(DIRECAO_X);
			}
			rc_motor_brake(MOTOR_TRAS);
			rc_motor_brake(MOTOR_FRENTE);
			break;
		case Y_POS:
			controleAndarReto(MOTOR_DIREITA,MOTOR_ESQUERDA, true, POTENCIA_NORMAL);
			andaMotores(DIRECAO_Y);
			while(!chegouNaLinhaPorLDR(eixo, cor_linha) && running)
			{
				lerSensores(SENSORES_CHASSIS);
				controleAndarReto(MOTOR_DIREITA,MOTOR_ESQUERDA, false, POTENCIA_NORMAL);
				andaMotores(DIRECAO_Y);
			}
			rc_motor_brake(MOTOR_DIREITA);
			rc_motor_brake(MOTOR_ESQUERDA);
			break;
		case Y_NEG:
			controleAndarReto(MOTOR_DIREITA,MOTOR_ESQUERDA, true, -POTENCIA_NORMAL);
			andaMotores(DIRECAO_Y);
			while(!chegouNaLinhaPorLDR(eixo, cor_linha) && running)
			{
				lerSensores(SENSORES_CHASSIS);
				controleAndarReto(MOTOR_DIREITA,MOTOR_ESQUERDA, false, -POTENCIA_NORMAL);
				andaMotores(DIRECAO_Y);
			}
			rc_motor_brake(MOTOR_DIREITA);
			rc_motor_brake(MOTOR_ESQUERDA);
			break;
		default:
			break;

	}
}

void andaDistancia(float dist,int eixo)
{
	long long int cont_inicial_1;
	long long int cont_inicial_2;
	long long int cont_atual_1 = 0;
	long long int cont_atual_2 = 0;
	switch(eixo)
	{
		case X_POS:
			cont_inicial_1 = leEncoder(MOTOR_FRENTE);
			cont_inicial_2 = leEncoder(MOTOR_TRAS);
			cont_atual_1 = cont_inicial_1;
			cont_atual_2 = cont_inicial_2;
			controleAndarReto(MOTOR_FRENTE,MOTOR_TRAS, true, POTENCIA_MAXIMA);
			andaMotores(DIRECAO_X);
			while(!acabouDeAndarDist(cont_atual_1-cont_inicial_1, cont_atual_2-cont_inicial_2, dist) && running)
			{
				cont_atual_1 = leEncoder(MOTOR_FRENTE);
				cont_atual_2 = leEncoder(MOTOR_TRAS);
				controleAndarReto(MOTOR_FRENTE,MOTOR_TRAS, false, POTENCIA_NORMAL);
				andaMotores(DIRECAO_X);
				cout<<"c1x+ "<<cont_atual_1<<" c2 "<<cont_atual_2<<endl;

			}
			rc_motor_brake(MOTOR_FRENTE);
			rc_motor_brake(MOTOR_TRAS);
			break;
		case X_NEG:
			cont_inicial_1 = leEncoder(MOTOR_FRENTE);
			cont_inicial_2 = leEncoder(MOTOR_TRAS);
			cont_atual_1 = cont_inicial_1;
			cont_atual_2 = cont_inicial_2;
			controleAndarReto(MOTOR_FRENTE,MOTOR_TRAS, true, -POTENCIA_MAXIMA);
			andaMotores(DIRECAO_X);
			while(!acabouDeAndarDist(cont_atual_1-cont_inicial_1, cont_atual_2-cont_inicial_2, dist) && running)
			{
				cont_atual_1 = leEncoder(MOTOR_FRENTE);
				cont_atual_2 = leEncoder(MOTOR_TRAS);
				controleAndarReto(MOTOR_FRENTE,MOTOR_TRAS, false, POTENCIA_NORMAL);
				cout<<"c1 x- "<<cont_atual_1<<" c2 "<<cont_atual_2<<endl;

				andaMotores(DIRECAO_X);
			}
			rc_motor_brake(MOTOR_FRENTE);
			rc_motor_brake(MOTOR_TRAS);
			break;
		case Y_POS:
			cont_inicial_1 = leEncoder(MOTOR_DIREITA);
			cont_inicial_2 = leEncoder(MOTOR_ESQUERDA);
			cont_atual_1 = cont_inicial_1;
			cont_atual_2 = cont_inicial_2;
			controleAndarReto(MOTOR_DIREITA,MOTOR_ESQUERDA, true, POTENCIA_MAXIMA);
			andaMotores(DIRECAO_Y);
			cout<<"c1 y+ii "<<cont_atual_1<<" c2 "<<cont_atual_2<<endl;

			while(!acabouDeAndarDist(cont_atual_1-cont_inicial_1, cont_atual_2-cont_inicial_2, dist) && running)
			{
				cont_atual_1 = leEncoder(MOTOR_DIREITA);
				cont_atual_2 = leEncoder(MOTOR_ESQUERDA);
				controleAndarReto(MOTOR_DIREITA,MOTOR_ESQUERDA, false, POTENCIA_NORMAL);
				cout<<"c1 y+ "<<cont_atual_1<<" c2 "<<cont_atual_2<<endl;
				andaMotores(DIRECAO_Y);
			}			
			rc_motor_brake(MOTOR_DIREITA);
			rc_motor_brake(MOTOR_ESQUERDA);
			break;
		case Y_NEG:
			cont_inicial_1 = leEncoder(MOTOR_DIREITA);
			cont_inicial_2 = leEncoder(MOTOR_ESQUERDA);
			cont_atual_1 = cont_inicial_1;
			cont_atual_2 = cont_inicial_2;
			controleAndarReto(MOTOR_DIREITA,MOTOR_ESQUERDA, true, -POTENCIA_MAXIMA);
			andaMotores(DIRECAO_Y);
			while(!acabouDeAndarDist(cont_atual_1-cont_inicial_1, cont_atual_2-cont_inicial_2, dist) && running)
			{
				cont_atual_1 = leEncoder(MOTOR_DIREITA);
				cont_atual_2 = leEncoder(MOTOR_ESQUERDA);
				cout<<"c1y- "<<cont_atual_1<<" c2 "<<cont_atual_2<<endl;
				controleAndarReto(MOTOR_DIREITA,MOTOR_ESQUERDA, false, -POTENCIA_NORMAL);
				andaMotores(DIRECAO_Y);
			}
			rc_motor_brake(MOTOR_DIREITA);
			rc_motor_brake(MOTOR_ESQUERDA);
			break;
		default:
			break;

	}
	//rc_motor_brake(TODOS_OS_MOTORES);
}
void andaDistanciaSemControle(float dist,int eixo)
{
	long long int cont_inicial_1;
	long long int cont_inicial_2;
	long long int cont_atual_1 = 0;
	long long int cont_atual_2 = 0;
	switch(eixo)
	{
		case X_POS:
			cont_inicial_1 = leEncoder(MOTOR_FRENTE);
			cont_inicial_2 = leEncoder(MOTOR_TRAS);
			cont_atual_1 = cont_inicial_1;
			cont_atual_2 = cont_inicial_2;
			controleAndarReto(MOTOR_FRENTE,MOTOR_TRAS, true, POTENCIA_MAXIMA);
			andaMotores(DIRECAO_X);
			while(!acabouDeAndarDist(cont_atual_1-cont_inicial_1, cont_atual_2-cont_inicial_2, dist) && running)
			{
				cont_atual_1 = leEncoder(MOTOR_FRENTE);
				cont_atual_2 = leEncoder(MOTOR_TRAS);
				//controleAndarReto(MOTOR_FRENTE,MOTOR_TRAS, false, POTENCIA_NORMAL);
				//andaMotores(DIRECAO_X);
				cout<<"c1x+ "<<cont_atual_1<<" c2 "<<cont_atual_2<<endl;

			}
			rc_motor_brake(MOTOR_FRENTE);
			rc_motor_brake(MOTOR_TRAS);
			break;
		case X_NEG:
			cont_inicial_1 = leEncoder(MOTOR_FRENTE);
			cont_inicial_2 = leEncoder(MOTOR_TRAS);
			cont_atual_1 = cont_inicial_1;
			cont_atual_2 = cont_inicial_2;
			controleAndarReto(MOTOR_FRENTE,MOTOR_TRAS, true, -POTENCIA_MAXIMA);
			andaMotores(DIRECAO_X);
			while(!acabouDeAndarDist(cont_atual_1-cont_inicial_1, cont_atual_2-cont_inicial_2, dist) && running)
			{
				cont_atual_1 = leEncoder(MOTOR_FRENTE);
				cont_atual_2 = leEncoder(MOTOR_TRAS);
				//controleAndarReto(MOTOR_FRENTE,MOTOR_TRAS, false, POTENCIA_NORMAL);
			cout<<"c1 x- "<<cont_atual_1<<" c2 "<<cont_atual_2<<endl;

				andaMotores(DIRECAO_X);
			}
			rc_motor_brake(MOTOR_FRENTE);
			rc_motor_brake(MOTOR_TRAS);
			break;
		case Y_POS:
			cont_inicial_1 = leEncoder(MOTOR_DIREITA);
			cont_inicial_2 = leEncoder(MOTOR_ESQUERDA);
			cont_atual_1 = cont_inicial_1;
			cont_atual_2 = cont_inicial_2;
			controleAndarReto(MOTOR_DIREITA,MOTOR_ESQUERDA, true, POTENCIA_MAXIMA);
			andaMotores(DIRECAO_Y);
			cout<<"c1 y+ii "<<cont_atual_1<<" c2 "<<cont_atual_2<<endl;

			while(!acabouDeAndarDist(cont_atual_1-cont_inicial_1, cont_atual_2-cont_inicial_2, dist) && running)
			{
				cont_atual_1 = leEncoder(MOTOR_DIREITA);
				cont_atual_2 = leEncoder(MOTOR_ESQUERDA);
				//controleAndarReto(MOTOR_DIREITA,MOTOR_ESQUERDA, false, POTENCIA_NORMAL);
			cout<<"c1 y+ "<<cont_atual_1<<" c2 "<<cont_atual_2<<endl;

				//andaMotores(DIRECAO_Y);
			}			
			rc_motor_brake(MOTOR_DIREITA);
			rc_motor_brake(MOTOR_ESQUERDA);
			break;
		case Y_NEG:
			cont_inicial_1 = leEncoder(MOTOR_DIREITA);
			cont_inicial_2 = leEncoder(MOTOR_ESQUERDA);
			cont_atual_1 = cont_inicial_1;
			cont_atual_2 = cont_inicial_2;
			controleAndarReto(MOTOR_DIREITA,MOTOR_ESQUERDA, true, -POTENCIA_MAXIMA);
			andaMotores(DIRECAO_Y);
			while(!acabouDeAndarDist(cont_atual_1-cont_inicial_1, cont_atual_2-cont_inicial_2, dist) && running)
			{
				cont_atual_1 = leEncoder(MOTOR_DIREITA);
				cont_atual_2 = leEncoder(MOTOR_ESQUERDA);
				cout<<"c1y- "<<cont_atual_1<<" c2 "<<cont_atual_2<<endl;
				//controleAndarReto(MOTOR_DIREITA,MOTOR_ESQUERDA, false, -POTENCIA_NORMAL);
				//andaMotores(DIRECAO_Y);
			}
			rc_motor_brake(MOTOR_DIREITA);
			rc_motor_brake(MOTOR_ESQUERDA);
			break;
		default:
			break;

	}
	//rc_motor_brake(TODOS_OS_MOTORES);
}
//manda potencia pros motores considerando a saturacao
void andaMotores(int direcao)
{
	if (direcao == DIRECAO_X)
	{
		andaMotor(MOTOR_FRENTE);
		andaMotor(MOTOR_TRAS);
	
	}else if(direcao == DIRECAO_Y)
	{
		andaMotor(MOTOR_DIREITA);
		andaMotor(MOTOR_ESQUERDA);
	}
}

// void andaMotores(int direcao) //DEPRECATED
// {
// 	if (direcao == DIRECAO_X)
// 	{
// 		if(abs(pot_ref[MOTOR_FRENTE-1]*NORMALIZA_POTENCIA) <= DUTY_CYCLE_MAXIMO) 
// 			rc_motor_set(MOTOR_FRENTE,pot_ref[MOTOR_FRENTE-1]*NORMALIZA_POTENCIA);
// 		else
// 			rc_motor_set(MOTOR_FRENTE,(abs(pot_ref[MOTOR_FRENTE-1])/(pot_ref[MOTOR_FRENTE-1]))*DUTY_CYCLE_MAXIMO);
		
// 		if(abs(pot_ref[MOTOR_TRAS-1]*NORMALIZA_POTENCIA) <= DUTY_CYCLE_MAXIMO)
// 			rc_motor_set(MOTOR_TRAS,pot_ref[MOTOR_TRAS-1]*NORMALIZA_POTENCIA);
// 		else
// 			rc_motor_set(MOTOR_TRAS,(abs(pot_ref[MOTOR_TRAS-1])/(pot_ref[MOTOR_TRAS-1]))*DUTY_CYCLE_MAXIMO);
// 	}else if(direcao == DIRECAO_Y)
// 	{
		
// 		if(abs(pot_ref[MOTOR_DIREITA-1]*NORMALIZA_POTENCIA) <= DUTY_CYCLE_MAXIMO)
// 			rc_motor_set(MOTOR_DIREITA,pot_ref[MOTOR_DIREITA-1]*NORMALIZA_POTENCIA);
// 		else
// 			rc_motor_set(MOTOR_DIREITA,(abs(pot_ref[MOTOR_DIREITA-1])/(pot_ref[MOTOR_DIREITA-1]))*DUTY_CYCLE_MAXIMO);		
		
// 		if(abs(pot_ref[MOTOR_ESQUERDA-1]*NORMALIZA_POTENCIA) <= DUTY_CYCLE_MAXIMO)	
// 			rc_motor_set(MOTOR_ESQUERDA,pot_ref[MOTOR_ESQUERDA-1]*NORMALIZA_POTENCIA);
// 		else
// 			rc_motor_set(MOTOR_ESQUERDA,(abs(pot_ref[MOTOR_ESQUERDA-1])/(pot_ref[MOTOR_ESQUERDA-1]))*DUTY_CYCLE_MAXIMO);
// 	}
// }

string SpiComm(int canal, char* dado, int tamanho_resposta)
{
	char* test_str = dado;
	int bytes = tamanho_resposta;	// get number of bytes in test string
	uint8_t buf[bytes];		// read buffer
	int ret;			// return value
	// attempt a string send/receive test
	printf("Sending  %d bytes: %s\n", bytes, test_str);
	if(canal == CANAL_CHASSIS){
		if(rc_spi_init_auto_slave(BARRAMENTO_CHASSIS, BUS_MODE, SPI_SPEED))
		{
			string vec = {'F'};
			return vec;
		}
	
		ret = rc_spi_transfer(BARRAMENTO_CHASSIS, (uint8_t*)test_str, bytes, buf);
	}else if(canal == CANAL_GARRA){
		if(rc_spi_init_auto_slave(BARRAMENTO_GARRA, BUS_MODE, SPI_SPEED))
		{
			string vec = {'F'};
			return vec;
		}
		ret = rc_spi_transfer(BARRAMENTO_GARRA, (uint8_t*)test_str, bytes, buf);
	}
	if(ret<0)
	{
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

int mexeGarra(int eixo, float distancia, bool espera)
{
	if (!espera) 
	{
		switch (eixo)
		{
			case EIXO_VERTICAL:{
				char * msg = ID_MEXE_GARRA_Z;
				msg[1] = (round(abs(distancia * NORMALIZA_GARRA_Z))>=127) ? (char)round(distancia * NORMALIZA_GARRA_Z) : (char)(distancia/abs(distancia))*127;  
				string aux = SpiComm(CANAL_GARRA, msg, TAMANHO_RESPOSTA_MEXE_GARRA);
				for (int i = 0; (i < signed(aux.length())); ++i)
				{
					if (aux[i] == '1')
						return 1;
				}
				break;
			}
			case EIXO_HORIZONTAL:{			
				char * msg = ID_MEXE_GARRA_R;
				msg[1] = (round(abs(distancia * NORMALIZA_GARRA_R))<=127) ? (char)round(distancia * NORMALIZA_GARRA_R) : (char)(distancia/abs(distancia))*127;
				string aux = SpiComm(CANAL_GARRA, msg, TAMANHO_RESPOSTA_MEXE_GARRA);
				for (int i = 0; (i < signed(aux.length())); ++i)
				{
					if (aux[i] == '1')
						return 1;
				}
				break;
			}	
			default:
				break;
		}
	}else{
		string aux = SpiComm(CANAL_GARRA, ID_CHECA_GARRA, TAMANHO_RESPOSTA_CHECA_GARRA);
		for (int i = 0; i < signed(aux.length()); ++i)
		{
			if (aux[i] != HANDSHAKE_INICIO_GARRA && aux[i] != HANDSHAKE_FIM_GARRA)
			{
				return (int)aux[i]; 
			}
		}
		return -1;
	}
	return -1;
}

bool ligaEletroIma(bool liga)
{
	if(liga)
	{
		string aux = SpiComm(CANAL_GARRA, ID_LIGA_ELETROIMA, TAMANHO_RESPOSTA_ELETROIMA);
		for (int i = 0; (i < signed(aux.length())); ++i)
		{
			if (aux[i] == '1')
				return true;
		}
	}else{
		string aux = SpiComm(CANAL_GARRA, ID_DESLIGA_ELETROIMA, TAMANHO_RESPOSTA_ELETROIMA);
		for (int i = 0; (i < signed(aux.length())); ++i)
		{
			if (aux[i] == '1')
				return true;
		}
	}
	return false;
}


void lerSensores(int sensor)
{
	switch(sensor)
	{
		case SENSORES_CHASSIS:{
			string aux = SpiComm(CANAL_CHASSIS, ID_SENSORES_CHASSIS, TAMANHO_RESPOSTA_CHASSIS);
			int j = 0;
			for (int i = 0; (i < signed(aux.length())) && (j < (QUANTIDADE_LDR + QUANTIDADE_COR)); ++i)
			{
				if (aux[i] >= '1' && aux[i] <= ('1'+3))
				{
					if(j<QUANTIDADE_LDR)
					{
						sensoresLDR[j] = aux[i] - '0';
					}
					else{
						sensoresCOR[j- QUANTIDADE_LDR] = aux[i] - '0';
					}
					j++;
				}
			}
			break;
		}
		case SENSORES_US:{
			string aux = SpiComm(CANAL_GARRA, ID_SENSORES_US, TAMANHO_RESPOSTA_US);
			int j = 0;
			for (int i = 0; (i < signed(aux.length())) && (j < (QUANTIDADE_US)); ++i)
			{
				if (aux[i] != HANDSHAKE_INICIO_US && aux[i] != HANDSHAKE_FIM_US)
				{
					if(j<QUANTIDADE_US)
					{
						sensoresUS[j] = (float) aux[i]*NORMALIZA_US;
					}
					j++;
				}
			}
			break;
		}
		case SENSOR_COR_GARRA:{
			string aux = SpiComm(CANAL_GARRA, ID_SENSOR_COR_GARRA, TAMANHO_RESPOSTA_COR_GARRA);
			int j = 0;
			for (int i = 0; (i < signed(aux.length())) && (j <= 0); ++i)
			{
				if (aux[i] >= '1' && aux[i] <= ('1' + 4))
				{
					sensor_cor_garra =  aux[i]-'0';
					j++;
				}
			}
			break;
		}	
	}
}


void imprimeLeituras(int sensores)
{
	switch(sensores)
	{
		case SENSORES_LDR:
			for (int i = 0; i < signed(sensoresLDR.size()); ++i)
				cout<<"sensorLDR "<<i<<"= "<<int(sensoresLDR[i])<<endl;
			break;
		case SENSORES_COR:
			for (int i = 0; i < signed(sensoresCOR.size()); ++i)
				cout<<"sensorCOR "<<i<<"= "<<int(sensoresCOR[i])<<endl;
		case SENSORES_US:
			for (int i = 0; i < signed(sensoresUS.size()); ++i)
				cout<<"sensorUS "<<i<<"= "<<float(sensoresUS[i])<<endl;
			break;
		default:
			break;
	}
}

[[gnu::unused]] static void __signal_handler([[gnu::unused]] int dummy)//__attribute__ ((unused)) int dummy)
{
	running=0;
	return;
}
