package com.rouies.remotedesktop.ctx;

import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.io.UnsupportedEncodingException;
import java.net.InetAddress;
import java.nio.ByteOrder;
import java.util.Arrays;
import java.util.concurrent.atomic.AtomicBoolean;
import java.util.concurrent.locks.ReentrantReadWriteLock;

import com.cosmos.utils.io.SignedDataTypeConverter;
import com.cosmos.utils.io.StreamProcessor;
import com.cosmos.utils.network.NetworkException;
import com.cosmos.utils.network.tcp.TcpClient;
import com.cosmos.utils.network.tcp.TcpServer;
import com.rouies.remotedesktop.decompress.BitImageOutputStream;
import com.rouies.remotedesktop.decompress.LZODecompresor;
import com.rouies.remotedesktop.network.LoginUsers;
import com.rouies.remotedesktop.network.UdpControlObject;
import com.rouies.remotedesktop.network.UdpRedisOp;
import com.rouies.remotedesktop.network.UdpReportObject;
import com.rouies.remotedesktop.network.UdpReportRedisOp;

public class Context {

	public static interface OpCode {
		
		public int value();

		public LevelOne getBase();
	}

	public static enum LevelOne {
		MOUSE_AND_KEYBOARD(0), IMAGE_CONTROL(1), SYSTEM_OP(2);

		private int value;

		LevelOne(int value) {
			this.value = value;
		}

		public int value() {
			return this.value;
		}
	}

	public static enum MouseAndKeyboardCode implements OpCode {
		MOUSE_MOVE(0), LB_MOUSE_DOWN(1), LB_MOUSE_UP(2), RB_MOUSE_DOWN(3), RB_MOUSE_UP(
				4), KEYBOARD_DOWN(5), KEYBOARD_UP(6);

		private int value;

		private static LevelOne baseLevel = LevelOne.MOUSE_AND_KEYBOARD;

		MouseAndKeyboardCode(int value) {
			this.value = value;
		}

		public LevelOne getBase() {
			return MouseAndKeyboardCode.baseLevel;
		}

		public int value() {
			return this.value;
		}
	}

	public static enum ImageCode implements OpCode {
		START_SEND_IMAGE(0), STOP_SEND_IMAGE(1);

		private int value;

		private static LevelOne baseLevel = LevelOne.IMAGE_CONTROL;

		ImageCode(int value) {
			this.value = value;
		}

		public LevelOne getBase() {
			return ImageCode.baseLevel;
		}

		public int value() {
			return this.value;
		}
	}
	
	public static enum SystemOpCode implements OpCode {
		MODIFY_AUTH(0), 
		MODIFY_LOCK(1),
	    MODIFY_ID(2),
	    MODIFY_LOGIN_NAME(3),
	    MODIFY_SUPPER_CODE(4),
	    LOGOUT(5),
	    SHOW_MESSAGE(6);

		private int value;

		private static LevelOne baseLevel = LevelOne.SYSTEM_OP;

		SystemOpCode(int value) {
			this.value = value;
		}

		public LevelOne getBase() {
			return SystemOpCode.baseLevel;
		}

		public int value() {
			return this.value;
		}
	}
	
	static String serverIp;
	
	static int imgPort;
	
	static int ctlPort;
	
	static int reportPort;
	
	static int reportTimeout;
	
	static int userTimeout;

	private static Context ctx = new Context();

	public static Context getInstance() {
		return ctx;
	}
	
	private Context(){
		try {
			this.server = new TcpServer(new ImageTcpProcessor());
		} catch (NetworkException e) {
			e.printStackTrace();
		}
	}
	
	private boolean isTcpServerStarted = false;
	
	private TcpServer server = null;
	
	private UdpControlObject controlObject = null;
	
	private UdpReportObject reportObject = null;

	private AtomicBoolean desktopConnected = new AtomicBoolean(false);

	private TcpClient connectedTcpClient;
		
	private String connectedAddress;
	
	private ReentrantReadWriteLock lock = new ReentrantReadWriteLock();
	
	private class ImageTcpProcessor implements TcpServer.TcpProcessor {
		
		private static final long timeout = 5000L;

		private LZODecompresor lzo = new LZODecompresor();
		
		private byte[] buffer = new byte[10 * 1024 * 1024];

