using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ComConnection
{
    public static class StringLiterals
    {
        public const string ConnectionFailed = "Connection failed";
        public const string ConnectionEmpty = "COM Port not selected";
        public const string ConnectionLost = "Connection lost";
        public const string UnknownError = "Unknown connection error";
        public const string ReadError = "Error while reading";
        public const string WriteError = "Error while writing";
        public const string OperationTimeOut = "Operation taking too long";
        public const string InvalidOperation = "Illegal state";
    }
}
