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
#nullable enable
namespace ComConnection
{

    class COMConnection : IDisposable
    {
        /// <summary>
        /// 每過幾毫秒就讀取資料 
        /// </summary>
        public readonly uint ListenInterval;
        /// <summary>
        /// 來自 input buffer 的內容
        /// </summary>
        public string MessageFromCOM { get; private set; }
        /// <summary>
        /// 我們要吐給 COM 的指令，以 byte[1]傳送
        /// </summary>
        public byte[] CommandBuffer = new byte[1];
        public readonly string COM_name;
        /// <summary>
        /// SerialPort 連線物件
        /// </summary>
        public SerialPort CurrentPort { get; private set; }
        /// <summary>
        /// 是否正在連線
        /// </summary>
        public bool IsConnected
        {
            get
            {
                return CurrentPort.IsOpen;
            }
        }
        /// <summary>
        /// When there are things available in COM port
        /// </summary>
        public event SerialDataReceivedEventHandler DataReceived;
        /// <summary>
        /// </summary>
        /// <param name="ComString">COM的名稱</param>        
        public COMConnection(string ComString, int baudRate = 115200, uint listenIntervalMs = 1000)
        {
            CurrentPort = new SerialPort(ComString, baudRate);
            COM_name = ComString;
            this.ListenInterval = listenIntervalMs < 100 ? listenIntervalMs : 1000;
            // Subscribe to event that occur when data is available
            this.DataReceived = new SerialDataReceivedEventHandler(PrintCOMMessage);
            MessageFromCOM = "";
        }

        /// <exception cref="InvalidOperationException"> 已經有其他連線時丟出 </exception>
        /// <exception cref="IOException"> 連線失敗時丟出 </exception>
        public virtual void Connect()
        {
            if (IsConnected) throw new InvalidOperationException(InvalidOperation);
            CurrentPort.DataBits = 8;
            CurrentPort.StopBits = StopBits.One;
            CurrentPort.Open();
            if (!IsConnected) throw new IOException(ConnectionFailed);
        }
        /// <exception cref="InvalidOperationException"> 根本沒有連線時丟出 </exception>
        public virtual void Disconnect()
        {
            if (!IsConnected) throw new InvalidOperationException("Nothing is connected");
            CurrentPort.Close();
        }
        /// <summary>
        /// 為了要搭配 using statement，需要實作 IDisposable 的 Dispose()
        /// </summary>
        public void Dispose()
        {
            this.Disconnect();
            CurrentPort.Dispose();
        }
        /// <summary>只給內部操作檢查連線狀況用，失去連線時拋出Exception</summary>
        /// <exception cref="IOException">  </exception>
        protected void CheckConnection()
        {
            if (!IsConnected) throw new IOException(ConnectionLost);
        }
        /// <summary>
        /// 還有沒有東西可以讀取
        /// </summary>
        public bool HasThingToRead
        {
            get => (CurrentPort.BytesToRead > 0);
        }
        /// <summary>
        /// 讀取 buffer 裡下一個 char
        /// </summary>
        /// <returns>null char when nothing to read</returns>
        public char NextChar()
        {
            CheckConnection();
            return HasThingToRead ?
                (char)CurrentPort.ReadChar():
                '\0';

        }
        /// <summary>
        /// 讀取下一個byte
        /// </summary>
        /// <returns>0 when nothing to read</returns>
        public byte NextByte()
        {
            CheckConnection();
            return HasThingToRead ? 
                (byte)CurrentPort.ReadByte():
                0;
        }
        /// <summary>
        /// 讀取一行文字
        /// </summary>
        /// <returns>String.Empty When nothing to read</returns>
        public string NextLine()
        {
            CheckConnection();
            return HasThingToRead ? 
                CurrentPort.ReadLine():
                "";
        }
        /// <summary>
        /// 直接讀出 buffer 所有內容
        /// </summary>
        /// <returns>String.Empty When nothing to read</returns>
        public string ReadAllContent()
        {
            CheckConnection();
            return HasThingToRead ? 
                CurrentPort.ReadExisting():
                "";
        }
        /// <summary>
        /// 印出東西，應該被 Event Handler 包住
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void PrintCOMMessage(object sender, SerialDataReceivedEventArgs e)
        {
            this.MessageFromCOM = this.ReadAllContent();
            Debug.WriteLine(this.MessageFromCOM);
        }
        public void WriteByte(byte[] buffer, int offset = 0, int count = 1)
        {
            if(buffer.Length != 1)
            {
                throw new ArgumentOutOfRangeException("buffer[] Length must be 1");
            }
            this.CurrentPort.Write(buffer, offset, count);
        }
        public void Write(string data)
        {
            CheckConnection();
            CurrentPort.Write(data);
        }
        public void WriteCharArray(char[] data, int offset, int length)
        {
            CheckConnection();
            CurrentPort.Write(data, offset, length);
        }
        
    }
}
