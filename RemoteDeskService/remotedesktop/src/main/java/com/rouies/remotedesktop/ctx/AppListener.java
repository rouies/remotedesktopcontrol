package com.rouies.remotedesktop.ctx;

import javax.servlet.ServletContextEvent;
import javax.servlet.ServletContextListener;

import redis.clients.jedis.JedisPoolConfig;

import com.cosmos.utils.network.NetworkException;
import com.rouies.utils.redis.RedisContext;

/**
 * Application Lifecycle Listener implementation class AppListener
 *
 */
public class AppListener implements ServletContextListener {

	public AppListener() {
	}

	/**
	 * @see ServletContextListener#contextDestroyed(ServletContextEvent)
	 */
	public void contextDestroyed(ServletContextEvent arg0) {
		Context.getInstance().stopTcpServer();
		Context.getInstance().stopUdpServer();
	}

	/**
	 * @see ServletContextListener#contextInitialized(ServletContextEvent)
	 */
	public void contextInitialized(ServletContextEvent sce) {
		Context.serverIp   = sce.getServletContext().getInitParameter("ListenAddress");
		Context.ctlPort    = new Integer(sce.getServletContext().getInitParameter("CtlPort"));
		Context.imgPort    = new Integer(sce.getServletContext().getInitParameter("ImagePort"));
		Context.reportPort = new Integer(sce.getServletContext().getInitParameter("ReportPort"));
		Context.reportTimeout = new Integer(sce.getServletContext().getInitParameter("report-timeout"));
		Context.userTimeout = new Integer(sce.getServletContext().getInitParameter("user-timeout"));
		String redisIp = sce.getServletContext().getInitParameter("redis-address");
		int redisPort = new Integer(sce.getServletContext().getInitParameter("redis-port"));
		String redisPasswd = sce.getServletContext().getInitParameter("redis-passwd");
		JedisPoolConfig config = new JedisPoolConfig();
		config.setMaxTotal(5);
		config.setMaxIdle(3);
		config.setMinIdle(1);
		//在borrow一个jedis实例时，是否提前进行validate操作；如果为true，则得到的jedis实例均是可用的；
		config.setTestOnBorrow(true);
		RedisContext.init("JMC",config, redisIp, redisPort, redisPasswd,10);
		try {
			Context.getInstance().startUdpServer();
		} catch (NetworkException e) {
			e.printStackTrace();
		}
	}

}
