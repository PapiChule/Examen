////////////////////////////////Cabeceras/////////////////////////////////////
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>
#include <algorithm>
#include <math.h>
/////////////////////////////////////////////////////////////////////////////

///////////////////////////////Espacio de nombres////////////////////////////
using namespace cv;
using namespace std;
/////////////////////////////////////////////////////////////////////////////

///////////////////////////////Espacio de constantes/////////////////////////
const float pi = 3.141592;
const float e = 2.718281828459;
/////////////////////////////////////////////////////////////////////////////

float** crear_kernel_dinamico(int);
void liberar_memoria_kernel(float**, int);
void inicializa_kernel_dinamico(float** kernel, int tam);
float valor_gaussiano(int, int, int);
void bordes_imagen(int, int, Mat, int);
void rellenar_kernel_dinamico(float**, int, int);
Mat escala_grises_imagen(Mat, int);
void mostrar_kernel(float**, int);
void rellenar_kernel_dinamico_sobel_x(float**, int);
void rellenar_kernel_dinamico_sobel_y(float**, int);
void mostrar_longitudes(Mat, String);

/////////////////////////Inicio de la funcion principal///////////////////
int main()
{

	/********Declaracion de variables generales*********/
	char NombreImagen[] = "./lena.png";
	Mat imagenO; // Matriz que contiene nuestra imagen sin importar el formato
	/************************/

	/*********Lectura de la imagen*********/
	imagenO = imread(NombreImagen);

	if (!imagenO.data)
	{
		cout << "Error al cargar la imagen: " << NombreImagen << endl;
		exit(1);
	}
	/************************/

	/************Procesos*********/
	int i, j, k, l, f;
	int fila_original = imagenO.rows;
	int columna_original = imagenO.cols;//Lectura de cuantas columnas

	int tam, sigma, bnd = 0;
	float** direccion, ** sobel_x, ** sobel_y;

	/*Pedir longitud de kernel gaussiano*/
	do {
		cout << "Introduce longitud kernel: ";
		cin >> tam;
		// Valida que sea impar la longitud
		if (tam % 2 == 0 || tam < 0 || tam == 1) {
			cout << "Longitud incorrecta... \n";
			bnd = 1;
		}
		else {
			bnd = 0;
		}

	} while (bnd != 0);

	/*Pedir sigma para kernel gaussiano*/
	do {
		cout << "Introduce sigma kernel: ";
		cin >> sigma;
		// Valida que sea un numero positivo
		if (sigma <= 0 ) {
			cout << "Valor incorrecta... \n";
			bnd = 1;
		}
		else {
			bnd = 0;
		}

	} while (bnd != 0);

	// Tamaño de los bordes que se quitaran
	f = (tam - 1) / 2;

	/*--------------Crear matrices para almacenar pixeles----------------*/

	Mat gris_fun(fila_original, columna_original, CV_8UC1);
	Mat gauss_fun_imagen(fila_original - (tam - 1), columna_original - (tam - 1), CV_8UC1);
	Mat sobel_x_fun_imagen(gauss_fun_imagen.rows - 2, gauss_fun_imagen.cols - 2, CV_8UC1);
	Mat sobel_y_fun_imagen(gauss_fun_imagen.rows - 2, gauss_fun_imagen.cols - 2, CV_8UC1);
	Mat sobel(gauss_fun_imagen.rows - 2, gauss_fun_imagen.cols - 2, CV_8UC1);
	Mat ecualizar_imagen(fila_original - (tam - 1), columna_original - (tam - 1), CV_8UC1);
	Mat angulos_imagen(gauss_fun_imagen.rows - 2, gauss_fun_imagen.cols - 2, CV_8UC1);
	Mat bordes_imagen(gauss_fun_imagen.rows - 2, gauss_fun_imagen.cols - 2, CV_8UC1);
	Mat bordes(gauss_fun_imagen.rows - 2, gauss_fun_imagen.cols - 2, CV_8UC1);

	/*Funcion para crear imagen en tonos de grises*/

	gris_fun = escala_grises_imagen(imagenO, tam);

	// Crear kernel para cada operacion
	direccion = crear_kernel_dinamico(tam);
	sobel_x = crear_kernel_dinamico(3);
	sobel_y = crear_kernel_dinamico(3);

	// Inicializamos cada kernel con 0
	inicializa_kernel_dinamico(direccion, tam);
	inicializa_kernel_dinamico(sobel_x, 3);
	inicializa_kernel_dinamico(sobel_y, 3);

	// Rellenar los kernel
	rellenar_kernel_dinamico(direccion, tam, sigma);
	rellenar_kernel_dinamico_sobel_x(sobel_x, 3);
	rellenar_kernel_dinamico_sobel_y(sobel_y, 3);

	// Desplejamos los valores almacenados de los kernel
	mostrar_kernel(direccion, tam);
	mostrar_kernel(sobel_x, 3);
	mostrar_kernel(sobel_y, 3);

	/*Funcion para aplicar filtro gaussiano*/

	double sum = 0;
	int a, b, m, n;

	for (i = f, m = 0; i < gris_fun.rows - f; i++, m++) {
		for (j = f, n = 0; j < gris_fun.cols - f; j++, n++) {

			//Ciclo para multiplicar cada los vecinos de (x, y) con los valores del kernel gaussiano
			for (a = 0; a < tam; a++) {
				for (b = 0; b < tam; b++) {
					sum += gris_fun.at<uchar>(j - f + a, i - f + b) * direccion[a][b];
				}
			}

			gauss_fun_imagen.at<uchar>(n, m) = uchar(sum);

			sum = 0;

		}

	}

	/*Funcion para aplicar filtro sobel x*/

	for (i = 1, m = 0; i < gauss_fun_imagen.rows - 1; i++, m++) {
		for (j = 1, n = 0; j < gauss_fun_imagen.cols - 1; j++, n++) {

			//Ciclo para multiplicar cada los vecinos de (x, y) con los valores del kernel de sobel x
			for (a = 0; a < 3; a++) {
				for (b = 0; b < 3; b++) {
					sum += gauss_fun_imagen.at<uchar>(j - 1 + a, i - 1 + b) * sobel_x[a][b];
				}
			}

			sobel_x_fun_imagen.at<uchar>(n, m) = uchar(abs(sum));

			sum = 0;

		}

	}

	/*Funcion para aplicar filtro sobel y*/

	for (i = 1, m = 0; i < gauss_fun_imagen.rows - 1; i++, m++) {
		for (j = 1, n = 0; j < gauss_fun_imagen.cols - 1; j++, n++) {

			//Ciclo para multiplicar cada los vecinos de (x, y) con los valores del kernel de sobel y
			for (a = 0; a < 3; a++) {
				for (b = 0; b < 3; b++) {
					sum += gauss_fun_imagen.at<uchar>(j - 1 + a, i - 1 + b) * sobel_y[a][b];
				}
			}

			sobel_y_fun_imagen.at<uchar>(n, m) = uchar(abs(sum));

			sum = 0;

		}

	}

	/*Funcion para aplicar filtro sobel |G|*/

	for (i = 0; i < sobel_x_fun_imagen.rows; i++) {
		for (j = 0; j < sobel_x_fun_imagen.cols; j++) {

			// Calculamos |G| de la forma sqrt (Gx^2 + Gy^2)
			sobel.at<uchar>(Point(j, i)) = uchar((pow(sobel_x_fun_imagen.at<uchar>(Point(j, i)), 2) + pow(sobel_y_fun_imagen.at<uchar>(Point(j, i)), 2)));

		}

	}

	/*Funcion ecualizar imagen*/

	int index, acumulado_min, min;
	// Creamos histogramas con los niveles de gris a trabajar
	int hist[256], hist_acumulado[256], lookup[256];

	// Inicializamos el histograma
	for (i = 0; i < 256; i++) {
		hist[i] = 0;
	}

	// Recorremos la imagen en valores de gris
	for (i = 0; i < gris_fun.rows; i++) {
		for (j = 0; j < gris_fun.cols; j++) {

			index = gris_fun.at<uchar>(j, i); // Obtenemos el nivel de gris como un indice
			hist[index] += 1; // Sumamos la unidad al histograma con el nivel de gris del pixel
		}
	}

	hist_acumulado[0] = hist[0]; //Inicializamos el histograma acumulatio

	for (i = 1; i < 256; i++) {
		hist_acumulado[i] = hist[i] + hist_acumulado[i - 1]; // Acumula valores de gris del histograma inicial
	}

	min = hist_acumulado[0];

	// Obtener el valor mínimo
	for (i = 0; i < 256; i++) {
		if (hist_acumulado[i] < min) {
			min = hist_acumulado[i];
		}
	}

	// Creamos tabla LookUp
	for (i = 0; i < 256; i++) {
		lookup[i] = floor(255 * (hist_acumulado[i] - min) / (gris_fun.rows * gris_fun.cols - min));
	}

	// Asignamos valores a la imagen ecualizada
	for (int i = 0; i < gris_fun.rows; i++)
		for (int j = 0; j < gris_fun.cols; j++)
			ecualizar_imagen.at<uchar >(j, i) = lookup[gris_fun.at<uchar>(j, i)];

	/*Funcion angulos de imagen*/

	int v1, v2, aux;
	float u_b, u_a, max;
	double ang;

	for (i = 0; i < sobel_x_fun_imagen.rows; i++) {
		for (j = 0; j < sobel_x_fun_imagen.cols; j++) {

			if ((int)sobel_x_fun_imagen.at<uchar>(j, i) == 0) {
				ang = 90;
			}
			else {
				ang = (int)sobel_y_fun_imagen.at<uchar>(j, i) / (int)sobel_x_fun_imagen.at<uchar>(j, i);
			}

			// Angulo de radianes a grados
			ang = atan(ang);
			ang = (ang * 180) / pi;

			angulos_imagen.at<uchar>(j, i) = uchar(ang);

		}

	}

	// Obtenemos bordes con la matriz de angulos
	for (i = 0; i < sobel.rows; i++) {
		for (j = 0; j < sobel.cols; j++) {

			ang = angulos_imagen.at<int>(j, i);

			// Calculamos vecinos fuertes y debiles del pixel (x,y)
			if (ang > 0 || ang < 23 || ang >= 158 || ang < 180) {
				v1 = sobel.at<uchar>(Point(j, i - 1));
				v2 = sobel.at<uchar>(Point(j, i + 1));
			}
			else if (ang >= 68 || ang < 113) {
				v1 = sobel.at<uchar>(Point(j - 1, i));
				v2 = sobel.at<uchar>(Point(j + 1, i));
			}
			else if (ang >= 23 || ang < 68) {
				v1 = sobel.at<uchar>(Point(j - 1, i + 1));
				v2 = sobel.at<uchar>(Point(j + 1, i - 1));
			}
			else if (ang >= 113 || ang < 158) {
				v1 = sobel.at<uchar>(Point(j - 1, i - 1));
				v2 = sobel.at<uchar>(Point(j + 1, i + 1));
			}

			if (sobel.at<uchar>(Point(j, i)) >= v1 && sobel.at<uchar>(Point(j, i)) >= v2) {
				bordes_imagen.at<uchar>(Point(j, i)) = sobel.at<uchar>(Point(j, i));
			}
			else {
				bordes_imagen.at<uchar>(Point(j, i)) = uchar(0);
			}

		}
	}

	max = (int) bordes_imagen.at<uchar>(Point(0, 0));
	
	for (i = 0; i < bordes_imagen.rows; i++) {
		for (j = 0; j < bordes_imagen.cols; j++) {
			
			aux = (int) bordes_imagen.at<uchar>(Point(j, i));

			if (aux > max) {
				max = aux;
			}
			
		}
	}

	u_a = max / 2;
	u_b = u_a / 2;

	for (i = 0; i < bordes_imagen.rows; i++) {
		for (j = 0; j < bordes_imagen.cols; j++) {

			aux = (int)bordes_imagen.at<uchar>(Point(j, i));

			if (aux <= u_b ) {
				bordes_imagen.at<uchar>(j, i) = uchar(0);
			}
			else if (aux > u_b && aux < u_a) {
				bordes_imagen.at<uchar>(j, i) = uchar(aux);
			}
			else if(aux > u_a) {
				bordes_imagen.at<uchar>(j, i) = uchar(255);
			}

		}
	}

	/*Funcion bordes Canny*/

	Canny(sobel, bordes, 10, 350);

	/*---------------Mostrar imagenes----------------*/

	namedWindow("Imagen Original", WINDOW_AUTOSIZE); //Creación de una ventana
	imshow("Imagen Original", imagenO); //Mostrar imagen en la ventana

	namedWindow("Imagen Gris", WINDOW_AUTOSIZE);
	imshow("Imagen Gris", gris_fun);

	namedWindow("Imagen Gauss", WINDOW_AUTOSIZE);
	imshow("Imagen Gauss", gauss_fun_imagen);

	namedWindow("Imagen Gx", WINDOW_AUTOSIZE);
	imshow("Imagen Gx", sobel_x_fun_imagen);

	namedWindow("Imagen Gy", WINDOW_AUTOSIZE);
	imshow("Imagen Gy", sobel_y_fun_imagen);

	namedWindow("Imagen Sobel |G|", WINDOW_AUTOSIZE);
	imshow("Imagen Sobel |G|", sobel);

	namedWindow("Imagen Ecualizada", WINDOW_AUTOSIZE);
	imshow("Imagen Ecualizada", ecualizar_imagen);

	namedWindow("Imagen Angulos", WINDOW_AUTOSIZE);
	imshow("Imagen Angulos", angulos_imagen);

	namedWindow("Imagen Bordes", WINDOW_AUTOSIZE);
	imshow("Imagen Bordes", bordes);

	/*---------------Mostrar tamaño de imagenes--------------*/

	mostrar_longitudes(imagenO, "Original");
	mostrar_longitudes(gris_fun, "Gris");
	mostrar_longitudes(gauss_fun_imagen, "Gauss");
	mostrar_longitudes(sobel_x_fun_imagen, "Gx");
	mostrar_longitudes(sobel_y_fun_imagen, "Gy");
	mostrar_longitudes(sobel, "Original");
	mostrar_longitudes(ecualizar_imagen, "Ecualizada");
	mostrar_longitudes(bordes_imagen, "Bordes Canny");

	waitKey(0); //Función para esperar
	return 1;
}
/////////////////////////////////////////////////////////////////////////

