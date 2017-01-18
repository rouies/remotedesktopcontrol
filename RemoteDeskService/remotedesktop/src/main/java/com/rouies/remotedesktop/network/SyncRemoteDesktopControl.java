package com.rouies.remotedesktop.network;

import java.io.IOException;
import java.util.Map;
import java.util.concurrent.SynchronousQueue;
import java.util.concurrent.TimeUnit;

import javax.servlet.ServletException;
import javax.servlet.ServletInputStream;
import javax.servlet.ServletOutputStream;
import javax.servlet.http.HttpServlet;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

import com.cosmos.utils.text.StringUtils;
import com.fasterxml.jackson.databind.ObjectMapper;
import com.rouies.remotedesktop.ctx.Context;

/**
 * Servlet implementation class SyncRemoteDesktopControl
 */
public class SyncRemoteDesktopControl extends HttpServlet {
	private static final long serialVersionUID = 1L;
	
	private ObjectMapper json = new ObjectMapper();
    /**
     * @see HttpServlet#HttpServlet()
     */
    public SyncRemoteDesktopControl() {
        super();
        // TODO Auto-generated constructor stub
    }

	/**
	 * @see HttpServlet#doGet(HttpServletRequest request, HttpServletResponse response)
	 */
	protected void doGet(HttpServletRequest request, HttpServletResponse response) throws ServletException, IOException {
		
	}

	/**
	 * @see HttpServlet#doPost(HttpServletRequest request, HttpServletResponse response)
	 */
	protected void doPost(HttpServletRequest request, HttpServletResponse response) throws ServletException, IOException {
		ServletInputStream stream = request.getInputStream();
		ServletOutputStream out = response.getOutputStream();
		Context ctx = Context.getInstance();
		@SuppressWarnings("unchecked")
		Map<Object,Object> val = json.readValue(stream, Map.class);
		Object ctlCode = val.get("CtlCode");
		Object address = val.get("address");
		Object port =    val.get("port");
		Object id   =    val.get("id");
		Object timeout = val.get("timeout");
		Object reqId =   val.get("requestId");
		String res = "{\"s\":%s,\"w\":%s,\"h\":%s}";
		if(StringUtils.isEmptyOrNull(timeout)){
			timeout = "10000";
		}
		long timeoutVal = 10000;
		try {
			timeoutVal = Long.valueOf(timeout.toString());
		} catch (NumberFormatException e) {

		}

		short reqIdVal = 0;
		if(!StringUtils.isEmptyOrNull(reqId)){
			try {
				reqIdVal = Short.valueOf(reqId.toString());
			} catch (NumberFormatException e) {
				out.write("1".getBytes());
				out.flush();
				return;
			}
		}
		if(reqIdVal < 0 || reqIdVal > 255){
			out.write("1".getBytes());
			out.flush();
			return;
		}
		
		
		
		if(StringUtils.isEmptyOrNull(ctlCode) || StringUtils.isEmptyOrNull(address) 
				|| StringUtils.isEmptyOrNull(port) || StringUtils.isEmptyOrNull(id)){
			out.write(String.format(res, "1","0","0").getBytes());
			out.flush();
			return;
		}
		
		
		if(ctlCode.equals("00004")){
			Object authCode = val.get("auth");
			if(StringUtils.isEmptyOrNull(authCode)){
				out.write(String.format(res, "1","0","0").getBytes());
				out.flush();
				return;
			}
			ctx.sendModifyAuthIns(reqIdVal,authCode.toString(),address.toString(), new Integer(port.toString()));
		} else if(ctlCode.equals("00005")){
			ctx.sendLogoutIns(reqIdVal,address.toString(), new Integer(port.toString()));
		} else if(ctlCode.equals("00007")){
			Object code = val.get("lock");
			if(StringUtils.isEmptyOrNull(code)){
				out.write(String.format(res, "1","0","0").getBytes());
				out.flush();
				return;
			}
			ctx.sendModifyLockIns(reqIdVal,!"0".equals(code),address.toString(), new Integer(port.toString()));
		} else if(ctlCode.equals("00008")){
			Object code = val.get("login_name");
			if(StringUtils.isEmptyOrNull(code)){
				out.write(String.format(res, "1","0","0").getBytes());
				out.flush();
				return;
			}
			ctx.sendModifyLoginNameIns(reqIdVal,code.toString(),address.toString(), new Integer(port.toString()));;
		} else if(ctlCode.equals("00009")){
			Object code = val.get("sp_code");
			if(StringUtils.isEmptyOrNull(code)){
				out.write(String.format(res, "1","0","0").getBytes());
				out.flush();
				return;
			}
			ctx.sendModifySuperCodeIns(reqIdVal,code.toString(),address.toString(), new Integer(port.toString()));
		} else if(ctlCode.equals("00010")){
			Object message = val.get("message");
			if(StringUtils.isEmptyOrNull(message)){
				out.write(String.format(res, "1","0","0").getBytes());
				out.flush();
				return;
			}
			ctx.sendShowMessageIns(reqIdVal,message.toString(),address.toString(), new Integer(port.toString()));
		} else {
			out.write(String.format(res, "2","0","0").getBytes());
			out.flush();
			return;
		}
		String key = id + ":" + reqIdVal;
		SynchronousQueue<Boolean> sync = SyncReportObject.put(key);
		Boolean rp = null;
		try {
			rp = sync.poll(timeoutVal,TimeUnit.MILLISECONDS);
		} catch (InterruptedException e) {
			e.printStackTrace();
		} finally{
			SyncReportObject.remove(key);
		}
		if(rp != null){
			out.write(String.format(res, "0","0","0").getBytes());
			//System.out.println("成功");
		} else {
			out.write(String.format(res, "3","0","0").getBytes());
		}
		out.flush();
	}

}
