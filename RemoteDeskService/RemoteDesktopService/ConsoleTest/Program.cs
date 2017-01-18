using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using DesktopControlService;
using System.Runtime.InteropServices;
using Microsoft.Win32;
using System.Xml;
using System.Net;

namespace ConsoleTest
{
    class Program
    {
        private class ProcessItem
        {
            public int index;
            public String process;
            public ProcessItem(int index, String process)
            {
                this.index = index;
                this.process = process;
            }
        }

        [DllImport(@"D:\DotNetProject\RemoteDesktopService\Debug\DesktopUtils.dll", CharSet = CharSet.Auto, CallingConvention = CallingConvention.Cdecl)]
        private static extern int ShowMessage([MarshalAs(UnmanagedType.LPWStr)]String title, [MarshalAs(UnmanagedType.LPWStr)]String message);

        static void Main(string[] args)
        {
            MapData.StartGetMapLocation(3000);
            Console.Read();
            //ShowMessage("标题", "内容");
            //String path = null;
            //if(args.Length == 0){
            //    path = Environment.CurrentDirectory + "/config.xml";
            //} else {
            //    path = args[0];
            //}
            //ControlItemConfiguration config = new ControlItemConfiguration();
            //XmlDocument doc = new XmlDocument();
            //doc.Load(path);
            //var root = doc.DocumentElement;
            //short sid = short.Parse(root.GetAttribute("id"));
            //config.Id = BitConverter.GetBytes(sid);
            //config.LocalControlPort = int.Parse(root.GetAttribute("port")); ;
            //config.Timer = int.Parse(root.GetAttribute("timer"));
            ////服务器
            //var server = (XmlElement)doc.SelectSingleNode("/configuration/server");
            //config.TargetAddress = server.GetAttribute("address");
            //config.TargetControlPort = int.Parse(server.GetAttribute("ctl-port")); ;
            //config.ReceiveImageDataPort = int.Parse(server.GetAttribute("img-port"));
            ////进程

            //var procList = doc.SelectNodes("/configuration/process-config/item");
            //List<ProcessItem> ls = new List<ProcessItem>(procList.Count);
            //String[] ps = new String[procList.Count];
         
            //foreach (var item in procList)
            //{
            //    XmlElement ele = item as XmlElement;
            //    int idx = int.Parse(ele.GetAttribute("index"));
            //    String name = ele.GetAttribute("name");
            //    ls.Add(new ProcessItem(idx, name));
            //}

            //ls.Sort((a, b) =>
            //{
            //    if (a.index > b.index)
            //    {
            //        return 1;
            //    }
            //    else if (a.index < b.index)
            //    {
            //        return -1;
            //    }
            //    else
            //    {
            //        return 0;
            //    }
            //});
            //for (int i = 0, len = ls.Count; i < len; i++)
            //{
            //    ps[i] = ls[i].process;
            //    Console.WriteLine("监控进程:" + ps[i]);
            //}
            //config.ProcessNames = ps;
            //DesktopControl ctl = new DesktopControl(config);
            //ctl.Start();
            //Console.WriteLine("启动成功");
        }
    }
}
