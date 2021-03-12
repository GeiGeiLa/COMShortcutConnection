﻿#pragma checksum "..\..\..\MainWindow.xaml" "{ff1816ec-aa5e-4d10-87f7-6f4963833460}" "F40DAA4000591D004B6C849F1B6D7ED524456EB7"
//------------------------------------------------------------------------------
// <auto-generated>
//     This code was generated by a tool.
//     Runtime Version:4.0.30319.42000
//
//     Changes to this file may cause incorrect behavior and will be lost if
//     the code is regenerated.
// </auto-generated>
//------------------------------------------------------------------------------

using ComConnection;
using System;
using System.Diagnostics;
using System.Windows;
using System.Windows.Automation;
using System.Windows.Controls;
using System.Windows.Controls.Primitives;
using System.Windows.Controls.Ribbon;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Ink;
using System.Windows.Input;
using System.Windows.Markup;
using System.Windows.Media;
using System.Windows.Media.Animation;
using System.Windows.Media.Effects;
using System.Windows.Media.Imaging;
using System.Windows.Media.Media3D;
using System.Windows.Media.TextFormatting;
using System.Windows.Navigation;
using System.Windows.Shapes;
using System.Windows.Shell;


namespace ComConnection {
    
    
    /// <summary>
    /// MainWindow
    /// </summary>
    public partial class MainWindow : System.Windows.Window, System.Windows.Markup.IComponentConnector {
        
        
        #line 13 "..\..\..\MainWindow.xaml"
        [System.Diagnostics.CodeAnalysis.SuppressMessageAttribute("Microsoft.Performance", "CA1823:AvoidUnusedPrivateFields")]
        internal System.Windows.Controls.ComboBox COMSelector;
        
        #line default
        #line hidden
        
        
        #line 14 "..\..\..\MainWindow.xaml"
        [System.Diagnostics.CodeAnalysis.SuppressMessageAttribute("Microsoft.Performance", "CA1823:AvoidUnusedPrivateFields")]
        internal System.Windows.Controls.Button btn_Connect;
        
        #line default
        #line hidden
        
        
        #line 18 "..\..\..\MainWindow.xaml"
        [System.Diagnostics.CodeAnalysis.SuppressMessageAttribute("Microsoft.Performance", "CA1823:AvoidUnusedPrivateFields")]
        internal System.Windows.Controls.Button btn_clear;
        
        #line default
        #line hidden
        
        
        #line 19 "..\..\..\MainWindow.xaml"
        [System.Diagnostics.CodeAnalysis.SuppressMessageAttribute("Microsoft.Performance", "CA1823:AvoidUnusedPrivateFields")]
        internal System.Windows.Controls.Button btn_viewAll;
        
        #line default
        #line hidden
        
        
        #line 20 "..\..\..\MainWindow.xaml"
        [System.Diagnostics.CodeAnalysis.SuppressMessageAttribute("Microsoft.Performance", "CA1823:AvoidUnusedPrivateFields")]
        internal System.Windows.Controls.Button btn_setTime;
        
        #line default
        #line hidden
        
        
        #line 21 "..\..\..\MainWindow.xaml"
        [System.Diagnostics.CodeAnalysis.SuppressMessageAttribute("Microsoft.Performance", "CA1823:AvoidUnusedPrivateFields")]
        internal System.Windows.Controls.Button btn_startRecording;
        
        #line default
        #line hidden
        
        
        #line 22 "..\..\..\MainWindow.xaml"
        [System.Diagnostics.CodeAnalysis.SuppressMessageAttribute("Microsoft.Performance", "CA1823:AvoidUnusedPrivateFields")]
        internal System.Windows.Controls.Button btn_stopToUART;
        
        #line default
        #line hidden
        
        
        #line 23 "..\..\..\MainWindow.xaml"
        [System.Diagnostics.CodeAnalysis.SuppressMessageAttribute("Microsoft.Performance", "CA1823:AvoidUnusedPrivateFields")]
        internal System.Windows.Controls.Button btn_resetToUART;
        
        #line default
        #line hidden
        
        
        #line 24 "..\..\..\MainWindow.xaml"
        [System.Diagnostics.CodeAnalysis.SuppressMessageAttribute("Microsoft.Performance", "CA1823:AvoidUnusedPrivateFields")]
        internal System.Windows.Controls.Button btn_ResetToBLE;
        
        #line default
        #line hidden
        
        private bool _contentLoaded;
        
