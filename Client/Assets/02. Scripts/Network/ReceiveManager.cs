using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System.Threading;
using System.Net.Sockets;
using NetworkProtocol;
using System.Runtime.InteropServices;
using System;
using System.IO;
using Unity.VisualScripting;

public class ReceiveManager : MonoBehaviour
{
    private PacketManager _packetmanager;

    private Queue<Tuple<ePacketType, byte[]>> PacketQueue = new Queue<Tuple<ePacketType, byte[]>>();


    private Thread workerThread;
    Mutex mutex = new Mutex(false, "QueueLock");

    public void Init(PacketManager packetManager)
    {
        _packetmanager = packetManager;
    }

    public void CreateRecvThread(TcpClient Connection)
    {
        workerThread = new Thread(() => WorkThread(Connection));
        workerThread.IsBackground = true;
        workerThread.Start();

        StartCoroutine(ProcessPacketQueue());
    }

    IEnumerator ProcessPacketQueue()
    {
        while (true)
        {
            
            while (PacketQueue.Count > 0)
            {
                mutex.WaitOne();
                var packetData = PacketQueue.Dequeue();
                mutex.ReleaseMutex();
                _packetmanager.GetProcessor(packetData.Item1).Process(_packetmanager,packetData.Item2);
            }
            yield return new WaitForFixedUpdate();
        }
    }

    private static T Deserialize<T>(byte[] bytes)
    {
        
        GCHandle gch = GCHandle.Alloc(bytes, GCHandleType.Pinned);
        T pBuffer = (T)Marshal.PtrToStructure(gch.AddrOfPinnedObject(), typeof(T));
        gch.Free();

        Debug.Log("Deserializing.");
        return pBuffer;
    }

    void WorkThread(TcpClient Connection)
    {
        Debug.Log("Thread Start.");

        NetworkStream stream = Connection.GetStream();

        var m_Buffer = new List<byte>();
        byte[] m_ReadBuffer = new byte[1000];

        int prevSize = 0;
        int recvSize = 0;

        bool isAborted = false;

        while (true)
        {
            //if(isDestroyed == false)
            //{
            //    stream.Close();
            //    Connection.Close();
            //    break;
            //}

            if (!Connection.Connected)
            {
                stream.Close();
                break;
            }

            if (stream.CanRead)
            {
                try
                {
                    recvSize = stream.Read(m_ReadBuffer, 0, sizeof(byte) * 1000);
                }
                catch (SocketException Exception)
                {
                    Debug.Log("Recv exception: " + Exception);
                    break;
                }
                catch (IOException e)
                {
                    Debug.Log($"Thread Aborting: {e}");
                    isAborted = true;
                    break;
                }

                if (isAborted) break;

                // 포인터가 없으므로 실제 패킷의 포인터 위치를 바탕으로 += 해나가는 방식은 사용할 수 없음.
                // 따라서, 리스트(C++의 Vector와 비슷함)
                // 배열을 실제로 자르거나 이동하는 부분이 많으므로 매우 비효율적인 방식임.
                // 따라서 좋은 방법이 발견되면 개선해 나가야 함.
                // C# 마스터를 찾아와야 한다 ㅂㄷ

                // 실제 사이즈만큼 잘라주기
                byte[] ConvertedRecv = new ArraySegment<byte>(m_ReadBuffer, 0, recvSize).ToArray();

                // 해당 버퍼를 남아있던 버퍼 뒷 부분에 붙여주기
                m_Buffer.AddRange(ConvertedRecv);

                // 이전 데이터 - [][][][]  이후 데이터 - [][][]
                // 합친 데이터 - [][][][][][][]
                // 합친 데이터에서, 가장 앞자리를 읽고, 해당 사이즈만큼 process packet 진행
                // Process Packet! - [][][][][] / [][]
                // 남은 데이터는 이후로
                // Remain - [][]

                // 처리해야할 데이터의 양
                int toProcessData = recvSize + prevSize;

                // 헤더 데이터 가져오기
                byte[] headerData = m_Buffer.GetRange(0, 2).ToArray();

                ushort headerSize = (ushort)Marshal.SizeOf(typeof(HEADER));

                // 헤더 데이터로 부터 패킷 사이즈 가져오기
                ushort packetSize = (ushort)(BitConverter.ToUInt16(headerData, 0) + headerSize);
                while (packetSize <= toProcessData)
                {
                    //TODO : packet 처리용 함수 ProcessPacket의 작성
                    byte[] packetList = m_Buffer.GetRange(0, packetSize).ToArray();

                    ProcessPacket(packetList);

                    // 처리했으므로, packetSize 만큼은 처리해야할 놈 줄여주기
                    toProcessData -= packetSize;

                    // 버퍼도 이미 사용한 내용을 지워주기
                    m_Buffer.RemoveRange(0, packetSize);

                    // 버퍼 지웠으니까, 다음 패킷을 처리하기 위해 패킷 사이즈 다시 찾아야 함.
                    if (toProcessData > 0)
                    {
                        headerData = m_Buffer.GetRange(0, 2).ToArray();
                        packetSize = (ushort)(BitConverter.ToUInt16(headerData, 0) + headerSize);
                    }
                    else break;

                }

                prevSize = toProcessData;
            }
        }
    }

    void ProcessPacket(byte[] packet)
    {
        int HeaderSize = Marshal.SizeOf(typeof(HEADER));
        byte[] headerData = new byte[HeaderSize];
        Buffer.BlockCopy(packet, 0, headerData, 0, HeaderSize);

        IntPtr ptr = Marshal.AllocHGlobal(HeaderSize);
        Marshal.Copy(headerData, 0, ptr, HeaderSize);
        HEADER header = (HEADER)Marshal.PtrToStructure(ptr, typeof(HEADER));
        Marshal.FreeHGlobal(ptr);

        byte[] data = new byte[header.flatBufferSize];
        Buffer.BlockCopy(packet, HeaderSize, data, 0, data.Length);

        var packetDataToProcess = new Tuple<ePacketType, byte[]>((ePacketType)header.type, data);

        mutex.WaitOne();
        PacketQueue.Enqueue(packetDataToProcess);
        mutex.ReleaseMutex();
    }

    public void OnDestroy()
    {
        if(workerThread != null) workerThread.Abort();
    }
}
