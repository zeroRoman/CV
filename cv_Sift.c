#include <string.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <stdbool.h>
#include "readConfigure.h"

int** readBMP(char* picName,int numberPicture)
{

	BITMAPFILEHEADER   bitHead;
	BITMAPINFOHEADER bitInfoHead;
	FILE* pfile;


	char strFile[50];
	strcpy(strFile, picName);
	pfile = fopen(strFile, "rb");//打开文件

	if (pfile != NULL)
	{
		printf("file %d.bmp open success.\n", numberPicture);
		//读取位图文件头信息
		WORD fileType;
		fread(&fileType, 1, sizeof(WORD), pfile);
		if (fileType != 0x4d42)
		{
			printf("file is not .bmp file!");
			return NULL;
		}

		fread(&bitHead, 1, sizeof(BITMAPFILEHEADER), pfile);
		fread(&bitInfoHead, 1, sizeof(BITMAPINFOHEADER), pfile);

	}
	else
	{
		printf("file open fail!\n");
		return NULL;
	}
	RGBQUAD *pRgb;

	if (bitInfoHead.biBitCount < 24)
	{
		//读取调色盘结信息
		long nPlantNum = pow(2, (bitInfoHead.biBitCount));    
		pRgb = (RGBQUAD *)malloc(nPlantNum * sizeof(RGBQUAD));
		memset(pRgb, 0, nPlantNum * sizeof(RGBQUAD));
		int num = fread(pRgb, 4, nPlantNum, pfile);

		printf("Color Plate Number: %d\n", nPlantNum);

		for (int i = 0; i<nPlantNum;i++)
		{
			if (i % 5 == 0)
			{
				printf("\n");
			}
		}

		printf("\n");

	}


	int width = bitInfoHead.biWidth;
	int height = bitInfoHead.biHeight;
	//分配内存空间把源图存入内存
	int l_width = WIDTHBYTES(width* bitInfoHead.biBitCount);//计算位图的实际宽度并确保它为32的倍数
	BYTE    *pColorData = (BYTE *)malloc(height*l_width);
	memset(pColorData, 0, height*l_width);
	long nData = height*l_width;

	//把位图数据信息读到数组里
	fread(pColorData, 1, nData, pfile);



	//将位图数据转化为RGB数据
	RGBQUAD* dataOfBmp;
	dataOfBmp = (RGBQUAD *)malloc(width*height * sizeof(RGBQUAD));//用于保存各像素对应的RGB数据
	memset(dataOfBmp, 0, width*height * sizeof(RGBQUAD));

	if (bitInfoHead.biBitCount<24)
	{
		int k;
		int index = 0;
		if (bitInfoHead.biBitCount == 1)
		{
			for (int i = 0;i<height;i++)
				for (int j = 0;j<width;j++)
				{
					BYTE mixIndex = 0;
					//k:取得该像素颜色数据在实际数据数组中的序号
					//j:提取当前像素的颜色的具体值
					k = i*l_width + j / 8;
					mixIndex = pColorData[k];
					switch (j % 8)
					{
					case 0:
						mixIndex = mixIndex << 7;
						mixIndex = mixIndex >> 7;
						break;
					case 1:
						mixIndex = mixIndex << 6;
						mixIndex = mixIndex >> 7;
						break;
					case 2:
						mixIndex = mixIndex << 5;
						mixIndex = mixIndex >> 7;
						break;

					case 3:
						mixIndex = mixIndex << 4;
						mixIndex = mixIndex >> 7;
						break;
					case 4:
						mixIndex = mixIndex << 3;
						mixIndex = mixIndex >> 7;
						break;

					case 5:
						mixIndex = mixIndex << 2;
						mixIndex = mixIndex >> 7;
						break;
					case 6:
						mixIndex = mixIndex << 1;
						mixIndex = mixIndex >> 7;
						break;

					case 7:
						mixIndex = mixIndex >> 7;
						break;
					}

					//将像素数据保存到数组中对应的位置
					dataOfBmp[index].rgbRed = pRgb[mixIndex].rgbRed;
					dataOfBmp[index].rgbGreen = pRgb[mixIndex].rgbGreen;
					dataOfBmp[index].rgbBlue = pRgb[mixIndex].rgbBlue;
					dataOfBmp[index].rgbReserved = pRgb[mixIndex].rgbReserved;
					index++;

				}
		}

		if (bitInfoHead.biBitCount == 2)
		{
			for (int i = 0;i<height;i++)
				for (int j = 0;j<width;j++)
				{
					BYTE mixIndex = 0;
					k = i*l_width + j / 4;
					mixIndex = pColorData[k];
					switch (j % 4)
					{
					case 0:
						mixIndex = mixIndex << 6;
						mixIndex = mixIndex >> 6;
						break;
					case 1:
						mixIndex = mixIndex << 4;
						mixIndex = mixIndex >> 6;
						break;
					case 2:
						mixIndex = mixIndex << 2;
						mixIndex = mixIndex >> 6;
						break;
					case 3:
						mixIndex = mixIndex >> 6;
						break;
					}

					//将像素数据保存到数组中对应的位置
					dataOfBmp[index].rgbRed = pRgb[mixIndex].rgbRed;
					dataOfBmp[index].rgbGreen = pRgb[mixIndex].rgbGreen;
					dataOfBmp[index].rgbBlue = pRgb[mixIndex].rgbBlue;
					dataOfBmp[index].rgbReserved = pRgb[mixIndex].rgbReserved;
					index++;


				}
		}
		if (bitInfoHead.biBitCount == 4)
		{
			for (int i = 0;i<height;i++)
				for (int j = 0;j<width;j++)
				{
					BYTE mixIndex = 0;
					k = i*l_width + j / 2;
					mixIndex = pColorData[k];
					if (j % 2 == 0)
					{//低
						mixIndex = mixIndex << 4;
						mixIndex = mixIndex >> 4;
					}
					else
					{//高
						mixIndex = mixIndex >> 4;
					}

					dataOfBmp[index].rgbRed = pRgb[mixIndex].rgbRed;
					dataOfBmp[index].rgbGreen = pRgb[mixIndex].rgbGreen;
					dataOfBmp[index].rgbBlue = pRgb[mixIndex].rgbBlue;
					dataOfBmp[index].rgbReserved = pRgb[mixIndex].rgbReserved;
					index++;

				}

		}
		if (bitInfoHead.biBitCount == 8)
		{
			for (int i = 0;i<height;i++)
				for (int j = 0;j<width;j++)
				{
					BYTE mixIndex = 0;

					k = i*l_width + j;

					mixIndex = pColorData[k];

					dataOfBmp[index].rgbRed = pRgb[mixIndex].rgbRed;
					dataOfBmp[index].rgbGreen = pRgb[mixIndex].rgbGreen;
					dataOfBmp[index].rgbBlue = pRgb[mixIndex].rgbBlue;
					dataOfBmp[index].rgbReserved = pRgb[mixIndex].rgbReserved;
					index++;



				}
		}
		if (bitInfoHead.biBitCount == 16)
		{
			for (int i = 0;i<height;i++)
				for (int j = 0;j<width;j++)
				{
					WORD mixIndex = 0;

					k = i*l_width + j * 2;
					WORD shortTemp;
					shortTemp = pColorData[k + 1];
					shortTemp = shortTemp << 8;

					mixIndex = pColorData[k] + shortTemp;

					dataOfBmp[index].rgbRed = pRgb[mixIndex].rgbRed;
					dataOfBmp[index].rgbGreen = pRgb[mixIndex].rgbGreen;
					dataOfBmp[index].rgbBlue = pRgb[mixIndex].rgbBlue;
					dataOfBmp[index].rgbReserved = pRgb[mixIndex].rgbReserved;
					index++;
				}
		}
	}
	else
	{
		int k;
		int index = 0;
		for (int i = 0;i<height;i++)
			for (int j = 0;j<width;j++)
			{
				k = i*l_width + j * 3;
				dataOfBmp[index].rgbRed = pColorData[k + 2];
				dataOfBmp[index].rgbGreen = pColorData[k + 1];
				dataOfBmp[index].rgbBlue = pColorData[k];
				index++;
			}
	}

	int** pic = (int**)malloc(sizeof(int*)*(width + 1));
	for (int i = 0; i <= width; i++) {
		pic[i] = (int*)malloc(sizeof(int)*height);
	}

	pic[0][0] = width; pic[0][1] = height;
	int index = 0;
	for (int i = 1; i <= width; i++) {
		for (int j = 0; j < height; j++) {
			pic[i][j] = (int)(0.299*dataOfBmp[index].rgbRed + 0.587*dataOfBmp[index].rgbGreen + 0.114*dataOfBmp[index].rgbBlue);
			index++;
		}
	}

	fclose(pfile);
	if (bitInfoHead.biBitCount<24)
	{
		free(pRgb);
	}
	free(dataOfBmp);
	free(pColorData);
	printf("\n");
	return pic;
}




void main()
{
	int** pic1 = readBMP("1.bmp",1);
	int** pic2 = readBMP("2.bmp",2);

	int** keyPoints1 = SIFT(pic1);
	int** keyPoints2 = SIFT(pic2);

	match(pic1, pic2, keyPoints1, keyPoints2);
	
	free(keyPoints1);
	free(keyPoints2);
	free(pic1);
	free(pic2);
	while (1);
}
