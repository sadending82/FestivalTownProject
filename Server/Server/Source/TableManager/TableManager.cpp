#include <fstream>
#include <sstream>
#include <iomanip>
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
    GameModeDatas.clear();

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
    mIsLoading = true;

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

    mIsLoading = false;
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

                std::tm openDate = {}, closeDate = {};
                std::istringstream ssOpenDate(row[(int)(ItemTable_Field::Item_Type)].to_string());
                std::istringstream ssCloseDate(row[(int)(ItemTable_Field::Close_Date)].to_string());
                ssOpenDate >> std::get_time(&openDate, "%Y-%m-%d");
                ssCloseDate >> std::get_time(&closeDate, "%Y-%m-%d");

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
        std::cerr << "Excel File Load Fail: " << e.what() << std::endl;
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
                std::tm openDate = {}, closeDate = {};
                std::istringstream ssOpenDate(row[(int)(GameModeOut_Field::Open_Date)].to_string());
                std::istringstream ssCloseDate(row[(int)(GameModeOut_Field::Close_Date)].to_string());
                ssOpenDate >> std::get_time(&openDate, "%Y-%m-%d");
                ssCloseDate >> std::get_time(&closeDate, "%Y-%m-%d");

                int index = row[0].value<int>();

               GameModeDatas[(GameMode)index]
                    =  GameModeData{
                    row[(int)(GameModeOut_Field::Player_Count)].value<int>(),
                    row[(int)(GameModeOut_Field::Team_Count)].value<int>(),
                    row[(int)(GameModeOut_Field::Team_Color)].value<int>(),
                    row[(int)(GameModeOut_Field::Play_Map)].value<int>(),
                    openDate,
                    closeDate
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

                if (GameModeDatas.find((GameMode)index) == GameModeDatas.end()) {
                    continue;
                }
                GameModeDatas[(GameMode)index].Play_Time = row[(int)(GameModeData_Field::Play_Time)].value<int>();
                GameModeDatas[(GameMode)index].Player_Spawn_Time = row[(int)(GameModeData_Field::Player_Spawn_Time)].value<int>();
                GameModeDatas[(GameMode)index].Life_Count = row[(int)(GameModeData_Field::Life_Count)].value<int>();
                GameModeDatas[(GameMode)index].Bomb_Spawn_Count = row[(int)(GameModeData_Field::Bomb_Spawn_Count)].value<int>();
                GameModeDatas[(GameMode)index].Bomb_Spawn_Time = row[(int)(GameModeData_Field::Bomb_Spawn_Time)].value<int>();
                GameModeDatas[(GameMode)index].Bomb_Delay_Time = row[(int)(GameModeData_Field::Bomb_Delay_Time)].value<int>();
                GameModeDatas[(GameMode)index].Weapon1_Spawn_Index = row[(int)(GameModeData_Field::Weapon1_Spawn_Index)].value<int>();
                GameModeDatas[(GameMode)index].Weapon1_Spawn_Count = row[(int)(GameModeData_Field::Weapon1_Spawn_Count)].value<int>();
                GameModeDatas[(GameMode)index].Weapon1_Spawn_Time = row[(int)(GameModeData_Field::Weapon1_Spawn_Time)].value<int>();
                GameModeDatas[(GameMode)index].Weapon2_Spawn_Index = row[(int)(GameModeData_Field::Weapon2_Spawn_Index)].value<int>();
                GameModeDatas[(GameMode)index].Weapon2_Spawn_Count = row[(int)(GameModeData_Field::Weapon2_Spawn_Count)].value<int>();
                GameModeDatas[(GameMode)index].Weapon2_Spawn_Time = row[(int)(GameModeData_Field::Weapon2_Spawn_Time)].value<int>();
                GameModeDatas[(GameMode)index].Block1_Spawn_Index = row[(int)(GameModeData_Field::Block1_Spawn_Index)].value<int>();
                GameModeDatas[(GameMode)index].Block1_Spawn_Count = row[(int)(GameModeData_Field::Block1_Spawn_Count)].value<int>();
                GameModeDatas[(GameMode)index].Block1_Spawn_Time = row[(int)(GameModeData_Field::Block1_Spawn_Time)].value<int>();
                GameModeDatas[(GameMode)index].Block2_Spawn_Index = row[(int)(GameModeData_Field::Block2_Spawn_Index)].value<int>();
                GameModeDatas[(GameMode)index].Block2_Spawn_Count = row[(int)(GameModeData_Field::Block2_Spawn_Count)].value<int>();
                GameModeDatas[(GameMode)index].Block2_Spawn_Time = row[(int)(GameModeData_Field::Block2_Spawn_Time)].value<int>();
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
        int startRow = 1;

        // Random Box Info
        mWorksheet = mWorkbook.sheet_by_index(0);
        for (auto row : mWorksheet.rows(false)) {
            if (rowIdx < startRow) {
                rowIdx++;
                continue;
            }

            if (!row.empty()) {
                std::tm openDate = {}, closeDate = {};
                std::istringstream ssOpenDate(row[(int)(RandomBox_Field::Open_Date)].to_string());
                std::istringstream ssCloseDate(row[(int)(RandomBox_Field::Close_Date)].to_string());
                ssOpenDate >> std::get_time(&openDate, "%Y-%m-%d");
                ssCloseDate >> std::get_time(&closeDate, "%Y-%m-%d");

                int index = row[(int)(RandomBox_Field::index)].value<int>();
                RandomBoxList[index]
                    =  RandomBox{
                    index,
                    (std::string)row[(int)(RandomBox_Field::name)].to_string(),
                    row[(int)(RandomBox_Field::Gacha_Group)].value<int>(),
                    row[(int)(RandomBox_Field::Pay_Item_Index)].value<int>(),
                    row[(int)(RandomBox_Field::Pay_Item_Value)].value<int>(),
                    openDate,
                    closeDate,
                    row[(int)(RandomBox_Field::Exp)].value<int>()
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
    }
    catch (const xlnt::exception& e) {
        std::cerr << "Gacha.xlsx Excel File Load Fail: " << e.what() << std::endl;
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
        std::cerr << "Excel File Load Fail: " << e.what() << std::endl;
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
    while (mIsLoading == true) {

    }
    return ItemInfos;
}

std::unordered_map<INDEX, CharacterStat>& TableManager::GetCharacterStats()
{
    while (mIsLoading == true) {

    }
    return CharacterStats;
}

std::unordered_map<INDEX, WeaponStat>& TableManager::GetWeaponStats()
{
    while (mIsLoading == true) {

    }
    return WeaponStats;
}

std::unordered_map<GameMode, GameModeData>& TableManager::GetGameModeData()
{
    while (mIsLoading == true) {

    }
    return GameModeDatas;
}

std::unordered_map<MapCode, Map>& TableManager::GetMapData()
{
    while (mIsLoading == true) {

    }
    return MapData;
}

std::unordered_map<GameMode, PointConstants>& TableManager::GetPointConstantList()
{
    while (mIsLoading == true) {

    }
    return PointConstantList;
}

std::unordered_map<GameMode, GameReward>& TableManager::GetGameRewardList()
{
    while (mIsLoading == true) {

    }
    return GameRewardList;
}

std::unordered_map<GameMode, std::unordered_map<int, BonusReward>>& TableManager::GetGameBonusRewardList()
{
    while (mIsLoading == true) {

    }
    return GameBonusRewardList;
}

std::unordered_map<INDEX, RandomBox>& TableManager::GetRandomBoxList()
{
    while (mIsLoading == true) {

    }
    return RandomBoxList;
}

std::unordered_map<GACHA_GROUP, std::unordered_map<INDEX, GachaItem>>& TableManager::GetGachaItemList()
{
    while (mIsLoading == true) {

    }
    return GachaItemList;
}

std::unordered_map<GameMode, std::vector<MapCode>>& TableManager::getMapListByMode()
{
    while (mIsLoading == true) {

    }
    return MapListByMode;
}

std::unordered_map<eMatchingType, std::vector<Matching_Table>>& TableManager::GetMatchingDatas()
{
    while (mIsLoading == true) {

    }

    return MatchingDatas;
}
