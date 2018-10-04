#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <string>
#include <iomanip>
#include <cstring.h>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <fstream>

using namespace cv;
using namespace std;


#define MIN_L_B 0
#define MAX_L_B 255

#define MIN_A_B 0
#define MAX_A_B 255

#define MIN_B_B 0
#define MAX_B_B 120


#define MIN_L_G 0
#define MAX_L_G 255

#define MIN_A_G 0
#define MAX_A_G 103

#define MIN_B_G 0
#define MAX_B_G 255


#define MIN_L_R 0
#define MAX_L_R 255

#define MIN_A_R 150
#define MAX_A_R 255

#define MIN_B_R 0
#define MAX_B_R 255

/**************/
#define LabRminthresh 150
#define LabRmaxthresh 255

#define LabGminthresh 0
#define LabGmaxthresh 103

#define LabBminthresh 0
#define LabBmaxthresh 120

/* Funcao que utiliza uma struct do tipo timeval e retorna o intervalo de tempo, em microsegundos,
   que se passaram desde a epoch do linux														*/
long long unsigned int  getMicrotime(){
	struct timeval currentTime;
	gettimeofday(&currentTime, NULL);
	return currentTime.tv_sec * (int)1e6 + currentTime.tv_usec;
}

/* Mostra a imagem 'image' na tela 'im' */
void show(Mat image, string im){
	namedWindow(im, WINDOW_NORMAL);
	imshow(im, image);
	resizeWindow(im, 600, 600);
}

/* Recebe como parametro uma imagem em BGR e utiliza o algoritmo de Sobel para encontrar bordas,
   retornando uma imagem CV_8UC1, Preto e Branco */
Mat sobelBorder(Mat image)
{
  Mat image_gray, image_src;
  // Acho que pode omitir essa linha abaixo e fazer a transformacao de cor diretamente na imagem parametro
  image_src = image;
  // Variaveis utilizadas na transformada de Sobel
  int scale = 1;
  int delta = 0;
  int ddepth = CV_16S;

  // Converte a imagem base para o espaco gray (preto e branco)
  // cvtColor( image_src, image_gray, CV_BGR2GRAY );

  // Define os dois resultados da transformacao, em y e em x e os respectivos valores absolutos
  Mat grad_x, grad_y, grad;
  Mat abs_grad_x, abs_grad_y;

  // Calculos em x
  //Scharr( src_gray, grad_x, ddepth, 1, 0, scale, delta, BORDER_DEFAULT );
  Sobel( image, grad_x, ddepth, 1, 0, 3, scale, delta, BORDER_DEFAULT );
  convertScaleAbs( grad_x, abs_grad_x );

  // Calculos em y
  //Scharr( src_gray, grad_y, ddepth, 0, 1, scale, delta, BORDER_DEFAULT );
  Sobel( image, grad_y, ddepth, 0, 1, 3, scale, delta, BORDER_DEFAULT );
  convertScaleAbs( grad_y, abs_grad_y );

  // Aproxima o gradiante total como uma media ponderada dos dois gradientes em x e em y
  addWeighted( abs_grad_x, 0.5, abs_grad_y, 0.5, 0, grad);

  // Mostra o gradiente final e retorna o seu valor
  show(grad, "imagem 2");
  return grad;
}

// Realiza um threshold e uma mascara nos canais 'a' e 'b' enviados como parametro
Mat masking(char color, vector<Mat> channels)
{
  Mat result1, result2;
  cout << "debugg" << color << endl;

  switch(color)
  {
    case 'r':
      inRange(channels[1], MIN_A_R, MAX_A_R, result1);
      //inRange(channels[2], MIN_B_R, MAX_B_R, result2);
      break;
    case 'b':
      inRange(channels[1], MIN_A_B, MAX_A_B, result1);
      inRange(channels[2], MIN_B_B, MAX_B_B, result2);
      break;
    case 'g':
      inRange(channels[1], MIN_A_G, MAX_A_G, result1);
      inRange(channels[2], MIN_B_G, MAX_B_G, result2);
      break;
    default:
      return result1;
      break;
  }

  /*channels[1] = result1;
  channels[2] = result2;
  merge(channels,result1);*/
  medianBlur(result1, result1, 3);
  show(result1,"imagem 1");

  return result1;
}

bool trycolor(Mat M, int color){
	Mat temp = M(rois[color]); // rois e um vector de Rect's, indicando as 8 posicoes de interesse dos containers na foto
							   // esse vetor pode ser preenchido a mao ou lido de um arquivo
	int cont = countNonZero(temp);
	if(cont > (temp.cols*temp.rows*2)/3)
		return true
	return false
}

