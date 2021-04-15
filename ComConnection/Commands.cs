using System;
using System.Collections.Generic;
using System.Collections.Immutable;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
#nullable enable
namespace ComConnection
{
    readonly struct Commands
    {
        /// <summary>
        /// Suffix for all commands except those marked as WHOLE
        /// </summary>
        public static readonly ImmutableArray<byte> setSay012_Whole = ImmutableArray.Create<byte>(new byte[]{ 0x55, 0x00, 0x00, 0xAA });
        public static readonly ImmutableArray<byte> ackSay012_Whole = ImmutableArray.Create<byte>(new byte[] { 0x55, 0x00, 0x01, 0x02, 0xAA });
        public static readonly ImmutableArray<byte> Suffix = ImmutableArray.Create<byte>(new byte[] { 0xAA });
        public static readonly ImmutableArray<byte> setTime_Prefix = ImmutableArray.Create<byte>(new byte[] { 0x55, 0x01, 0x06 });
        public static readonly ImmutableArray<byte> ackTime_Prefix = ImmutableArray.Create<byte>(new byte[] { 0x55, 0x01, 0x01 });
        public static readonly ImmutableArray<byte> ackTime_Error_Payload = ImmutableArray.Create<byte>(new byte[] { 0x00, 0xAA });
        public static readonly ImmutableArray<byte> ackTime_Success_Payload = ImmutableArray.Create<byte>(new byte[] { 0x01, 0xAA });
        // Restart
        public static readonly ImmutableArray<byte> setRestart_Prefix = ImmutableArray.Create<byte>(new byte[]{ 0x55, 0x02, 0x01 });
        public static readonly ImmutableArray<byte> Restart_Record_Payload = ImmutableArray.Create<byte>(new byte[]{ 0x01 });
        public static readonly ImmutableArray<byte> Restart_UART_Payload = ImmutableArray.Create<byte>(new byte[]{ 0x02 });
        public static readonly ImmutableArray<byte> Restart_BLE_Payload = ImmutableArray.Create<byte>(new byte[]{ 0x03 });
        public static readonly ImmutableArray<byte> ackRestart_Whole = ImmutableArray.Create<byte>(new byte[]{ 0x02 });
        // Erase
        public static readonly ImmutableArray<byte> setErase_Prefix_Payload = ImmutableArray.Create<byte>(new byte[]{ 0x55, 0x03, 0x02, 0x00, 0xFF });
        public static readonly ImmutableArray<byte> ackErase_Prefix_Payload = ImmutableArray.Create<byte>(new byte[]{0x55,0x03,0x01});
        public static readonly ImmutableArray<byte> ackErase_Error_Payload = ImmutableArray.Create<byte>(new byte[]{ 0x00, 0xAA });
        public static readonly ImmutableArray<byte> ackErase_InProgress_Payload = ImmutableArray.Create<byte>(new byte[]{ 0x01, 0xAA });
        public static readonly ImmutableArray<byte> ackErase_Success_Payload = ImmutableArray.Create<byte>(new byte[]{ 0x02, 0xAA });
        /// <summary>
        /// no payload
        /// </summary>
        public static readonly ImmutableArray<byte> setPageSize_Prefix = ImmutableArray.Create<byte>(new byte[]{ 0x55, 0x04, 0x00 });
        public static readonly ImmutableArray<byte> setPageSize_PrefixReversed = ImmutableArray.Create<byte>(new byte[]{ 0x0, 0x04, 0x55 });
        /// <summary>
        /// Payload is little-endian
        /// </summary>
        public static readonly ImmutableArray<byte> ackPageSize_Prefix = ImmutableArray.Create<byte>(new byte[]{ 0x55, 0x04, 0x04 });
        public static readonly ImmutableArray<byte> setPageIndex_Prefix = ImmutableArray.Create<byte>(new byte[]{ 0x55,0x05,0x04 });
        public static readonly ImmutableArray<byte> ackPageIndex_Prefix = ImmutableArray.Create<byte>(new byte[]{ 0x55, 0x05, 0xFF });// Need checksum after payload bytes
        public static readonly ImmutableArray<byte> ackPageIndex_Error_Prefix = ImmutableArray.Create<byte>(new byte[]{ 0x55, 0x05, 0x01 });
        public static readonly ImmutableArray<byte> ackPageIndex_Error_NotInitial_Payload = ImmutableArray.Create<byte>(new byte[]{ 0x00, 0xAA });
        public static readonly ImmutableArray<byte> ackPageIndex_Error_General_Payload = ImmutableArray.Create<byte>(new byte[]{ 0xFF, 0xAA });
        public static readonly ImmutableArray<byte> setToUART_Whole = ImmutableArray.Create<byte>(new byte[]{0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA});
        public static readonly ImmutableArray<byte> ackToUART_Whole = ImmutableArray.Create<byte>(new byte[]{ 0x55, 0x06, 0x00, 0x00, 0xAA });
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
