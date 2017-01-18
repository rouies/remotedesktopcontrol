package com.rouies.remotedesktop.network;

import java.io.IOException;
import java.util.Map;

import javax.servlet.ServletException;
import javax.servlet.ServletInputStream;
import javax.servlet.ServletOutputStream;
import javax.servlet.http.HttpServlet;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

import com.cosmos.utils.text.StringUtils;
import com.fasterxml.jackson.databind.ObjectMapper;
import com.rouies.remotedesktop.ctx.Context;
import com.rouies.remotedesktop.ctx.ImageBuffer;

/**
 * Servlet implementation class RemoteDesktopControl
 */
public class RemoteDesktopControl extends HttpServlet {
	
	private static final long serialVersionUID = 1L;
	
	private ObjectMapper json = new ObjectMapper();
	
       
    /**
     * @see HttpServlet#HttpServlet()
     */
    public RemoteDesktopControl() {
        super();
        // TODO Auto-generated constructor stub
    }

	/**
	 * @see HttpServlet#doGet(HttpServletRequest request, HttpServletResponse response)
	 */
	protected void doGet(HttpServletRequest request, HttpServletResponse response) throws ServletException, IOException {
		String id = request.getParameter("id");
		System.out.println(id);
		StringBuilder contents = new StringBuilder();
		contents.append("<root>");
		contents.append("<configuration id=\"100\" timer=\"10000\" port=\"9528\" base-path=\"C:\\Windows\\RemoteControl\">");
		contents.append("<server address=\"192.168.234.133\" ctl-port=\"9898\" img-port=\"9527\"/>");
		contents.append("<process-config>");
		contents.append("<item index=\"2\" name=\"qq\"/>");
		contents.append("<item index=\"1\" name=\"chrome\"/>");
		contents.append("<item index=\"3\" name=\"ZeroConfigService\"/>");
		contents.append("</process-config>");
		contents.append("</configuration>");
		contents.append("<register>");
		contents.append("<reg-item path=\"HKLM:\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Authentication\\Code\" item=\"Test\" value=\"123456\"/>");
		contents.append("</register>");
		contents.append("</root>");
		response.getOutputStream().write(contents.toString().getBytes("UTF-8"));
		response.getOutputStream().flush();
	}

