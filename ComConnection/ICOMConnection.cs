using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

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
        
    }
}
