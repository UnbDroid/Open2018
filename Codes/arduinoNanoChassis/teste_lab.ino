#define S2 10
#define S3 11
#define OUT 12

#define VERMELHO 0
#define VERDE 1
#define AZUL 2
#define PRETO 3
#define BRANCO 4

#define MEDIANA 7

#define ALPHA 0.15


double cores[3];
int cores_conhecidas[5][3][3];
int medidas[MEDIANA];
void convert2lab();
int indice;

//*
//Vermelho

//*/
int mediana (int *medidas);
double classificarKNN ();
void read_tcs();

void setup() {
  cores_conhecidas[0][0][0] = 230;
  cores_conhecidas[0][1][0] = -37;
  cores_conhecidas[0][2][0] = 4;
  cores_conhecidas[0][0][1] = 234;
  cores_conhecidas[0][1][1] = -35;
  cores_conhecidas[0][2][1] = 4;
  cores_conhecidas[0][0][2] = 236;
  cores_conhecidas[0][1][2] = -35;
  cores_conhecidas[0][2][2] = 3;

  //Verde
  cores_conhecidas[1][0][0] = 220;
  cores_conhecidas[1][1][0] = 16;
  cores_conhecidas[1][2][0] = 1;
  cores_conhecidas[1][0][1] = 220;
  cores_conhecidas[1][1][1] = 17;
  cores_conhecidas[1][2][1] = 1;
  cores_conhecidas[1][0][2] = 240;
  cores_conhecidas[1][1][2] = 11;
  cores_conhecidas[1][2][2] = 0;

  //Azul
  cores_conhecidas[2][0][0] = 198;
  cores_conhecidas[2][1][0] = 13;
  cores_conhecidas[2][2][0] = 22;
  cores_conhecidas[2][0][1] = 244;
  cores_conhecidas[2][1][1] = -5;
  cores_conhecidas[2][2][1] = 26;
  cores_conhecidas[2][0][2] = 248;
  cores_conhecidas[2][1][2] = -9;
  cores_conhecidas[2][2][2] = 33;

  //Preto
  cores_conhecidas[3][0][0] = 255;
  cores_conhecidas[3][1][0] = 0;
  cores_conhecidas[3][2][0] = 0;
  cores_conhecidas[3][0][1] = 255;
  cores_conhecidas[3][1][1] = -3;
  cores_conhecidas[3][2][1] = 4;
  cores_conhecidas[3][0][2] = 255;
  cores_conhecidas[3][1][2] = 0;
  cores_conhecidas[3][2][2] = 2;

  //Branco
  cores_conhecidas[4][0][0] = 132;
  cores_conhecidas[4][1][0] = 1;
  cores_conhecidas[4][2][0] = 5;
  cores_conhecidas[4][0][1] = 131;
  cores_conhecidas[4][1][1] = 0;
  cores_conhecidas[4][2][1] = 5;
  cores_conhecidas[4][0][1] = 135;
  cores_conhecidas[4][1][1] = 0;
  cores_conhecidas[4][2][1] = 5;

  Serial.begin(9600);
  pinMode (S2, OUTPUT);
  pinMode (S3, OUTPUT);
  pinMode (OUT, INPUT);
}


void loop() {

  Serial.println ("R\tG\tB\tW\tSoma\tResultado");
  read_tcs();  
  convert2lab();
  for (int i = 0; i < 3; i++) {
    Serial.print (cores[i]);
    Serial.print ("\t");
  }
  Serial.print(":");
  int classe, canal, valores;
  double aux, proximo;// = 6400000.0;
  proximo = 6400000.0;
  //Serial.println();
  for (classe = 0; classe < 5 ; classe++) {
    //Serial.print(classe);
    //Serial.println();
    aux = 0;
    for (valores = 0; valores < 3; valores ++) {
      for (canal = 0; canal < 3; canal++) {
        double aux2 = (cores[canal] - (double) cores_conhecidas[classe][canal][valores]);

        if (aux2 < 0)
          aux2 = -1.0 * aux2;
        
          //Serial.print("V:");
          //Serial.print(aux2);
          //Serial.print(":\t");
          //
        aux += aux2;
      }
      if (aux < proximo) {
        indice = classe;
        proximo = aux;
      }
      //Serial.println(aux);
      aux = 0;
    }
  }
  Serial.print(proximo);

  switch (indice) {
    case VERMELHO:
      Serial.println ("\tVermelho");
      break;
    case VERDE:
      Serial.println ("\tVERDE");
      break;
    case AZUL:
      Serial.println ("\tAZUL");
      break;
    case PRETO:
      Serial.println ("\tPRETO");
      break;
    case BRANCO:
      Serial.println ("\tBRANCO");
      break;
    default:
      Serial.print ("aqui");
      Serial.println (indice);
  }
  Serial.print ("\n");
  delay (400);
}

