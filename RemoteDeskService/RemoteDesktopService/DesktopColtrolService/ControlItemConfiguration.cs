using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;

namespace DesktopControlService
{
    public class ControlItemConfiguration
    {

        [DllImport("DesktopUtils.dll", CharSet = CharSet.Auto, CallingConvention = CallingConvention.Cdecl)]
        private static extern int GetCpuUsageRate(ref int rate);

        [DllImport("DesktopUtils.dll", CharSet = CharSet.Auto, CallingConvention = CallingConvention.Cdecl)]
        private static extern int GetGlobalMemoryUsageRate(ref int rate);
       
        private byte[] id;

        public byte[] Id
        {
            get { return id; }
            set { id = value; }
        }

        /// <summary>
        /// 目标地址
        /// </summary>
        private String targetAddress;

        public String TargetAddress
        {
            get { return targetAddress; }
            set { targetAddress = value; }
        }

        /// <summary>
        /// 服务端用来接收图片的TCP端口
        /// </summary>
        private int receiveImageDataPort;

        public int ReceiveImageDataPort
        {
            get { return receiveImageDataPort; }
            set { receiveImageDataPort = value; }
        }

        /// <summary>
        /// 本地UDP监听指令端口
        /// </summary>
        private int localControlPort;

        public int LocalControlPort
        {
            get { return localControlPort; }
            set { localControlPort = value; }
        }

        /// <summary>
        /// 服务端接收心跳包的端口
        /// </summary>
        private int targetControlPort;

        public int TargetControlPort
        {
            get { return targetControlPort; }
            set { targetControlPort = value; }
        }

        private int timer;
        public int Timer
        {
            get { return timer; }
            set { timer = value; }
        }

        private String[] processNames;

        public String[] ProcessNames
        {
            get { return processNames; }
            set { processNames = value; }
        }

        private String basePath;

        public String BasePath
        {
            get { return basePath; }
            set { basePath = value; }
        }

        private ReportMessage reportClient;

        public ReportMessage ReportClient
        {
            get { return reportClient; }
            set { reportClient = value; }
        }

        private Int32 GetProcessStatus()
        {
            Int32 result = 0;
            if (this.processNames != null)
            {
                Int32 index = 1;
                for (int i = 0, len = this.processNames.Length > 30 ? 30 : this.processNames.Length; i < len; i++)
                {
                    var item = Process.GetProcessesByName(this.processNames[i]);
                    if (item!=null && item.Length != 0)
                    {
                        Console.WriteLine("N - " + this.processNames[i]);
                        result |= index;
                    }
                    else
                    {
                        Console.WriteLine("F - " + this.processNames[i]);
                    }
                    index <<= 1;
                }
            }
            return result;
        }

        public byte[] GetPacketContents()
        {
            int cpu = 0;
            int mem = 0;
            GetCpuUsageRate(ref cpu);
            GetGlobalMemoryUsageRate(ref mem);
            var status = GetProcessStatus();
            var pstatus = BitConverter.GetBytes(status);
            //地图数据
            long x;
            long y;
            MapData.GetLocation(out x, out y);
            byte[] bx = BitConverter.GetBytes(x);
            byte[] by = BitConverter.GetBytes(y);

            MemoryStream res = new MemoryStream(20);
            res.Write(this.id, 0, this.id.Length);
            res.WriteByte((byte)cpu);
            res.WriteByte((byte)mem);
            res.Write(pstatus, 0, pstatus.Length);
            //写入地图数据
            res.Write(bx, 0, bx.Length);
            res.Write(by, 0, by.Length);

            return res.ToArray();
           
            
        }
    }
}
