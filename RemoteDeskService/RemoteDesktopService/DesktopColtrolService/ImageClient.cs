using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Net.Sockets;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading;

namespace DesktopControlService
{
    public class ImageClient
    {
        [DllImport(@"DesktopUtils.dll", CharSet = CharSet.Auto, CallingConvention = CallingConvention.Cdecl)]
        private static extern int CreateUserProcess([MarshalAs(UnmanagedType.LPWStr)]String app);

        private String basePath;

        public ImageClient(String basePath)
        {
            this.basePath = basePath;
        }

        [MethodImpl(MethodImplOptions.Synchronized)]
        public void StopSend()
        {
            var proList = Process.GetProcessesByName("RemoteDesktopWindow");
            foreach (var proc in proList)
            {
                try
                {
                    proc.Kill();
                }
                catch (Exception)
                {
                }
            }
        }

        [MethodImpl(MethodImplOptions.Synchronized)]
        public bool SendImageData(String address, int port)
        {
            bool result = false;
            var proList = Process.GetProcessesByName("RemoteDesktopWindow");
            if (proList.Length == 0)
            {
                var appPath = String.Format("{0}\\RemoteDesktopWindow.exe 99 {1} {2}",this.basePath, address, port);
                int state = CreateUserProcess(appPath);
                if (state == 0)
                {
                    result = true;
                }
            }
            return result;
        }
    }
}
