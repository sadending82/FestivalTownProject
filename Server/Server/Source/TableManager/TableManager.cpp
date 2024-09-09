#include <fstream>
#include "TableManager.h"

TableManager::TableManager()
{
    ItemInfos.clear();
}

TableManager::~TableManager()
{
    for (auto& pair : ItemInfos) {
        delete pair.second;
    }
    for (auto& pair : CharacterStats) {
        delete pair.second;
    }
    for (auto& pair : WeaponStats) {
        delete pair.second;
    }
    for (auto& pair : FITH_Data) {
        delete pair.second;
    }
    for (auto& pair : MapData) {
        delete pair.second;
    }
}

void TableManager::ClearAllTable()
{
    for (auto& pair : ItemInfos) {
        delete pair.second;
    }
    for (auto& pair : CharacterStats) {
        delete pair.second;
    }
    for (auto& pair : WeaponStats) {
        delete pair.second;
    }
    for (auto& pair : FITH_Data) {
        delete pair.second;
    }
    for (auto& pair : MapData) {
        delete pair.second;
    }

    ItemInfos.clear();
    CharacterStats.clear();
    WeaponStats.clear();
    FITH_Data.clear();
    MapData.clear();
}

void TableManager::ReadAllDataTable()
{
    //ReadItemTable();
    ReadCharacterStat();
    ReadWeaponStat();
    ReadFITHModeTable();
    ReadMapData();
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
                    (int)row[static_cast<int>(CharacterStat_Field::rollSpeed)].value<int>()
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
            if (idx < 2) {
                idx++;
                continue;
            }

            if (!row.empty()) {
                WeaponStats[(int)row[static_cast<int>(WeaponStat_Field::index)].value<int>()]
                    = new WeaponStat{
                    (int)row[static_cast<int>(WeaponStat_Field::index)].value<int>(),
                    (std::string)row[static_cast<int>(WeaponStat_Field::name)].to_string(),
                    (int)row[static_cast<int>(WeaponStat_Field::Weapon_Power)].value<int>(),
                    (int)row[static_cast<int>(WeaponStat_Field::Weapon_Limit)].value<int>(),
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

void TableManager::ReadFITHModeTable()
{
    try {
        xlnt::workbook wb;
        wb.load("GameData/FITH_Mode.xlsx");

        int idx = 0;
        int sheetIdx = 1;

        for (int i = GameMode::FITH_Team_battle_One; i <= GameMode::FITH_Team_battle_Three; ++i) {
            MapListByMode[(GameMode)i].push_back(MapCode::TEST);
            xlnt::worksheet ws = wb.sheet_by_index(i);

            for (auto row : ws.rows(false)) {
                if (idx == variableNameIdx) {
                    idx++;
                    continue;
                }

                if (!row.empty()) {
                    FITH_Data[(GameMode)i]
                        = new FITH{
                        row[static_cast<int>(FITH_Field::Play_Time)].value<int>(),
                        row[static_cast<int>(FITH_Field::Player_Spawn_Time)].value<int>(),
                        row[static_cast<int>(FITH_Field::Team_Life_Count)].value<int>(),
                        row[static_cast<int>(FITH_Field::Bomb_Spawn_Count)].value<int>(),
                        row[static_cast<int>(FITH_Field::Bomb_Spawn_Time)].value<int>(),
                        row[static_cast<int>(FITH_Field::Bomb_Delay_Time)].value<int>(),
                        row[static_cast<int>(FITH_Field::Bomb_Spawn_Location_MinX)].value<float>(),
                        row[static_cast<int>(FITH_Field::Bomb_Spawn_Location_MaxX)].value<float>(),
                        row[static_cast<int>(FITH_Field::Bomb_Spawn_Location_MinY)].value<float>(),
                        row[static_cast<int>(FITH_Field::Bomb_Spawn_Location_MaxY)].value<float>(),
                        row[static_cast<int>(FITH_Field::Weapon_Spawn_Time)].value<int>(),
                        row[static_cast<int>(FITH_Field::Weapon_Spawn_Count)].value<int>(),
                        row[static_cast<int>(FITH_Field::Block_Spawn_Count)].value<int>(),
                        row[static_cast<int>(FITH_Field::Block_Spawn_Time)].value<int>(),
                        row[static_cast<int>(FITH_Field::Block_Spawn_Location_MinX)].value<float>(),
                        row[static_cast<int>(FITH_Field::Block_Spawn_Location_MaxX)].value<float>(),
                        row[static_cast<int>(FITH_Field::Block_Spawn_Location_MinY)].value<float>(),
                        row[static_cast<int>(FITH_Field::Block_Spawn_Location_MaxY)].value<float>(),
                    };
                }

                idx++;
            }
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
