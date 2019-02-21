#include <string.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <stdbool.h>
#include "readConfigure.h"

double* calGra(int i, int j, int** copy, int edge) {
	double * res = (double*)malloc(sizeof(double) * 2);
	for (int ii = max(0, i - 1); ii <= min(edge - 1, i + 1); ii++) {
		for (int jj = max(0, j - 1); jj <= min(edge - 1, j + 1); jj++) {
			if ((ii == i) && (jj == j)) {
				continue;
			}
			double length = sqrt((ii - i)*(ii - i) + (jj - j)*(jj - j));
			//�ֱ�����x,y�����������ݶ�
			double x = (double)(ii - i)*(double)(copy[ii][jj] - copy[i][j]) / length;
			double y = (double)(jj - j)*(double)(copy[ii][jj] - copy[i][j]) / length;

			res[0] = x / 8; res[1] = y / 8;
		}
	}

	return res;
}

double calDis(double a, double b) {
	return sqrt(a*a + b*b);
}

double myMod(double a, double b) {
	while (a < 0) {
		a += b;
	}
	return fmod(a, b);
}

//��������������ռ�������СΪ(size * num)^2

double*** descriptor(int** pic, int** kp, int size, int num) {
	int kp_size = kp[0][0];
	printf("The number of feature points :  %d\n", kp_size);
	for (int i = 1; i <= kp_size; i++) {
		printf("Feature points :  %d  %d\n", kp[i][0], kp[i][1]);
	}

	double*** dpt = (double***)malloc(sizeof(double**)*(kp_size));
	for (int di = 0; di < kp_size; di++) {
		dpt[di] = (double**)malloc(sizeof(double*)*(num*num));
		for (int i = 0; i < num*num; i++) {
			dpt[di][i] = (double*)malloc(sizeof(double) * 8);
		}
	}


	double** GF = GaussFilter(size*num, sqrt(2));

	//ֱ��ͼ���⻯,��������Ӱ��
	for (int ki = 1; ki <= kp_size; ki++) {
		int x = kp[ki][0]; int y = kp[ki][1];
		static const int a = 1;
		int** copy = generateDynamic2IntArray(size*num, size*num);
		for (int i = 0; i < size*num; i++) {
			for (int j = 0; j < size*num; j++) {
				//pic��һ�д��ųߴ�����ֵ
				if (x - size*num / 2 + 1 + i < 0 || x - size*num / 2 + 1 + i >= pic[0][0]) {
					printf("����Խ��x %d  %d\n", x, x - size*num / 2 + 1 + i);
				}
				if (y - size*num / 2 + 1 + j < 0 || y - size*num / 2 + 1 + j >= pic[0][1]) {
					printf("����Խ��y %d  %d\n", y, y - size*num / 2 + 1 + j);
				}

				copy[i][j] = pic[x - size*num / 2 + 2 + i][y - size*num / 2 + 1 + j];
			}
		}


		for (int i = 0; i < size*num; i += size) {
			for (int j = 0; j < size*num; j += size) {

				//�󼫴�Сֵ
				int min = 255; int max = 0;
				for (int ii = i; ii < i + size; ii++) {
					for (int jj = j; jj < j + size; jj++) {
						if (copy[ii][jj] > max) {
							max = copy[ii][jj];
						}
						if (copy[ii][jj] < min) {
							min = copy[ii][jj];
						}
					}
				}


				int scale = max - min;
				for (int ii = i; ii < i + size; ii++) {
					for (int jj = j; jj < j + size; jj++) {
						copy[ii][jj] = (double)(copy[ii][jj] - min) * (double)255 / (double)scale;
						
					}
					
				}

			}
		}

		//��������ÿһ��������ݶ�
		double sum_x = 0, sum_y = 0;
		//�洢�ݶȳ��Ⱥ�ƫת�Ƕ�
		double*** gradient = (double***)malloc(sizeof(double**)*(size*num));
		for (int i = 0; i < num*size; i++) {
			gradient[i] = (double**)malloc(sizeof(double*)*(size*num));
			for (int j = 0; j < num*size; j++) {
				gradient[i][j] = (double*)malloc(sizeof(double)*(2));
				double*gra = calGra(i, j, copy, size*num);
				gradient[i][j][0] = calDis(gra[0], gra[1]);
				gradient[i][j][1] = atan2(gra[1], gra[0]);//atan2����yֵ��ǰ
				free(gra);
				sum_x += gradient[i][j][0];
				sum_y += gradient[i][j][1];
			}
		}

		//���������
		//����������ƫת�Ƕ�
		double theta = atan2(sum_y, sum_x);
		for (int i = 0; i < num*size; i++) {
			for (int j = 0; j < num*size; j++) {
				gradient[i][j][1] -= theta;
			}
		}

		//�ݶȸ�˹��Ȩ��
		for (int i = 0; i < num*size; i++) {
			for (int j = 0; j < num*size; j++) {
				gradient[i][j][0] = gradient[i][j][0] * GF[i][j] * (double)(num*size) * (double)(num*size);
				gradient[i][j][1] = gradient[i][j][1] * GF[i][j] * (double)(num*size) * (double)(num*size);
			}
		}

		//����������
		//��ʼ��������
		for (int i = 0; i < num*num; i++) {
			for (int j = 0; j < 8; j++) {
				//ki��1��ʼ
				dpt[ki - 1][i][j] = 0;
			}
		}
		int dpt_index = 0;
		for (int i = 0; i < size*num; i += size) {
			for (int j = 0; j < size*num; j += size) {

				for (int ii = i; ii < i + size; ii++) {
					for (int jj = j; jj < j + size; jj++) {
						//��360�Ȼ���Ϊ8������ÿ������45��
						int direction = (myMod(gradient[ii][jj][0], (double)360)) / (double)45;
						dpt[ki - 1][dpt_index][direction] += gradient[ii][jj][0];
					}
				}


				dpt_index++;
			}
		}

		free(gradient);
		free(copy);
	}

	free(GF);

	return dpt;
}