using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
#nullable enable
namespace ComConnection
{
    static class Commands
    {
        const string Suffix = "AA";
        // Time settings
        const string setTime_Prefix = "550106";
        const string ackTime_Prefix = "550101";
        const string ackTime_Error = "00";
        const string ackTime_Success = "01";
        // Restart and start recording
        const string setRestart_Prefix = "550201";
        const string ackRestart_Prefix = "02";
        // Erase flash data
        const string setErase_Prefix = "55030200FF";
        const string ackErase_Prefix = "550301";
        const string ackErase_Error = "00";
        const string ackErase_InProgress = "01";
        const string ackErase_Success = "02";
        // Get Total Page Number
        const string setPageSize_Prefix = "550400";
        const string ackPageSize_Prefix = "550404";
        // Get Page[N]
        const string setPageIndex_Prefix = "550504";
        const string ackPageIndex_Prefix = "5505FF";// Need checksum after payload bytes
        const string ackPageIndex_Error_Prefix = "550501";
        const string ackPageIndex_Error_NotInitial = "00";
        const string ackPageIndex_Error_General = "FF";
        // Record to UART
        const string setToUART_Whole = "AAAAAAAAAAAAAA";
        const string ackToUART_Whole = "55060000AA";
        /// <summary>
        /// 根據給定的Payload片段去解讀目前ACC有幾頁資料
        /// </summary>
        /// <exception cref="FormatException"></exception>
        /// <param name="PayloadSegment">必須傳進Payload片段，而不是整個ack message</param>
        /// <returns>頁面總數</returns>
        public static int NumberOfPages(string PayloadSegment)
        {
            if (PayloadSegment.Length > 512) throw new ArgumentException("Payload " + PayloadSegment ?? "null payload" + " is illegal");
            List<byte> payloadBytes = new();
            for(int d = PayloadSegment.Length -2; d >= 0 ; d -= 2)
            {

            }
            return 0;
        }
        public static int NumberOfPages(byte[] PayloadSegment)
        { 
            return 0;
        }




    }
}
