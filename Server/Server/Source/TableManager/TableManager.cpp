#define _WINSOCKAPI_
#include <windows.h>  
// 이거 안쓰면 Flatbuffers 관련 헤더랑 충돌 일어남
#undef max
#undef min

#include <fstream>
#include <sstream>
#include <iomanip>
#include <chrono>
#include <ctime>
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
    GameModeDatas.clear();

    // map
    MapData.clear();

    for (auto& outer_pair : MapListByMode) {
        outer_pair.second.clear();
    }
    MapListByMode.clear();

    for (auto& outer_pair : MapThemeList) {
        outer_pair.second.clear();
    }
    MapThemeList.clear();

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

    // PassList
    for (auto& passList : PassList) {
        for (auto& passLevelList : passList.second.passLevelList) {
            passLevelList.second.clear();
        }
        passList.second.passLevelList.clear();
    }
    PassList.clear();
}

void TableManager::ReadAllDataTable()
{
    mIsLoading.store(true);

    ClearAllTable();

    ReadItemTable();
    ReadCharacterStat();
    ReadWeaponStat();
    ReadGameModeTable();
    ReadMapData();
    ReadPointConstantTable();
    ReadGachaTable();
    ReadGameReward();

    ReadMatchingTable();
    ReadEventTable();

    ReadSlangList();

    ReadPassList();

    mIsLoading.store(false);
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

                std::time_t openTime = static_cast<std::time_t>((row[(int)(Event_Main_Field::Open_Date)].value<double>() - 25569) * 86400);
                std::time_t closeTime = static_cast<std::time_t>((row[(int)(Event_Main_Field::Close_Date)].value<double>() - 25569) * 86400);

                std::tm openDate = {}, closeDate = {};

                localtime_s(&openDate, &openTime);
                localtime_s(&closeDate, &closeTime);

                ItemInfos[index] = ItemTable{
                    index,
                    row[(int)(ItemTable_Field::Name)].to_string(),
                    row[(int)(ItemTable_Field::File_Name)].to_string(),
                    (ItemType)row[(int)(ItemTable_Field::Item_Type)].value<int>(),
                    (ItemGrade)row[(int)(ItemTable_Field::Item_Grade)].value<int>(),
                    openDate,
                    closeDate
                };
            }

            idx++;
        }
    }
    catch (const xlnt::exception& e) {
        std::cerr << "ReadItemTable Excel File Load Fail: " << e.what() << std::endl;
    }
}

