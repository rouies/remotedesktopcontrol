package com.rouies.remotedesktop.network;

import java.io.ByteArrayOutputStream;
import java.net.DatagramPacket;
import java.net.InetAddress;
import java.nio.ByteOrder;

import redis.clients.jedis.Jedis;

import com.cosmos.utils.io.SignedDataTypeConverter;
import com.cosmos.utils.io.exception.SignedTypeFormatException;
import com.cosmos.utils.network.udp.UdpClient;
import com.fasterxml.jackson.core.JsonFactory;
import com.fasterxml.jackson.core.JsonGenerator;
import com.rouies.utils.redis.RedisContext;

public class UdpRedisOp implements UdpClient.UdpProcessor{

	private JsonFactory jsonFactory = new JsonFactory();
	
	private int timeout = 10;
	
	public  UdpRedisOp(int timeout){
		this.timeout = timeout;
	}
	@Override
	public void process(DatagramPacket dp) {
		InetAddress address = dp.getAddress();
		int port = dp.getPort();
		byte[] id = dp.getData();
		if(id.length >= 24){
			int cpu = id[2];
			int mem = id[3];
			byte[] st = new byte[4];
			byte[] mx = new byte[8];
			byte[] my = new byte[8];
			System.arraycopy(id, 4, st, 0, 4);
			System.arraycopy(id, 8, mx, 0, 8);
			System.arraycopy(id, 16, my, 0, 8);
			int pstatus;
			long x;
			long y;
			try {
				pstatus = SignedDataTypeConverter.toInt32(st,ByteOrder.LITTLE_ENDIAN);
				x       = SignedDataTypeConverter.toInt64(mx,ByteOrder.LITTLE_ENDIAN);
				y       = SignedDataTypeConverter.toInt64(my,ByteOrder.LITTLE_ENDIAN);
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
			String key = "USER:" + ids;
			Jedis client = null;
			ByteArrayOutputStream out = new ByteArrayOutputStream(200);
			try {
				client = RedisContext.getClient("JMC");
				client.select(0);
				JsonGenerator wtr = jsonFactory.createGenerator(out);
				wtr.writeStartObject();
				wtr.writeStringField("address", address.getHostAddress());
				wtr.writeObjectField("port", port);
				wtr.writeObjectField("cpu", cpu);
				wtr.writeObjectField("mem", mem);
				wtr.writeObjectField("process", pstatus);
				wtr.writeObjectField("last", System.currentTimeMillis());
				wtr.writeEndObject();
				wtr.close();
				String val = new String(out.toByteArray(),"ISO-8859-1");
				client.setex(key, this.timeout, val);
				client.select(2);
				String mval = x + "," + y;
				client.set(key, mval);
			} catch (Exception e) {
				e.printStackTrace();
			} finally {
				if(client != null){
					client.close();
				}
			}			
		} 
		
	}

}
