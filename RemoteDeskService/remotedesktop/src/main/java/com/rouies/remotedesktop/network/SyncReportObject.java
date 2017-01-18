package com.rouies.remotedesktop.network;

import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.SynchronousQueue;

public class SyncReportObject {
	
	private static ConcurrentHashMap<String, SynchronousQueue<Boolean>> ctx = new ConcurrentHashMap<String, SynchronousQueue<Boolean>>();
	
	public static SynchronousQueue<Boolean> put(String key){
		SynchronousQueue<Boolean> res = new SynchronousQueue<>();
		ctx.put(key, res);
		return res;
	}
	
	public static boolean exists(String key){
		return ctx.containsKey(key);
	}
	
	public static SynchronousQueue<Boolean> get(String key){
		return ctx.get(key);
	}
	
	public static void remove(String key){
		ctx.remove(key);
	}
}
