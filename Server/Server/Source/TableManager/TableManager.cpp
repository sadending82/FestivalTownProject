#include <fstream>
#include <chrono>
#include "TableManager.h"

TableManager::TableManager()
{

}

TableManager::~TableManager()
{
    ClearAllTable();
}

void TableManager::ClearAllTable()
{
    // item
    ItemInfos.clear();

    // character
    CharacterStats.clear();

    // weapon
    WeaponStats.clear();

    // gamemode
    GameModeData.clear();

    // map
    MapData.clear();

    // point constant
    PointConstantList.clear();

    // Game Reward
    GameRewardList.clear();

    // Game Bonus Reward
    for (auto& outer_pair : GameBonusRewardList) {
        outer_pair.second.clear();
    }
    GameBonusRewardList.clear();

    // random box
    RandomBoxList.clear();

    // gacha item
    for (auto& outer_pair : GachaItemList) {
        outer_pair.second.clear();
    }
    GachaItemList.clear();
}

void TableManager::ReadAllDataTable()
{
    ReadItemTable();
    ReadCharacterStat();
    ReadWeaponStat();
    ReadGameModeTable();
    ReadMapData();
    ReadPointConstantTable();
    ReadGachaTable();

    ReadGameReward();
}

void TableManager::ReadItemTable()
{
    try {
        int sheetIdx = 0;
        mWorkbook.load("GameData/Item.xlsx");
        mWorksheet = mWorkbook.sheet_by_index(sheetIdx);

        int idx = 0;

        for (auto row : mWorksheet.rows(false)) {
            if (idx == variableNameIdx) {
                idx++;
                continue;
            }

            if (!row.empty()) {
                int index = (int)row[0].value<int>();

                /*std::chrono::year_month_day openDate, closeDate;
                std::istringstream ss1(row[ItemTable_Field::IT_Item_Type].to_string());
                std::chrono::from_stream(ss1, "%Y-%m-%d", openDate);

                std::istringstream ss2(row[ItemTable_Field::IT_Close_Date].to_string());
                std::chrono::from_stream(ss2, "%Y-%m-%d", closeDate);*/

                ItemInfos[index] = ItemTable{
                    index,
                    row[ItemTable_Field::IT_Name].to_string(),
                    row[ItemTable_Field::IT_File_Name].to_string(),
                    (ItemType)row[ItemTable_Field::IT_Item_Type].value<int>(),
                    (ItemGrade)row[ItemTable_Field::IT_Item_Grade].value<int>(),
                    /*openDate,
                    closeDate*/
                };
            }

            idx++;
        }
    }
    catch (const xlnt::exception& e) {
        std::cerr << "Excel File Load Fail: " << e.what() << std::endl;
    }
}

