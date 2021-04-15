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
using static ComConnection.Commands;
#nullable enable
namespace ComConnection
{

    class COMConnection : ICOMConnection
    {
        /// <summary>
        /// 每過幾毫秒就讀取資料 
        /// </summary>
        public readonly uint ListenInterval;
        /// <summary>
        /// 來自 input buffer 的內容
        /// </summary>
        public byte[] MessageInBytes { get; private set; }
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
        public virtual bool IsConnected
        {
            get
            {
                return CurrentPort.IsOpen;
            }
        }
 
        /// <summary>
        /// </summary>
        /// <param name="ComString">COM的名稱</param>        
        public COMConnection(string ComString, int baudRate = 115200, uint listenIntervalMs = 1000)
        {
            CurrentPort = new SerialPort(ComString, baudRate);
            COM_name = ComString;
            this.ListenInterval = listenIntervalMs < 100 ? listenIntervalMs : 1000;
            MessageInBytes = new byte[300];
        }

        /// <exception cref="InvalidOperationException"> 已經有其他連線時丟出 </exception>
        /// <exception cref="IOException"> 連線失敗時丟出 </exception>
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
        /// <exception cref="InvalidOperationException"> 根本沒有連線時丟出 </exception>
        public virtual void Disconnect()
        {
            if (!IsConnected) return;
            CurrentPort.Close();
        }
        /// <summary>
        /// 斷開連線並且清除資源
        /// </summary>
        public virtual void Dispose()
        {
            this.Disconnect();
            CurrentPort.Dispose();
        }
        /// <summary>
        /// 還有沒有東西可以讀取
        /// </summary>
        public virtual bool HasThingToRead
        {
            get => (CurrentPort.BytesToRead > 0);
        }
        /// <summary>
        /// 直接讀出 buffer 所有內容
        /// </summary>
        /// <returns>String.Empty When nothing to read</returns>
        public virtual string ReadAllContent()
        {
            return HasThingToRead ? 
                CurrentPort.ReadExisting():
                "";
        }
        /// <summary>
        /// 讀取Port內容 
        /// </summary>
        /// <returns>byte collection</returns>
        public IEnumerable<byte> ReadBytes()
        {
            int byteElement;
            while( (byteElement = CurrentPort.ReadByte()) != -1 )
            {
                yield return (byte)byteElement;
            }
        }
        /// <summary>
        /// 將接收到的訊息寫入buffer
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        public void PrintCOMMessage(object sender, SerialDataReceivedEventArgs e)
        {

        }
        /// <summary>
        /// 寫入指令到COM port
        /// </summary>
        /// <exception cref="ArgumentNullException"></exception>
        /// <exception cref="InvalidOperationException"></exception>
        /// <exception cref="ArgumentOutOfRangeException"></exception>
        /// <param name="buffer"></param>
        /// <param name="offset"></param>
        /// <param name="count"></param>
        public void WriteBytes(byte[] buffer, int offset , int count )
        {
            try
            {
                this.CurrentPort.Write(buffer, offset, count);

            }catch(IOException ioex)
            {
                Debug.WriteLine(ioex.Message);
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
