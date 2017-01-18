package com.rouies.remotedesktop.network;

import java.io.ByteArrayOutputStream;
import java.nio.ByteOrder;

import com.cosmos.utils.io.SignedDataTypeConverter;
import com.cosmos.utils.io.exception.SignedTypeFormatException;

public class InstructionsCode {
	
	private int direct;

    private int levelOneCode;

    private int levelTwoCode;

    private byte[] data;
    
	public int getDirect() {
		return direct;
	}

	public void setDirect(int direct) {
		this.direct = direct;
	}

	public int getLevelOneCode() {
		return levelOneCode;
	}

	public void setLevelOneCode(int levelOneCode) {
		this.levelOneCode = levelOneCode;
	}

	public int getLevelTwoCode() {
		return levelTwoCode;
	}

	public void setLevelTwoCode(int levelTwoCode) {
		this.levelTwoCode = levelTwoCode;
	}

	public byte[] getData() {
		return data;
	}

	public void setData(byte[] data) {
		this.data = data;
	}
	
	public static InstructionsCode Parse(byte[] pipe) throws SignedTypeFormatException
    {
		InstructionsCode result = new InstructionsCode();
        result.direct = pipe[0];
        result.levelOneCode = pipe[1];
        result.levelTwoCode = pipe[2];
        int length = SignedDataTypeConverter.toInt16(new byte[]{pipe[3],pipe[4]});
        result.data = null;
        if (length > 0)
        {
            result.data = new byte[length];
            System.arraycopy(pipe, 5, result.data, 0, length);
        }
        else
        {
            result.data = new byte[0];
        }
        return result;
    }

    public byte[] ToCode()
    {
        ByteArrayOutputStream result = new ByteArrayOutputStream();
        result.write(this.direct);
        result.write(this.levelOneCode);
        result.write(this.levelTwoCode);
        if(this.data != null && this.data.length > 0){
            short len = (short)this.data.length;
            byte[] blen = SignedDataTypeConverter.toByte(len,ByteOrder.LITTLE_ENDIAN);
            result.write(blen, 0, blen.length);
            result.write(this.data, 0, this.data.length);
        }
        else
        {
            short len = 0;
            byte[] blen = SignedDataTypeConverter.toByte(len,ByteOrder.LITTLE_ENDIAN);
            result.write(blen, 0, blen.length);
        }
      
        return result.toByteArray();
    }

	
}