void mostrar_longitudes(Mat matriz, String nombre) {
	cout << "Imagen " << nombre << endl;
	cout << "filas: " << matriz.rows << endl;
	cout << "columnas: " << matriz.cols << endl << endl;
}

Mat escala_grises_imagen(Mat original, int tam) {

	Mat grises(original.rows, original.cols, CV_8UC1);

	int i, j;
	double azul, verde, rojo, gris_n;
	int f = (tam - 1) / 2;

	for (i = 0; i < original.rows; i++) {
		for (j = 0; j < original.cols; j++) {

			azul = original.at<Vec3b>(Point(j, i)).val[0];
			verde = original.at<Vec3b>(Point(j, i)).val[1];
			rojo = original.at<Vec3b>(Point(j, i)).val[2];

			gris_n = (rojo * 0.299 + verde * 0.587 + azul * 0.114);


			if (i - f < 0 || j - f < 0 || i + f > original.rows || j + f > original.cols) {
				grises.at<uchar>(Point(j, i)) = uchar(0);
			}
			else {
				grises.at<uchar>(Point(j, i)) = uchar(gris_n);
			}

		}
	}

	return grises;

}

void mostrar_kernel(float** kernel, int tam) {

	int i, j;

	for (i = 0; i < tam; i++) {
		for (j = 0; j < tam; j++) {
			cout << kernel[i][j] << " ";
		}
		cout << endl;
	}

}


