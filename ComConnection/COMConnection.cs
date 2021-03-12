using System;
using System.Collections.Generic;
using System.Text;
using System.IO;
using System.IO.Ports;
using System.Threading;
using System.Threading.Tasks;
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
        /// 外面請自己用 using statement包起來！
        /// </summary>
        /// <param name="ComString">COM的名稱</param>        
        public COMConnection(string ComString, int baudRate = 115200, uint listenIntervalMs = 1000)
        {
            CurrentPort = new SerialPort(ComString, baudRate);
            COM_name = ComString;
            this.ListenInterval = listenIntervalMs < 100 ? listenIntervalMs: 1000;
            MessageFromCOM = "";
        }
        /// <summary>
        /// 來自 input buffer 的內容
        /// </summary>
        public string MessageFromCOM { get; private set; }
        /// <summary>
        /// 我們要吐給 COM 的指令
        /// </summary>
        public string OurCommand = "";
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
        /// Whether COM accepts our commands 
        /// </summary>
        public bool IsListening { get; private set; }
        /// <summary>
        /// COM starts listening to Commands<br/>
        /// It is recommended to do this in a new thread
        /// </summary>
        public async void StartListening()
        {
            var buffer = new byte[1024];
            try
            {
                while(this.IsListening)
                {
                    if (this.HasThingToRead)
                    {

                    }

                    Task.Delay(1000);
                }
            }
            catch(Exception ex)
            {

            }
            finally
            {

            }
        }


        /// <exception cref="InvalidOperationException"> 已經有其他連線時丟出 </exception>
        /// <exception cref="IOException"> 連線失敗時丟出 </exception>
        public virtual void Connect()
        {
            if (IsConnected) throw new InvalidOperationException("Port already opened");
            CurrentPort.DataBits = 8;
            CurrentPort.StopBits = StopBits.One;
            CurrentPort.Open();
            if (!IsConnected) throw new IOException("Cannot connect to the port");
        }
        /// <exception cref="InvalidOperationException"> 根本沒有連線時丟出 </exception>
        public virtual void Disconnect()
        {
            if (!IsConnected) throw new InvalidOperationException("Nothing is connected");
            CurrentPort.Close();
            this.IsListening = false;
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
            if (!IsConnected) throw new IOException("COM connection lost");
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
        /// 當input buffer有東西時會自動呼叫此函式
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void On_DataReceived(object sender, SerialDataReceivedEventArgs e)
        {
            this.MessageFromCOM = this.ReadAllContent();
        }
        private bool _isEventSet;
        /// <summary>
        /// 設定觸發事件來新增至Message
        /// </summary>
        public void SetOn_DataReceived()
        {
            if (_isEventSet) return;
            CurrentPort.DataReceived += new SerialDataReceivedEventHandler(On_DataReceived);
            _isEventSet = true;
        }
        public void Write(string data)
        {
            CheckConnection();
            CurrentPort.Write(data);
        }
        public void WriteLine(string data)
        {
            this.Write(data+"\n");
        }
        public void WriteByte(byte[] data, int offset, int length)
        {
            CheckConnection();
            CurrentPort.Write(data, offset, length);
        }
        public void WriteCharArray(char[] data, int offset, int length)
        {
            CheckConnection();
            CurrentPort.Write(data, offset, length);
        }
        
    }
}