void TableManager::ReadCharacterStat()
{
    try {
       
        mWorkbook.load("GameData/Ch_Stat.xlsx");

        int idx = 0;
        mWorksheet = mWorkbook.sheet_by_index(Ch_Attack_Sheet);
        for (auto row : mWorksheet.rows(false)) {
            if (idx < 1) {
                idx++;
                continue;
            }

            if (!row.empty()) {
                AttackStats[(int)row[(int)(AttackStat_Field::Index)].value<int>()]
                    = AttackStat{
                    (int)row[(int)(AttackStat_Field::Index)].value<int>(),
                    (std::string)row[(int)(AttackStat_Field::Name)].to_string(),
                    (float)row[(int)(AttackStat_Field::Value)].value<float>(),
                    (float)row[(int)(AttackStat_Field::Attack_Speed)].value<float>(),
                    (float)row[(int)(AttackStat_Field::Attack_Range)].value<float>(),
                    (int)row[(int)(AttackStat_Field::Ch_StaminaConsume)].value<int>(),
                    (int)row[(int)(AttackStat_Field::Vanish_Stamina)].value<int>()
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
                MoveStats[(int)row[(int)(MoveStat_Field::Index)].value<int>()]
                    = MoveStat{
                    (int)row[(int)(MoveStat_Field::Index)].value<int>(),
                    (std::string)row[(int)(MoveStat_Field::Name)].to_string(),
                    (float)row[(int)(MoveStat_Field::Value)].value<float>(),
                    (int)row[(int)(MoveStat_Field::Ch_StaminaConsume)].value<int>(),
                    (int)row[(int)(MoveStat_Field::Ch_Stamina_recovery)].value<int>()
                };
            }
            idx++;
        }

        idx = 0;

        mWorksheet = mWorkbook.sheet_by_index(Ch_Stat_Sheet);
        for (auto row : mWorksheet.rows(false)) {
            if (idx < 1) {
                idx++;
                continue;
            }

            int charIndex = row[(int)(CharacterStat_Field::index)].value<int>();

            if (!row.empty()) {
                CharacterStats[charIndex]
                    = CharacterStat{
                    (int)row[(int)(CharacterStat_Field::index)].value<int>(),
                    (std::string)row[(int)(CharacterStat_Field::name)].to_string(),
                    (float)row[(int)(CharacterStat_Field::hp)].value<float>(),
                    (int)row[(int)(CharacterStat_Field::stamina)].value<int>(),
                    (float)row[(int)(CharacterStat_Field::strength)].value<float>(),
                    (int)row[(int)(CharacterStat_Field::speed)].value<int>()
                };

                for (const auto& pair : AttackStats) {
                    CharacterStats[charIndex].attackStats[pair.first] = pair.second;
                }

                for (const auto& pair : MoveStats) {
                    CharacterStats[charIndex].moveStats[pair.first] = pair.second;
                }
            }
            idx++;
        }

    }
    catch (const xlnt::exception& e) {
        std::cerr << "ReadCharacterStat Excel File Load Fail: " << e.what() << std::endl;
    }

}

void TableManager::ReadMapData()
{
    try {

        mWorkbook.load("GameData/Map.xlsx");

        int idx = 0;

        mWorksheet = mWorkbook.sheet_by_index(MapTheme_Sheet);

        for (auto row : mWorksheet.rows(false)) {
            if (idx == variableNameIdx) {
                idx++;
                continue;
            }

            if (!row.empty()) {
                int Map_Index = row[(int)(MapTheme_Field::Map_Index)].value<int>();
                int theme = row[(int)(MapTheme_Field::Map_Theme)].value<int>();

                MapThemeList[Map_Index].push_back(theme);
            }
            idx++;
        }

       idx = 0;

        mWorksheet = mWorkbook.sheet_by_index(MapData_Sheet);

        for (auto row : mWorksheet.rows(false)) {
            if (idx == variableNameIdx) {
                idx++;
                continue;
            }

            if (!row.empty()) {
                int Map_Index = row[(int)(MapData_Field::Map_Index)].value<int>();
                if (Map_Index != 0) {
                    int Mode_Index = row[(int)(MapData_Field::Mode_Index)].value<int>();
                    ReadMapStructure(Map_Index);
                    MapListByMode[(GameMode)Mode_Index].push_back(Map_Index);
                }
            }
            idx++;
        }
    }
    catch (const xlnt::exception& e) {
        std::cerr << "ReadMapData Excel File Load Fail: " << e.what() << std::endl;
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
                WeaponStats[(int)row[(int)(WeaponStat_Field::index)].value<int>()]
                    = WeaponStat{
                    (int)row[(int)(WeaponStat_Field::index)].value<int>(),
                    (std::string)row[(int)(WeaponStat_Field::name)].to_string(),
                    (int)row[(int)(WeaponStat_Field::Weapon_Power)].value<int>(),
                    (float)row[(int)(WeaponStat_Field::Weapon_Range)].value<float>(),
                    (float)row[(int)(WeaponStat_Field::Weapon_Speed)].value<float>(),
                    (int)row[(int)(WeaponStat_Field::Weapon_Type)].value<int>(),
                    (int)row[(int)(WeaponStat_Field::Weapon_StaminaConsume)].value<int>()
                };
            }
            idx++;
        }
    }
    catch (const xlnt::exception& e) {
        std::cerr << "ReadWeaponStat Excel File Load Fail: " << e.what() << std::endl;
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
                std::time_t openTime = static_cast<std::time_t>((row[(int)(GameModeOut_Field::Open_Date)].value<double>() - 25569) * 86400);
                std::time_t closeTime = static_cast<std::time_t>((row[(int)(GameModeOut_Field::Close_Date)].value<double>() - 25569) * 86400);

                std::tm openDate = {}, closeDate = {};

                localtime_s(&openDate, &openTime);
                localtime_s(&closeDate, &closeTime);

                int index = row[0].value<int>();
                int teamCount = row[(int)(GameModeOut_Field::Team_Count)].value<int>();

               GameModeOutDatas[(GameMode)index]
                    =  GameModeOutData{
                    row[(int)(GameModeOut_Field::Player_Count)].value<int>(),
                    teamCount,
                    row[(int)(GameModeOut_Field::Team_Color)].value<int>(),
                    row[(int)(GameModeOut_Field::Play_Map)].value<int>(),
                    openDate,
                    closeDate
                };
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
                int mapIndex = row[(int)(GameModeData_Field::Map_Index)].value<int>();
                GameMode modeIndex = (GameMode)row[(int)(GameModeData_Field::Mode_Index)].value<int>();

                GameModeDatas[mapIndex][modeIndex] = GameModeData{
                    row[(int)(GameModeData_Field::Play_Time)].value<int>(),
                row[(int)(GameModeData_Field::Player_Spawn_Time)].value<int>(),
                row[(int)(GameModeData_Field::Life_Count)].value<int>(),
                row[(int)(GameModeData_Field::Bomb_Damage)].value<int>(),
                row[(int)(GameModeData_Field::Bomb_Spawn_Count)].value<int>(),
                row[(int)(GameModeData_Field::Bomb_Spawn_Time)].value<int>(),
                row[(int)(GameModeData_Field::Bomb_Delay_Time)].value<int>(),
                row[(int)(GameModeData_Field::Weapon1_Spawn_Index)].value<int>(),
                row[(int)(GameModeData_Field::Weapon1_Spawn_Count)].value<int>(),
                row[(int)(GameModeData_Field::Weapon1_Spawn_Time)].value<int>(),
                row[(int)(GameModeData_Field::Weapon2_Spawn_Index)].value<int>(),
                row[(int)(GameModeData_Field::Weapon2_Spawn_Count)].value<int>(),
                row[(int)(GameModeData_Field::Weapon2_Spawn_Time)].value<int>(),
                row[(int)(GameModeData_Field::Block1_Spawn_Index)].value<int>(),
                row[(int)(GameModeData_Field::Block1_Spawn_Count)].value<int>(),
                row[(int)(GameModeData_Field::Block1_Spawn_Time)].value<int>(),
                row[(int)(GameModeData_Field::Block2_Spawn_Index)].value<int>(),
                row[(int)(GameModeData_Field::Block2_Spawn_Count)].value<int>(),
                row[(int)(GameModeData_Field::Block2_Spawn_Time)].value<int>(),
                row[(int)(GameModeData_Field::Ch_Groggy)].value<int>(),
                row[(int)(GameModeData_Field::Block_Hit)].value<int>() };
            }

            idx++;
        }
    }
    catch (const xlnt::exception& e) {
        std::cerr << "ReadGameModeTable Excel File Load Fail: " << e.what() << std::endl;
    }
}

