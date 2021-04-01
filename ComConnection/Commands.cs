using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
#nullable enable
namespace ComConnection
{
    readonly struct Commands
    {
        public const int NumOfBytes = 300;
        /// <summary>
        /// Suffix for all commands except those marked as WHOLE
        /// </summary>
        public static readonly byte[] Suffix = { 0xAA };
        public static readonly byte[] setTime_Prefix = { 0x55, 0x01, 0x06 };
        public static readonly byte[] ackTime_Prefix = { 0x55, 0x01, 0x01 };
        public static readonly byte[] ackTime_Error_Payload = { 0x00, 0xAA };
        public static readonly byte[] ackTime_Success_Payload = { 0x01, 0xAA };
        // Restart
        public static readonly byte[] setRestart_Prefix = { 0x55, 0x02, 0x01 };
        public static readonly byte[] Restart_Record_Payload = { 0x01 };
        public static readonly byte[] Restart_UART_Payload = { 0x02 };
        public static readonly byte[] Restart_BLE_Payload = { 0x03 };
        public static readonly byte[] ackRestart_Whole = { 0x02 };
        // Erase
        public static readonly byte[] setErase_Prefix_Payload = { 0x55, 0x03, 0x02, 0x00, 0xFF };
        public static readonly byte[] ackErase_Prefix_Payload = {0x55,0x03,0x01};
        public static readonly byte[] ackErase_Error_Payload = { 0x00, 0xAA };
        public static readonly byte[] ackErase_InProgress_Payload = { 0x01, 0xAA };
        public static readonly byte[] ackErase_Success_Payload = { 0x02, 0xAA };
        /// <summary>
        /// no payload
        /// </summary>
        public static readonly byte[] setPageSize_Prefix = { 0x55, 0x04, 0x00 };
        public static readonly byte[] setPageSize_PrefixReversed = { 0x0, 0x04, 0x55 };
        /// <summary>
        /// Payload is little-endian
        /// </summary>
        public static readonly byte[] ackPageSize_Prefix = { 0x55, 0x04, 0x04 };
        public static readonly byte[] setPageIndex_Prefix = { 0x55,0x05,0x04 };
        public static readonly byte[] ackPageIndex_Prefix = { 0x55, 0x05, 0xFF };// Need checksum after payload bytes
        public static readonly byte[] ackPageIndex_Error_Prefix = { 0x55, 0x05, 0x01 };
        public static readonly byte[] ackPageIndex_Error_NotInitial_Payload = { 0x00, 0xAA };
        public static readonly byte[] ackPageIndex_Error_General_Payload = { 0xFF, 0xAA };
        public static readonly byte[] setToUART_Whole = {0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA};
        public static readonly byte[] ackToUART_Whole = { 0x55, 0x06, 0x00, 0x00, 0xAA };
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