        /// <summary>
        /// InitializeComponent
        /// </summary>
        [System.Diagnostics.DebuggerNonUserCodeAttribute()]
        [System.CodeDom.Compiler.GeneratedCodeAttribute("PresentationBuildTasks", "5.0.1.0")]
        public void InitializeComponent() {
            if (_contentLoaded) {
                return;
            }
            _contentLoaded = true;
            System.Uri resourceLocater = new System.Uri("/ComConnection;V1.0.0.0;component/mainwindow.xaml", System.UriKind.Relative);
            
            #line 1 "..\..\..\MainWindow.xaml"
            System.Windows.Application.LoadComponent(this, resourceLocater);
            
            #line default
            #line hidden
        }
        
        [System.Diagnostics.DebuggerNonUserCodeAttribute()]
        [System.CodeDom.Compiler.GeneratedCodeAttribute("PresentationBuildTasks", "5.0.1.0")]
        [System.ComponentModel.EditorBrowsableAttribute(System.ComponentModel.EditorBrowsableState.Never)]
        [System.Diagnostics.CodeAnalysis.SuppressMessageAttribute("Microsoft.Design", "CA1033:InterfaceMethodsShouldBeCallableByChildTypes")]
        [System.Diagnostics.CodeAnalysis.SuppressMessageAttribute("Microsoft.Maintainability", "CA1502:AvoidExcessiveComplexity")]
        [System.Diagnostics.CodeAnalysis.SuppressMessageAttribute("Microsoft.Performance", "CA1800:DoNotCastUnnecessarily")]
        void System.Windows.Markup.IComponentConnector.Connect(int connectionId, object target) {
            switch (connectionId)
            {
            case 1:
            this.COMSelector = ((System.Windows.Controls.ComboBox)(target));
            
            #line 13 "..\..\..\MainWindow.xaml"
            this.COMSelector.PreviewMouseDown += new System.Windows.Input.MouseButtonEventHandler(this.COMSelector_PreviewMouseDown);
            
            #line default
            #line hidden
            
            #line 13 "..\..\..\MainWindow.xaml"
            this.COMSelector.SelectionChanged += new System.Windows.Controls.SelectionChangedEventHandler(this.COMSelector_SelectionChanged);
            
            #line default
            #line hidden
            return;
            case 2:
            this.btn_Connect = ((System.Windows.Controls.Button)(target));
            
            #line 14 "..\..\..\MainWindow.xaml"
            this.btn_Connect.Click += new System.Windows.RoutedEventHandler(this.Connect_Button_Click);
            
            #line default
            #line hidden
            return;
            case 3:
            this.btn_clear = ((System.Windows.Controls.Button)(target));
            
            #line 18 "..\..\..\MainWindow.xaml"
            this.btn_clear.Click += new System.Windows.RoutedEventHandler(this.Btn_Clear_Click);
            
            #line default
            #line hidden
            return;
            case 4:
            this.btn_viewAll = ((System.Windows.Controls.Button)(target));
            
            #line 19 "..\..\..\MainWindow.xaml"
            this.btn_viewAll.Click += new System.Windows.RoutedEventHandler(this.btn_viewAll_Click);
            
            #line default
            #line hidden
            return;
            case 5:
            this.btn_setTime = ((System.Windows.Controls.Button)(target));
            
            #line 20 "..\..\..\MainWindow.xaml"
            this.btn_setTime.Click += new System.Windows.RoutedEventHandler(this.btn_setTime_Click);
            
            #line default
            #line hidden
            return;
            case 6:
            this.btn_startRecording = ((System.Windows.Controls.Button)(target));
            
            #line 21 "..\..\..\MainWindow.xaml"
            this.btn_startRecording.Click += new System.Windows.RoutedEventHandler(this.btn_startRecording_Click);
            
            #line default
            #line hidden
            return;
            case 7:
            this.btn_stopToUART = ((System.Windows.Controls.Button)(target));
            
            #line 22 "..\..\..\MainWindow.xaml"
            this.btn_stopToUART.Click += new System.Windows.RoutedEventHandler(this.btn_stopToUART_Click);
            
            #line default
            #line hidden
            return;
            case 8:
            this.btn_resetToUART = ((System.Windows.Controls.Button)(target));
            
            #line 23 "..\..\..\MainWindow.xaml"
            this.btn_resetToUART.Click += new System.Windows.RoutedEventHandler(this.btn_resetToUART_Click);
            
            #line default
            #line hidden
            return;
            case 9:
            this.btn_ResetToBLE = ((System.Windows.Controls.Button)(target));
            
            #line 24 "..\..\..\MainWindow.xaml"
            this.btn_ResetToBLE.Click += new System.Windows.RoutedEventHandler(this.btn_ResetToBLE_Click);
            
            #line default
            #line hidden
            return;
            }
            this._contentLoaded = true;
        }
    }
}
