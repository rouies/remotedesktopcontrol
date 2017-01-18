var RemoteDesktop;

if (!RemoteDesktop) {
	RemoteDesktop = new Object();
}

RemoteDesktop.View = function(/* canvas */canvas,/*String*/ip,/*String*/ports) {
	var img = new Image();
	var websocket = null;
	this.enableEvent = false;
	var cvs = canvas;
	var ipAddress = ip;
	var port = ports;
	var self = this;

	var getOffsetTop = function(obj) {
		var tmp = obj.offsetTop;
		var val = obj.offsetParent;
		while (val != null) {
			tmp += val.offsetTop;
			val = val.offsetParent;
		}
		return tmp;
	};
	var getOffsetLeft = function(obj) {
		var tmp = obj.offsetLeft;
		var val = obj.offsetParent;
		while (val != null) {
			tmp += val.offsetLeft;
			val = val.offsetParent;
		}
		return tmp;
	};
	var offsetTop =  getOffsetTop(canvas);
	var offsetLeft = getOffsetLeft(canvas);
	/*--------------------------Event--------------------------*/
	var onLBMouseDown = function() {
		if(self.enableEvent){
			websocket.send("C:LMD:" + ipAddress + ":" + port);
		}
	};
	var onLBMouseUp = function() {
		if(self.enableEvent){
			websocket.send("C:LMU:" + ipAddress + ":" + port);
		}
	};
	var onRBMouseDown = function() {
		if(self.enableEvent){
			websocket.send("C:RMD:" + ipAddress + ":" + port);
		}
	};
	var onRBMouseUp = function() {
		if(self.enableEvent){
			websocket.send("C:RMU:" + ipAddress + ":" + port);
		}
	};
	var onMouseMove = function(/* int */posX,/* int */posY) {
		if(self.enableEvent){
			websocket.send("C:MV:" + ipAddress + ":" + port + ":" + posX + ":" + posY);
		}
	};
	var onKeyDown = function(ch) {
		if(self.enableEvent){
			websocket.send("C:KD:" + ipAddress + ":" + port + ":" + ch);
		}
	};
	var onKeyUp = function(ch) {
		if(self.enableEvent){
			websocket.send("C:KU:" + ipAddress + ":" + port + ":" + ch);
		}
	};

	var context = canvas.getContext('2d');

	var mouse_down = function(/* Event */eventArgs) {
		if (eventArgs.button == 0) {
			onLBMouseDown();
			cvs.focus(); 
		} else if (eventArgs.button == 2) {
			onRBMouseDown();
		}
	};

	var mouse_up = function(/* Event */eventArgs) {
		if (eventArgs.button == 0) {
			onLBMouseUp();
			can
		} else if (eventArgs.button == 2) {
			onRBMouseUp();
		}
	};
	
	var key_down = function(/* Event */eventArgs){
		var code = eventArgs.keyCode ? eventArgs.keyCode :eventArgs.which;
		onKeyDown(code);
	};
	
	var key_up = function(/* Event */eventArgs){
		var code = eventArgs.keyCode ? eventArgs.keyCode :eventArgs.which;
		onKeyUp(code);
	};

	var mouse_move = function(/*Event*/eventArgs) {
		var mouseX = eventArgs.clientX + document.body.scrollLeft;
		var mouseY = eventArgs.clientY + document.body.scrollTop;
		var objX = mouseX-offsetLeft;
		var objY = mouseY-offsetTop;
		onMouseMove(objX, objY);
	};

	$(canvas).bind('mousedown', mouse_down);
	$(canvas).bind('mouseup', mouse_up);
	$(canvas).bind('mousemove', mouse_move);
	$(canvas).bind('keydown', key_down);
	$(canvas).bind('keyup', key_up);
	$(canvas).bind('contextmenu', function() {
		return false;
	});

	this.ws_error;

	this.ws_open = function(){
		alert("打开");
	};

	this.ws_close = function(){
		alert("断开");
	};

	var ws_message = function(event) {
		var reader = new FileReader();
		reader.onload = function(eve) {
			if (eve.target.readyState == FileReader.DONE) {
				img.src = this.result;
				context.drawImage(img, 0, 0, cvs.width, cvs.height);
				websocket.send("R:" + ipAddress);
			}
		};
		reader.readAsDataURL(event.data);
	};

	this.init = function(/* String */url) {
		//alert(url);
		if ('WebSocket' in window) {
			websocket = new WebSocket(url);
			
			websocket.onerror = this.ws_error;
			websocket.onopen = this.ws_open;
			websocket.onclose = this.ws_close;
			websocket.onmessage = ws_message;
			return true;
		} else {
			return false;
		}
	};

	this.close = function() {
		if (websocket) {
			websocket.close();
		}
	};
	
	this.requestRemoteDesktopService = function(){
		var parameter = {CtlCode:"00001",address:ipAddress,port:port};
        var args = JSON.stringify(parameter);
       	$.ajax({
       		async:false,
            url:'/remotedesktop/RemoteDesktopControl',
            data:args,
            type:'post',
            dataType:'text',
            success:function(res){
            	var resObj = JSON.parse(res);
  	         	if(resObj.s == 0){
  	         		cvs.width = resObj.w;
  	         		cvs.height = resObj.h;
  	         		alert("远程链接成功");
  	         		ipAddress = ip;
  	         		websocket.send("R:" + ip);
  	         		//view.enableEvent = true;
  	         	} else {
  	         		alert("远程主机没有响应");
  	         	}

            },
            error:function(code){
            	alert("服务无法链接，请稍后再试!");
            }
        });
	};
	
	this.getScreenSize = function(){
		var parameter = {CtlCode:"00006"};
        var args = JSON.stringify(parameter);
        var result = null;
       	$.ajax({
       		async:false,
            url:'/remotedesktop/RemoteDesktopControl',
            data:args,
            type:'post',
            dataType:'text',
            success:function(res){
            	alert(res);
            	result = JSON.parse(res);
            },
            error:function(code){
            	alert("服务无法链接，请稍后再试!");
            }
        });
       	return result;
	};
	
	
	this.showMessage = function(msg){
		var parameter = {requestId:"156",id:"100",CtlCode:"00010",address:"192.168.234.100",port:"9528",message:msg};
        var args = JSON.stringify(parameter);
        var result = null;
       	$.ajax({
       		async:false,
            url:'/remotedesktop/SyncRemoteDesktopControl',
            data:args,
            type:'post',
            dataType:'text',
            success:function(res){
            	result = JSON.parse(res);
            	alert(res);
            },
            error:function(code){
            	alert("服务无法链接，请稍后再试!");
            }
        });
       	return result;
	};

}