void TableManager::ReadCharacterStat()
{
    try {
       
        mWorkbook.load("GameData/Ch_Stat.xlsx");

        int idx = 0;
        mWorksheet = mWorkbook.sheet_by_index(Ch_Stat_Sheet);
        for (auto row : mWorksheet.rows(false)) {
            if (idx < 1) {
                idx++;
                continue;
            }

            if (!row.empty()) {
                CharacterStats[(int)row[static_cast<int>(CharacterStat_Field::index)].value<int>()]
                    = CharacterStat{
                    (int)row[static_cast<int>(CharacterStat_Field::index)].value<int>(),
                    (std::string)row[static_cast<int>(CharacterStat_Field::name)].to_string(),
                    (int)row[static_cast<int>(CharacterStat_Field::hp)].value<int>(),
                    (int)row[static_cast<int>(CharacterStat_Field::stamina)].value<int>(),
                    (int)row[static_cast<int>(CharacterStat_Field::strength)].value<int>(),
                    (int)row[static_cast<int>(CharacterStat_Field::speed)].value<int>()
                };
            }
            idx++;
        }

        idx = 0;
        mWorksheet = mWorkbook.sheet_by_index(Ch_Attack_Sheet);
        for (auto row : mWorksheet.rows(false)) {
            if (idx < 1) {
                idx++;
                continue;
            }

            if (!row.empty()) {
                AttackStats[(int)row[static_cast<int>(AttackStat_Field::Index)].value<int>()]
                    = AttackStat{
                    (int)row[static_cast<int>(AttackStat_Field::Index)].value<int>(),
                    (std::string)row[static_cast<int>(AttackStat_Field::Name)].to_string(),
                    (float)row[static_cast<int>(AttackStat_Field::Value)].value<float>(),
                    (int)row[static_cast<int>(AttackStat_Field::Attack_Speed)].value<int>(),
                    (int)row[static_cast<int>(AttackStat_Field::Ch_StaminaConsume)].value<int>(),
                    (int)row[static_cast<int>(AttackStat_Field::Steal_Stamina)].value<int>()
                };
            }

            idx++;
        }

        idx = 0;
        mWorksheet = mWorkbook.sheet_by_index(Ch_Move_Sheet);
        for (auto row : mWorksheet.rows(false)) {
            if (idx < 1) {
                idx++;
                continue;
            }

            if (!row.empty()) {
                MoveStats[(int)row[static_cast<int>(MoveStat_Field::Index)].value<int>()]
                    = MoveStat{
                    (int)row[static_cast<int>(MoveStat_Field::Index)].value<int>(),
                    (std::string)row[static_cast<int>(MoveStat_Field::Name)].to_string(),
                    (float)row[static_cast<int>(MoveStat_Field::Value)].value<float>(),
                    (int)row[static_cast<int>(MoveStat_Field::Ch_StaminaConsume)].value<int>(),
                    (int)row[static_cast<int>(MoveStat_Field::Ch_Stamina_recovery)].value<int>()
                };
            }
            idx++;
        }

    }
    catch (const xlnt::exception& e) {
        std::cerr << "Excel Character_Stat File Load Fail: " << e.what() << std::endl;
    }

}

void TableManager::ReadWeaponStat()
{
    try {
       
        mWorkbook.load("GameData/Weapon_Stat.xlsx");

        int idx = 0;

       mWorksheet = mWorkbook.active_sheet();

        for (auto row : mWorksheet.rows(false)) {
            if (idx < 1) {
                idx++;
                continue;
            }

            if (!row.empty()) {
                WeaponStats[(int)row[static_cast<int>(WeaponStat_Field::index)].value<int>()]
                    = WeaponStat{
                    (int)row[static_cast<int>(WeaponStat_Field::index)].value<int>(),
                    (std::string)row[static_cast<int>(WeaponStat_Field::name)].to_string(),
                    (int)row[static_cast<int>(WeaponStat_Field::Weapon_Power)].value<int>(),
                    (int)row[static_cast<int>(WeaponStat_Field::Weapon_Range)].value<int>(),
                    (int)row[static_cast<int>(WeaponStat_Field::Weapon_Type)].value<int>(),
                    (int)row[static_cast<int>(WeaponStat_Field::Weapon_StaminaConsume)].value<int>()
                };
            }
            idx++;
        }
    }
    catch (const xlnt::exception& e) {
        std::cerr << "Excel Weapon File Load Fail: " << e.what() << std::endl;
    }

}

