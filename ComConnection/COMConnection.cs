using System;
using System.Collections.Generic;
using System.Text;
using System.IO;
using System.IO.Ports;
using System.Threading;
using System.Threading.Tasks;
using System.Diagnostics;
// My using directives
using static ComConnection.StringLiterals;
using static ComConnection.CommandExplanations;
#nullable enable
namespace ComConnection
{

    class COMConnection : ICOMConnection
    {
        /// <summary>
        /// If COM port is busy, WriteBytes method will do nothing and discard your commands
        /// Becoming free when ack message(DataReceivedEvent) received
        /// </summary>
        public bool IsBusy
        {
            get; private set;
        }
        public readonly uint ListenInterval;
        public byte[] MessageInBytes { get; private set; }
        /// <summary>
        /// Command that we want to send to ACC
        /// </summary>
        public byte[] CommandBuffer = new byte[1];
        public readonly string COM_name;
        /// <summary>
        /// SerialPort object
        /// </summary>
        public SerialPort CurrentPort { get; private set; }
        public virtual bool IsConnected
        {
            get => CurrentPort.IsOpen;
        }

        /// <summary>
        /// </summary>
        /// <param name="ComString">The name of COM</param>        
        public COMConnection(string ComString, int baudRate = 115200, uint listenIntervalMs = 1000)
        {
            CurrentPort = new SerialPort(ComString, baudRate);
            COM_name = ComString;
            this.ListenInterval = listenIntervalMs < 100 ? listenIntervalMs : 1000;
            MessageInBytes = new byte[300];
            CurrentPort.DataReceived += (object s, SerialDataReceivedEventArgs e) =>
            {
                IsBusy = false;
            };
        }

        /// <exception cref="InvalidOperationException"> Is already connected </exception>
        /// <exception cref="IOException"> Thrown when cannot connect </exception>
        public virtual void Connect()
        {
            if (IsConnected) throw new InvalidOperationException(InvalidOperation);
            CurrentPort.DataBits = 8;
            CurrentPort.BaudRate = 115200;
            CurrentPort.Parity = Parity.None;
            CurrentPort.StopBits = StopBits.One;
            CurrentPort.Open();
            if (!IsConnected) throw new IOException(ConnectionFailed);
        }
        /// <exception cref="InvalidOperationException"> Connection is empty </exception>
        public virtual void Disconnect()
        {
            if (!IsConnected) return;
            CurrentPort.Close();
        }
        public virtual void Dispose()
        {
            this.Disconnect();
            CurrentPort.Dispose();
        }
        /// <summary>
        /// whether there is something to read
        /// </summary>
        public virtual bool HasThingToRead
        {
            get => (CurrentPort.BytesToRead > 0);
        }
        /// <summary>
        /// Read all contents in buffer
        /// </summary>
        /// <returns>String.Empty When nothing to read</returns>
        public virtual string ReadAllContent()
        {
            return HasThingToRead ?
                CurrentPort.ReadExisting() :
                "";
        }
        /// <summary>
        /// Read Port contents 
        /// </summary>
        /// <returns>byte collection</returns>
        public IEnumerable<byte> ReadBytes()
        {
            int byteElement;
            while ((byteElement = CurrentPort.ReadByte()) != -1)
            {
                yield return (byte)byteElement;
            }
        }

        /// <summary>
        /// Set DataReceived event
        /// </summary>
        /// <param name="handler"></param>
        /// <param name="p"></param>
        public void SetEventHandler(SerialDataReceivedEventHandler handler, SerialPort? p = null)
        {
            if (p == null)
                CurrentPort.DataReceived += handler;
            else
                p.DataReceived += handler;
        }
        /// <summary>
        /// Write commands to COM port, =>true:success
        /// </summary>
        /// <exception cref="ArgumentNullException"></exception>
        /// <exception cref="InvalidOperationException"></exception>
        /// <exception cref="ArgumentOutOfRangeException"></exception>
        /// <param name="buffer"></param>
        /// <param name="offset"></param>
        /// <param name="count"></param>
        public bool WriteBytes(byte[] buffer, int offset , int count )
        {
            try
            {
                if (IsBusy) return false;
                this.CurrentPort.Write(buffer, offset, count);
                IsBusy = true;
                return true;
            }catch(IOException ioex)
            {
                Debug.WriteLine(ioex.Message);
                return false;
            }
        }
        public void Write(byte cmdByte, int offset, int count)
        {
            try
            { 
            }
            catch (IOException ioex)
            {
                Debug.WriteLine(ioex.Message);
            }
        }
    }
}
