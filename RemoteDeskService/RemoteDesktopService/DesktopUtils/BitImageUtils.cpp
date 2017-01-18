#include "BitImageUtils.h"
#include "minilzo.h"

#define MAX_LEN 1024
#define COMPRESS_BUFFER_SIZE 10485760
#define HEAP_ALLOC(var, size) lzo_align_t __LZO_MMODEL var[((size)+(sizeof(lzo_align_t)-1)) / sizeof(lzo_align_t)]
//the wkmem is used for compressed
static HEAP_ALLOC(wkmem, LZO1X_1_MEM_COMPRESS);

static BYTE lzoBuffer[COMPRESS_BUFFER_SIZE];

/*压缩数据*/
int BitMapCompress(BYTE* in, BYTE* out, ULONG inLen, ULONG* outLen){
	if (lzo_init() != LZO_E_OK) {
		return BITIMAGEUTILS_LZO_INIT_ERROR;
	}
	int ret = -1;
	if ((ret = lzo1x_1_compress(in, inLen, out, outLen, wkmem)) != LZO_E_OK) {
		return BITIMAGEUTILS_LZO_COMPRESS_ERROR;
	}
	return BITIMAGEUTILS_NO_ERROR;
}

/*压缩数据*/
BYTE* BitMapCompress(BYTE* in, ULONG inLen, ULONG* outLen, int* error){
	if (inLen <= COMPRESS_BUFFER_SIZE){
		*error = BitMapCompress(in, lzoBuffer, inLen, outLen);
		if (*error != BITIMAGEUTILS_NO_ERROR){
			return nullptr;
		}
		else {
			return lzoBuffer;
		}
	}
	*error = BITIMAGEUTILS_LZO_COMPRESS_ERROR;
	return nullptr;
}

/*解压缩数据*/
int BitMapDecompress(BYTE* in, BYTE* out, ULONG inLen, ULONG* outLen){
	if (lzo_init() != LZO_E_OK) {
		return BITIMAGEUTILS_LZO_INIT_ERROR;
	}
	int ret = -1;
	if ((ret = lzo1x_decompress(in, inLen, out, outLen, NULL)) != LZO_E_OK) {
		return BITIMAGEUTILS_LZO_DECOMPRESS_ERROR;
	}
	return BITIMAGEUTILS_NO_ERROR;
}

/*解压缩数据*/
BYTE* BitMapDecompress(BYTE* in, ULONG inLen, ULONG* outLen,int* error){
	if (inLen <= COMPRESS_BUFFER_SIZE){
		*error = BitMapDecompress(in, lzoBuffer, inLen, outLen);
		if (*error != BITIMAGEUTILS_NO_ERROR){
			return nullptr;
		}
		else {
			return lzoBuffer;
		}
	}
	*error = BITIMAGEUTILS_LZO_DECOMPRESS_ERROR;
	return nullptr;
}

HBITMAP CopyScreenToBitmap(LPRECT lpRect)
{
	HCURSOR hCursor = NULL;
	POINT pt;
	HWND hwndPt;
	DWORD dwTid = 0, dwCurTid = 0;
	HDC hScrDC, hMemDC;
	// 屏幕和内存设备描述表
	HBITMAP hBitmap, hOldBitmap;
	// 位图句柄
	int nX, nY, nX2, nY2;
	// 选定区域坐标
	int nWidth, nHeight;
	// 位图宽度和高度
	int xScrn, yScrn;
	// 屏幕分辨率
	// 确保选定区域不为空矩形
	if (IsRectEmpty(lpRect)){
		return NULL;
	}
	//为屏幕创建设备描述表
	hScrDC = CreateDC(L"DISPLAY", NULL, NULL, NULL);
	//为屏幕设备描述表创建兼容的内存设备描述表
	hMemDC = CreateCompatibleDC(hScrDC);
	// 获得选定区域坐标
	nX = lpRect->left;
	nY = lpRect->top;
	nX2 = lpRect->right;
	nY2 = lpRect->bottom;
	// 获得屏幕分辨率
	xScrn = GetDeviceCaps(hScrDC, HORZRES);
	yScrn = GetDeviceCaps(hScrDC, VERTRES);
	//确保选定区域是可见的
	if (nX < 0)
		nX = 0;
	if (nY < 0)
		nY = 0;
	if (nX2 > xScrn)
		nX2 = xScrn;
	if (nY2 > yScrn)
		nY2 = yScrn;
	nWidth = nX2 - nX;
	nHeight = nY2 - nY;
	// 创建一个与屏幕设备描述表兼容的位图
	hBitmap = CreateCompatibleBitmap(hScrDC, nWidth, nHeight);
	// 把新位图选到内存设备描述表中
	hOldBitmap = (HBITMAP)SelectObject(hMemDC, hBitmap);
	// 把屏幕设备描述表拷贝到内存设备描述表中
	BitBlt(hMemDC, 0, 0, nWidth, nHeight, hScrDC, nX, nY, SRCCOPY);
	
	//画鼠标
	GetCursorPos(&pt); //获取鼠标坐标   
	hwndPt = WindowFromPoint(pt); //根据坐标找窗口句柄   
	dwTid = GetWindowThreadProcessId(hwndPt, NULL); //获取该窗口所属的线程id   
	dwCurTid = GetCurrentThreadId(); //获取当前线程id   
	if (dwTid != dwCurTid)
	{
		AttachThreadInput(dwCurTid, dwTid, TRUE); //线程输入附加   
		hCursor = GetCursor(); //获取到鼠标光标句柄   
		AttachThreadInput(dwCurTid, dwTid, FALSE);
	}
	else { 
		hCursor = GetCursor(); 
	}
	DrawIconEx(hMemDC, pt.x, pt.y, hCursor, 0, 0, 0, NULL, DI_NORMAL | DI_COMPAT | DI_DEFAULTSIZE); //把鼠标光标画出来 
	//得到屏幕位图的句柄
	hBitmap = (HBITMAP)SelectObject(hMemDC, hOldBitmap);
	//清除 
	DeleteDC(hScrDC);
	DeleteDC(hMemDC);
	// 返回位图句柄
	return hBitmap;
}

