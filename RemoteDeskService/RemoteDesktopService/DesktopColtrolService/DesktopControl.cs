using System;
using System.Collections.Generic;
using System.Linq;
using System.Net;
using System.Net.Sockets;
using System.Text;
using System.Threading;

namespace DesktopControlService
{
    public class DesktopControl : IDisposable
    {
        private ControlItemConfiguration config;

        private Socket server;

        //private Socket client;

        private Thread thread;

        private IPEndPoint targetPoint;

        private byte[] udpBuffer = new byte[512];

        private Timer timer;
        public DesktopControl(ControlItemConfiguration config)
        {
            this.config = config;
            this.thread = new Thread(CommandControlListen);
            this.targetPoint = new IPEndPoint(IPAddress.Parse(this.config.TargetAddress), this.config.TargetControlPort);
            this.server = new Socket(AddressFamily.InterNetwork, SocketType.Dgram, ProtocolType.Udp);
            this.server.Bind(new IPEndPoint(IPAddress.Any, this.config.LocalControlPort));
           
        }

        ~DesktopControl()
        {
            this.server.Close();
        }

        /// <summary>
        /// 向服务器发送心跳包并在NAT上打洞
        /// </summary>
        public void Start()
        {
            this.thread.Start(this.server);
            this.timer = new Timer(SendTestPacket, this.server, this.config.Timer, this.config.Timer);
            MapData.StartGetMapLocation(3 * 60 * 1000);
        }

        private void SendTestPacket(Object obj)
        {
            Socket clt = obj as Socket;
            if (clt != null)
            {
                try
                {
                    var testPacket = this.config.GetPacketContents();
                    clt.SendTo(testPacket, this.targetPoint);
                }
                catch (Exception)
                {

                }
            }
        }


        private void CommandControlListen(Object obj)
        {
            Socket svr = obj as Socket;
           
            EndPoint tp = new IPEndPoint(IPAddress.Parse(this.config.TargetAddress), 0);
            if (svr != null)
            {
                while (true)
                {
                    try
                    {
                        int len = svr.ReceiveFrom(udpBuffer, ref tp);
                        var iinfo = InstructionsInfo.Parse(udpBuffer);
                        InstructionsCode.ExecuteInstructionsCode(iinfo, this.config);
                    }
                    catch (Exception)
                    {
                        //Console.WriteLine(ex.Message);
                    }
                }
            }
        }




        public void Dispose()
        {
            try
            {
                this.thread.Abort();
                this.timer.Dispose();
            }
            catch (Exception)
            {
                throw;
            }
            this.server.Close();
        }
    }
}
