using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;

namespace DesktopControlService
{
    public class InstructionsInfo
    {
       
        private int direct;

        public int Direct
        {
            get { return direct; }
            set { direct = value; }
        }

        private int levelOneCode;

        public int LevelOneCode
        {
            get { return levelOneCode; }
            set { levelOneCode = value; }
        }

        private int levelTwoCode;

        public int LevelTwoCode
        {
            get { return levelTwoCode; }
            set { levelTwoCode = value; }
        }

        private byte[] data;

        public byte[] Data
        {
            get { return data; }
            set { data = value; }
        }

        public static InstructionsInfo Parse(byte[] pipe)
        {
            InstructionsInfo result = new InstructionsInfo();
            result.direct = pipe[0];
            result.levelOneCode = pipe[1];
            result.levelTwoCode = pipe[2];
            int length = BitConverter.ToInt16(pipe, 3);
            result.data = null;
            if (length > 0)
            {
                result.data = new byte[length];
                Array.Copy(pipe, 5, result.data, 0, result.data.Length);
            }
            else
            {
                result.data = new byte[0];
            }
            return result;
        }

        public byte[] ToCode()
        {
            MemoryStream result = new MemoryStream();
            result.WriteByte((byte)this.direct);
            result.WriteByte((byte)this.levelOneCode);
            result.WriteByte((byte)this.levelTwoCode);
            if(this.data != null && this.data.Length > 0){
                short len = (short)this.data.Length;
                byte[] blen = BitConverter.GetBytes(len);
                result.Write(blen, 0, blen.Length);
                result.Write(this.data, 0, this.data.Length);
            }
            else
            {
                short len = 0;
                byte[] blen = BitConverter.GetBytes(len);
                result.Write(blen, 0, blen.Length);
            }
          
            return result.ToArray();
        }

    }
}
