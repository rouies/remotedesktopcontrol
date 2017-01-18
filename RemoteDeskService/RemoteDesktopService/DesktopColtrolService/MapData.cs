using System;
using System.Collections.Generic;
using System.Linq;
using System.Net;
using System.Runtime.CompilerServices;
using System.Text;
using System.Threading;

namespace DesktopControlService
{
    public class MapData
    {
        private static ReaderWriterLock rwLock = new ReaderWriterLock();

        private static WebClient client = new WebClient();

        private static Timer timer = null;

        private static String mapUrl = "http://map.baidu.com/?qt=ipLocation";

        private static long x = 0;

        private static long y = 0;

        public static void GetLocation(out long x, out long y)
        {
            try
            {
                rwLock.AcquireReaderLock(1000);
                x = MapData.x;
                y = MapData.y;
            }
            catch (Exception)
            {
                x = 0;
                y = 0;
            }
            finally
            {
                rwLock.ReleaseReaderLock();
            }
        }

        [MethodImpl(MethodImplOptions.Synchronized)]
        public static void StartGetMapLocation(long period)
        {
            if (timer == null)
            {
                timer = new Timer(GetMapPoint, null, 0, period);
            }
        }

        private static void GetMapPoint(object state)
        {
            long x = 0;
            long y = 0;
            try
            {
                byte[] data = client.DownloadData(mapUrl);
                String dataStr = Encoding.ASCII.GetString(data);
                int startIndex = dataStr.IndexOf("\"x\":") + 4;
                int endIndex = dataStr.IndexOf(",", startIndex);
                String xStr = dataStr.Substring(startIndex, endIndex - startIndex);
                startIndex = endIndex + 5;
                endIndex = dataStr.IndexOf("}", startIndex);
                String  yStr = dataStr.Substring(startIndex, endIndex - startIndex);
                x = long.Parse(xStr);
                y = long.Parse(yStr);
            }
            catch (Exception e)
            {
                Console.WriteLine(e.Message);
                return;
            }
            try
            {
                rwLock.AcquireWriterLock(2000);
                MapData.x = x;
                MapData.y = y;
            }
            catch (Exception e)
            {
                Console.WriteLine(e.Message);
            }
            finally
            {
                rwLock.ReleaseWriterLock();
            }
        }
    }
}
