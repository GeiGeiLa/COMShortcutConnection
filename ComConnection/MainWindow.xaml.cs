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
            MessageInBytes = new byte[NumOfBytes];
            this.SelectedCOMPort = new("");
            InitializeComponent();
            RefreshCOMPorts(autoSetWhenUniquePort:true);
            cmdButtons = new[]
            { btn_clear, btn_ResetToBLE, btn_resetToUART, btn_setTime, btn_startRecording, btn_stopToUART, btn_viewAll,
            btn_Say012 };
            txtblk.Text = "";



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
                            Connection!.CurrentPort.Read(this.MessageInBytes, 0, messageLength);
                            string s = BitConverter.ToString(this.MessageInBytes, 0, messageLength);
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
                                toggleCommandButtons(true);
                            }));
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

        public IEnumerable<byte> reversedBytes(IEnumerable<byte> bts)
        {
            for (int i = bts.Count() -1; i >= 0; i--)
            {
                yield return bts.ElementAt(i);
            }
        }
        private void Refresh_Button_Click(object sender, RoutedEventArgs e)
        {
            RefreshCOMPorts();
        }
        public enum Source { Send, Receive};
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
        private void OutputWindowSimple(byte[] message, Source source)
        {
            txtblk.Text += (source == Source.Send) ? "i>" : "o<";
            if (message.Length == 1 && source == Source.Receive)// espacially for ack message for changing mode
            {
                txtblk.Text += "Mode changed\n";
                return;
            }
            try
            {
                // message[1] is the command byte
                switch(message[1])
                {
                    case 0x00:// say hello
                        txtblk.Text += (source == Source.Receive)?"Hello! from nRF52":"Try to say hello";
                        break;
                    case 0x01:// set time
                        txtblk.Text += (source == Source.Receive) ? 
                        message[3] switch
                        {
                            0x00 => "Error while setting time",
                            0x01 => "Date time is updated successfully",
                            _ => throw new ArgumentException("Invalid payload at [3]:" + message[3])
                        } 
                        : "Setting time";
                        break;
                    // no 0x02!!!
                    case 0x03:// erase mode
                        txtblk.Text += (source == Source.Receive) ? 
                            message[3] switch
                            {
                                0x01 => "not in initial mode!",
                                0X02 => "Erase in progress, please wait!",
                                0x03 => "Erase DONE!",
                                _ =>  throw new ArgumentException("Invalid payload at [3]:" + message[3])
                            }:
                            "Try to erase memory";
                        break;
                    case 0x04: // Get number of pages
                        if(source == Source.Send)
                        {
                            txtblk.Text += "Acquiring number of pages";
                            break;
                        }
                        int numOfPages = 0;
                        // shift += 8: to multiply 
                        for(int i = 3, shift = 0; i <= message.Length -2; i++, shift+= 8)
                        {
                            // message[low byte... hight byte...]
                            numOfPages += message[i] << shift; 
                        }
                        txtblk.Text += "Num of pages in nRF52:" + numOfPages;
                        break;
                        // TODO: implement explanation of page data
                    case 0x05: // Get data from specified page
                        if(source == Source.Send)
                        {
                            txtblk.Text += "Retrieving data";
                            break;
                        }
                        if (message[2] == 0xFF) // message length: return 1 on error 
                        {
                            txtblk.Text += "Raw:";
                            for (int i = 3; i < message.Length - 2; i++)
                            {
                                txtblk.Text += message[3] + '-';
                            }
                            // will break
                        }
                        else // erro occurred
                        {
                            txtblk.Text += message[3] switch
                            {
                                0x00 => "Read not initial error",
                                0x01 => "read error",
                                _ => throw new ArgumentException("Invalid payload at [3]:" + message[3])
                            };
                        }
                        break;
                    case 0x06: // to UART
                        if(source == Source.Send)
                        {
                            txtblk.Text += "Switching to UART";
                            break;
                        }
                        txtblk.Text += message[3] == 0x00 ? "To uART" :
                            throw new ArgumentException("Invalid payload at [3]:" + message[3]);
                        break;
                    default:
                        throw new ArgumentException("Unknown command byte:" + message[1]);

                }
            }
            catch
            {
                Connection!.Dispose();
            }
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