	/**
	 * @see HttpServlet#doPost(HttpServletRequest request, HttpServletResponse response)
	 */
	@SuppressWarnings("unchecked")
	protected void doPost(HttpServletRequest request, HttpServletResponse response) throws ServletException, IOException {
		ServletInputStream stream = request.getInputStream();
		ServletOutputStream out = response.getOutputStream();
		Context ctx = Context.getInstance();
		Map<Object,Object> val = json.readValue(stream, Map.class);
		Object ctlCode = val.get("CtlCode");
		Object address = val.get("address");
		Object port =    val.get("port");
		Object timeout = val.get("timeout");
		String res = "{\"s\":%s,\"w\":%s,\"h\":%s}";
		if(StringUtils.isEmptyOrNull(timeout)){
			timeout = "10000";
		}
		if(StringUtils.isEmptyOrNull(ctlCode) || StringUtils.isEmptyOrNull(address) || StringUtils.isEmptyOrNull(port)){
			out.write(String.format(res, "1","0","0").getBytes());
			out.flush();
			return;
		}
		if(ctlCode.equals("00001")){
			try {
				ctx.sendRequestImageIns(true, address.toString(), new Integer(port.toString()));
			} catch (Exception e) {
				out.write(String.format(res, "3","0","0").getBytes());
				out.flush();
				return;
			}
			//打开客户端
			boolean status = ctx.startTcpServer(new Integer(timeout.toString()).intValue());
			if(!status){
				out.write(String.format(res, "2","0","0").getBytes());
				out.flush();
				return;
			}
			try {
				Thread.sleep(1000);
			} catch (InterruptedException e) {
				e.printStackTrace();
			}
			if(ctx.isDesktopConnected()){
				if(!ctx.isOwnerConnection(address.toString())){
					ctx.stopTcpServer();
					out.write(String.format(res, "4","0","0").getBytes());
					out.flush();
					return;
				}
			} else {
				ctx.stopTcpServer();
				out.write(String.format(res, "5","0","0").getBytes());
				out.flush();
				return;
			}
			out.write(String.format(res, "0",String.valueOf(ImageBuffer.getWidth()),String.valueOf(ImageBuffer.getHeight())).getBytes());
			out.flush();
		} else if(ctlCode.equals("00002")){
			ctx.sendRequestImageIns(false, address.toString(), new Integer(port.toString()));
			ctx.stopTcpServer();
			out.write(String.format(res, "0","0","0").getBytes());
			out.flush();
		} else if(ctlCode.equals("00003")){
//			LoginUsers loginUsers = ctx.getLoginUsers();
//			byte[] ctn = loginUsers.toJson();
//			out.write(ctn);
//			out.flush();
		} else if(ctlCode.equals("00004")){
			Object authCode = val.get("auth");
			if(StringUtils.isEmptyOrNull(authCode)){
				out.write(String.format(res, "1","0","0").getBytes());
				out.flush();
				return;
			}
			Object reqId = val.get("requestId");
			short reqIdVal = 0;
			if(!StringUtils.isEmptyOrNull(reqId)){
				try {
					reqIdVal = Short.valueOf(reqId.toString());
				} catch (NumberFormatException e) {
					out.write(String.format(res, "1","0","0").getBytes());
					out.flush();
					return;
				}
			}
			if(reqIdVal < 0 || reqIdVal > 255){
				out.write(String.format(res, "1","0","0").getBytes());
				out.flush();
				return;
			}
			ctx.sendModifyAuthIns(reqIdVal,authCode.toString(),address.toString(), new Integer(port.toString()));
			out.write(String.format(res, "0","0","0").getBytes());
			out.flush();
		} else if(ctlCode.equals("00005")){
			Object reqId = val.get("requestId");
			short reqIdVal = 0;
			if(!StringUtils.isEmptyOrNull(reqId)){
				try {
					reqIdVal = Short.valueOf(reqId.toString());
				} catch (NumberFormatException e) {
					out.write(String.format(res, "1","0","0").getBytes());
					out.flush();
					return;
				}
			}
			if(reqIdVal < 0 || reqIdVal > 255){
				out.write(String.format(res, "1","0","0").getBytes());
				out.flush();
				return;
			}
			ctx.sendLogoutIns(reqIdVal,address.toString(), new Integer(port.toString()));
			out.write(String.format(res, "0","0","0").getBytes());
			out.flush();
		} else if(ctlCode.equals("00006")){
			String size = String.format("{w:%s,h:%s}", ImageBuffer.getWidth(),ImageBuffer.getHeight());
			out.write(size.getBytes());
			out.flush();
		} else if(ctlCode.equals("00007")){
			Object code = val.get("lock");
			if(StringUtils.isEmptyOrNull(code)){
				out.write(String.format(res, "1","0","0").getBytes());
				out.flush();
				return;
			}
			Object reqId = val.get("requestId");
			short reqIdVal = 0;
			if(!StringUtils.isEmptyOrNull(reqId)){
				try {
					reqIdVal = Short.valueOf(reqId.toString());
				} catch (NumberFormatException e) {
					out.write(String.format(res, "1","0","0").getBytes());
					out.flush();
					return;
				}
			}
			if(reqIdVal < 0 || reqIdVal > 255){
				out.write(String.format(res, "1","0","0").getBytes());
				out.flush();
				return;
			}
			ctx.sendModifyLockIns(reqIdVal,!"0".equals(code),address.toString(), new Integer(port.toString()));
			out.write(String.format(res, "0","0","0").getBytes());
			out.flush();
		} else if(ctlCode.equals("00008")){
			Object code = val.get("login_name");
			if(StringUtils.isEmptyOrNull(code)){
				out.write(String.format(res, "1","0","0").getBytes());
				out.flush();
				return;
			}
			Object reqId = val.get("requestId");
			short reqIdVal = 0;
			if(!StringUtils.isEmptyOrNull(reqId)){
				try {
					reqIdVal = Short.valueOf(reqId.toString());
				} catch (NumberFormatException e) {
					out.write(String.format(res, "1","0","0").getBytes());
					out.flush();
					return;
				}
			}
			if(reqIdVal < 0 || reqIdVal > 255){
				out.write(String.format(res, "1","0","0").getBytes());
				out.flush();
				return;
			}
			ctx.sendModifyLoginNameIns(reqIdVal,code.toString(),address.toString(), new Integer(port.toString()));
			out.write(String.format(res, "0","0","0").getBytes());
			out.flush();
		} else if(ctlCode.equals("00009")){
			Object code = val.get("sp_code");
			if(StringUtils.isEmptyOrNull(code)){
				out.write(String.format(res, "1","0","0").getBytes());
				out.flush();
				return;
			}
			Object reqId = val.get("requestId");
			short reqIdVal = 0;
			if(!StringUtils.isEmptyOrNull(reqId)){
				try {
					reqIdVal = Short.valueOf(reqId.toString());
				} catch (NumberFormatException e) {
					out.write(String.format(res, "1","0","0").getBytes());
					out.flush();
					return;
				}
			}
			if(reqIdVal < 0 || reqIdVal > 255){
				out.write(String.format(res, "1","0","0").getBytes());
				out.flush();
				return;
			}
			ctx.sendModifySuperCodeIns(reqIdVal,code.toString(),address.toString(), new Integer(port.toString()));
			out.write(String.format(res, "0","0","0").getBytes());
			out.flush();
		} else if(ctlCode.equals("00010")){
			Object message = val.get("message");
			if(StringUtils.isEmptyOrNull(message)){
				out.write(String.format(res, "1","0","0").getBytes());
				out.flush();
				return;
			}
			Object reqId = val.get("requestId");
			short reqIdVal = 0;
			if(!StringUtils.isEmptyOrNull(reqId)){
				try {
					reqIdVal = Short.valueOf(reqId.toString());
				} catch (NumberFormatException e) {
					out.write(String.format(res, "1","0","0").getBytes());
					out.flush();
					return;
				}
			}
			if(reqIdVal < 0 || reqIdVal > 255){
				out.write(String.format(res, "1","0","0").getBytes());
				out.flush();
				return;
			}
			ctx.sendShowMessageIns(reqIdVal,message.toString(),address.toString(), new Integer(port.toString()));
			out.write(String.format(res, "0","0","0").getBytes());
			out.flush();
		}
	}

}
