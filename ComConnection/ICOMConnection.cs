using System;
using System.IO.Ports;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
#nullable enable
namespace ComConnection
{
    interface ICOMConnection:IDisposable
    {
        /// <summary>
        /// Connect To COM Ports
        /// </summary>
        void Connect();
        /// <summary>
        /// Disconnect to COM Ports
        /// </summary>
        void Disconnect();
        /// <summary>
        /// Check if COM connected, wrapping the SerialPort function call
        /// </summary>
        bool IsConnected { get; }
        /// <summary>
        /// Check if there are avail content to read, wrapping the SerialPort function call
        /// </summary>
        bool HasThingToRead { get; }

        /// <summary>
        /// Directly write commands to the serial port
        /// </summary>
        /// <param name="buffer"></param>
        /// <param name="offset"></param>
        /// <param name="count"></param>
        bool WriteBytes(byte[] buffer, int offset, int count);

        /// <summary>
        /// What do when received ack message from Port
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        void SetEventHandler(SerialDataReceivedEventHandler handler, SerialPort? p = null);
    }
}
