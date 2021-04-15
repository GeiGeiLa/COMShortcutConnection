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
        public unsafe static byte[] FastConcat(this byte[] src1, int src1Offset, int src1Count, byte[] src2, int src2Offset, int src2Count )
        {
            byte[] target = new byte[src1Count + src2Count];
            int index = 0;
            fixed(byte* s1 = src1, s2 = src2, t = target)
            {
                for(int i = 0; i < src1Count; i ++, index++)
                {
                    t[index] = s1[i];
                }
                for(int i = 0; i < src2Count; i++, index++)
                {
                    t[index] = s2[i];
                }
            }
            return target;
        }
        /// <summary>
        /// Convert payload to intger value byte by byte
        /// </summary>
        /// <param name="payload"></param>
        /// <param name="isLittleEndian"></param>
        /// <returns></returns>
        public static int CalcByteArrayValue(byte[] payload, bool isLittleEndian = true)
        {
            int value = 0;
            if(isLittleEndian)
            {
                foreach (var i in Enumerable.Range(0, payload.Length))
                {
                    // 1 byte == 8 bits
                    value += ( payload[i] << (8 * i) );
                }
            }
            else
            {
                int lastIndex = payload.Length - 1;
                foreach (var i in Enumerable.Range(0, payload.Length))
                {
                    // 1 byte == 8 bits
                    value += ( payload[lastIndex - i] << (8 * i) );
                }
            }
            return value;
        }
    }
}
