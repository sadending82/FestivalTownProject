using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System.Threading;
using System.Net.Sockets;
using NetworkProtocol;
using System.Runtime.InteropServices;
using System;

public class ReceiveManager : MonoBehaviour
{
    private PacketManager _packetmanager;
    private GameObject playerManager;

    private Queue<Tuple<int, byte[]>> PacketQueue = new Queue<Tuple<int, byte[]>>();


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
                _packetmanager.GetProcessor(packetData.Item1).Process(_packetmanager,packetData.Item2, playerManager);
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
                catch(SocketException Exception)
                {
                    Debug.Log("Recv exception: " + Exception);
                    break;
                }

                // �����Ͱ� �����Ƿ� ���� ��Ŷ�� ������ ��ġ�� �������� += �س����� ����� ����� �� ����.
                // ����, ����Ʈ(C++�� Vector�� �����)
                // �迭�� ������ �ڸ��ų� �̵��ϴ� �κ��� �����Ƿ� �ſ� ��ȿ������ �����.
                // ���� ���� ����� �߰ߵǸ� ������ ������ ��.
                // C# �����͸� ã�ƿ;� �Ѵ� ����

                // ���� �����ŭ �߶��ֱ�
                byte[] ConvertedRecv = new ArraySegment<byte>(m_ReadBuffer, 0, recvSize).ToArray();

                // �ش� ���۸� �����ִ� ���� �� �κп� �ٿ��ֱ�
                m_Buffer.AddRange(ConvertedRecv);

                // ���� ������ - [][][][]  ���� ������ - [][][]
                // ��ģ ������ - [][][][][][][]
                // ��ģ �����Ϳ���, ���� ���ڸ��� �а�, �ش� �����ŭ process packet ����
                // Process Packet! - [][][][][] / [][]
                // ���� �����ʹ� ���ķ�
                // Remain - [][]

                // ó���ؾ��� �������� ��
                int toProcessData = recvSize + prevSize;

                // ��� ������ ��������
                byte[] headerData = m_Buffer.GetRange(0, 2).ToArray();

                ushort headerSize = (ushort)Marshal.SizeOf(typeof(HEADER));

                // ��� �����ͷ� ���� ��Ŷ ������ ��������
                ushort packetSize = (ushort)(BitConverter.ToUInt16(headerData, 0) + headerSize);
                while (packetSize <= toProcessData)
                {
                    //TODO : packet ó���� �Լ� ProcessPacket�� �ۼ�
                    byte[] packetList = m_Buffer.GetRange(0, packetSize).ToArray();

                    ProcessPacket(packetList);

                    // ó�������Ƿ�, packetSize ��ŭ�� ó���ؾ��� �� �ٿ��ֱ�
                    toProcessData -= packetSize;

                    // ���۵� �̹� ����� ������ �����ֱ�
                    m_Buffer.RemoveRange(0, packetSize);

                    // ���� �������ϱ�, ���� ��Ŷ�� ó���ϱ� ���� ��Ŷ ������ �ٽ� ã�ƾ� ��.
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

        byte[] data = new byte[header.size];
        Buffer.BlockCopy(packet, HeaderSize, data, 0, data.Length);

        var packetDataToProcess = new Tuple<int, byte[]>(header.type, data);

        mutex.WaitOne();
        PacketQueue.Enqueue(packetDataToProcess);
        mutex.ReleaseMutex();
    }

    public void SetPlayerManager(GameObject playerManager)
    {
        this.playerManager = playerManager;
    }

    public void OnDestroy()
    {
        workerThread.Abort();
    }
}
