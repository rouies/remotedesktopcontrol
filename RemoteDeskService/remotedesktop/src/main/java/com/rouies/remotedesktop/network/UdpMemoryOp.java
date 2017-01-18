package com.rouies.remotedesktop.network;

import java.net.DatagramPacket;
import java.net.InetAddress;
import java.nio.ByteOrder;

import com.cosmos.utils.io.SignedDataTypeConverter;
import com.cosmos.utils.io.exception.SignedTypeFormatException;
import com.cosmos.utils.network.udp.UdpClient;

public class UdpMemoryOp implements UdpClient.UdpProcessor{

	@Override
	public void process(DatagramPacket dp) {
		InetAddress address = dp.getAddress();
		int port = dp.getPort();
//		System.out.println("收到数据:" + address.getHostAddress() + ":" + port);
		byte[] id = dp.getData();
		if(id.length >= 8){
			LoginUsers users = LoginUsers.getInstance();
			int cpu = id[2];
			int mem = id[3];
			byte[] st = new byte[4];
			System.arraycopy(id, 4, st, 0, 4);
			int pstatus;
			try {
				pstatus = SignedDataTypeConverter.toInt32(st,ByteOrder.LITTLE_ENDIAN);
			} catch (SignedTypeFormatException e) {
				return;
			}
			byte[] bid = new byte[2];
			System.arraycopy(id, 0, bid, 0, 2);
			String ids;
			try {
				ids = String.valueOf(SignedDataTypeConverter.toInt16(bid,ByteOrder.LITTLE_ENDIAN));
			} catch (SignedTypeFormatException e) {
				return;
			}
			users.put(ids, address, port,cpu,mem,pstatus);		
		} 
	}
	
}
