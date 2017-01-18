using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Text;

namespace DesktopControlService
{
    class SendImageInstructionsCode : InstructionsCode
    {
        private ImageClient client;

        public enum LevelCode
        {
            START_SEND_IMAGE = 0,
            STOP_SEND_IMAGE = 1
        }

        public SendImageInstructionsCode()
        {
           
        }

        [MethodImpl(MethodImplOptions.Synchronized)]
        public override byte[] Execute(InstructionsInfo insCode, ControlItemConfiguration config)
        {
            if (this.client == null)
            {
                this.client = new ImageClient(config.BasePath);
            }
            int levelCode = insCode.LevelTwoCode;
            byte[] data = insCode.Data;
            LevelCode lv = (LevelCode)levelCode;
            switch (lv)
            {
                case LevelCode.START_SEND_IMAGE:
                    bool res = this.client.SendImageData(config.TargetAddress, config.ReceiveImageDataPort);
                    if (res)
                    {
                        return new byte[] { 0x0 };
                    }
                    else
                    {
                        return new byte[] { 0xF };
                    }
                case LevelCode.STOP_SEND_IMAGE:
                    this.client.StopSend();
                    break;
            }
            return null;
        }
    }
}
