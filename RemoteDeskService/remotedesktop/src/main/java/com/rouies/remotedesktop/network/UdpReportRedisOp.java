package com.rouies.remotedesktop.network;

import java.net.DatagramPacket;
import java.nio.ByteOrder;
import java.util.concurrent.SynchronousQueue;
import java.util.concurrent.TimeUnit;

import redis.clients.jedis.Jedis;

import com.cosmos.utils.io.SignedDataTypeConverter;
import com.cosmos.utils.io.UnsignedDataTypeConverter;
import com.cosmos.utils.io.exception.SignedTypeFormatException;
import com.cosmos.utils.network.udp.UdpClient;
import com.rouies.utils.redis.RedisContext;

public class UdpReportRedisOp implements UdpClient.UdpProcessor{
	
	private int timeout = 10;
	
	public  UdpReportRedisOp(int timeout){
		this.timeout = timeout;
	}
	
	@Override
	public void process(DatagramPacket dp) {
		byte[] data = dp.getData();
		byte requestId = data[0];
		byte[] bid = new byte[2];
		System.arraycopy(data, 1, bid, 0, 2);
		int state = data[3]; 
		short reqId = UnsignedDataTypeConverter.convertUInt8ToInt16(ByteOrder.LITTLE_ENDIAN,requestId);
		short id = -1;
		try {
			id  = SignedDataTypeConverter.toInt16(bid,ByteOrder.LITTLE_ENDIAN);
		} catch (SignedTypeFormatException e) {
			return;
		}
		String key = String.format("%s:%s", id,reqId);
//		System.out.println("收到反馈包:" + key);
		SynchronousQueue<Boolean> sync = SyncReportObject.get(key);
		if(sync != null){
			try {
				sync.offer(true, 10000, TimeUnit.MILLISECONDS);
			} catch (InterruptedException e) {
				e.printStackTrace();
			}
		}
		Jedis client = null;
		try {
			client = RedisContext.getClient("JMC");
			client.select(1);
			String val = String.valueOf(state);
			client.setex(key, this.timeout, val);
		} catch (Exception e) {
			e.printStackTrace();
		} finally {
			if(client != null){
				client.close();
			}
		}
		
	}

}
