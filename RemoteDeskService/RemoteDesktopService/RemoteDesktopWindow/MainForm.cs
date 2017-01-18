using RemoteDesktopWindow.Control;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Net;
using System.Net.Sockets;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading;
using System.Windows.Forms;

namespace RemoteDesktopWindow
{
    public partial class MainForm : Form
    {
        private readonly static int bufferSize = 1024 * 1024 * 5;

        private IntPtr buffer;

        private static byte[] imageBuffer = new byte[bufferSize];

        [StructLayout(LayoutKind.Sequential)]
        private struct BitMapInfo
        {
            public IntPtr source;
            public uint size;
            public uint src_size;
        };
        [DllImport(@"DesktopUtils.dll", CharSet = CharSet.Auto, CallingConvention = CallingConvention.Cdecl)]
        private static extern int GetBitImageByFullScreen(ref BitMapInfo bitMap);

        [DllImport(@"DesktopUtils.dll", CharSet = CharSet.Auto, CallingConvention = CallingConvention.Cdecl)]
        private static extern int DecompressImage(ref BitMapInfo bitMap);

        [DllImport(@"DesktopUtils.dll", CharSet = CharSet.Auto, CallingConvention = CallingConvention.Cdecl)]
        private static extern void GetScreenSize(ref int width, ref int height);

        [DllImport("DesktopUtils.dll", CharSet = CharSet.Auto, CallingConvention = CallingConvention.Cdecl)]
        private static extern int LogoutWindows(bool force);

        private TcpClient client = null;

        private IPEndPoint targetPoint;

        private UdpClient ctlServer = null;

        private Thread thread = null;

        private String address;

        private String op;

        private int port;

        public MainForm(String op,String address,int port)
        {
            this.targetPoint = new IPEndPoint(IPAddress.Parse("127.0.0.1"), 10086);
            this.address = address;
            this.port = port;
            this.op = op;
            this.buffer = Marshal.AllocHGlobal(bufferSize);
            InitializeComponent();
        }

        ~MainForm()
        {
            if (this.buffer != IntPtr.Zero)
            {
                Marshal.FreeHGlobal(this.buffer);
            }
        }


        

        private void MainForm_Load(object sender, EventArgs e)
        {
            if (op == "0")
            {
                //注销系统
                LogoutWindows(true);
                return;
            }
            this.client = new TcpClient();
            try
            {
                this.client.Connect(address, port);
            }
            catch (Exception)
            {
                this.Close();
            }
            //先获取分辨率
            int width = 0;
            int height = 0;
            GetScreenSize(ref width, ref height);
            var stream = this.client.GetStream();
            byte[] widthBuf = BitConverter.GetBytes(width);
            byte[] heightBuf = BitConverter.GetBytes(height);

            try
            {
                stream.Write(widthBuf, 0, widthBuf.Length);
                stream.Write(heightBuf, 0, heightBuf.Length);
            }
            catch (Exception)
            {
                this.client.Close();
                this.Close();
            }
            int ch = -1;
            try
            {
                ch = stream.ReadByte();
            }
            catch (Exception)
            {
                this.client.Close();
                this.Close();
            }
            if (ch == 0xF)
            {
                this.thread = new Thread(SendImageDelegate);
                this.thread.Start(this.client);
            }
            else
            {
                this.client.Close();
                this.Close();
            }
            StartUDPListener();
        }

        public void SendImageDelegate(object c)
        {
            TcpClient clt = c as TcpClient;
            if (clt != null)
            {
                var pipe = clt.GetStream();
                while (true)
                {
                    BitMapInfo info = new BitMapInfo();
                    info.size = (uint)bufferSize;
                    info.source = buffer;
                    if (GetBitImageByFullScreen(ref info) == 0)
                    {
                        if (info.size < bufferSize)
                        {
                            try
                            {
                                Marshal.Copy(buffer, imageBuffer, 0, (int)info.size);
                                long size = info.size;
                                long srcsize = info.src_size;
                                byte[] ot = BitConverter.GetBytes(size);
                                pipe.Write(ot, 0, ot.Length);
                                ot = BitConverter.GetBytes(srcsize);
                                pipe.Write(ot, 0, ot.Length);
                                pipe.Write(imageBuffer, 0, (int)info.size);
                                pipe.Flush();
                                int ch = pipe.ReadByte();
                                if (ch != 0xF)
                                {
                                    break;
                                }
                            }
                            catch (Exception)
                            {
                                break;
                            }
                        }
                    }
                    else
                    {
                        break;
                    }
                }
                this.client.Close();
                this.client = null;
                this.thread = null;
                Application.Exit();
                

            }
        }

        private void StartUDPListener()
        {
            this.ctlServer = new UdpClient(this.targetPoint);
            this.ctlServer.BeginReceive(this.ReceiveCommand, this.ctlServer);
        }

        private void ReceiveCommand(IAsyncResult ar)
        {
            IPEndPoint tp = new IPEndPoint(IPAddress.Parse("127.0.0.1"), 0);
            try
            {
                byte[] receiveBytes = this.ctlServer.EndReceive(ar, ref tp);
                MouseAndKeyboardEvent.Execute(receiveBytes);
                
            }
            catch (Exception)
            {
                
            }
            finally
            {
                this.ctlServer.BeginReceive(this.ReceiveCommand, this.ctlServer);
            }
        }

        private void MainForm_FormClosed(object sender, FormClosedEventArgs e)
        {
            if (this.buffer != IntPtr.Zero)
            {
                Marshal.FreeHGlobal(this.buffer);
                this.buffer = IntPtr.Zero;
            }
        }


        private void MainForm_Shown(object sender, EventArgs e)
        {
            this.Visible = false;
        }


    }
}
