using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ComConnection
{
    /// <summary>
    /// 擴充String class
    /// </summary>
    public static class ByteExtension
    {
        public static string BytesHexString(this IEnumerable<byte> bts)
        {
            StringBuilder s = new("");
            foreach(var bt in bts)
            {
                s.Append(bt.ToString("X"));
            }
            return s.ToString();
        }
    }
}