void TableManager::ReadMapStructure(int mapIndex)
{
    std::string filePath = "GameData/Map/" + std::to_string(mapIndex) + ".txt";

    std::ifstream map1_inputFile(filePath);

    if (!map1_inputFile.is_open()) {
        std::cerr << "File Not Exist " << mapIndex << std::endl;
        return;
    }

    MapData[mapIndex].SetMapIndex(mapIndex);
    std::string line;
    bool canReadStructure = false;
    while (std::getline(map1_inputFile, line)) {
        std::vector<char> row;
        char character;
        std::istringstream iss(line);

        if (line == "MapStructure") {
            canReadStructure = true;
            continue;
        }

        if (canReadStructure == true) {
            while (iss >> character) {
                row.push_back(character);
            }

            MapData[mapIndex].GetStructure().push_back(row);
        }
    }
    std::reverse(MapData[mapIndex].GetStructure().begin(), MapData[mapIndex].GetStructure().end());

    int size = MapData[mapIndex].GetStructure().size();
    for (int lineCnt = 0; lineCnt < size; ++lineCnt) {
        std::vector<char>& row = MapData[mapIndex].GetStructure()[lineCnt];

        for (int colCnt = 0; colCnt < row.size(); ++colCnt) {
            char block = row[colCnt];
            switch ((int)block) {
            case FITH_MapBlock::FM_Normal: {
                MapData[mapIndex].GetBlockDropIndexes().push_back({ colCnt, lineCnt });
            }break;
            case FITH_MapBlock::FM_ObjectSpwan: {
                MapData[mapIndex].GetObjectSpawnIndexes().push_back({ colCnt, lineCnt });
            }break;
            case FITH_MapBlock::FM_TeamRedSpawn: {
                MapData[mapIndex].GetPlayerSpawnIndexes(TeamCode::Team_RED).push_back({ colCnt, lineCnt });
            }break;
            case FITH_MapBlock::FM_TeamBlueSpawn: {
                MapData[mapIndex].GetPlayerSpawnIndexes(TeamCode::Team_BLUE).push_back({ colCnt, lineCnt });
            }break;
            case FITH_MapBlock::FM_TeamGreenSpawn: {
                MapData[mapIndex].GetPlayerSpawnIndexes(TeamCode::Team_GREEN).push_back({ colCnt, lineCnt });
            }break;
            default: {

            }break;
            }
        }
    }
    map1_inputFile.close();
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

                GameMode modeIndex = (GameMode)row[(int)PointConstant_Field::Mode_Index].value<int>();
                BattleResult battleResult = (BattleResult)row[(int)PointConstant_Field::Battle_Result].value<int>();
                ConstantType type = (ConstantType)row[(int)PointConstant_Field::Type].value<int>();

                switch (type) {
                case ConstantType::Kill_Point: {
                    switch (battleResult) {
                    case BattleResult::BR_Win: {
                        PointConstantList[modeIndex].Win_Kill_Point.WorkType = row[(int)PointConstant_Field::WorkType].value<int>();
                        PointConstantList[modeIndex].Win_Kill_Point.Value = row[(int)PointConstant_Field::Value].value<int>();
                    }
                    break;
                    case BattleResult::BR_Lose: {
                        PointConstantList[modeIndex].Lose_Kill_Point.WorkType = row[(int)PointConstant_Field::WorkType].value<int>();
                        PointConstantList[modeIndex].Lose_Kill_Point.Value = row[(int)PointConstant_Field::Value].value<int>();
                    }
                    break;
                    case BattleResult::BR_Draw: {
                        PointConstantList[modeIndex].Draw_Kill_Point.WorkType = row[(int)PointConstant_Field::WorkType].value<int>();
                        PointConstantList[modeIndex].Draw_Kill_Point.Value = row[(int)PointConstant_Field::Value].value<int>();
                    }
                    break;
                    default:
                        break;
                    }
                }
                break;
                case ConstantType::Death_Point: {
                    switch (battleResult) {
                    case BattleResult::BR_Win: {
                        PointConstantList[modeIndex].Win_Death_Point.WorkType = row[(int)PointConstant_Field::WorkType].value<int>();
                        PointConstantList[modeIndex].Win_Death_Point.Value = row[(int)PointConstant_Field::Value].value<int>();
                    }
                                             break;
                    case BattleResult::BR_Lose: {
                        PointConstantList[modeIndex].Lose_Death_Point.WorkType = row[(int)PointConstant_Field::WorkType].value<int>();
                        PointConstantList[modeIndex].Lose_Death_Point.Value = row[(int)PointConstant_Field::Value].value<int>();
                    }
                                              break;
                    case BattleResult::BR_Draw: {
                        PointConstantList[modeIndex].Draw_Death_Point.WorkType = row[(int)PointConstant_Field::WorkType].value<int>();
                        PointConstantList[modeIndex].Draw_Death_Point.Value = row[(int)PointConstant_Field::Value].value<int>();
                    }
                                              break;
                    default:
                        break;
                    }
                }
                break;
                case ConstantType::Bomb_Point: {
                    switch (battleResult) {
                    case BattleResult::BR_Win: {
                        PointConstantList[modeIndex].Win_Bomb_Point.WorkType = row[(int)PointConstant_Field::WorkType].value<int>();
                        PointConstantList[modeIndex].Win_Bomb_Point.Value = row[(int)PointConstant_Field::Value].value<int>();
                    }
                                             break;
                    case BattleResult::BR_Lose: {
                        PointConstantList[modeIndex].Lose_Bomb_Point.WorkType = row[(int)PointConstant_Field::WorkType].value<int>();
                        PointConstantList[modeIndex].Lose_Bomb_Point.Value = row[(int)PointConstant_Field::Value].value<int>();
                    }
                                              break;
                    case BattleResult::BR_Draw: {
                        PointConstantList[modeIndex].Draw_Bomb_Point.WorkType = row[(int)PointConstant_Field::WorkType].value<int>();
                        PointConstantList[modeIndex].Draw_Bomb_Point.Value = row[(int)PointConstant_Field::Value].value<int>();
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
        std::cerr << "ReadPointConstantTable Excel File Load Fail: " << e.what() << std::endl;
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
                GameMode modeIndex = (GameMode)row[(int)Reward_Field::Mode_Index].value<int>();
                BattleResult battleResult = (BattleResult)row[(int)Reward_Field::Battle_Result].value<int>();

                switch (battleResult) {
                case BattleResult::BR_Win: {
                    GameRewardList[modeIndex].Win_Reward1_Index = row[(int)Reward_Field::Reward1_Index].value<int>();
                    GameRewardList[modeIndex].Win_Reward1_Value = row[(int)Reward_Field::Reward1_Value].value<int>();
                    GameRewardList[modeIndex].Win_Reward2_Index = row[(int)Reward_Field::Reward2_Index].value<int>();
                    GameRewardList[modeIndex].Win_Reward2_Value = row[(int)Reward_Field::Reward2_Value].value<int>();
                    GameRewardList[modeIndex].Win_Reward3_Index = row[(int)Reward_Field::Reward3_Index].value<int>();
                    GameRewardList[modeIndex].Win_Reward3_Value = row[(int)Reward_Field::Reward3_Value].value<int>();
                }
                break;
                case BattleResult::BR_Lose: {
                    GameRewardList[modeIndex].Lose_Reward1_Index = row[(int)Reward_Field::Reward1_Index].value<int>();
                    GameRewardList[modeIndex].Lose_Reward1_Value = row[(int)Reward_Field::Reward1_Value].value<int>();
                    GameRewardList[modeIndex].Lose_Reward2_Index = row[(int)Reward_Field::Reward2_Index].value<int>();
                    GameRewardList[modeIndex].Lose_Reward2_Value = row[(int)Reward_Field::Reward2_Value].value<int>();
                    GameRewardList[modeIndex].Lose_Reward3_Index = row[(int)Reward_Field::Reward3_Index].value<int>();
                    GameRewardList[modeIndex].Lose_Reward3_Value = row[(int)Reward_Field::Reward3_Value].value<int>();
                }
                break;
                case BattleResult::BR_Draw: {
                    GameRewardList[modeIndex].Draw_Reward1_Index = row[(int)Reward_Field::Reward1_Index].value<int>();
                    GameRewardList[modeIndex].Draw_Reward1_Value = row[(int)Reward_Field::Reward1_Value].value<int>();
                    GameRewardList[modeIndex].Draw_Reward2_Index = row[(int)Reward_Field::Reward2_Index].value<int>();
                    GameRewardList[modeIndex].Draw_Reward2_Value = row[(int)Reward_Field::Reward2_Value].value<int>();
                    GameRewardList[modeIndex].Draw_Reward3_Index = row[(int)Reward_Field::Reward3_Index].value<int>();
                    GameRewardList[modeIndex].Draw_Reward3_Value = row[(int)Reward_Field::Reward3_Value].value<int>();
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
                GameMode modeIndex = (GameMode)row[(int)BonusReward_Field::Mode_Index].value<int>();
                BattleResult battleResult = (BattleResult)row[(int)BonusReward_Field::Battle_Result].value<int>();
                int type = row[(int)BonusReward_Field::Type].value<int>();
                int point = row[(int)BonusReward_Field::Point].value<int>();

                switch (battleResult) {
                case BattleResult::BR_Win: {
                    switch (type) {
                    case 1: {
                        GameBonusRewardList[modeIndex][point].Win_Reward1_Index = row[(int)BonusReward_Field::Reward1_Index].value<int>();
                        GameBonusRewardList[modeIndex][point].Win_Reward1_Value = row[(int)BonusReward_Field::Reward1_Value].value<int>();
                        GameBonusRewardList[modeIndex][point].Win_Reward2_Index = row[(int)BonusReward_Field::Reward2_Index].value<int>();
                        GameBonusRewardList[modeIndex][point].Win_Reward2_Value = row[(int)BonusReward_Field::Reward2_Value].value<int>();
                        GameBonusRewardList[modeIndex][point].Win_Reward3_Index = row[(int)BonusReward_Field::Reward3_Index].value<int>();
                        GameBonusRewardList[modeIndex][point].Win_Reward3_Value = row[(int)BonusReward_Field::Reward3_Value].value<int>();
                    }
                    break;
                    case 2: {
                        GameBonusRewardList[modeIndex][point].MVP_Reward1_Index = row[(int)BonusReward_Field::Reward1_Index].value<int>();
                        GameBonusRewardList[modeIndex][point].MVP_Reward1_Value = row[(int)BonusReward_Field::Reward1_Value].value<int>();
                        GameBonusRewardList[modeIndex][point].MVP_Reward2_Index = row[(int)BonusReward_Field::Reward2_Index].value<int>();
                        GameBonusRewardList[modeIndex][point].MVP_Reward2_Value = row[(int)BonusReward_Field::Reward2_Value].value<int>();
                        GameBonusRewardList[modeIndex][point].MVP_Reward3_Index = row[(int)BonusReward_Field::Reward3_Index].value<int>();
                        GameBonusRewardList[modeIndex][point].MVP_Reward3_Value = row[(int)BonusReward_Field::Reward3_Value].value<int>();
                    }
                          break;
                    default:
                        break;
                    }

                }
                break;
                case BattleResult::BR_Lose: {
                    GameBonusRewardList[modeIndex][point].Lose_Reward1_Index = row[(int)BonusReward_Field::Reward1_Index].value<int>();
                    GameBonusRewardList[modeIndex][point].Lose_Reward1_Value = row[(int)BonusReward_Field::Reward1_Value].value<int>();
                    GameBonusRewardList[modeIndex][point].Lose_Reward2_Index = row[(int)BonusReward_Field::Reward2_Index].value<int>();
                    GameBonusRewardList[modeIndex][point].Lose_Reward2_Value = row[(int)BonusReward_Field::Reward2_Value].value<int>();
                    GameBonusRewardList[modeIndex][point].Lose_Reward3_Index = row[(int)BonusReward_Field::Reward3_Index].value<int>();
                    GameBonusRewardList[modeIndex][point].Lose_Reward3_Value = row[(int)BonusReward_Field::Reward3_Value].value<int>();
                }
                break;
                case BattleResult::BR_Draw: {
                    GameBonusRewardList[modeIndex][point].Draw_Reward1_Index = row[(int)BonusReward_Field::Reward1_Index].value<int>();
                    GameBonusRewardList[modeIndex][point].Draw_Reward1_Value = row[(int)BonusReward_Field::Reward1_Value].value<int>();
                    GameBonusRewardList[modeIndex][point].Draw_Reward2_Index = row[(int)BonusReward_Field::Reward2_Index].value<int>();
                    GameBonusRewardList[modeIndex][point].Draw_Reward2_Value = row[(int)BonusReward_Field::Reward2_Value].value<int>();
                    GameBonusRewardList[modeIndex][point].Draw_Reward3_Index = row[(int)BonusReward_Field::Reward3_Index].value<int>();
                    GameBonusRewardList[modeIndex][point].Draw_Reward3_Value = row[(int)BonusReward_Field::Reward3_Value].value<int>();
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
        std::cerr << "ReadGameReward Excel File Load Fail: " << e.what() << std::endl;
    }
}

void TableManager::ReadGachaTable()
{
    try {
       
        mWorkbook.load("GameData/Gacha.xlsx");

        int rowIdx = 0;
        int startRow = 1;

        // Random Box Info
        mWorksheet = mWorkbook.sheet_by_index(0);
        for (auto row : mWorksheet.rows(false)) {
            if (rowIdx < startRow) {
                rowIdx++;
                continue;
            }

            if (!row.empty()) {
                std::time_t openTime = static_cast<std::time_t>((row[(int)(RandomBox_Field::Open_Date)].value<double>() - 25569) * 86400);
                std::time_t closeTime = static_cast<std::time_t>((row[(int)(RandomBox_Field::Close_Date)].value<double>() - 25569) * 86400);

                std::tm openDate = {}, closeDate = {};

                localtime_s(&openDate, &openTime);
                localtime_s(&closeDate, &closeTime);

                int index = row[(int)(RandomBox_Field::index)].value<int>();
                RandomBoxList[index]
                    =  RandomBox{
                    index,
                    (std::string)row[(int)(RandomBox_Field::name)].to_string(),
                    row[(int)(RandomBox_Field::Gacha_Group)].value<int>(),
                    row[(int)(RandomBox_Field::Pay_Item_Index1)].value<int>(),
                    row[(int)(RandomBox_Field::Pay_Item_Value1)].value<int>(),
                     row[(int)(RandomBox_Field::Pay_Item_Index2)].value<int>(),
                    row[(int)(RandomBox_Field::Pay_Item_Value2)].value<int>(),
                    openDate,
                    closeDate
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
                int gachaGroup = row[(int)(GachaItem_Field::Gacha_Group)].value<int>();
                int index = row[(int)(GachaItem_Field::index)].value<int>();

                GachaItemList[gachaGroup][index]
                    = GachaItem{
                    index,
                    (std::string)row[(int)(GachaItem_Field::name)].to_string(),
                    row[(int)(GachaItem_Field::Gacha_Group)].value<int>(),
                    row[(int)(GachaItem_Field::Gacha_Weight)].value<int>(),
                    row[(int)(GachaItem_Field::Reward_Item_Index)].value<int>(),
                    row[(int)(GachaItem_Field::Reward_Item_Value)].value<int>(),
                };
            }
            rowIdx++;
        }

        // Acquired Mileages
        mWorksheet = mWorkbook.sheet_by_index(2);
        rowIdx = 0;
        for (auto row : mWorksheet.rows(false)) {
            if (rowIdx < startRow) {
                rowIdx++;
                continue;
            }

            if (!row.empty()) {
                int itemGrade = row[(int)(GachaWhenDup_Field::item_grade)].value<int>();
                int acquiredMileage = row[(int)(GachaWhenDup_Field::acquired_mileage)].value<int>();

                GachaAcquiredMileages[itemGrade] = acquiredMileage;
            }
            rowIdx++;
        }
    }
    catch (const xlnt::exception& e) {
        std::cerr << "ReadGachaTable Excel File Load Fail: " << e.what() << std::endl;
    }
}

void TableManager::ReadMatchingTable()
{
    try {

        mWorkbook.load("GameData/Mode_Ver3.xlsx");
        int idx = 0;
        mWorksheet = mWorkbook.sheet_by_index(Matching_Sheet);

        for (auto row : mWorksheet.rows(false)) {
            if (idx == variableNameIdx) {
                idx++;
                continue;
            }

            if (!row.empty()) {

                eMatchingType matchingMode = (eMatchingType)row[(int)(Matching_Field::Matching_Mode)].value<int>();

                MatchingDatas[matchingMode].push_back(
                    Matching_Table{
                        matchingMode,
                        row[(int)(Matching_Field::Matching_Min_Time)].value<int>(),
                        row[(int)(Matching_Field::Matching_Max_Time)].value<int>(),
                        row[(int)(Matching_Field::Mode1_Index)].value<int>(),
                        row[(int)(Matching_Field::Mode2_Index)].value<int>(),
                        row[(int)(Matching_Field::Recruit_Player)].value<int>()
                    }
                );
            }

            idx++;
        }
    }
    catch (const xlnt::exception& e) {
        std::cerr << "ReadMatchingTable Excel File Load Fail: " << e.what() << std::endl;
    }
}

void TableManager::ReadEventTable()
{
    try {

        mWorkbook.load("GameData/Event.xlsx");
        int idx = 0;
        mWorksheet = mWorkbook.sheet_by_index(Evnet_Main_Sheet);

        for (auto row : mWorksheet.rows(false)) {
            if (idx == variableNameIdx) {
                idx++;
                continue;
            }

            if (!row.empty()) {

                int index = row[(int)(Event_Main_Field::Event_ID)].value<int>();

                std::time_t openTime = static_cast<std::time_t>((row[(int)(Event_Main_Field::Open_Date)].value<double>()- 25569) * 86400);
                std::time_t closeTime = static_cast<std::time_t>((row[(int)(Event_Main_Field::Close_Date)].value<double>() - 25569) * 86400);

                std::tm openDate = {}, closeDate = {};

                localtime_s(&openDate, &openTime);
                localtime_s(&closeDate, &closeTime);

                EventList[index] =
                    Event_Main{
                        index,
                        (std::string)row[(int)(Event_Main_Field::Name)].to_string(),
                        row[(int)(Event_Main_Field::Type)].value<int>(),
                        openDate,
                        closeDate,
                };
            }

            idx++;
        }

        idx = 0;
        mWorksheet = mWorkbook.sheet_by_index(Evnet_List_Sheet);

        for (auto row : mWorksheet.rows(false)) {
            if (idx == variableNameIdx) {
                idx++;
                continue;
            }

            if (!row.empty()) {

                int index = row[(int)(Event_List_Field::Event_List)].value<int>();
                int day = row[(int)(Event_List_Field::Day)].value<int>();

                std::time_t openTime = static_cast<std::time_t>((row[(int)(Event_Main_Field::Open_Date)].value<double>() - 25569) * 86400);
                std::time_t closeTime = static_cast<std::time_t>((row[(int)(Event_Main_Field::Close_Date)].value<double>() - 25569) * 86400);

                std::tm openDate = {}, closeDate = {};

                localtime_s(&openDate, &openTime);
                localtime_s(&closeDate, &closeTime);

                EventRewardList[index][day] =
                    Event_List{
                        index,
                        row[(int)(Event_List_Field::Event_List)].value<int>(),
                        day,
                        row[(int)(Event_List_Field::Reward_Item_Index)].value<int>(),
                        row[(int)(Event_List_Field::Reward_Item_Value)].value<int>(),
                        openDate,
                        closeDate,
                };
            }

            idx++;
        }
    }
    catch (const xlnt::exception& e) {
        std::cerr << "ReadEventTable Excel File Load Fail: " << e.what() << std::endl;
    }
}

void TableManager::ReadSlangList()
{
    try {

        mWorkbook.load("GameData/SlangList.xlsx");
        int idx = 0;
        mWorksheet = mWorkbook.sheet_by_index(0);

        for (auto row : mWorksheet.rows(false)) {
            if (!row.empty()) {
                idx++;
                std::string slang = row[0].to_string();

                int wSlangLen = MultiByteToWideChar(CP_UTF8, 0, slang.c_str(), -1, NULL, 0);
                wchar_t* wSlang = new wchar_t[wSlangLen];
                MultiByteToWideChar(CP_UTF8, 0, slang.c_str(), -1, wSlang, wSlangLen);
                SlangList.Insert(wSlang);
            }
        }
    }
    catch (const xlnt::exception& e) {
        std::cerr << "ReadSlangList Excel File Load Fail: " << e.what() << std::endl;
    }
}

void TableManager::ReadPassList()
{
    try {

        mWorkbook.load("GameData/Pass.xlsx");

        int idx = 0;
        mWorksheet = mWorkbook.sheet_by_index(PassList_Sheet);

        for (auto row : mWorksheet.rows(false)) {
            if (idx == variableNameIdx) {
                idx++;
                continue;
            }

            if (!row.empty()) {
                int index = row[(int)(PassList_Field::index)].value<int>();
                int limit_exp_daily_mission = row[(int)(PassList_Field::limit_exp_daily_mission)].value<int>();
                int level_repeated_reward = row[(int)(PassList_Field::level_repeated_reward)].value<int>();
                std::time_t openTime = static_cast<std::time_t>((row[(int)(PassList_Field::open_date)].value<double>() - 25569) * 86400);
                std::time_t closeTime = static_cast<std::time_t>((row[(int)(PassList_Field::close_date)].value<double>() - 25569) * 86400);
                std::tm openDate = {}, closeDate = {};

                localtime_s(&openDate, &openTime);
                localtime_s(&closeDate, &closeTime);

                PassInfo passInfo;

                passInfo.index = index;
                passInfo.limit_exp_daily_mission = limit_exp_daily_mission;
                passInfo.level_repeated_reward = level_repeated_reward;
                passInfo.open_date = openDate;
                passInfo.close_date = closeDate;

                PassList[index] = passInfo;
            }

            idx++;
        }

        idx = 0;
        mWorksheet = mWorkbook.sheet_by_index(PassLevel_Sheet);

        for (auto row : mWorksheet.rows(false)) {
            if (idx == variableNameIdx) {
                idx++;
                continue;
            }

            if (!row.empty()) {

                int index = row[(int)(PassLevel_Field::index)].value<int>();
                int pass = row[(int)(PassLevel_Field::pass)].value<int>();
                int level = row[(int)(PassLevel_Field::level)].value<int>();
                int passType = row[(int)(PassLevel_Field::Pass_Type)].value<int>();

                PassLevel levelInfo;

                levelInfo.index = index;
                levelInfo.pass = pass;
                levelInfo.level = level;
                levelInfo.Exp_Required = row[(int)(PassLevel_Field::Exp_Required)].value<int>();
                levelInfo.Pass_Type = passType;
                levelInfo.Reward_Item_Index = row[(int)(PassLevel_Field::Reward_Item_Index)].value<int>();
                levelInfo.Reward_Item_Amount = row[(int)(PassLevel_Field::Reward_Item_Amount)].value<int>();

                PassList[pass].passLevelList[level][passType] = levelInfo;
            }

            idx++;
        }


        idx = 0;
        mWorksheet = mWorkbook.sheet_by_index(PassMission_Sheet);

        for (auto row : mWorksheet.rows(false)) {
            if (idx == variableNameIdx) {
                idx++;
                continue;
            }

            if (!row.empty()) {
                int index = row[(int)(PassMission_Field::index)].value<int>();
                int passIndex = row[(int)(PassMission_Field::pass_index)].value<int>();
                int type = row[(int)(PassMission_Field::type)].value<int>();
                int missionCategory = row[(int)(PassMission_Field::mission_category)].value<int>();
                int missionGroup = row[(int)(PassMission_Field::mission_group)].value<int>();
                int missionStep = row[(int)(PassMission_Field::mission_step)].value<int>();
                int required_item_grade = row[(int)(PassMission_Field::required_item_grade)].value<int>();
                int required_count = row[(int)(PassMission_Field::required_count)].value<int>();
                int reward_exp = row[(int)(PassMission_Field::reward_exp)].value<int>();
                int reward_item = row[(int)(PassMission_Field::reward_item)].value<int>();
                int reward_item_amount = row[(int)(PassMission_Field::reward_item_amount)].value<int>();

                PassMissionInfo missionInfo = {
                index, passIndex, type, missionCategory, missionGroup, missionStep, required_item_grade, required_count, reward_exp, reward_item, reward_item_amount
                };

                PassMissionIndexList[passIndex][type][missionCategory][missionGroup][missionStep] = index;
                PassMissionDataListByIndex[index] = missionInfo;
            }

            idx++;
        }
    }
    catch (const xlnt::exception& e) {
        std::cerr << "ReadPassList Excel File Load Fail: " << e.what() << std::endl;
    }
}

void TableManager::Lock()
{
    while (mLockFlag.test_and_set(std::memory_order_acquire)) {

    }
}

void TableManager::UnLock()
{
    mLockFlag.clear();
}

std::unordered_map<INDEX, ItemTable>& TableManager::GetItemInfos()
{
    while (mIsLoading.load() == true) {

    }
    return ItemInfos;
}

std::unordered_map<INDEX, CharacterStat>& TableManager::GetCharacterStats()
{
    while (mIsLoading.load() == true) {

    }
    return CharacterStats;
}

std::unordered_map<INDEX, WeaponStat>& TableManager::GetWeaponStats()
{
    while (mIsLoading.load() == true) {

    }
    return WeaponStats;
}

std::unordered_map<GameMode, GameModeOutData>& TableManager::GetGameModeOutData()
{
    while (mIsLoading.load() == true) {

    }
    return GameModeOutDatas;
}

std::unordered_map<int, std::unordered_map<GameMode, GameModeData>>& TableManager::GetGameModeData()
{
    while (mIsLoading.load() == true) {

    }
    return GameModeDatas;
}

std::unordered_map<int, Map>& TableManager::GetMapData()
{
    while (mIsLoading.load() == true) {

    }
    return MapData;
}

std::unordered_map<GameMode, PointConstants>& TableManager::GetPointConstantList()
{
    while (mIsLoading.load() == true) {

    }
    return PointConstantList;
}

std::unordered_map<GameMode, GameReward>& TableManager::GetGameRewardList()
{
    while (mIsLoading.load() == true) {

    }
    return GameRewardList;
}

std::unordered_map<GameMode, std::unordered_map<int, BonusReward>>& TableManager::GetGameBonusRewardList()
{
    while (mIsLoading.load() == true) {

    }
    return GameBonusRewardList;
}

std::unordered_map<INDEX, RandomBox>& TableManager::GetRandomBoxList()
{
    while (mIsLoading.load() == true) {

    }
    return RandomBoxList;
}

std::unordered_map<GACHA_GROUP, std::unordered_map<INDEX, GachaItem>>& TableManager::GetGachaItemList()
{
    while (mIsLoading.load() == true) {

    }
    return GachaItemList;
}

std::unordered_map<int, int>& TableManager::GetGachaAcquiredMileages()
{
    while (mIsLoading.load() == true) {

    }
    return GachaAcquiredMileages;
}

std::unordered_map<GameMode, std::vector<int>>& TableManager::getMapListByMode()
{
    while (mIsLoading.load() == true) {

    }
    return MapListByMode;
}

std::unordered_map<int, std::vector<int>>& TableManager::GetMapThemeList()
{
    while (mIsLoading.load() == true) {

    }
    return MapThemeList;
}

std::unordered_map<eMatchingType, std::vector<Matching_Table>>& TableManager::GetMatchingDatas()
{
    while (mIsLoading.load() == true) {

    }

    return MatchingDatas;
}

std::unordered_map<INDEX, Event_Main>& TableManager::GetEventList()
{
    while (mIsLoading.load() == true) {

    }

    return EventList;
}

std::unordered_map<INDEX, std::unordered_map<int, Event_List>>& TableManager::GetEventRewardList()
{
    while (mIsLoading.load() == true) {

    }

    return EventRewardList;
}

std::unordered_map<int, PassInfo>& TableManager::GetPassList()
{
    while (mIsLoading.load() == true) {

    }

    return PassList;
}

PASS_MISSION_LIST& TableManager::GetPassMissionIndexList()
{
    while (mIsLoading.load() == true) {

    }

    return PassMissionIndexList;
}

std::unordered_map<int, PassMissionInfo>& TableManager::GetPassMissionDataListByIndex()
{
    while (mIsLoading.load() == true) {

    }

    return PassMissionDataListByIndex;
}

Trie& TableManager::GetSlangList()
{
    while (mIsLoading.load() == true) {

    }

    return SlangList;
}
