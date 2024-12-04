using Google.FlatBuffers;
using PacketTable.UtilitiesTable;
using PacketTable.LoginTable;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.Rendering.PostProcessing;
using System.Diagnostics.Tracing;


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

                // 캐릭터 커마
                var characterCustomizing = userInfo.CharacterCustomizing.Value;
                for (int i = 0; i < characterCustomizing.CustomizingItemsLength; i++)
                {
                    var item = characterCustomizing.CustomizingItems(i).Value;

                    int item_UID = item.ItemUid;
                    int item_Code = item.ItemCode;
                    int item_Type = item.Type;

                    Define.ItemData itemData = new Define.ItemData();
                    itemData.ItemUid = item_UID;
                    itemData.ItemCode = item_Code;
                    itemData.Type = item_Type;

                    Debug.Log($"Item Type : {itemData.Type}, ItemCode : {item_Code}");

                    Managers.Data.PlayerCustomizingData[item_Type] = itemData;
                }

                Debug.Log($"gold : {Data.Gold}, dia : {Data.Dia}, Mileage : {Data.Mileage}, NickName : {userInfo.Nickname}");
                
                Managers.Data.SetGold(Data.Gold);
                Managers.Data.SetDiamond(Data.Dia);
                Managers.Data.SetMileage(Data.Mileage);
                Managers.Data.SetNickName(userInfo.Nickname);
                Managers.Scene.LoadScene(Define.Scene.Home);

                // 출석 이벤트
                for (int i = 0; i < Data.AttendanceEventLength; ++i)
                {
                    var attendanceEventStatus = Data.AttendanceEvent(i).Value;
                    int eventCode = attendanceEventStatus.EventCode;
                    bool has_attendance_today = attendanceEventStatus.HasAttendanceToday;
                    for (int j = 0; j < attendanceEventStatus.DaysInfoLength; ++j)
                    {
                        var dayInfo = attendanceEventStatus.DaysInfo(j).Value;
                        // 몇번째 출석
                        int dayCount = dayInfo.DayNumber;

                        // 출석 날짜
                        int date_year = dayInfo.AttendanceDate.Value.Year;
                        int date_month = dayInfo.AttendanceDate.Value.Month;
                        int date_day = dayInfo.AttendanceDate.Value.Day;

                        // 보상 수령 여부
                        bool is_rewarded = dayInfo.IsRewarded;

                        Define.AttendanceEventData aed = new Define.AttendanceEventData();
                        aed.EventCode = eventCode;
                        aed.DayCount = dayCount;
                        aed.isRewarded = is_rewarded;

                        Managers.Data.AttendanceEventDataDict.TryAdd(dayCount, aed);
                        Debug.Log($"Event : {eventCode}, day_count : {dayCount}, date: {date_year}-{date_month}-{date_day}");
                    }

                    Debug.Log($"Event : {eventCode}, has_attendance_today : {has_attendance_today}");
                    //Managers.Network.GetPacketManager().SendAttendanceEventRequestPacket(eventCode);

                }

                Managers.Data.SetAttendanceDataRecved(true);

                
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
