using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Google.FlatBuffers;

namespace Network.PacketProcessor
{
    public class PacketProcessor
    { 
        public virtual void Process(byte[] data) { }

        private FlatBufferBuilder mBuilder;
    }
}
