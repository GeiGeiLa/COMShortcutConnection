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
using System.Collections.Immutable;
using static ComConnection.StringLiterals;
using static ComConnection.CommandExplanations;
using System.IO.Ports;
#nullable enable
namespace ComConnection
{
    
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        const string seperator = "---------------------------------";
        private bool isVerbose = false;
        private bool shouldScroll = false;
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
            MessageInBytes = Array.Empty<byte>();
            this.SelectedCOMPort = new("");
            InitializeComponent();
            RefreshCOMPorts(autoSetWhenUniquePort:true);
            cmdButtons = new[]
            { btn_clear, btn_ResetToBLE, btn_resetToUART, btn_setTime, btn_startRecording, btn_stopToUART, btn_viewAll,
            btn_Say012 };
            txtblk.Text = "";
            //suppress button toggle when erase in progress message received
            AcksThatSuppressButtonToggling.Add(ConcatToIEnumerable(ackErase_Prefix, ackErase_InProgress_Payload, Suffix));
            AcksThatSuppressButtonToggling.Add(ConcatToIEnumerable(ackSay012_Whole));

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
                COMSelector!.IsEnabled = true;
                this.RefreshCOMPorts();
                btn_Connect.Content = "連線";
                return;
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
                    if(Connection != null && Connection.IsConnected)
                    {
                        Connection.Dispose();
                    }
                    Connection = new(SelectedCOMPort);
                    Connection.SetEventHandler(
                        (object sender, SerialDataReceivedEventArgs e) =>
                        {
                            int messageLength = Connection!.CurrentPort.BytesToRead;
                            this.MessageInBytes = new byte[messageLength];
                            Connection!.CurrentPort.Read(this.MessageInBytes, 0, messageLength);
                            string s = BitConverter.ToString(this.MessageInBytes, 0, messageLength);
                            // Must use begin invoke bacause Event handler is not called in Main thread
                            // Or InvalidOperationException will be thrown
                            Dispatcher.BeginInvoke(new Action(() =>
                            {
                                if(!isVerbose)
                                {
                                    OutputWindowSimple(this.MessageInBytes, Source.Receive);
                                }
                                else
                                {
                                    OutputWindowVerbose(s, Source.Receive);
                                }
                                // DO NOT toggle on button when receive "erasing in progress" message
                                if(AcksThatSuppressButtonToggling.ContainsIEumerableObject(MessageInBytes))
                                {
                                    toggleCommandButtons(false);
                                    Debug.WriteLine("Waiting for process");
                                    MessageBox.Show("This is taking about 3 mins, please wait in patience");
                                }
                                else
                                {
                                    toggleCommandButtons(true);
                                }
                            }));// end event handle settings
                        }
                    );
                    Connection.Connect();
                    btn_Connect.Content = "斷線";
                    byte[] bitarr = setSay012_Whole.ToArray();
                    Connection.WriteBytes(bitarr, 0, setSay012_Whole.Length);
                    COMSelector!.IsEnabled = false ;
                    OutputWindowVerbose(BitConverter.ToString(bitarr), Source.Send);

                }
                catch (System.IO.IOException ex)
                {
                    MessageBox.Show(ConnectionFailed + "，再試試看？");
                    Debug.WriteLine("Exception thrown:" + ex.Message);
                    Connection!.Dispose();
                }
                catch(UnauthorizedAccessException uex)
                {
                    MessageBox.Show(ConnectionFailed + "，有其他程式正在占用此port!", uex.Message , 
                        MessageBoxButton.OK, MessageBoxImage.Error);
                }
                await Task.Run(() => { Task.Delay(3000); });
                toggleCommandButtons(Connection!.IsConnected);
                btn_Connect.IsEnabled = true;
            }

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
            if(Connection!.IsBusy)
            {
                ShowErrorMessage("Still busy, please wait",String.Empty);
                return;
            }
            txtblk.Text += seperator + "\n";
            var buffer = new byte[setErase_Prefix_Payload.Length + Suffix.Length];
            Buffer.BlockCopy(setErase_Prefix_Payload.ToArray(), 0, buffer, 0, setErase_Prefix_Payload.Length);
            Buffer.BlockCopy(Suffix.ToArray(), 0, buffer, setErase_Prefix_Payload.Length, Suffix.Length);
            try
            {
                toggleCommandButtons(false);
                Connection!.WriteBytes(buffer, 0, buffer.Length);
            }
            catch
            {
                ShowErrorMessage(ConnectionLost, "請重新連線");
                Connection?.Dispose();
            }

        }
        private void btn_Say012_Click(object snder, RoutedEventArgs e)
        {
            txtblk.Text += seperator + "\n";
            byte[] bitarr = setSay012_Whole.ToArray();
            Connection!.WriteBytes(bitarr, 0, setSay012_Whole.Length);
            if (isVerbose) OutputWindowVerbose(BitConverter.ToString(bitarr), Source.Send);
            else OutputWindowSimple(bitarr, Source.Send);
        }
        private void btn_viewAll_Click(object sender, RoutedEventArgs e)
        {
            txtblk.Text += seperator + "\n";
            var sw = new Stopwatch();
            sw.Start();
            var buffer = new byte[setPageSize_Prefix.Length + Suffix.Length];
            Buffer.BlockCopy(setPageSize_Prefix.ToArray(), 0, buffer, 0, setPageSize_Prefix.Length);
            Buffer.BlockCopy(Suffix.ToArray(), 0, buffer, setPageIndex_Prefix.Length, Suffix.Length);
            try
            {
                Connection!.WriteBytes(buffer, 0, buffer.Length);
                if(isVerbose)
                {
                    OutputWindowVerbose(BitConverter.ToString(buffer), Source.Send);
                }
                else
                {
                    OutputWindowSimple(buffer, Source.Send);
                }
                toggleCommandButtons(false);
            }
            catch
            {
                ShowErrorMessage(ConnectionLost, "");
                Connection?.Dispose();
            }
        }

        private void btn_setTime_Click(object sender, RoutedEventArgs e)
        {
            txtblk.Text += seperator + "\n";
            ShowErrorMessage("敬啟期待功能開放","");
        }

        private void btn_startRecording_Click(object sender, RoutedEventArgs e)
        {
            txtblk.Text += seperator + "\n";
            var buffer = new byte[setRestart_Prefix.Length + Restart_Record_Payload.Length + Suffix.Length];
            Buffer.BlockCopy(setRestart_Prefix.ToArray(), 0, dst: buffer, 0, count:setRestart_Prefix.Length);
            Buffer.BlockCopy(Restart_Record_Payload.ToArray(), 0, dst: buffer, setRestart_Prefix.Length, Restart_Record_Payload.Length);
            Buffer.BlockCopy(Suffix.ToArray(), 0, dst: buffer, setRestart_Prefix.Length + Restart_Record_Payload.Length, count: Suffix.Length);
            try
            {
                Connection!.WriteBytes(buffer, 0, buffer.Length);
                if (isVerbose) OutputWindowVerbose(BitConverter.ToString(buffer), Source.Send);
                else OutputWindowSimple(buffer, Source.Send);
                toggleCommandButtons(false);
            }
            catch
            {
                ShowErrorMessage(ConnectionLost, "");
                Connection?.Dispose();
            }
        }

        private void btn_stopToUART_Click(object sender, RoutedEventArgs e)
        {
            txtblk.Text += seperator + "\n";
        }

        private void btn_resetToUART_Click(object sender, RoutedEventArgs e)
        {
            txtblk.Text += seperator + "\n";
        }

        private void btn_ResetToBLE_Click(object sender, RoutedEventArgs e)
        {
            txtblk.Text += seperator + "\n";
        }


        private void Refresh_Button_Click(object sender, RoutedEventArgs e)
        {
            RefreshCOMPorts();
        }
        private void OutputWindowVerbose(string text, Source source)
        {
            string src = (source == Source.Send) ? "i>" : "o<";
            txtblk.Text += src + text + "\n";
            if (shouldScroll)
            {
                Debug.WriteLine("SCROLL END");
                scrollViewer.ScrollToEnd();
            }
        }
        private void OutputWindowSimple(byte[] message, Source source, bool suppressDebugMessage = false)
        {
            message.Count();
            if(!suppressDebugMessage)
            {
                Debug.WriteLine(
                    BitConverter.ToString(message,0,message.Length)
                    );
            }
            txtblk.Text += (source == Source.Send) ? "i>" : "o<";
            txtblk.Text += MeaningOfAck(message, source);
            txtblk.Text += "\n";
            if(shouldScroll)
            {
                Debug.WriteLine("SCROLL END");
                scrollViewer.ScrollToEnd();
            }
        }
        public void ShowErrorMessage(string content, string caption)
        {
            MessageBox.Show(content, caption, MessageBoxButton.OK, MessageBoxImage.Error);
        }
        private void FixedPointerVSBlockCopy()
        {
            var sw2 = new Stopwatch();
            unsafe
            {
                byte[] buf2 = new byte[0];
                sw2.Start();

                for (int i = 0; i < 10000000; i++)
                {
                    buf2 = setPageIndex_Prefix.ToArray().FastConcat(0, setPageSize_Prefix.Length, Suffix.ToArray(), 0, Suffix.Length);
                }
                Debug.WriteLine(buf2.ToString());
                sw2.Stop();
            }
            var sw = new Stopwatch();
            sw.Start();
            byte[] buffer = new byte[0];
            for (int i = 0; i < 10000000; i++)
            {
                buffer = new byte[setPageSize_Prefix.Length + Suffix.Length];
                Buffer.BlockCopy(setPageSize_Prefix.ToArray(), 0, buffer, 0, setPageSize_Prefix.Length);
                Buffer.BlockCopy(Suffix.ToArray(), 0, buffer, setPageIndex_Prefix.Length, Suffix.Length);
            }
            Debug.WriteLine(buffer.ToString());

            sw.Stop();
            Debug.WriteLine("BlockCopy:" + sw.ElapsedMilliseconds);
            Debug.WriteLine("Fixed pointer:" + sw2.ElapsedMilliseconds);
        }

        private void checkbox_verbose_Click(object sender, RoutedEventArgs e)
        {
            isVerbose = checkbox_verbose.IsChecked ?? false;
        }
        private void checkbox_autoscroll_Click(object sender, RoutedEventArgs e)
        {
            shouldScroll = checkbox_autoscroll.IsChecked ?? false;
        }
    }
}
