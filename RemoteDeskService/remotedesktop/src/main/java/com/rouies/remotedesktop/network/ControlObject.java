package com.rouies.remotedesktop.network;

import com.cosmos.utils.network.NetworkException;

public abstract class ControlObject {
	
	public void sendCommand(short reqId,int levelOne,int levelTwo,byte[] data,String address,int port) throws NetworkException{
		if(reqId < 0 || reqId > 255){
			throw new NetworkException("请求ID必须在[0,255]");
		}
		InstructionsCode code = new InstructionsCode();
		code.setDirect(reqId);
		code.setLevelOneCode(levelOne);
		code.setLevelTwoCode(levelTwo);
		code.setData(data);
		this.sendInstructionsCode(code,address,port);
	}
	
	public abstract void sendInstructionsCode(InstructionsCode code,String address,int port) throws NetworkException;
}
