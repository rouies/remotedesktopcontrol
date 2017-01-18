package com.rouies.remotedesktop.network;

import com.cosmos.utils.network.NetworkException;
import com.cosmos.utils.network.udp.UdpClient;

public class UdpReportObject {
	
	private UdpClient client;
	
	public UdpReportObject(String address,int port,UdpClient.UdpProcessor proc) throws NetworkException{
		this.client = new UdpClient(address, port, proc);
	}
	
	public void listen(){
		this.client.listen();
	}
	
	public void stop(){
		this.client.close();
	}
}