		@Override
		public void process(TcpClient client) {
			
			if(ctx.isDesktopConnected()){
				//已经有桌面链接，拒绝请求
				try {
					client.close();
				} catch (IOException e) {
				}
				return;
			}
			ctx.lock.writeLock().lock();
			ImageBuffer.ipAddress = client.getBaseClient().getInetAddress().getHostAddress();
			ctx.desktopConnected.set(true);
			ctx.connectedAddress = client.getBaseClient().getInetAddress().getHostAddress();
			ctx.lock.writeLock().unlock();
			StreamProcessor processor = client.getProcessor();
			processor.setOrder(ByteOrder.LITTLE_ENDIAN);
			try {
				byte eof = 0xF;
				int width = processor.readInt32();
				int height = processor.readInt32();
				ImageBuffer.width = width;
				ImageBuffer.height = height;
				ImageBuffer.clear();
				processor.writeBytes(eof);
				while (true) {
					try {
						long size = processor.readInt64();
						long srcsize = processor.readInt64();
						ByteArrayOutputStream out = new BitImageOutputStream(buffer);
						processor.writeToOutputStream(out, size);
						lzo.decompress(buffer, (int) size, (int) srcsize);
						ImageBuffer.appendImage(Arrays.copyOf(buffer,(int) srcsize),timeout);
						processor.writeBytes(eof);
					} catch (Exception e) {
						break;
					}
				}
			} catch (Exception e) {
				e.printStackTrace();
			} finally {
				ctx.lock.writeLock().lock();
				ImageBuffer.ipAddress = null;
				ctx.connectedAddress = null;
				ctx.connectedTcpClient = null;
				ctx.isTcpServerStarted = false;
				try {
					client.close();
				} catch (IOException e) {
				}
				ctx.desktopConnected.set(false);
				ctx.lock.writeLock().unlock();
			}
		}
	}
	
	public synchronized boolean startTcpServer(int timeout) throws NetworkException{
		if(isTcpServerStarted){
			return false;
		}
		try {
			this.connectedTcpClient = this.server.listenOnce(InetAddress.getByName(serverIp), imgPort, timeout);
		} catch (IOException e) {
			return false;
		}
		if(this.connectedTcpClient == null){
			return false;
		} else {
			ctx.lock.writeLock().lock();
			this.isTcpServerStarted = true;
			ctx.lock.writeLock().unlock();
			return true;
		}
	}
	
	public synchronized void stopTcpServer(){
		if(this.connectedTcpClient != null){
			try {
				connectedTcpClient.close();
			} catch (IOException e) {
			}
			ctx.lock.writeLock().lock();
			ImageBuffer.ipAddress = null;
			ImageBuffer.clear();
			ctx.desktopConnected.set(false);
			isTcpServerStarted = false;
			ctx.lock.writeLock().unlock();
		}
		
	}
	
	public void startUdpServer() throws NetworkException{
		if(this.controlObject != null || this.reportObject!=null){
			return;
		}
		this.controlObject = new UdpControlObject(serverIp, ctlPort,new UdpRedisOp(userTimeout));
		this.reportObject  = new UdpReportObject(serverIp, reportPort, new UdpReportRedisOp(reportTimeout));
		new Thread(new Runnable() {
			@Override
			public void run() {
				try {
					ctx.controlObject.listen();
				} catch (Exception e) {
					e.printStackTrace();
				}
			}
		}).start();
		new Thread(new Runnable() {
			@Override
			public void run() {
				try {
					ctx.reportObject.listen();
				} catch (Exception e) {
					e.printStackTrace();
				}
			}
		}).start();
	}
	
	public void stopUdpServer(){
		if(this.controlObject != null){
			this.controlObject.stop();
		}
	}
	
	
	
	public synchronized boolean isOwnerConnection(String ipAddress){
		if(this.connectedAddress == null){
			return false;
		}
		return connectedAddress.equals(ipAddress);
	}

	public boolean isDesktopConnected() {
		return desktopConnected.get();
	}

	public synchronized void sendCommand(short reqId,OpCode op, byte[] data,
			String address, int port) throws NetworkException {
		this.controlObject.sendCommand(reqId,op.getBase().value(), op.value(), data, address, port);
	}


	public LoginUsers getLoginUsers() {
		return LoginUsers.getInstance();
	}
	
	public void sendRequestImageIns(boolean startStatus,String address,int port) throws NetworkException{
		this.sendCommand((short)0,(startStatus ? ImageCode.START_SEND_IMAGE : ImageCode.STOP_SEND_IMAGE), null, address, port);
	}
	