void TableManager::ReadGameModeTable()
{
    try {
       
        mWorkbook.load("GameData/Mode_Ver3.xlsx");
        int idx = 0;
        mWorksheet = mWorkbook.sheet_by_index(Mode_Out_Sheet);

        for (auto row : mWorksheet.rows(false)) {
            if (idx == variableNameIdx) {
                idx++;
                continue;
            }

            if (!row.empty()) {
                int index = row[0].value<int>();

               GameModeData[(GameMode)index]
                    =  GameModeInfo{
                    row[GameModeOut_Field::Player_Count].value<int>(),
                    row[GameModeOut_Field::Team_Count].value<int>(),
                    row[GameModeOut_Field::Team_Color].value<int>(),
                    row[GameModeOut_Field::Play_Map].value<int>()
                };

               MapListByMode[(GameMode)index].push_back(MapCode::TEST);
            }

            idx++;
        }

        idx = 0;
        mWorksheet = mWorkbook.sheet_by_index(FITH_Mode_Sheet);

        for (auto row : mWorksheet.rows(false)) {
            if (idx == variableNameIdx) {
                idx++;
                continue;
            }

            if (!row.empty()) {
                int index = row[0].value<int>();

                if (GameModeData.find((GameMode)index) == GameModeData.end()) {
                    continue;
                }
                GameModeData[(GameMode)index].Play_Time = row[GameModeInfo_Field::GM_Play_Time].value<int>();
                GameModeData[(GameMode)index].Player_Spawn_Time = row[GameModeInfo_Field::GM_Player_Spawn_Time].value<int>();
                GameModeData[(GameMode)index].Life_Count = row[GameModeInfo_Field::GM_Life_Count].value<int>();
                GameModeData[(GameMode)index].Bomb_Spawn_Count = row[GameModeInfo_Field::GM_Bomb_Spawn_Count].value<int>();
                GameModeData[(GameMode)index].Bomb_Spawn_Time = row[GameModeInfo_Field::GM_Bomb_Spawn_Time].value<int>();
                GameModeData[(GameMode)index].Bomb_Delay_Time = row[GameModeInfo_Field::GM_Bomb_Delay_Time].value<int>();
                GameModeData[(GameMode)index].Weapon1_Spawn_Index = row[GameModeInfo_Field::GM_Weapon1_Spawn_Index].value<int>();
                GameModeData[(GameMode)index].Weapon1_Spawn_Count = row[GameModeInfo_Field::GM_Weapon1_Spawn_Count].value<int>();
                GameModeData[(GameMode)index].Weapon1_Spawn_Time = row[GameModeInfo_Field::GM_Weapon1_Spawn_Time].value<int>();
                GameModeData[(GameMode)index].Weapon2_Spawn_Index = row[GameModeInfo_Field::GM_Weapon2_Spawn_Index].value<int>();
                GameModeData[(GameMode)index].Weapon2_Spawn_Count = row[GameModeInfo_Field::GM_Weapon2_Spawn_Count].value<int>();
                GameModeData[(GameMode)index].Weapon2_Spawn_Time = row[GameModeInfo_Field::GM_Weapon2_Spawn_Time].value<int>();
                GameModeData[(GameMode)index].Block1_Spawn_Index = row[GameModeInfo_Field::GM_Block1_Spawn_Index].value<int>();
                GameModeData[(GameMode)index].Block1_Spawn_Count = row[GameModeInfo_Field::GM_Block1_Spawn_Count].value<int>();
                GameModeData[(GameMode)index].Block1_Spawn_Time = row[GameModeInfo_Field::GM_Block1_Spawn_Time].value<int>();
                GameModeData[(GameMode)index].Block2_Spawn_Index = row[GameModeInfo_Field::GM_Block2_Spawn_Index].value<int>();
                GameModeData[(GameMode)index].Block2_Spawn_Count = row[GameModeInfo_Field::GM_Block2_Spawn_Count].value<int>();
                GameModeData[(GameMode)index].Block2_Spawn_Time = row[GameModeInfo_Field::GM_Block2_Spawn_Time].value<int>();
            }

            idx++;
        }
    }
    catch (const xlnt::exception& e) {
        std::cerr << "Excel File Load Fail: " << e.what() << std::endl;
    }
}

void TableManager::ReadMapData()
{
    std::ifstream inputFile("GameData/Map1.txt");

    if (!inputFile.is_open()) {
        std::cerr << "File Not Exist" << std::endl;
        return;
    }

    std::string line;
    int lineCnt = 0;
    while (std::getline(inputFile, line)) {
        std::vector<char> row;
        char character;
        std::istringstream iss(line);

        int colCnt = 0;
        while (iss >> character) {
            row.push_back(character);

            switch (character) {
            case 'n': {
                MapData[MapCode::TEST].GetBlockDropIndexes().push_back({ colCnt, lineCnt });
            }break;
            case 'b': {
                MapData[MapCode::TEST].GetObjectSpawnIndexes().push_back({ colCnt, lineCnt });
                if (colCnt < 10) {
                    MapData[MapCode::TEST].GetRedObjectSpawnIndexes().push_back({ colCnt, lineCnt });
                }
                else {
                    MapData[MapCode::TEST].GetBlueObjectSpawnIndexes().push_back({ colCnt, lineCnt });
                }
            }break;
            case 'R': {
                MapData[MapCode::TEST].GetPlayerSpawnIndexes((int)TeamCode::RED).push_back({colCnt, lineCnt});
            }break;
            case 'B': {
                MapData[MapCode::TEST].GetPlayerSpawnIndexes((int)TeamCode::BLUE).push_back({ colCnt, lineCnt });
            }break;
            default: {

            }break;
            }

            colCnt++;
        }

        MapData[MapCode::TEST].GetStructure().push_back(row);
        lineCnt++;
    }

    inputFile.close();
}

