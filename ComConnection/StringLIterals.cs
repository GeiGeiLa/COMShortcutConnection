using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ComConnection
{
    public static class StringLiterals
    {
        public const string ConnectionFailed = "無法連線";
        public const string ConnectionEmpty = "沒有選擇 COM Port";
        public const string ConnectionLost = "連線遭到中斷";
        public const string UnknownError = "未知的連線問題";
        public const string ReadError = "讀取錯誤";
        public const string WriteError = "寫入錯誤";
        public const string OperationTimeOut = "操作異常耗時";
        public const string InvalidOperation = "無效的操作";
    }
}
