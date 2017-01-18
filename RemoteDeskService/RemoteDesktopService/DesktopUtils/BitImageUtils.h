#ifndef BITIMAGEUTILS
#define BITIMAGEUTILS

#define BITIMAGEUTILS_NO_ERROR 0
#define BITIMAGEUTILS_BUFFER_ERROR 1
#define BITIMAGEUTILS_LZO_INIT_ERROR 2
#define BITIMAGEUTILS_LZO_COMPRESS_ERROR 3
#define BITIMAGEUTILS_LZO_DECOMPRESS_ERROR 4


#include "stdafx.h"

/*该结构用于跟与非托管代码进行交互而使用*/
typedef struct{
	//表示位图内容
	BYTE* out;
	//表示位图大小
	unsigned int size;
	unsigned int src_size;
} BitMapInfo, *LBitMapInfo;

/*数据压缩*/
int BitMapCompress(BYTE* in, BYTE* out, ULONG inLen, ULONG* outLen);
/*数据压缩*/
BYTE* BitMapCompress(BYTE* in, ULONG inLen, ULONG* outLen, int* error);
/*数据解压缩*/
int BitMapDecompress(BYTE* in, BYTE* out, ULONG inLen, ULONG* outLen);
/*数据解压缩*/
BYTE* BitMapDecompress(BYTE* in, ULONG inLen, ULONG* outLen, int* error);


/*该方法用于将屏幕特定区域截图到HBITMAP*/
HBITMAP CopyScreenToBitmap(LPRECT lpRect);

/*该方法用于将HBITMAP装入内存*/
int SaveBitmapToMemory(HBITMAP hBitmap, LBitMapInfo image, BOOL isCompress = false);

#endif