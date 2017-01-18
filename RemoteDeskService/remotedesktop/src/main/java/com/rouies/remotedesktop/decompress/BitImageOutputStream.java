package com.rouies.remotedesktop.decompress;

import java.io.ByteArrayOutputStream;

public class BitImageOutputStream extends ByteArrayOutputStream{
	
	public BitImageOutputStream(byte[] buffer){
		this.buf = buffer;
	}
	
}
