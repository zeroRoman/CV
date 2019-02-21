#include <stdbool.h>
#include "readConfigure.h"
#include <math.h>

double max(double a, double b) {
	if (a > b) {
		return a;
	}
	else {
		return b;
	}
}

double min(double a, double b) {
	if (a < b) {
		return a;
	}
	else {
		return b;
	}
}

//debug log: sizeof double 
double** generateDynamic2DoubleArray(int width, int height) {
	double** arr = (double**)malloc(sizeof(int*)*width);
	for (int i = 0; i < width; i++) {
		arr[i] = (double*)malloc(sizeof(double)*height);
	}
	return arr;
}

int** generateDynamic2IntArray(int width, int height) {
	int** arr = (int**)malloc(sizeof(int*)*width);
	for (int i = 0; i < width; i++) {
		arr[i] = (int*)malloc(sizeof(int)*height);
	}
	return arr;
}

double** GaussFilter(int kernelSize, double sigma) {
	int n1, n2; double hg; double con;
	double** kernel = generateDynamic2DoubleArray(kernelSize, kernelSize);
	int center = kernelSize / 2; double sum = 0;
	for (int i = 0; i < kernelSize; i++) {
		for (int j = 0; j < kernelSize; j++) {
			n1 = abs(i - center);
			n2 = abs(j - center);
			con = -(double)(n1*n1 + n2*n2) / (double)(2 * sigma*sigma);
			hg = exp(con);
			kernel[i][j] = hg;
			sum += hg;
		}
	}
	for (int i = 0; i < kernelSize; i++) {
		for (int j = 0; j < kernelSize; j++) {
			kernel[i][j] = kernel[i][j] / sum;
		}
	}
	return kernel;
}

bool isLocalMin(int fil, int i, int j, double*** pic_fil) {
	for (int l = fil - 1; l <= fil + 1; l++) {
		for (int x = i - 1; x <= i + 1; x++) {
			for (int y = j - 1; y <= j + 1; y++) {
				if (l == fil && x == i && y == j) {
					continue;
				}
				if (pic_fil[fil][i][j] >= pic_fil[l][x][y]) {
					return false;
				}
			}
		}
	}

	return true;
}

int** SIFT(int** pic)
{
	int kernelSize = 3;  int numFilters = 5;
	int width = pic[0][0];
	int height = pic[0][1];
	int kp_size = sizeof(int*)*width*height / 100;
	int** kp = (int**)malloc(kp_size);//储存特征点
	int kp_index = 1;
	double*** filters = (double***)malloc(sizeof(double**) * numFilters);
	for (int sigma = 1; sigma <= numFilters; sigma += 1) {
		filters[sigma - 1] = GaussFilter(kernelSize, pow(2, (double)sigma) / (double)2);
	}

	printf("Gauss Filter\n");
	for (int l = 0; l < numFilters; l++) {
		for (int x = 0; x < kernelSize; x++) {
			for (int y = 0; y < kernelSize; y++) {
				printf("%lf ", filters[l][x][y]);
			}
			printf("\n");
		}
		printf("\n");
	}
	double*** pic_fil = (int***)malloc(sizeof(int**) * numFilters);
	double*** pic_fil_comp = (int***)malloc(sizeof(int**) * numFilters);//差分补图
	int center = kernelSize / 2;
	for (int fil = 0; fil<numFilters; fil++) {
		pic_fil[fil] = generateDynamic2DoubleArray(width, height);

		//用原图片初始化pic_fil

		for (int i = 1; i <= width; i++) {
			for (int j = 0; j < height; j++) {
				pic_fil[fil][i - 1][j] = pic[i][j];

			}
		}

		for (int i = kernelSize / 2; i < width - kernelSize / 2; i++) {
			for (int j = kernelSize / 2; j < height - kernelSize / 2; j++) {
				double sum = 0;
				for (int ii = -kernelSize / 2; ii <= kernelSize / 2; ii++) {
					for (int jj = -kernelSize / 2; jj <= kernelSize / 2; jj++) {
						sum += pic_fil[fil][i + ii][j + jj] * filters[fil][center + ii][center + jj];
					}
				}
				pic_fil[fil][i][j] = max(0, min(sum, 255));
			}
		}
	}

	for (int fil = 0; fil < numFilters - 1; fil++) {
		for (int i = kernelSize / 2; i < width - kernelSize / 2; i++) {
			for (int j = kernelSize / 2; j < height - kernelSize / 2; j++) {
				pic_fil[fil][i][j] = min(max(0, pic_fil[fil][i][j] - pic_fil[fil + 1][i][j]), 255);
			}
		}
	}


	for (int fil = 0; fil < numFilters - 1; fil++) {
		pic_fil_comp[fil] = generateDynamic2DoubleArray(width, height);
		for (int i = kernelSize / 2; i < width - kernelSize / 2; i++) {
			for (int j = kernelSize / 2; j < height - kernelSize / 2; j++) {
				pic_fil_comp[fil][i][j] = min(255, max(255 - pic_fil[fil][i][j], 0));

			}
		}
	}


	//求极值，包括极大值和极小值

	for (int i = kernelSize / 2 + 1; i < width - kernelSize / 2 - 1; i++) {
		for (int j = kernelSize / 2 + 1; j < height - kernelSize / 2 - 1; j++) {
			for (int fil = 1; fil < numFilters - 2; fil++) {
				if (isLocalMin(fil, i, j, pic_fil)) {
					kp[kp_index] = (int*)malloc(sizeof(int) * 2);
					kp[kp_index][0] = i; kp[kp_index][1] = j;
					kp_index++;
					break;
				}
				else if (isLocalMin(fil, i, j, pic_fil_comp)) {
					kp[kp_index] = (int*)malloc(sizeof(int) * 2);
					kp[kp_index][0] = i; kp[kp_index][1] = j;
					kp_index++;
					break;
				}
			}
		}
	}



	for (int i = 0; i < numFilters; i++) {
		free(filters[i]);
		free(pic_fil[i]);
		if (i < numFilters - 1) {
			free(pic_fil_comp[i]);
		}
	}


	kp[0] = (int*)malloc(sizeof(int));
	kp[0][0] = kp_index - 1;

	return kp;
}