#include "com_rouies_remotedesktop_decompress_LZODecompresor.h"
#include "minilzo.h"
#include <stdlib.h>
#include <string.h>

#define MAX_LEN 1024

#define BYTE unsigned char

#define HEAP_ALLOC(var, size) lzo_align_t __LZO_MMODEL var[((size)+(sizeof(lzo_align_t)-1)) / sizeof(lzo_align_t)]
#define NO_ERROR 0
#define LZO_INIT_ERROR 1
#define LZO_COMPRESS_ERROR 2
#define LZO_DECOMPRESS_ERROR 3

#define COMPRESS_BUFFER_SIZE 10485760
static HEAP_ALLOC(wkmem, LZO1X_1_MEM_COMPRESS);

static BYTE lzoBuffer[COMPRESS_BUFFER_SIZE];

/*压缩数据*/
int BitMapCompress( const lzo_bytep in, lzo_bytep  out, lzo_uint inLen,lzo_uintp outLen){
    if (lzo_init() != LZO_E_OK) {
        return LZO_INIT_ERROR;
    }
    int ret = -1;
    if ((ret = lzo1x_1_compress(in, inLen, out, outLen, wkmem)) != LZO_E_OK) {
        return LZO_COMPRESS_ERROR;
    }
    return NO_ERROR;
}

/*解压缩数据*/
int BitMapDecompress( const lzo_bytep in, lzo_bytep  out, lzo_uint inLen,lzo_uintp outLen){
    if (lzo_init() != LZO_E_OK) {
        return LZO_INIT_ERROR;
    }
    int ret = -1;
    if ((ret = lzo1x_decompress(in, inLen, out, outLen, NULL)) != LZO_E_OK) {
        return LZO_DECOMPRESS_ERROR;
    }
    return NO_ERROR;
}


JNIEXPORT jint JNICALL Java_com_rouies_remotedesktop_decompress_LZODecompresor_compress
  (JNIEnv* env, jobject, jbyteArray in, jint size){
    jsize len = env->GetArrayLength(in);
    if( len < size ){
        return 0;
    }
    jbyte* src = env->GetByteArrayElements(in,0);
    lzo_uint inLength = size;
    lzo_uint outLength = -1;
    if(BitMapCompress((BYTE*)src,lzoBuffer,inLength,&outLength) != NO_ERROR){
         return 0;
    }
    env->SetByteArrayRegion(in,0,outLength,(jbyte*)lzoBuffer);
    env->ReleaseByteArrayElements(in,src,JNI_ABORT);
    return outLength;
}


JNIEXPORT void JNICALL Java_com_rouies_remotedesktop_decompress_LZODecompresor_decompress
  (JNIEnv* env, jobject, jbyteArray in, jint size, jint src_size){
    jthrowable exc = NULL;
    lzo_uint inLength = size;
    lzo_uint outLength = src_size;
    jsize len = env->GetArrayLength(in);
     jbyte* src = env->GetByteArrayElements(in,0);
    if( len < src_size ||   len < size){
        exc = env->ExceptionOccurred();
    } else {
        if(BitMapDecompress((BYTE*)src,lzoBuffer,inLength,&outLength) != NO_ERROR){
             exc = env->ExceptionOccurred();
        }
        if(outLength > (lzo_uint)len){
             exc = env->ExceptionOccurred();
        }
    }
    if (exc) {
        jclass newExcCls;
        env->ExceptionDescribe();
        env->ExceptionClear();
        newExcCls = env->FindClass("java/io/IOException");
        env->ThrowNew(newExcCls, "decompress-error");
    } else {
        env->SetByteArrayRegion(in,0,outLength,(jbyte*)lzoBuffer);
    }
    env->ReleaseByteArrayElements(in,src,JNI_ABORT);
}



