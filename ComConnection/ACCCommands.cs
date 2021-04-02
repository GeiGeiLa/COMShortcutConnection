using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
#nullable enable
namespace ComConnection
{
    public static class ACCCommands
    {
        public const string Suffix = "AA";
        // Time settings
        public const string setTime_Prefix = "550106";
        public const string ackTime_Prefix = "550101";
        public const string ackTime_Error = "00";
        public const string ackTime_Success = "01";
        // Restart and start recording
        public const string setRestart_Prefix = "550201";
        public const string ackRestart_Prefix = "02";
        // Erase flash data
        public const string setErase_Prefix = "55030200FF";
        public const string ackErase_Prefix = "550301";
        public const string ackErase_Error = "00";
        public const string ackErase_InProgress = "01";
        public const string ackErase_Success = "02";
        // Get Total Page Number
        public const string setPageSize_Prefix = "550400";
        public const string ackPageSize_Prefix = "550404";
        // Get Page[N]
        public const string setPageIndex_Prefix = "550504";
        public const string ackPageIndex_Prefix = "5505FF";// Need checksum after payload bytes
        public const string ackPageIndex_Error_Prefix = "550501";
        public const string ackPageIndex_Error_NotInitial = "00";
        public const string ackPageIndex_Error_General = "FF";
        // Record to UART
        public const string setToUART_Whole = "AAAAAAAAAAAAAA";
        public const string ackToUART_Whole = "55060000AA";
        /// <summary>
        /// 根據給定的Payload片段去解讀目前ACC有幾頁資料
        /// </summary>
        /// <exception cref="FormatException"></exception>
        /// <param name="PayloadSegment">必須傳進Payload片段，而不是整個ack message</param>
        /// <returns>頁面總數</returns>





    }
}