	public void sendMouseMoveIns(int x,int y,String address,int port) throws NetworkException{
		byte[] data = new byte[8];
		byte[] bx = SignedDataTypeConverter.toByte(x,ByteOrder.LITTLE_ENDIAN);
		byte[] by = SignedDataTypeConverter.toByte(y,ByteOrder.LITTLE_ENDIAN);
		System.arraycopy(bx, 0, data, 0, 4);
		System.arraycopy(by, 0, data, 4, 4);
		this.sendCommand((short)0,MouseAndKeyboardCode.MOUSE_MOVE, data, address, port);
	}
	
	public void sendMouseLBDownIns(String address,int port) throws NetworkException{
		this.sendCommand((short)0,MouseAndKeyboardCode.LB_MOUSE_DOWN, null, address, port);
	}
	
	public void sendMouseLBUpIns(String address,int port) throws NetworkException{
		this.sendCommand((short)0,MouseAndKeyboardCode.LB_MOUSE_UP, null, address, port);
	}
	
	public void sendMouseRBDownIns(String address,int port) throws NetworkException{
		this.sendCommand((short)0,MouseAndKeyboardCode.RB_MOUSE_DOWN, null, address, port);
	}
	
	public void sendMouseRBUpIns(String address,int port) throws NetworkException{
		this.sendCommand((short)0,MouseAndKeyboardCode.RB_MOUSE_UP, null, address, port);
	}
	
	public void sendKeyBoardDownIns(int ch,String address,int port) throws NetworkException{
		byte[] code = SignedDataTypeConverter.toByte(ch,ByteOrder.LITTLE_ENDIAN);
		this.sendCommand((short)0,MouseAndKeyboardCode.KEYBOARD_DOWN, code, address, port);
	}
	
	public void sendKeyBoardUpIns(int ch,String address,int port) throws NetworkException{
		byte[] code = SignedDataTypeConverter.toByte(ch,ByteOrder.LITTLE_ENDIAN);
		this.sendCommand((short)0,MouseAndKeyboardCode.KEYBOARD_UP, code, address, port);
	}
	
	public void sendLogoutIns(short reqId,String address,int port) throws NetworkException{
		this.sendCommand(reqId,SystemOpCode.LOGOUT, null, address, port);
	}
	
	public void sendModifyAuthIns(short reqId,String authCode,String address,int port) throws NetworkException{
		byte[] data;
		try {
			data = authCode.getBytes("ISO-8859-1");
		} catch (UnsupportedEncodingException e) {
			throw new NetworkException("无法解析命令");
		}
		this.sendCommand(reqId,SystemOpCode.MODIFY_AUTH, data, address, port);
	}
	
	public void sendModifyLockIns(short reqId,boolean lkcode,String address,int port) throws NetworkException{
		String code = lkcode ? "1" : "0";
		byte[] data;
		try {
			data = code.getBytes("ISO-8859-1");
		} catch (UnsupportedEncodingException e) {
			throw new NetworkException("无法解析命令");
		}
		this.sendCommand(reqId,SystemOpCode.MODIFY_LOCK, data, address, port);
	}
	
	public void sendModifyIDIns(short reqId,String id,String address,int port) throws NetworkException{
		byte[] data;
		try {
			data = id.getBytes("ISO-8859-1");
		} catch (UnsupportedEncodingException e) {
			throw new NetworkException("无法解析命令");
		}
		this.sendCommand(reqId,SystemOpCode.MODIFY_ID, data, address, port);
	}
	
	public void sendModifyLoginNameIns(short reqId,String name,String address,int port) throws NetworkException{
		byte[] data;
		try {
			data = name.getBytes("ISO-8859-1");
		} catch (UnsupportedEncodingException e) {
			throw new NetworkException("无法解析命令");
		}
		this.sendCommand(reqId,SystemOpCode.MODIFY_LOGIN_NAME, data, address, port);
	}
	
	public void sendShowMessageIns(short reqId,String message,String address,int port) throws NetworkException{
		byte[] data;
		try {
			data = message.getBytes("UTF-8");
		} catch (UnsupportedEncodingException e) {
			throw new NetworkException("无法解析命令");
		}
		this.sendCommand(reqId,SystemOpCode.SHOW_MESSAGE, data, address, port);
	}
	
	public void sendModifySuperCodeIns(short reqId,String code,String address,int port) throws NetworkException{
		byte[] data;
		try {
			data = code.getBytes("ISO-8859-1");
		} catch (UnsupportedEncodingException e) {
			throw new NetworkException("无法解析命令");
		}
		this.sendCommand(reqId,SystemOpCode.MODIFY_SUPPER_CODE, data, address, port);
	}

}