int SaveBitmapToMemory(HBITMAP hBitmap, LBitMapInfo image, BOOL isCompress)
{
	int result = BITIMAGEUTILS_NO_ERROR;
	HDC     hDC;
	//设备描述表
	int     iBits;
	//当前显示分辨率下每个像素所占字节数
	WORD    wBitCount;
	//位图中每个像素所占字节数
	//定义调色板大小， 位图中像素字节大小 ，  位图文件大小 ， 写入文件字节数
	DWORD           dwPaletteSize = 0, dwBmBitsSize, dwDIBSize;
	BITMAP          Bitmap;
	//位图属性结构
	BITMAPFILEHEADER   bmfHdr;
	//位图文件头结构
	BITMAPINFOHEADER   bi;
	//位图信息头结构 
	LPBITMAPINFOHEADER lpbi;
	//指向位图信息头结构
	HANDLE          hDib, hPal;
	HPALETTE     hOldPal = NULL;
	//定义文件，分配内存句柄，调色板句柄
	//计算位图文件每个像素所占字节数
	hDC = CreateDC(L"DISPLAY", NULL, NULL, NULL);
	iBits = GetDeviceCaps(hDC, BITSPIXEL) * GetDeviceCaps(hDC, PLANES);
	DeleteDC(hDC);
	//8位色
	wBitCount = 8;
	//计算调色板大小
	if (wBitCount <= 8)
		dwPaletteSize = (1 << wBitCount)*sizeof(RGBQUAD);
	//设置位图信息头结构
	GetObject(hBitmap, sizeof(BITMAP), (LPSTR)&Bitmap);
	bi.biSize = sizeof(BITMAPINFOHEADER);
	bi.biWidth = Bitmap.bmWidth;
	bi.biHeight = Bitmap.bmHeight;
	bi.biPlanes = 1;
	bi.biBitCount = wBitCount;
	bi.biCompression = BI_RGB;
	bi.biSizeImage = 0;
	bi.biXPelsPerMeter = 0;
	bi.biYPelsPerMeter = 0;
	bi.biClrUsed = 0;
	bi.biClrImportant = 0;
	dwBmBitsSize = ((Bitmap.bmWidth*wBitCount + 31) / 32) * 4 * Bitmap.bmHeight;
	//为位图内容分配内存
	hDib = GlobalAlloc(GHND, dwBmBitsSize + dwPaletteSize + sizeof(BITMAPINFOHEADER));
	lpbi = (LPBITMAPINFOHEADER)GlobalLock(hDib);
	*lpbi = bi;
	// 处理调色板   
	hPal = GetStockObject(DEFAULT_PALETTE);
	if (hPal)
	{
		hDC = ::GetDC(NULL);
		hOldPal = SelectPalette(hDC, (HPALETTE)hPal, FALSE);
		RealizePalette(hDC);
	}
	// 获取该调色板下新的像素值
	GetDIBits(hDC, hBitmap, 0, (UINT)Bitmap.bmHeight, (LPSTR)lpbi + sizeof(BITMAPINFOHEADER)+dwPaletteSize, (BITMAPINFO *)lpbi, DIB_RGB_COLORS);
	//恢复调色板   
	if (hOldPal)
	{
		SelectPalette(hDC, hOldPal, TRUE);
		RealizePalette(hDC);
		::ReleaseDC(NULL, hDC);
	}
	// 设置位图文件头
	bmfHdr.bfType = 0x4D42;  // "BM"
	dwDIBSize = sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER)+dwPaletteSize + dwBmBitsSize;
	bmfHdr.bfSize = dwDIBSize;
	bmfHdr.bfReserved1 = 0;
	bmfHdr.bfReserved2 = 0;
	bmfHdr.bfOffBits = (DWORD)sizeof(BITMAPFILEHEADER)+(DWORD)sizeof(BITMAPINFOHEADER)+dwPaletteSize;
	/*将内容拷贝参数*/
	
	UINT total = sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER)+dwPaletteSize + dwBmBitsSize;
	image->src_size = total;
	if (image->size >= total){
		
		UINT index = 0;
		UINT len = sizeof(BITMAPFILEHEADER);
		char* head = (char*)&bmfHdr;
		for (UINT i = 0; i < len; i++){
			image->out[index++] = head[i];
		}
		len = sizeof(BITMAPINFOHEADER)+dwPaletteSize + dwBmBitsSize;
		char* body = (char*)lpbi;
		for (UINT i = 0; i < len; i++){
			image->out[index++] = body[i];
		}
		image->size = total;
		
		if (isCompress){
			index = 0;
			ULONG outw = 0;
			int error = 0;
			BYTE* res = BitMapCompress(image->out, total, &outw, &error);
			if (error != BITIMAGEUTILS_NO_ERROR){
				result = error;
			}
			else {
				for (UINT i = 0; i < outw; i++){
					image->out[index++] = res[i];
				}
			}
			image->size = outw;
		}
		
	}
	else {
		result = BITIMAGEUTILS_BUFFER_ERROR;
	}
	
	//清除   
	GlobalUnlock(hDib);
	GlobalFree(hDib);
	return result;
}
