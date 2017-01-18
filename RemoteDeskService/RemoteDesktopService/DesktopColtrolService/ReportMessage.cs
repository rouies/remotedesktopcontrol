using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Net.Sockets;
using System.Text;

namespace DesktopControlService
{
    public class ReportMessage
    {
        private UdpClient client = new UdpClient();

        public ReportMessage(String address,int port)
        {
            this.client.Connect(address, port);
        }

        public void SendPacket(byte requestId, byte[] id,params byte[] state)
        {
            MemoryStream mem = new MemoryStream(15);
            mem.WriteByte(requestId);
            mem.Write(id, 0, id.Length);
            if (state.Length > 0) 
            { 
                mem.Write(state,0,state.Length);
            }
            var data = mem.ToArray();
            this.client.Send(data, data.Length);
        }
    }
}
