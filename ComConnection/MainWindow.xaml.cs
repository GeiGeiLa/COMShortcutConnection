using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using System.Diagnostics;
using static ComConnection.StringLiterals;
using static ComConnection.Commands;
using System.IO.Ports;
#nullable enable
namespace ComConnection
{
    
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        private const int NumOfBytes = 256;
        private byte[] MessageInBytes;
        Button[] cmdButtons;
        /// <summary>
        /// COMPort connection object
        /// </summary>
        COMConnection? Connection;
        public string SelectedCOMPort { get; private set; }
        public MainWindow()
        {
            MessageInBytes = new byte[NumOfBytes];
            this.SelectedCOMPort = new("");
            InitializeComponent();
            RefreshCOMPorts(autoSetWhenUniquePort:true);
            cmdButtons = new[]
            { btn_clear, btn_ResetToBLE, btn_resetToUART, btn_setTime, btn_startRecording, btn_stopToUART, btn_viewAll };
        }

        private void COMSelector_PreviewMouseDown(object sender, MouseButtonEventArgs e)
        {
            RefreshCOMPorts();
        }

        /// <summary>
        /// 重新偵測可用的 COM Port<br></br>
        /// 若只有一個COM Port可用，則會自動選擇該 Port
        /// </summary>
        /// <returns></returns>
        private void RefreshCOMPorts(bool autoSetWhenUniquePort = false)
        {
            this.SelectedCOMPort = "";
            COMSelector.Items.Clear();
            var ports = System.IO.Ports.SerialPort.GetPortNames();
            foreach (var p in ports)
            {
                COMSelector.Items.Add(p);
            }
            if (ports.Length == 1 && autoSetWhenUniquePort)
            {
                COMSelector.SelectedIndex = 0;
                this.SelectedCOMPort = ports[0];
                btn_Connect.IsEnabled = true;
            }
        }

        private async void Connect_Button_Click(object sender, RoutedEventArgs e)
        {
            SelectedCOMPort = COMSelector?.SelectedItem?.ToString() ?? "";
            if (SelectedCOMPort == "") return;
            btn_Connect.IsEnabled = false;
            toggleCommandButtons(false);

            // Connected and try to disconnect
            if (Connection != null && Connection.IsConnected)
            {
                Connection.Dispose();
                COMSelector.IsEnabled = true;
                this.RefreshCOMPorts();
                btn_Connect.Content = "連線";
            }
            // Not connected
            else
            {
                if (this.SelectedCOMPort == "")
                {
                    MessageBox.Show(ConnectionEmpty + ", please choose one.");
                    return;
                }
                try
                {
                    Connection = new(SelectedCOMPort);
                    Connection.CurrentPort.DataReceived += new SerialDataReceivedEventHandler(this.PrintCOMMessage);
                    Connection.Connect();
                    btn_Connect.Content = "斷線";
                    var buffer = new byte[setToUART_Whole.Length];
                    var i = new byte[] { 0x55, 0x00, 0x00, 0xAA };
                    var rb = reversedBytes(i).ToArray();
                    MessageBox.Show(i.BytesHexString());
                    Connection.WriteBytes(i, 0, i.Length);
                    COMSelector.IsEnabled = false ;
                }
                catch (System.IO.IOException ex)
                {
                    MessageBox.Show(ConnectionFailed + "，再試試看？");
                    Debug.WriteLine("Exception thrown:" + ex.Message);
                    Connection.Dispose();
                }
                catch(UnauthorizedAccessException uex)
                {
                    MessageBox.Show(ConnectionFailed + "有其他程式正在占用此port!");
                }
            }
            await Task.Run( () => { Task.Delay(2000);} );
            toggleCommandButtons( Connection?.IsConnected ?? false );
            btn_Connect.IsEnabled = true;
        }
        private void toggleCommandButtons(bool shouldOpen)
        {
            foreach(var i in Enumerable.Range(0, cmdButtons.Length))
            {
                cmdButtons[i].IsEnabled = shouldOpen;
            }
        }
        private void Btn_Clear_Click(object sender, RoutedEventArgs e)
        {
            var buffer = new byte[setErase_Prefix_Payload.Length + Suffix.Length];
            Buffer.BlockCopy(setErase_Prefix_Payload, 0, buffer, 0, setErase_Prefix_Payload.Length);
            Buffer.BlockCopy(Suffix, 0, buffer, setErase_Prefix_Payload.Length, Suffix.Length);
            Connection.WriteBytes(buffer, 0, buffer.Length);
            toggleCommandButtons(false);
        }

        private void btn_viewAll_Click(object sender, RoutedEventArgs e)
        {
            var buffer = new byte[setPageSize_Prefix.Length + Suffix.Length];
            Buffer.BlockCopy(Suffix, 0, buffer, 0, Suffix.Length);
            Buffer.BlockCopy(setPageSize_PrefixReversed, 0, buffer, Suffix.Length, setPageSize_Prefix.Length);
            Connection.WriteBytes(buffer, 0, buffer.Length);
            toggleCommandButtons(false);
        }

        private void btn_setTime_Click(object sender, RoutedEventArgs e)
        {

        }

        private void btn_startRecording_Click(object sender, RoutedEventArgs e)
        {

        }

        private void btn_stopToUART_Click(object sender, RoutedEventArgs e)
        {

        }

        private void btn_resetToUART_Click(object sender, RoutedEventArgs e)
        {

        }

        private void btn_ResetToBLE_Click(object sender, RoutedEventArgs e)
        {

        }
        /// <summary>
        /// Must use invoke to update UI!!!
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void PrintCOMMessage(object sender, SerialDataReceivedEventArgs e)
        {
            int messageLength = Connection.CurrentPort.Read(this.MessageInBytes, 0, this.MessageInBytes.Length);
            string s = BitConverter.ToString(this.MessageInBytes);
            Dispatcher.BeginInvoke(new Action(()=>
            {
                MessageBox.Show(s);
                toggleCommandButtons(true);

            }));
        }

        private void COMSelector_MouseDown(object sender, MouseButtonEventArgs e)
        {
            RefreshCOMPorts();
            Debug.WriteLine("refresh");
        }

        private void COMSelector_MouseEnter(object sender, MouseEventArgs e)
        {
            RefreshCOMPorts();
            Debug.WriteLine("refresh");
        }
        public IEnumerable<byte> reversedBytes(IEnumerable<byte> bts)
        {
            for (int i = bts.Count()-1; i >= 0; i--)
            {
                yield return bts.ElementAt(i);
            }
        }
    }
}
