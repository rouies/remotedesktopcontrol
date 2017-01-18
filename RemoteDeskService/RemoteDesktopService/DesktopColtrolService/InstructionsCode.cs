using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;

namespace DesktopControlService
{
    /*
     * {[指令方向(8bit)][一级指令码(8bit)][二级指令码(8bit)][指令数据长度(16bite)][指令数据]}
     */
    public abstract class InstructionsCode
    {
        public enum OneLevelCode
        {
            MOUSE_AND_KEYBOARD = 0,
            IMAGE_SEND = 1,
            SYSTEM_OP = 2
        }

        //private static int SERVER_TO_CLIENT = 0;

        //private static int CLIENT_TO_SERVER = 1;

        private static KeyboardAndMouseInstructionsCode kic = new KeyboardAndMouseInstructionsCode();

        private static SendImageInstructionsCode siic = new SendImageInstructionsCode();

        private static SystemOpInstructionsCode soic = new SystemOpInstructionsCode();

        public static byte[] ExecuteInstructionsCode(InstructionsInfo insCode, ControlItemConfiguration config)
        {
            byte[] result = null;
           
            OneLevelCode lv = (OneLevelCode)insCode.LevelOneCode;
            InstructionsCode invoker = null;
            switch (lv)
            {
                case OneLevelCode.MOUSE_AND_KEYBOARD:
                    invoker = kic;
                    break;
                case OneLevelCode.IMAGE_SEND:
                    invoker = siic;
                    break;
                case OneLevelCode.SYSTEM_OP:
                    invoker = soic;
                    break;
            }
            if (invoker != null)
            {
                if (insCode.Direct != 0)
                {
                    config.ReportClient.SendPacket((byte)insCode.Direct, config.Id, 0);
                }
                result = invoker.Execute(insCode,config);
            }
            return result;
        }
        public abstract byte[] Execute(InstructionsInfo insCode, ControlItemConfiguration config);
    }
}