void convert2lab() {
  double x, y, z;

  for (int i = 0 ; i < 3; i++) {
    if (cores[i] > 1024.0) {
      cores[i] = 1024.0;
    }
    cores[i] = cores[i] / 1024.0;
  }

  x = 0.412453 * cores[0] + 0.357580 * cores[1] + 0.180423 * cores[2];
  y = 0.212671 * cores[0] + 0.715160 * cores[1] + 0.072169 * cores[2];
  z = 0.019334 * cores[0] + 0.119193 * cores[1] + 0.950227 * cores[2];
  x = x / 0.950456;
  z = z / 1.088754;
  if (y <= 0.008856) {
    cores[0] = 903.3 * y;
  }
  else {
    cores[0] = 116 * pow(y, 0.33333) - 16;
  }
  if (x > 0.008856) {
    x = pow(x, 0.3333);
  }
  else {
    x = 7.787 * x + 16 / 116;
  }
  if (y > 0.008856) {
    y = pow(y, 0.3333);
  }
  else {
    y = 7.787 * y + 16 / 116;
  }

  if (z > 0.008856) {
    z = pow(z, 0.3333);
  }
  else {
    z = 7.787 * z + (16 / 116);
  }
  cores[1] = 500 * (x - y);
  cores[2] = 200 * (y - z);
  cores[0] = cores[0] * 255 / 100;

}

double classificarKNN () {
  //*
  int classe, canal, valores;
  double aux, proximo = 6400000.0;
  for (classe = 0; classe < 5 ; classe++) {
    aux = 0;
    for (valores = 0; valores < 3; valores ++) {
      for (canal = 0; canal < 4; canal++) {
        double aux2 = (cores[canal] - (double) cores_conhecidas[classe][canal][valores]);
        if (aux2 < 0)
          aux2 = -1.0 * aux2;
        aux += aux2;
      }
      if (aux < proximo) {
        indice = classe;
        proximo = aux;
      }
      aux = 0;
    }
    //.println("1");
  }

  //*/
  //
  return proximo;
}

int mediana (int *medidas) {
  float maximo = 0;
  int indice;
  for (int i = 0; i < 5 ; i++) {
    for (int j = 0; j < MEDIANA - i; j++) {
      if (medidas[j] > maximo) {
        maximo = medidas [j];
        indice = j;
      }
    }
    if (indice != MEDIANA - 1) {
      maximo = medidas[MEDIANA - i];
      medidas[MEDIANA - i] = medidas[indice];
      medidas[indice] = maximo;
    }
    maximo = 0;
  }
  return medidas[3];
}

void read_tcs() {
  //Vermelho
  digitalWrite(S2, LOW);
  digitalWrite(S3, LOW);
  for (int i = 0; i < MEDIANA ; i++) {
    medidas[i] = pulseIn (OUT, digitalRead(OUT) == HIGH ? LOW : HIGH);
  }
  cores[0] = (ALPHA) * cores[0] + (1 - ALPHA) * mediana(medidas);

  //Azul
  digitalWrite(S3, HIGH);
  for (int i = 0; i < MEDIANA ; i++) {
    medidas[i] = pulseIn (OUT, digitalRead(OUT) == HIGH ? LOW : HIGH);
  }
  cores[2] = (ALPHA) * cores[2] + (1 - ALPHA) * mediana(medidas);

  //Verde
  digitalWrite(S2, HIGH);
  for (int i = 0; i < MEDIANA ; i++) {
    medidas[i] = pulseIn (OUT, digitalRead(OUT) == HIGH ? LOW : HIGH);
  }
  cores[1] = (ALPHA) * cores[1] + (1 - ALPHA) * mediana(medidas);

}
