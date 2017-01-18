package com.rouies.remotedesktop.network;

import com.cosmos.utils.network.NetworkException;
import com.cosmos.utils.network.udp.UdpClient;

public class UdpControlObject extends ControlObject{

	private UdpClient client;
	
	public UdpControlObject(String address,int port,UdpClient.UdpProcessor proc) throws NetworkException{
		this.client = new UdpClient(address, port, proc);
	}
	
	public void listen(){
		this.client.listen();
	}
	
	public void stop(){
		this.client.close();
	}
	
	@Override
	public void sendInstructionsCode(InstructionsCode code,String address,int port) throws NetworkException {
		byte[] tcode = code.ToCode();
		this.client.sendPacket(tcode, address, port);
	}

}
