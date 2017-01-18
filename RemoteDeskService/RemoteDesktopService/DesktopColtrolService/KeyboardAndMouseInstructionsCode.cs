using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Net;
using System.Net.Sockets;
using System.Runtime.InteropServices;
using System.Text;

namespace DesktopControlService
{
    class KeyboardAndMouseInstructionsCode : InstructionsCode
    {
        public enum LevelCode
        {
            MOUSE_MOVE = 0,
            LB_MOUSE_DOWN = 1,
            LB_MOUSE_UP = 2,
            RB_MOUSE_DOWN = 3,
            RB_MOUSE_UP = 4,
            KEYBOARD_DOWN = 5,
            KEYBOARD_UP = 6

        }

        private UdpClient client = new UdpClient();
        
        public KeyboardAndMouseInstructionsCode(){
            this.client.Connect(new IPEndPoint(IPAddress.Parse("127.0.0.1"), 10086));
        }

        private void SendCommand(byte[] data)
        {
            client.Send(data, data.Length);
        }

        public override byte[] Execute(InstructionsInfo insCode, ControlItemConfiguration config)
        {
            int levelCode = insCode.LevelTwoCode;
            byte[] data = insCode.Data;
            MemoryStream command = new MemoryStream(10);
            command.WriteByte((byte)levelCode);
            command.Write(data, 0, data.Length);
            this.SendCommand(command.ToArray());
            return null;
        }
    }
}
