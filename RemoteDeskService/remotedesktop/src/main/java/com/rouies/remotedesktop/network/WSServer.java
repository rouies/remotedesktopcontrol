package com.rouies.remotedesktop.network;

import java.nio.ByteBuffer;
import java.util.concurrent.atomic.AtomicReference;

import javax.websocket.OnClose;
import javax.websocket.OnError;
import javax.websocket.OnMessage;
import javax.websocket.OnOpen;
import javax.websocket.Session;
import javax.websocket.server.ServerEndpoint;

import com.rouies.remotedesktop.ctx.Context;
import com.rouies.remotedesktop.ctx.ImageBuffer;

@ServerEndpoint("/websocket")
public class WSServer {
    /**
     * 连接建立成功调用的方法
     * @param session  可选的参数。session为与某个客户端的连接会话，需要通过它来给客户端发送数据
     */
		
	private static AtomicReference<String> id = new AtomicReference<String>(null);
	
//	private static Lock lock = new ReentrantLock();
	
    @OnOpen
    public synchronized void onOpen(Session session){
    	
    }
     
    /**
     * 连接关闭调用的方法
     */
    @OnClose
    public synchronized void onClose(Session session){
    	if(session.getId().equals(id.get())){
    		id.set(null);
    		Context.getInstance().stopTcpServer();
    	}
    }
    
     
    /**
     * 收到客户端消息后调用的方法
     * @param message 客户端发送过来的消息
     * @param session 可选的参数
     */
    @OnMessage
    public void onMessage(String message, Session session) {
    	String[] split = message.split(":");
    	if(split[0].equals("R") && split[1].equals(ImageBuffer.getAddress())){
    		if(id.get() == null){
    			id.set(session.getId());
    		} 
    		if(!session.getId().equals(id.get())){
				return;
			}
			session.setMaxBinaryMessageBufferSize(10 * 1024 * 1024);
			try {
				byte[] image = ImageBuffer.getImage();
				session.getBasicRemote().sendBinary(ByteBuffer.wrap(image));
			} catch (Exception e) {
				e.printStackTrace();
			}
    	} else if(split[0].equals("C")){
    		Context ctx = Context.getInstance();
    		if(split[1].equals("LMD")){
    			try {
					ctx.sendMouseLBDownIns(split[2], new Integer(split[3]));
				} catch (Exception e) {
				} 
    		} else if(split[1].equals("LMU")){
    			try {
					ctx.sendMouseLBUpIns(split[2], new Integer(split[3]));
				} catch (Exception e) {
				} 
    		} else if(split[1].equals("RMD")){
    			try {
					ctx.sendMouseRBDownIns(split[2], new Integer(split[3]));
				} catch (Exception e) {
				} 
    		} else if(split[1].equals("RMU")){
    			try {
					ctx.sendMouseRBUpIns(split[2], new Integer(split[3]));
				} catch (Exception e) {
				} 
    		} else if(split[1].equals("MV")){
    			try {
					ctx.sendMouseMoveIns(new Integer(split[4]), new Integer(split[5]),split[2], new Integer(split[3]));
				} catch (Exception e) {
				}
    		} else if(split[1].equals("KD")){
    			try {
					ctx.sendKeyBoardDownIns(new Integer(split[4]),split[2], new Integer(split[3]));
				} catch (Exception e) {
				}
    		} else if(split[1].equals("KU")){
    			try {
    				ctx.sendKeyBoardUpIns(new Integer(split[4]),split[2], new Integer(split[3]));
				} catch (Exception e) {
				}
    		}
    	}
    }
     
    /**
     * 发生错误时调用
     * @param session
     * @param error
     */
    @OnError
    public void onError(Session session, Throwable error){
    	if(session.getId().equals(id.get())){
    		id.set(null);
    		Context.getInstance().stopTcpServer();
    	}
    }
}
