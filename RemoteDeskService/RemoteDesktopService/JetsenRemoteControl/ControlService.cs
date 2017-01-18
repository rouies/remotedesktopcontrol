using DesktopControlService;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.ServiceProcess;
using System.Text;
using System.Windows.Forms;
using System.Xml;

namespace JetsenRemoteControl
{
    partial class ControlService : ServiceBase
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

        public ControlService()
        {
            InitializeComponent();
            
        }

        private DesktopControl ctl;

        private ControlItemConfiguration GetConfig(String path)
        {
            ControlItemConfiguration config = new ControlItemConfiguration();
            XmlDocument doc = new XmlDocument();
            doc.Load(path);
            var root = doc.DocumentElement;
            short sid = short.Parse(root.GetAttribute("id"));
            config.Id = BitConverter.GetBytes(sid);
            config.LocalControlPort = int.Parse(root.GetAttribute("port")); ;
            config.Timer = int.Parse(root.GetAttribute("timer"));
            config.BasePath = root.GetAttribute("base-path");
            //服务器
            var server = (XmlElement)doc.SelectSingleNode("/configuration/server");
            config.TargetAddress = server.GetAttribute("address");
            config.TargetControlPort = int.Parse(server.GetAttribute("ctl-port")); ;
            config.ReceiveImageDataPort = int.Parse(server.GetAttribute("img-port"));
            var reportPort = int.Parse(server.GetAttribute("report-port"));
            config.ReportClient = new ReportMessage(config.TargetAddress, reportPort);
            //进程
            var procList = doc.SelectNodes("/configuration/process-config/item");
            List<ProcessItem> ls = new List<ProcessItem>(procList.Count);
            String[] ps = new String[procList.Count];

            foreach (var item in procList)
            {
                XmlElement ele = item as XmlElement;
                int idx = int.Parse(ele.GetAttribute("index"));
                String name = ele.GetAttribute("name");
                ls.Add(new ProcessItem(idx, name));
            }

            ls.Sort((a, b) =>
            {
                if (a.index > b.index)
                {
                    return 1;
                }
                else if (a.index < b.index)
                {
                    return -1;
                }
                else
                {
                    return 0;
                }
            });
            for (int i = 0, len = ls.Count; i < len; i++)
            {
                ps[i] = ls[i].process;
            }
            config.ProcessNames = ps;
            return config;
        }

        protected override void OnStart(string[] args)
        {
            String path =  "C:\\Windows\\RemoteControl\\config.xml";
            ControlItemConfiguration config = this.GetConfig(path);
            this.ctl = new DesktopControl(config);
            ctl.Start();
            
        }

        protected override void OnStop()
        {
            ctl.Dispose();
        }
    }
}
