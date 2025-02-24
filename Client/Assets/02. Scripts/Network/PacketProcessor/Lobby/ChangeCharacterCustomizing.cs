using ExcelDataStructure;
using Google.FlatBuffers;
using NetworkProtocol;
using PacketTable.LobbyTable;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Unity.VisualScripting;
using UnityEngine;

namespace Network.PacketProcessor
{
    public class ChangeCharacterCustomizingProcessor : PacketProcessor
    {
        public override void Process(PacketManager packetmanager, byte[] data)
        {
            var bb = new ByteBuffer(data);

            var Data = ChangeCharacterCustomizing.GetRootAsChangeCharacterCustomizing(bb);

            Debug.Log(Data.CustomizingItemsLength);

            for (int i = 0; i < Data.CustomizingItemsLength; i++)
            {
                var item = Data.CustomizingItems(i).Value;

                int item_UID = item.ItemUid;
                int item_Code = item.ItemCode;
                int item_Type = item.Type;

                if (item_Code != 0)
                {
                    if (Managers.Scene.CurrentScene.GetComponent<GameScene>() != null)
                    {
                        GameObject tPlayer = Managers.Player.GetMyPlayer();
                        tPlayer.GetComponent<CharacterStatus>().ChangeCustomizing(item_Code);
                    }
                }
            }

            if (Managers.Scene.CurrentScene.GetComponent<HomeScene>() != null)
            {
                var ui = Managers.UI.ShowPopUpUI<UI_Notice>();
                ui.Init();
                ui.NoticeTextChange("커스터마이징이 변경되었습니다.");
                ui.BindPopupCloseEvent();
            }
        }
    }
}
