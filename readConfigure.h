#define   WIDTHBYTES(bits) (((bits)+31)/32*4)

typedef unsigned char BYTE;
typedef unsigned short WORD;
typedef unsigned long DWORD;
typedef long LONG;


//λͼ�ļ�ͷ��Ϣ�ṹ����

typedef struct tagBITMAPFILEHEADER {

	DWORD bfSize;           //�ļ���С
	WORD   bfReserved1; 	//������
	WORD   bfReserved2; 	//������
	DWORD bfOffBits;        //ʵ��λͼ���ݵ�ƫ���ֽ���
} BITMAPFILEHEADER;


//��ϢͷBITMAPINFOHEADER��

typedef struct tagBITMAPINFOHEADER {
	//public:
	DWORD   biSize;         	//ָ���˽ṹ��ĳ��ȣ�Ϊ40
	LONG    biWidth;       		//λͼ��
	LONG    biHeight;       	//λͼ��
	WORD    biPlanes;       	//ƽ������Ϊ1
	WORD    biBitCount;     	//������ɫλ����������1��2��4��8��16��24���µĿ�����32
	DWORD   biCompression;  	//ѹ����ʽ��������0��1��2������0��ʾ��ѹ��
	DWORD   biSizeImage;    	//ʵ��λͼ����ռ�õ��ֽ���
	LONG    biXPelsPerMeter;	//X����ֱ���
	LONG    biYPelsPerMeter;	//Y����ֱ���
	DWORD   biClrUsed;      	//ʹ�õ���ɫ�������Ϊ0�����ʾĬ��ֵ(2^��ɫλ��)
	DWORD   biClrImportant; 	//��Ҫ��ɫ�������Ϊ0�����ʾ������ɫ������Ҫ��
} BITMAPINFOHEADER;


//��ɫ��Palette


typedef struct tagRGBQUAD {
	//public:
	BYTE     rgbBlue; //����ɫ����ɫ����
	BYTE     rgbGreen; //����ɫ����ɫ����
	BYTE     rgbRed; //����ɫ�ĺ�ɫ����
	BYTE     rgbReserved; //����ֵ
}RGBQUAD;
