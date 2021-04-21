using System;
using System.Collections.Generic;
using System.Diagnostics;
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
        /// <param name="isBigEndian"></param>
        /// <returns></returns>
        public static int ToInt(this byte[] payload, bool isBigEndian = false)
        {
            int value = 0;
            if(!isBigEndian)
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
        public static int ToInt(this byte[] payload, int startIndex, int count, bool isBigEndian = false)
        {
            int value = 0;
            if (!isBigEndian)
            {
                Debug.WriteLine("Little");
                foreach (var shift in Enumerable.Range(0,  count))
                {
                    // 1 byte == 8 bits
                    value += (payload[startIndex + shift] << (8 * shift));
                }
            }
            else
            {
                int lastIndex = startIndex + count - 1;
                foreach (var i in Enumerable.Range(0, count))
                {
                    // 1 byte == 8 bits
                    value += (payload[lastIndex - i] << (8 * i));
                }
            }
            Debug.WriteLine(value);
            return value;
        }
        /// <summary>
        /// Check if enumerable object is in List&lt;IEnumerable&gt;
        /// </summary>
        /// <param name="list"></param>
        /// <param name="bts"></param>
        /// <returns></returns>
        public static bool ContainsIEumerableObject(this List< IEnumerable<byte> > list, IEnumerable<byte> bts)
        {
            foreach(var iem in list)
            {
                if(iem.IsIdenticalTo(bts))
                {
                    return true;
                }
            }
            return false;
        }
        private static bool IsIdenticalTo(this IEnumerable<byte> obj, IEnumerable<byte> ib)
        {
            if (obj.Count() != ib.Count()) return false;
            for(int i = 0; i < obj.Count(); i++)
            {
                if (obj.ElementAt(i) != ib.ElementAt(i))
                    return false;
            }
            return true;
        }
        public static IEnumerable<byte> Reverse(this IEnumerable<byte> obj, IEnumerable<byte> bts)
        {
            for (int i = bts.Count() - 1; i >= 0; i--)
            {
                yield return obj.ElementAt(i);
            }
        }
    }
}