Mat FindCorners(Mat M){
	Mat dst;
	cornerHarris(M, dst, 2, 3, 0.04, BORDER_DEFAULT);

	// Como a funcao acima devolve uma imagem do tipo CV_32FC1, primeiro normalizamos e depois voltamos pra CV_8UC1
	normalize(dst, dst, 0, 255, NORM_MINMAX, CV_32FC1);
	convertScaleAbs(dst, dst);

	return dst;
}

// Principal funcao do programa
int main(int argc, char ** argv){

	if(!strcmp(agrv[1], "map")){
		// Caso essa seja a opcao escolhida, uma foto é tirada e com ela são preenchidas as cores dos containers

		// Abre o arquivo com as posicoes dos containers, caso nao exista o arquivo, sera criado um
		fstream colorfile;
		colorfile.open("colors.txt");

		if(!colorfile.is_open()){
			cout << "Erro ao abrir file!" << endl;
			return -1;
		};

		// Declara o vetor com as cores dos containers espera-se um vetor com tamanho [3][4][4] = 48
		vector<char> containers_colors(48, 0); // 0 - nao lido ainda, 1 - R, 2 - G, 3 - B, 4 - Retirado

		string line;
		int i=0;
		while(getline(colorfile, line)){
			vector[i] = line[0];
			i++;
		};
		colorfile.close();

		// Comeca a leitura de video
		VideoCapture cap(0);

		// Verifica se esta aberto corretamente
    	if ( !cap.isOpened() ){
        	printf("Could not communicate with the webcam \n");
        	return -1;
    	}

		// Define a frame e tira uma frame da camera
		Mat frame;
		bool success = cap.read(frame);
		if(!success || frame.empty()){
			cout << "Could not read a frame!!" << endl;
			return -1;
		};

		// Muda o tamanho da frame original para metade do tamanho
		resize(frame, frame, 0, 0.5, 0.5, INTER_LINEAR);

		// Converte para Cie Lab
		cvtColor(frame,frame,CV_BGR2Lab);
		// Separa os canais
		vector<Mat> channels;
		split(frame,channels);

		// Create a thresholded image to be used as test
		Mat thresholded;

		int pos = (int)argv[2][0]; // 0, 1, 2, 3, 4 ou 5. Indica de qual fileira e a foto
		for(i=0;i<3;i++){
			switch(i){
				case 0:
					inRange(channels[1], LabRminthresh, LabRmaxthresh, thresholded);
					break;
				case 1:
					inRange(channels[1], LabGminthresh, LabGmaxthresh, thresholded);
					break;
				case 2:
					inRange(channels[2], LabBminthresh, LabBmaxthresh, thresholded);
					break;
				default:
					return;
			};
			for(int j=0;j<8;j++){
				// 8 regions of interest in one picture for mapping
				bool thiscolor = trycolor(thresholded, j);
				if(thiscolor == true){
					containers_colors[(pos/2)*16 + (pos%2)*8 + j] = i;
				};
			};
		};

		// Escreve de volta as cores dos containers
		colorfile.open("colors.txt", ios::out | ios::trunc);

		if(!colorfile.is_open()){
			cout << "Erro ao abrir file!" << endl;
			return -1;
		};

		for(i=0;i<48;i++){
			colorfile << containers_colors[i] << endl;
		};
		colorfile.close();
	}
	else if(!strcmp(argv[1], "align")){
		// Primeiro pegamos a cor que queremos alinhar

		// Comeca a leitura de video
		VideoCapture cap(0);

		// Verifica se esta aberto corretamente
    	if ( !cap.isOpened() ){
        	printf("Could not communicate with the webcam \n");
        	return -1;
    	}

		// Define a frame e tira uma frame da camera
		Mat frame;
		bool success = cap.read(frame);
		if(!success || frame.empty()){
			cout << "Could not read a frame!!" << endl;
			return -1;
		};

		// Muda o tamanho da frame original para metade do tamanho
		resize(frame, frame, 0, 0.5, 0.5, INTER_LINEAR);

		// Converte para Cie Lab
		cvtColor(frame,frame,CV_BGR2Lab);
		// Separa os canais, verificar se isso nao impacta muito o desempenho
		vector<Mat> channels;
		split(frame,channels);

		int color = (int)argv[2][0];

		Mat thresholded;
		// first threshold the frame acording to the color asked
		switch(color){
			case 0:
				inRange(channels[1], LabRminthresh, LabRmaxthresh, thresholded);
				break;
			case 1:
				inRange(channels[1], LabGminthresh, LabGmaxthresh, thresholded);
				break;
			case 2:
				inRange(channels[2], LabBminthresh, LabBmaxthresh, thresholded);
				break;
			default:
				return;
		};

		// Realiza uma mascara simples
		medianBlur(thresholded, thresholded, 3);

		// Busca os cantos na imagem
		Mat result = FindCorners(thresholded);
	}
	else {
		return -1;
	}
  	return 0;
}
