using System;
using System.Collections.Generic;
using System.Collections.Immutable;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
#nullable enable
namespace ComConnection
{
    public static class CommandExplanations
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
        public static readonly ImmutableArray<byte> ackErase_Prefix = ImmutableArray.Create<byte>(new byte[]{0x55,0x03,0x01});
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
        /// If some ack message indicates that COM is busy, add it to this collection
        /// </summary>
        /// <returns></returns>
        public static List<IEnumerable<byte> > AcksThatSuppressButtonToggling;
        static CommandExplanations()
        {
            AcksThatSuppressButtonToggling = new List<IEnumerable<byte>>();
            // If you want to add element to this list, please do it in your client 
        }
        /// <summary>
        /// Get a collection which is the combination of all elements of ImmutableArray &lt;byte&gt; params
        /// </summary>
        /// <param name="imarrays"></param>
        /// <returns></returns>
        public static IEnumerable<byte> ConcatToIEnumerable(this ImmutableArray<byte> obj, params ImmutableArray<byte>[] imarrays)
        {
            foreach (var o in obj)
            {
                yield return o;
            }
            foreach(var arr in imarrays)
            {
                foreach(var element in arr)
                {
                    yield return element;
                }
            }
        }
        /// <summary>
        /// return object of IEnumerable&lt;byte&gt; type, whose elements are values of all elements of ImmutableArray&lt;byte&gt;
        /// </summary>
        /// <param name="imarrays"></param>
        /// <returns></returns>
        public static IEnumerable<byte> ConcatToSingleIEnumearble(params ImmutableArray<byte>[] imarrays)
        {
            foreach (var arr in imarrays)
            {
                foreach (var element in arr)
                {
                    yield return element;
                }
            }
        }

        /// <summary>
        /// Decode the number of data pages available in ACC
        /// </summary>
        /// <returns>Number of pages</returns>
        public static int NumberOfPages(byte[] payload, int startIndex, int count, bool isBigEndian)
        {
            return payload.ToInt(startIndex, count, isBigEndian);
        }
        /// <summary>
        /// What does the ack says?
        /// </summary>
        /// <param name="message"></param>
        /// <param name="source"></param>
        /// <returns></returns>
        public static string MeaningOfAck(byte[] message, Source source)
        {
            if (message.Length == 1 && source == Source.Receive)// espacially for ack message for changing mode
            {
                return "Mode changed";
            }
            const string InvalidPayloadError = "Invalid payload at [3]: ";
            const int payloadIndex = 3;
            string explanation;
            // message[1] is the command byte
            switch (message[1])
            {
                case 0x00:// say hello
                    explanation = (source == Source.Receive) ? "Hello! from nRF52" : "Try to say hello";
                    break;
                case 0x01:// set time
                    explanation = (source == Source.Receive) ?
                    message[payloadIndex] switch
                    {
                        0x00 => "Error while setting time",
                        0x01 => "Date time is updated successfully",
                        _ => throw new ArgumentException(InvalidPayloadError + message[3])
                    }
                    : "Setting time";
                    break;
                // no 0x02!!!
                case 0x03:// erase mode
                    explanation = (source == Source.Receive) ?
                        message[payloadIndex] switch
                        {
                            0x00 => "not in initial mode!",
                            0X01 => "Erase in progress, please wait!",
                            0x02 => "Erase DONE!",
                            _ => throw new ArgumentException(InvalidPayloadError + message[3])
                        } :
                        "Try to erase memory";
                    break;
                case 0x04: // Get number of pages
                    const int payloadLength = 4;
                    if (source == Source.Send)
                    {
                        explanation = "Acquiring number of pages";
                        break;
                    }
                    int numOfPages = message.ToInt(payloadIndex, payloadLength, isBigEndian: false);
                    explanation = "Num of pages in nRF52:" + numOfPages;
                    break;
                // TODO: implement explanation of page data
                case 0x05: // Get data from specified page
                    if (source == Source.Send)
                    {
                        explanation = "Retrieving data";
                        break;
                    }
                    if (message[2] == 0xFF) // message length: return 1 on error 
                    {
                        explanation = "Raw:";
                        for (int i = payloadIndex; i < message.Length - 2; i++)
                        {
                            explanation += message[i] + '-';
                        }
                        break; // for readability improvement only
                    }
                    else // erro occurred
                    {
                        explanation = message[payloadIndex] switch
                        {
                            0x00 => "Read not initial error",
                            0x01 => "read error",
                            _ => throw new ArgumentException(InvalidPayloadError + message[payloadIndex])
                        };
                    }
                    break;
                case 0x06: // to UART
                    if (source == Source.Send)
                    {
                        explanation = "Switching to UART";
                        break;
                    }
                    explanation = message[payloadIndex] == 0x00 ? "To uART" :
                        throw new ArgumentException(InvalidPayloadError + message[payloadIndex]);
                    break;
                default:
                    throw new ArgumentException("Unknown command byte:" + message[1]);
            }
            return explanation;
        }
    }
}
