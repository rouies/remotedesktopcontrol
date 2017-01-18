package com.rouies.remotedesktop.decompress;

import java.io.IOException;

public class LZODecompresor {
	static {
		System.loadLibrary("BitImageLZO");
	}
	public native int compress(byte[] source,int size) throws IOException;
	public native void decompress(byte[] source,int size,int srcSize) throws IOException;
}
