using Google.FlatBuffers;
using PacketTable.UtilitiesTable;
using PacketTable.LoginTable;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.Rendering.PostProcessing;


namespace Network.PacketProcessor
{
    public class LoginResponseProcessor : PacketProcessor
    {
        public override void Process(PacketManager packetmanager, byte[] data)
        {
            var bb = new ByteBuffer(data);
            var Data = LoginResponse.GetRootAsLoginResponse(bb);

            int resultCode = Data.ResultCode;

            Debug.Log($"Login Result : {resultCode}");

            if (resultCode == 1)
            {
                var userInfo = Data.UserInfo.Value;

                var characterCustomizing = userInfo.CharacterCustomizing.Value;

                for (int i = 0; i < characterCustomizing.CustomizingItemsLength; i++)
                {
                    var item = characterCustomizing.CustomizingItems(i).Value;

                    int item_UID = item.ItemUid;
                    int item_Code = item.ItemCode;
                    int item_Type = item.Type;

                }

                Debug.Log($"gold : {Data.Gold}, dia : {Data.Dia}, Mileage : {Data.Mileage}, NickName : {userInfo.Nickname}");
                
                Managers.Data.SetGold(Data.Gold);
                Managers.Data.SetDiamond(Data.Dia);
                Managers.Data.SetMileage(Data.Mileage);
                Managers.Data.SetNickName(userInfo.Nickname);
                Managers.Scene.LoadScene(Define.Scene.Home);


            }
            else
            {
                var ui = Managers.UI.ShowPopUpUI<UI_SignInSuccess>();
                ui.Init();
                ui.TextChange("로그인을 실패했습니다.");
                
            }
        }


    }
}