void TableManager::ReadPointConstantTable()
{
    try {

       
        mWorkbook.load("GameData/Mode_Ver3.xlsx");

        int idx = 0;

       mWorksheet = mWorkbook.sheet_by_index(Mode_Point_Sheet);

        for (auto row : mWorksheet.rows(false)) {
            if (idx == variableNameIdx) {
                idx++;
                continue;
            }
            if (!row.empty()) {

                GameMode modeIndex = (GameMode)row[PointConstant_Field::PC_Mode_Index].value<int>();
                BattleResult battleResult = (BattleResult)row[PointConstant_Field::PC_Battle_Result].value<int>();
                ConstantType type = (ConstantType)row[PointConstant_Field::PC_Type].value<int>();

                switch (type) {
                case ConstantType::CT_Kill_Point: {
                    switch (battleResult) {
                    case BattleResult::BR_Win: {
                        PointConstantList[modeIndex].Win_Kill_Point.WorkType = row[PointConstant_Field::PC_WorkType].value<int>();
                        PointConstantList[modeIndex].Win_Kill_Point.Value = row[PointConstant_Field::PC_Value].value<int>();
                    }
                    break;
                    case BattleResult::BR_Lose: {
                        PointConstantList[modeIndex].Lose_Kill_Point.WorkType = row[PointConstant_Field::PC_WorkType].value<int>();
                        PointConstantList[modeIndex].Lose_Kill_Point.Value = row[PointConstant_Field::PC_Value].value<int>();
                    }
                    break;
                    default:
                        break;
                    }
                }
                break;
                case ConstantType::CT_Death_Point: {
                    switch (battleResult) {
                    case BattleResult::BR_Win: {
                        PointConstantList[modeIndex].Win_Death_Point.WorkType = row[PointConstant_Field::PC_WorkType].value<int>();
                        PointConstantList[modeIndex].Win_Death_Point.Value = row[PointConstant_Field::PC_Value].value<int>();
                    }
                                             break;
                    case BattleResult::BR_Lose: {
                        PointConstantList[modeIndex].Lose_Death_Point.WorkType = row[PointConstant_Field::PC_WorkType].value<int>();
                        PointConstantList[modeIndex].Lose_Death_Point.Value = row[PointConstant_Field::PC_Value].value<int>();
                    }
                                              break;
                    default:
                        break;
                    }
                }
                break;
                case ConstantType::CT_Bomb_Point: {
                    switch (battleResult) {
                    case BattleResult::BR_Win: {
                        PointConstantList[modeIndex].Win_Bomb_Point.WorkType = row[PointConstant_Field::PC_WorkType].value<int>();
                        PointConstantList[modeIndex].Win_Bomb_Point.Value = row[PointConstant_Field::PC_Value].value<int>();
                    }
                                             break;
                    case BattleResult::BR_Lose: {
                        PointConstantList[modeIndex].Lose_Bomb_Point.WorkType = row[PointConstant_Field::PC_WorkType].value<int>();
                        PointConstantList[modeIndex].Lose_Bomb_Point.Value = row[PointConstant_Field::PC_Value].value<int>();
                    }
                                              break;
                    default:
                        break;
                    }
                }
                break;
                default:
                    break;
                }
            }

            idx++;
        }
    }
    catch (const xlnt::exception& e) {
        std::cerr << "Excel File Load Fail: " << e.what() << std::endl;
    }
}