float** crear_kernel_dinamico(int tam) {

	float** kernel = NULL;
	int i;

	kernel = (float**)malloc(tam * sizeof(float**));

	for (i = 0; i < tam; i++) {

		kernel[i] = (float*)malloc(tam * sizeof(float));

	}

	return kernel;

}

void liberar_memoria_kernel(float** kernel, int tam) {

	int i;

	for (i = 0; i < tam; i++) {

		free(kernel[i]);

	}

	free(kernel);

	kernel = NULL;

}

void inicializa_kernel_dinamico(float** kernel, int tam) {

	int i, j;

	for (i = 0; i < tam; i++) {
		for (j = 0; j < tam; j++) {
			kernel[i][j] = 0;
		}
	}

}

float valor_gaussiano(int x, int y, int sigma) {

	float valor, potencia;

	potencia = (pow(x, 2) + pow(y, 2)) / (2 * pow(sigma, 2));

	valor = (1) / ((2 * pi) * pow(sigma, 2));
	valor = valor * pow(e, 0 - potencia);

	return valor;

}

void bordes_imagen(int cols, int rows, Mat mat, int tam) {

	int i, j;
	int f = (tam - 1) / 2;

	for (i = 0; i < rows; i++) {
		for (j = 0; j < cols; j++) {

			if (i - f < 0 || j - f < 0 || i + f > rows || j + f > cols) {
				mat.at<uchar>(Point(j, i)) = uchar(0);
			}
			else {
				mat.at<uchar>(Point(j, i)) = uchar(255);
			}

		}
	}

}

void rellenar_kernel_dinamico(float** kernel, int tam, int sigma) {

	int i, j, x, y;
	int rango = (tam - 1) / 2;

	for (i = 0, y = -rango; i < tam; i++, y++) {
		for (j = 0, x = -rango; j < tam; j++, x++) {
			kernel[i][j] = valor_gaussiano(x, y, sigma);
		}
	}

}

void rellenar_kernel_dinamico_sobel_x(float** kernel, int tam) {

	kernel[0][0] = -1;
	kernel[1][0] = -2;
	kernel[2][0] = -1;

	kernel[0][2] = 1;
	kernel[1][2] = 2;
	kernel[2][2] = 1;

}

void rellenar_kernel_dinamico_sobel_y(float** kernel, int tam) {

	kernel[0][0] = 1;
	kernel[0][1] = 2;
	kernel[0][2] = 1;

	kernel[2][0] = -1;
	kernel[2][1] = -2;
	kernel[2][2] = -1;

}
