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
#nullable enable
namespace ComConnection
{
    
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {


        /// <summary>
        /// COMPort connection object
        /// </summary>
        COMConnection? Connection;
        public string SelectedCOMPort { get; private set; }
        public MainWindow()
        {
            this.SelectedCOMPort = new("");
            InitializeComponent();
            RefreshCOMPorts(autoSetWhenUniquePort:true);
        }

        private void COMSelector_PreviewMouseDown(object sender, MouseButtonEventArgs e)
        {
            RefreshCOMPorts();
        }

        private void COMSelector_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            this.SelectedCOMPort = COMSelector.SelectedItem?.ToString() ?? "";

            if(this.SelectedCOMPort != "")
            {
                btn_Connect.IsEnabled = true;
            }

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
            btn_Connect.IsEnabled = false;
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

        private void Connect_Button_Click(object sender, RoutedEventArgs e)
        {
            if (this.SelectedCOMPort == "") throw new InvalidOperationException(ConnectionEmpty);
            MessageBox.Show(SelectedCOMPort);
            Connection = new(SelectedCOMPort);
            try
            {
                Connection.Connect();
            }
            catch(Exception ex)
            {
                MessageBox.Show(ConnectionFailed+"，再試試看？");
                Debug.WriteLine("Exception thrown:" + ex.Message);
                Connection.Dispose();
            }
        }

        private void Btn_Clear_Click(object sender, RoutedEventArgs e)
        {
            var btn = sender as Button ?? throw new InvalidCastException();
            btn.IsEnabled = false;
            Connection?.WriteByte()
            btn.IsEnabled = true;
        }

        private void btn_viewAll_Click(object sender, RoutedEventArgs e)
        {

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
    }
}