void TableManager::ReadGameReward()
{
    try {
       
        mWorkbook.load("GameData/Mode_Ver3.xlsx");

        // Mode_Reward_Sheet
        int idx = 0;
       mWorksheet = mWorkbook.sheet_by_index(Mode_Reward_Sheet);
        for (auto row : mWorksheet.rows(false)) {
            if (idx == variableNameIdx) {
                idx++;
                continue;
            }
            if (!row.empty()) {
                GameMode modeIndex = (GameMode)row[Reward_Field::RW_Mode_Index].value<int>();
                BattleResult battleResult = (BattleResult)row[Reward_Field::RW_Battle_Result].value<int>();

                switch (battleResult) {
                case BattleResult::BR_Win: {
                    GameRewardList[modeIndex].Win_Reward1_Index = row[Reward_Field::RW_Reward1_Index].value<int>();
                    GameRewardList[modeIndex].Win_Reward1_Value = row[Reward_Field::RW_Reward1_Value].value<int>();
                    GameRewardList[modeIndex].Win_Reward2_Index = row[Reward_Field::RW_Reward2_Index].value<int>();
                    GameRewardList[modeIndex].Win_Reward2_Value = row[Reward_Field::RW_Reward2_Value].value<int>();
                    GameRewardList[modeIndex].Win_Reward3_Index = row[Reward_Field::RW_Reward3_Index].value<int>();
                    GameRewardList[modeIndex].Win_Reward3_Value = row[Reward_Field::RW_Reward3_Value].value<int>();
                }
                break;
                case BattleResult::BR_Lose: {
                    GameRewardList[modeIndex].Lose_Reward1_Index = row[Reward_Field::RW_Reward1_Index].value<int>();
                    GameRewardList[modeIndex].Lose_Reward1_Value = row[Reward_Field::RW_Reward1_Value].value<int>();
                    GameRewardList[modeIndex].Lose_Reward2_Index = row[Reward_Field::RW_Reward2_Index].value<int>();
                    GameRewardList[modeIndex].Lose_Reward2_Value = row[Reward_Field::RW_Reward2_Value].value<int>();
                    GameRewardList[modeIndex].Lose_Reward3_Index = row[Reward_Field::RW_Reward3_Index].value<int>();
                    GameRewardList[modeIndex].Lose_Reward3_Value = row[Reward_Field::RW_Reward3_Value].value<int>();
                }
                break;
                default:{

                }
                break;
                }
            }

            idx++;
        }


        // Mode_BonusReward
        idx = 0;
        mWorksheet = mWorkbook.sheet_by_index(Mode_BonusReward_Sheet);
        for (auto row : mWorksheet.rows(false)) {
            if (idx == variableNameIdx) {
                idx++;
                continue;
            }
            if (!row.empty()) {
                GameMode modeIndex = (GameMode)row[BonusReward_Field::BR_Mode_Index].value<int>();
                BattleResult battleResult = (BattleResult)row[BonusReward_Field::BR_Battle_Result].value<int>();
                int type = row[BonusReward_Field::BR_Type].value<int>();
                int point = row[BonusReward_Field::BR_Point].value<int>();

                switch (battleResult) {
                case BattleResult::BR_Win: {
                    switch (type) {
                    case 1: {
                        GameBonusRewardList[modeIndex][point].Win_Reward1_Index = row[BonusReward_Field::BR_Reward1_Index].value<int>();
                        GameBonusRewardList[modeIndex][point].Win_Reward1_Value = row[BonusReward_Field::BR_Reward1_Value].value<int>();
                        GameBonusRewardList[modeIndex][point].Win_Reward2_Index = row[BonusReward_Field::BR_Reward2_Index].value<int>();
                        GameBonusRewardList[modeIndex][point].Win_Reward2_Value = row[BonusReward_Field::BR_Reward2_Value].value<int>();
                        GameBonusRewardList[modeIndex][point].Win_Reward3_Index = row[BonusReward_Field::BR_Reward3_Index].value<int>();
                        GameBonusRewardList[modeIndex][point].Win_Reward3_Value = row[BonusReward_Field::BR_Reward3_Value].value<int>();
                    }
                          break;
                    case 2: {
                        GameBonusRewardList[modeIndex][point].MVP_Reward1_Index = row[BonusReward_Field::BR_Reward1_Index].value<int>();
                        GameBonusRewardList[modeIndex][point].MVP_Reward1_Value = row[BonusReward_Field::BR_Reward1_Value].value<int>();
                        GameBonusRewardList[modeIndex][point].MVP_Reward2_Index = row[BonusReward_Field::BR_Reward2_Index].value<int>();
                        GameBonusRewardList[modeIndex][point].MVP_Reward2_Value = row[BonusReward_Field::BR_Reward2_Value].value<int>();
                        GameBonusRewardList[modeIndex][point].MVP_Reward3_Index = row[BonusReward_Field::BR_Reward3_Index].value<int>();
                        GameBonusRewardList[modeIndex][point].MVP_Reward3_Value = row[BonusReward_Field::BR_Reward3_Value].value<int>();
                    }
                          break;
                    default:
                        break;
                    }

                }
                break;
                case BattleResult::BR_Lose: {
                    GameBonusRewardList[modeIndex][point].Lose_Reward1_Index = row[BonusReward_Field::BR_Reward1_Index].value<int>();
                    GameBonusRewardList[modeIndex][point].Lose_Reward1_Value = row[BonusReward_Field::BR_Reward1_Value].value<int>();
                    GameBonusRewardList[modeIndex][point].Lose_Reward2_Index = row[BonusReward_Field::BR_Reward2_Index].value<int>();
                    GameBonusRewardList[modeIndex][point].Lose_Reward2_Value = row[BonusReward_Field::BR_Reward2_Value].value<int>();
                    GameBonusRewardList[modeIndex][point].Lose_Reward3_Index = row[BonusReward_Field::BR_Reward3_Index].value<int>();
                    GameBonusRewardList[modeIndex][point].Lose_Reward3_Value = row[BonusReward_Field::BR_Reward3_Value].value<int>();
                }
                break;
                default: 
                    break;
                }
            }

            idx++;
        }
    }
    catch (const xlnt::exception& e) {
        std::cerr << "Excel File Load Fail: " << e.what() << std::endl;
    }
}

