#include <fstream>
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
    for (auto& pair : ItemInfos) {
        delete pair.second;
    }
    ItemInfos.clear();

    for (auto& pair : CharacterStats) {
        delete pair.second;
    }
    CharacterStats.clear();

    for (auto& pair : WeaponStats) {
        delete pair.second;
    }
    WeaponStats.clear();

    for (auto& pair : GameModeData) {
        delete pair.second;
    }
    GameModeData.clear();

    for (auto& pair : MapData) {
        delete pair.second;
    }
    MapData.clear();

    for (auto& outer_pair : ScoreConstantList) {
        for (auto& inner_pair : outer_pair.second) {
            for (auto& pair : inner_pair.second) {
                delete pair.second;
            }
        }
        outer_pair.second.clear();
    }
    ScoreConstantList.clear();

    for (auto& pair : RandomBoxList) {
        delete pair.second;
    }
    RandomBoxList.clear();

    for (auto& outer_pair : GachaItemList) {
        for (auto& inner_pair : outer_pair.second) {
            delete inner_pair.second;
        }
        outer_pair.second.clear();
    }
    GachaItemList.clear();
}

void TableManager::ReadAllDataTable()
{
    //ReadItemTable();
    ReadCharacterStat();
    ReadWeaponStat();
    ReadGameModeTable();
    ReadMapData();
    ReadScoreConstantTable();
    ReadGachaTable();
}

