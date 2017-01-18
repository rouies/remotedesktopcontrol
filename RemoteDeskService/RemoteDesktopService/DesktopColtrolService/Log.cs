using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;

namespace DesktopControlService
{
    public class Log
    {
        public static void AppendLog(String path,String text)
        {
            FileStream fs = null;
            if (File.Exists(path))
            {
                fs = File.Create(path);
            }
            else
            {
                fs = new FileStream(path, FileMode.Append, FileAccess.Write);
            }
            byte[] contents = Encoding.Default.GetBytes(text + "\r\n");
            fs.Write(contents, 0, contents.Length);
            fs.Flush();
            fs.Close();
        }
    }
}