void TableManager::ReadGachaTable()
{
    try {
       
        mWorkbook.load("GameData/Gacha.xlsx");

        int rowIdx = 0;
        int startRow = 2;

        // Random Box Info
       mWorksheet = mWorkbook.sheet_by_index(0);
        for (auto row : mWorksheet.rows(false)) {
            if (rowIdx < startRow) {
                rowIdx++;
                continue;
            }

            if (!row.empty()) {
                int index = row[static_cast<int>(RandomBox_Field::index)].value<int>();
                RandomBoxList[index]
                    =  RandomBox{
                    index,
                    (std::string)row[static_cast<int>(RandomBox_Field::name)].to_string(),
                    row[static_cast<int>(RandomBox_Field::Gacha_Group)].value<int>(),
                    row[static_cast<int>(RandomBox_Field::Pay_Item_Index)].value<int>(),
                    row[static_cast<int>(RandomBox_Field::Pay_Item_Value)].value<int>(),
                    row[static_cast<int>(RandomBox_Field::Open_Date)].value<int>(),
                    row[static_cast<int>(RandomBox_Field::Close_Date)].value<int>(),
                };
            }
            rowIdx++;
        }

        // Gacha Items
        mWorksheet = mWorkbook.sheet_by_index(1);
        rowIdx = 0;
        for (auto row : mWorksheet.rows(false)) {
            if (rowIdx < startRow) {
                rowIdx++;
                continue;
            }

            if (!row.empty()) {
                int gachaGroup = row[static_cast<int>(GachaItem_Field::Gacha_Group)].value<int>();
                int index = row[static_cast<int>(GachaItem_Field::index)].value<int>();

                GachaItemList[gachaGroup][index]
                    = GachaItem{
                    index,
                    (std::string)row[static_cast<int>(GachaItem_Field::name)].to_string(),
                    row[static_cast<int>(GachaItem_Field::Gacha_Group)].value<int>(),
                    row[static_cast<int>(GachaItem_Field::Gacha_Weight)].value<int>(),
                    row[static_cast<int>(GachaItem_Field::Reward_Item_Index)].value<int>(),
                    row[static_cast<int>(GachaItem_Field::Reward_Item_Value)].value<int>(),
                };
            }
            rowIdx++;
        }
    }
    catch (const xlnt::exception& e) {
        std::cerr << "Gacha.xlsx Excel File Load Fail: " << e.what() << std::endl;
    }
}