void TableManager::ReadItemTable()
{
    try {
        xlnt::workbook wb;
        wb.load("GameData/ItemTable.xlsx");
        xlnt::worksheet ws = wb.active_sheet();

        int idx = 0;

        for (auto row : ws.rows(false)) {
            if (idx == 0) {
                idx++;
                continue;
            }

            int uid = 0;
            std::string name = "";
            int price = 0;

            if (!row.empty()) {
                uid = (int)row[0].value<int>();
                name = (std::string)row[1].to_string();
                price = (int)row[2].value<int>();
            }
            ItemInfos[uid] = new ItemTable(uid, name, price);

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
        xlnt::workbook wb;
        wb.load("GameData/Character_Stat.xlsx");

        int idx = 0;
        int sheetIdx = 1;

        xlnt::worksheet ws = wb.sheet_by_index(sheetIdx);

        for (auto row : ws.rows(false)) {
            if (idx < 2) {
                idx++;
                continue;
            }

            if (!row.empty()) {
                CharacterStats[(int)row[static_cast<int>(CharacterStat_Field::index)].value<int>()]
                    = new CharacterStat{
                    (int)row[static_cast<int>(CharacterStat_Field::index)].value<int>(),
                    (std::string)row[static_cast<int>(CharacterStat_Field::name)].to_string(),  
                    (int)row[static_cast<int>(CharacterStat_Field::hp)].value<int>(), 
                    (int)row[static_cast<int>(CharacterStat_Field::stamina)].value<int>(), 
                    (int)row[static_cast<int>(CharacterStat_Field::strength)].value<int>(), 
                    (int)row[static_cast<int>(CharacterStat_Field::speed)].value<int>(), 
                    (int)row[static_cast<int>(CharacterStat_Field::attack)].value<int>(),
                    (int)row[static_cast<int>(CharacterStat_Field::headAttack)].value<int>(),
                    (int)row[static_cast<int>(CharacterStat_Field::jumpKick)].value<int>(),
                    (int)row[static_cast<int>(CharacterStat_Field::powerAttack)].value<int>(), 
                    (int)row[static_cast<int>(CharacterStat_Field::walkSpeed)].value<int>(), 
                    (int)row[static_cast<int>(CharacterStat_Field::runSpeed)].value<int>(), 
                    (int)row[static_cast<int>(CharacterStat_Field::rollSpeed)].value<int>(),
                    (int)row[static_cast<int>(CharacterStat_Field::staminaRecovery)].value<int>()
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
        xlnt::workbook wb;
        wb.load("GameData/Weapon_Stat.xlsx");

        int idx = 0;

        xlnt::worksheet ws = wb.active_sheet();

        for (auto row : ws.rows(false)) {
            if (idx < 1) {
                idx++;
                continue;
            }

            if (!row.empty()) {
                WeaponStats[(int)row[static_cast<int>(WeaponStat_Field::index)].value<int>()]
                    = new WeaponStat{
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
        xlnt::workbook wb;
        wb.load("GameData/Mode_Ver3.xlsx");

        int idx = 0;
        xlnt::worksheet ws = wb.sheet_by_index(Mode_Out_Sheet);

        for (auto row : ws.rows(false)) {
            if (idx == variableNameIdx) {
                idx++;
                continue;
            }

            if (!row.empty()) {
                int index = row[0].value<int>();

               GameModeData[(GameMode)index]
                    = new GameModeInfo{
                    row[GameModeOut_Field::Player_Count].value<int>(),
                    row[GameModeOut_Field::Team_Count].value<int>(),
                    row[GameModeOut_Field::Team_Color].value<int>(),
                    row[GameModeOut_Field::Play_Map].value<int>()
                };
            }

            idx++;
        }

        idx = 0;
        ws = wb.sheet_by_index(FITH_Mode_Sheet);

        for (auto row : ws.rows(false)) {
            if (idx == variableNameIdx) {
                idx++;
                continue;
            }

            if (!row.empty()) {
                int index = row[0].value<int>();

                if (GameModeData.find((GameMode)index) == GameModeData.end()) {
                    continue;
                }
                GameModeData[(GameMode)index]->Play_Time = row[GameModeInfo_Field::GM_Play_Time].value<int>();
                GameModeData[(GameMode)index]->Player_Spawn_Time = row[GameModeInfo_Field::GM_Player_Spawn_Time].value<int>();
                GameModeData[(GameMode)index]->Life_Count = row[GameModeInfo_Field::GM_Life_Count].value<int>();
                GameModeData[(GameMode)index]->Bomb_Spawn_Count = row[GameModeInfo_Field::GM_Bomb_Spawn_Count].value<int>();
                GameModeData[(GameMode)index]->Bomb_Spawn_Time = row[GameModeInfo_Field::GM_Bomb_Spawn_Time].value<int>();
                GameModeData[(GameMode)index]->Bomb_Delay_Time = row[GameModeInfo_Field::GM_Bomb_Delay_Time].value<int>();
                GameModeData[(GameMode)index]->Weapon1_Spawn_Index = row[GameModeInfo_Field::GM_Weapon1_Spawn_Index].value<int>();
                GameModeData[(GameMode)index]->Weapon1_Spawn_Count = row[GameModeInfo_Field::GM_Weapon1_Spawn_Count].value<int>();
                GameModeData[(GameMode)index]->Weapon1_Spawn_Time = row[GameModeInfo_Field::GM_Weapon1_Spawn_Time].value<int>();
                GameModeData[(GameMode)index]->Weapon2_Spawn_Index = row[GameModeInfo_Field::GM_Weapon2_Spawn_Index].value<int>();
                GameModeData[(GameMode)index]->Weapon2_Spawn_Count = row[GameModeInfo_Field::GM_Weapon2_Spawn_Count].value<int>();
                GameModeData[(GameMode)index]->Weapon2_Spawn_Time = row[GameModeInfo_Field::GM_Weapon2_Spawn_Time].value<int>();
                GameModeData[(GameMode)index]->Block1_Spawn_Index = row[GameModeInfo_Field::GM_Block1_Spawn_Index].value<int>();
                GameModeData[(GameMode)index]->Block1_Spawn_Count = row[GameModeInfo_Field::GM_Block1_Spawn_Count].value<int>();
                GameModeData[(GameMode)index]->Block1_Spawn_Time = row[GameModeInfo_Field::GM_Block1_Spawn_Time].value<int>();
                GameModeData[(GameMode)index]->Block2_Spawn_Index = row[GameModeInfo_Field::GM_Block2_Spawn_Index].value<int>();
                GameModeData[(GameMode)index]->Block2_Spawn_Count = row[GameModeInfo_Field::GM_Block2_Spawn_Count].value<int>();
                GameModeData[(GameMode)index]->Block2_Spawn_Time = row[GameModeInfo_Field::GM_Block2_Spawn_Time].value<int>();
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
    MapData[MapCode::TEST] = new Map;
    while (std::getline(inputFile, line)) {
        std::vector<char> row;
        char character;
        std::istringstream iss(line);

        int colCnt = 0;
        while (iss >> character) {
            row.push_back(character);

            switch (character) {
            case 'n': {
                MapData[MapCode::TEST]->GetBlockDropIndexes().push_back({ colCnt, lineCnt });
            }break;
            case 'b': {
                MapData[MapCode::TEST]->GetObjectSpawnIndexes().push_back({ colCnt, lineCnt });
                if (colCnt < 10) {
                    MapData[MapCode::TEST]->GetRedObjectSpawnIndexes().push_back({ colCnt, lineCnt });
                }
                else {
                    MapData[MapCode::TEST]->GetBlueObjectSpawnIndexes().push_back({ colCnt, lineCnt });
                }
            }break;
            case 'R': {
                MapData[MapCode::TEST]->GetPlayerSpawnIndexes((int)TeamCode::RED).push_back({colCnt, lineCnt});
            }break;
            case 'B': {
                MapData[MapCode::TEST]->GetPlayerSpawnIndexes((int)TeamCode::BLUE).push_back({ colCnt, lineCnt });
            }break;
            default: {

            }break;
            }

            colCnt++;
        }

        MapData[MapCode::TEST]->GetStructure().push_back(row);
        lineCnt++;
    }

    inputFile.close();
}

void TableManager::ReadScoreConstantTable()
{
    try {

        xlnt::workbook wb;
        wb.load("GameData/Mode_Ver3.xlsx");

        int idx = 0;

        xlnt::worksheet ws = wb.sheet_by_index(Mode_Point);

        for (auto row : ws.rows(false)) {
            if (idx == variableNameIdx) {
                idx++;
                continue;
            }
            if (!row.empty()) {

                GameMode modeIndex = (GameMode)row[ScoreConstant_Field::SC_Mode_Index].value<int>();
                BattleResult battleResult = (BattleResult)row[ScoreConstant_Field::SC_Battle_Result].value<int>();
                ConstantType type = (ConstantType)row[ScoreConstant_Field::SC_Type].value<int>();

                ScoreConstantList[modeIndex][battleResult][type] = new ScoreConstant{
                    row[type].value<int>(),
                    row[ScoreConstant_Field::SC_WorkType].value<int>(),
                    row[ScoreConstant_Field::SC_Value].value<int>()
                };
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
        xlnt::workbook wb;
        wb.load("GameData/Gacha.xlsx");

        int rowIdx = 0;
        int startRow = 2;

        // Random Box Info
        xlnt::worksheet ws = wb.sheet_by_index(0);
        for (auto row : ws.rows(false)) {
            if (rowIdx < startRow) {
                rowIdx++;
                continue;
            }

            if (!row.empty()) {
                int index = row[static_cast<int>(RandomBox_Field::index)].value<int>();
                RandomBoxList[index]
                    = new RandomBox{
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
        ws = wb.sheet_by_index(1);
        rowIdx = 0;
        for (auto row : ws.rows(false)) {
            if (rowIdx < startRow) {
                rowIdx++;
                continue;
            }

            if (!row.empty()) {
                int gachaGroup = row[static_cast<int>(GachaItem_Field::Gacha_Group)].value<int>();
                int index = row[static_cast<int>(GachaItem_Field::index)].value<int>();

                GachaItemList[gachaGroup][index]
                    = new GachaItem{
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
