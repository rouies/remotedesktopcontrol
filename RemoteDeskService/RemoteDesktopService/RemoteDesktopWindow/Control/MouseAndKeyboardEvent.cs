using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;

namespace RemoteDesktopWindow.Control
{
    class MouseAndKeyboardEvent
    {
        private enum LevelCode
        {
            MOUSE_MOVE = 0,
            LB_MOUSE_DOWN = 1,
            LB_MOUSE_UP = 2,
            RB_MOUSE_DOWN = 3,
            RB_MOUSE_UP = 4,
            KEYBOARD_DOWN = 5,
            KEYBOARD_UP = 6

        }

        [DllImport("DesktopUtils.dll", CharSet = CharSet.Auto, CallingConvention = CallingConvention.Cdecl)]
        private static extern void MouseMove(int x, int y);

        [DllImport("DesktopUtils.dll", CharSet = CharSet.Auto, CallingConvention = CallingConvention.Cdecl)]
        private static extern void LBMouseDown();

        [DllImport("DesktopUtils.dll", CharSet = CharSet.Auto, CallingConvention = CallingConvention.Cdecl)]
        private static extern void LBMouseUp();

        [DllImport(@"DesktopUtils.dll", CharSet = CharSet.Auto, CallingConvention = CallingConvention.Cdecl)]
        private static extern void RBMouseDown();

        [DllImport(@"DesktopUtils.dll", CharSet = CharSet.Auto, CallingConvention = CallingConvention.Cdecl)]
        private static extern void RBMouseUp();

        [DllImport(@"DesktopUtils.dll", CharSet = CharSet.Auto, CallingConvention = CallingConvention.Cdecl)]
        private static extern void KeyBoardDown(int ch);

        [DllImport(@"DesktopUtils.dll", CharSet = CharSet.Auto, CallingConvention = CallingConvention.Cdecl)]
        private static extern void KeyBoardUp(int ch);

        public static void Execute(byte[] data)
        {
            LevelCode lv = (LevelCode)data[0];
            switch (lv)
            {
                case LevelCode.MOUSE_MOVE:
                    if (data != null && data.Length == 9)
                    {
                        int x = BitConverter.ToInt32(data, 1);
                        int y = BitConverter.ToInt32(data, 5);
                        MouseMove(x, y);
                    }
                    break;
                case LevelCode.LB_MOUSE_DOWN:
                    LBMouseDown();
                    break;
                case LevelCode.LB_MOUSE_UP:
                    LBMouseUp();
                    break;
                case LevelCode.RB_MOUSE_DOWN:
                    RBMouseDown();
                    break;
                case LevelCode.RB_MOUSE_UP:
                    RBMouseUp();
                    break;
                case LevelCode.KEYBOARD_DOWN:
                    if (data != null && data.Length == 5)
                    {
                        int code = BitConverter.ToInt32(data, 1);
                        KeyBoardDown(code);
                    }
                    break;
                case LevelCode.KEYBOARD_UP:
                    if (data != null && data.Length == 5)
                    {
                        int code = BitConverter.ToInt32(data, 1);
                        KeyBoardUp(code);
                    }
                    break;
            }
        }
    }
}
