package com.rouies.remotedesktop.ctx;

import java.util.concurrent.ArrayBlockingQueue;
import java.util.concurrent.BlockingQueue;
import java.util.concurrent.TimeUnit;

public class ImageBuffer {
	
	static int width = 0;
	
	static int height = 0;
	
	static String ipAddress;
	
	private static BlockingQueue<byte[]> queue = new ArrayBlockingQueue<byte[]>(5,true);
	
	public static void appendImage(byte[] src) throws InterruptedException{
		queue.put(src);
	}
	
	public static boolean appendImage(byte[] src,long timeout) throws InterruptedException{
		return queue.offer(src, timeout, TimeUnit.MILLISECONDS);
	}
	
	public static byte[] getImage() throws InterruptedException{
		return queue.take();
	}
	
	public static byte[] getImage(long timeout) throws InterruptedException{
		return queue.poll(timeout, TimeUnit.MILLISECONDS);
	}
	
	public static int getWidth(){
		return width;
	}
	
	public static int getHeight(){
		return height;
	}
	
	public static String getAddress(){
		return ipAddress;
	}
	
	public static void clear(){
		queue.clear();
	}
}
