using Microsoft.Win32;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;

namespace DesktopControlService
{
    class SystemOpInstructionsCode : InstructionsCode
    {
        [DllImport("DesktopUtils.dll", CharSet = CharSet.Auto, CallingConvention = CallingConvention.Cdecl)]
        private static extern int SetAuthCode([MarshalAs(UnmanagedType.LPWStr)]String value);

        [DllImport("DesktopUtils.dll", CharSet = CharSet.Auto, CallingConvention = CallingConvention.Cdecl)]
        private static extern int SetAuthLock([MarshalAs(UnmanagedType.LPWStr)]String value);

        [DllImport("DesktopUtils.dll", CharSet = CharSet.Auto, CallingConvention = CallingConvention.Cdecl)]
        private static extern int SetMHID([MarshalAs(UnmanagedType.LPWStr)]String value);

        [DllImport("DesktopUtils.dll", CharSet = CharSet.Auto, CallingConvention = CallingConvention.Cdecl)]
        private static extern int SetLoginName([MarshalAs(UnmanagedType.LPWStr)]String value);

        [DllImport("DesktopUtils.dll", CharSet = CharSet.Auto, CallingConvention = CallingConvention.Cdecl)]
        private static extern int SetSupperCode([MarshalAs(UnmanagedType.LPWStr)]String value);

        [DllImport("DesktopUtils.dll", CharSet = CharSet.Auto, CallingConvention = CallingConvention.Cdecl)]
        private static extern int CreateUserProcess([MarshalAs(UnmanagedType.LPWStr)]String app);

        [DllImport("DesktopUtils.dll", CharSet = CharSet.Auto, CallingConvention = CallingConvention.Cdecl)]
        private static extern int ShowMessage([MarshalAs(UnmanagedType.LPWStr)]String title, [MarshalAs(UnmanagedType.LPWStr)]String message);


        public enum LevelCode
        {
            MODIFY_AUTH = 0,
            MODIFY_LOCK = 1,
            MODIFY_ID = 2,
            MODIFY_LOGIN_NAME = 3,
            MODIFY_SUPPER_CODE = 4,
            LOGOUT = 5,
            SHOW_MESSAGE = 6,
            TERMINATE_AUTH = 7
        }

        public override byte[] Execute(InstructionsInfo insCode, ControlItemConfiguration config)
        {
            int levelCode = insCode.LevelTwoCode;
            byte[] data = insCode.Data;
            LevelCode lv = (LevelCode)levelCode;
            var appPath = String.Format("{0}\\RemoteDesktopWindow.exe 0 0 0", config.BasePath);
            //消息反馈
            switch (lv)
            {
                case LevelCode.MODIFY_AUTH:
                    if (data != null && data.Length == 6)
                    {
                        var value = Encoding.ASCII.GetString(data);
                        if (value.Length == 6)
                        {
                            SetAuthCode(value);
                        }
                    }
                    break;
                case LevelCode.MODIFY_LOCK:
                    if (data != null && data.Length == 1)
                    {
                        var value = Encoding.ASCII.GetString(data);
                        if (value.Length == 1)
                        {
                            SetAuthLock(value);
                            CreateUserProcess(appPath);
                        }
                    }
                    break;
                case LevelCode.MODIFY_ID:
                    if (data != null)
                    {
                        var value = Encoding.ASCII.GetString(data);
                        SetMHID(value);
                    }
                    break;
                case LevelCode.MODIFY_LOGIN_NAME:
                    if (data != null)
                    {
                        var value = Encoding.ASCII.GetString(data);
                        SetLoginName(value);
                    }
                    break;
                case LevelCode.MODIFY_SUPPER_CODE:
                    if (data != null)
                    {
                        var value = Encoding.ASCII.GetString(data);
                        SetSupperCode(value);
                    }
                    break;
                case LevelCode.LOGOUT:
                    int state = CreateUserProcess(appPath);
                    break;
                case LevelCode.SHOW_MESSAGE:
                    if (data != null) 
                    { 
                        var msg = Encoding.UTF8.GetString(data);
                        ShowMessage("来自管理员的消息", msg);
                    }
                    break;
                case LevelCode.TERMINATE_AUTH:
                    SetAuthCode("010101");
                    CreateUserProcess(appPath);
                    break;
            }
            return null;
        }
    }
}
