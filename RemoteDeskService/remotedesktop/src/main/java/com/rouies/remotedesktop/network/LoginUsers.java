package com.rouies.remotedesktop.network;

import java.net.InetAddress;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.Iterator;
import java.util.List;
import java.util.Map;
import java.util.Map.Entry;
import java.util.concurrent.ConcurrentHashMap;

import com.fasterxml.jackson.core.JsonProcessingException;
import com.fasterxml.jackson.databind.ObjectMapper;

public class LoginUsers {
	
	private static LoginUsers users = new LoginUsers();
	
	public static LoginUsers getInstance(){
		return  users;
	}
	
	private static ObjectMapper json = new ObjectMapper();
	
//	public static class ID {
//		
//		private static Long HASH_CODE = 6505617023771270117L;
//		
//		private byte[] contents;
//		
//		public ID(byte[] contents){
//			this.contents = contents;
//		}
//		
//		public ID(byte[] contents,int startIndex,int length){
//			this.contents = new byte[length];
//			System.arraycopy(contents, startIndex, this.contents, 0, length);
//		}
//		
//		public byte[] getIdContents(){
//			return this.contents;
//		}
//		
//		@Override
//		public int hashCode() {
//			return HASH_CODE.hashCode();
//		}
//		
//		@Override
//		public boolean equals(Object obj) {
//			byte[] dt = ((ID) obj).contents;
//			return ArraysUtils.byteArrayEquals(dt, this.contents);
//		}
//	}
//	
	public static class User{
		
		private User(){};
		
		private InetAddress address;
		
		private int port;
		
		private long lastAccessTime;
		
		private int cpuRate;
		
		private int menRate;
		
		private int processStatus;
		
		public int getCpuRate() {
			return cpuRate;
		}

		public int getMenRate() {
			return menRate;
		}

		public int getProcessStatus() {
			return processStatus;
		}

//		private byte[] tag;
//
//		public byte[] getTag() {
//			return tag;
//		}
//
//		public void setTag(byte[] tag) {
//			this.tag = tag;
//		}

		public long getLastAccessTime() {
			return lastAccessTime;
		}

		public InetAddress getAddress() {
			return address;
		}
		
		public int getPort() {
			return port;
		}
		
	}
	
	private LoginUsers(){}
	
//	private ConcurrentHashMap<ID, User> contents = new ConcurrentHashMap<>();
	
	private ConcurrentHashMap<String, User> contents = new ConcurrentHashMap<>();
	
	public void put(String id,InetAddress address,int port,int cpu,int men,int processStatus){
		User user = new User();
		user.address = address;
		user.port = port;
		user.lastAccessTime = System.currentTimeMillis();
		user.cpuRate = cpu;
		user.menRate = men;
		user.processStatus = processStatus;
		this.contents.put(id, user);
	}
	
	public User getLoginUserById(String id){
		return this.contents.get(id);
	}
	
	public Map<String, User> getLoginUsers(){
		HashMap<String, User> result = new HashMap<String, LoginUsers.User>();
		Iterator<Entry<String, User>> iterator = this.contents.entrySet().iterator();
		while(iterator.hasNext()){
			Entry<String, User> item = iterator.next();
			result.put(item.getKey(), item.getValue());
		}
		return result;
	}
	
	public byte[] toJson(String id) throws JsonProcessingException{
		byte[] result = null;
		User user = this.contents.get(id);
		if(user != null){
			HashMap<String, Object> it = new HashMap<String, Object>();
			it.put("id", id);
			it.put("address", user.getAddress().getHostAddress());
			it.put("port", user.getPort());
			it.put("last", user.getLastAccessTime());
			it.put("cpu", user.getCpuRate());
			it.put("memory", user.getMenRate());
			it.put("process", user.getProcessStatus());
			result = json.writeValueAsBytes(it);
		}
		
		return result;
	}
	
	public byte[] toJson() throws JsonProcessingException{
		List<Map<String, Object>> rs = new ArrayList<Map<String,Object>>(this.contents.size());
		for (Entry<String, User> item : this.contents.entrySet()) {
			HashMap<String, Object> it = new HashMap<String, Object>();
			it.put("id", item.getKey());
			it.put("address", item.getValue().getAddress().getHostAddress());
			it.put("port", item.getValue().getPort());
			it.put("last", item.getValue().getLastAccessTime());
			it.put("cpu", item.getValue().getCpuRate());
			it.put("memory", item.getValue().getMenRate());
			it.put("process", item.getValue().getProcessStatus());
			rs.add(it);
		}
		byte[] ctn = json.writeValueAsBytes(rs);
		return ctn;
	}
	
}
