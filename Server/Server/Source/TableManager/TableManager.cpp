#include <fstream>
#include "TableManager.h"

TableManager::TableManager()
{
    ItemInfos.clear();
}

TableManager::~TableManager()
{

}

void TableManager::ClearAllTable()
{
    ItemInfos.clear();
}

void TableManager::ReadAllDataTable()
{
    //ReadItemTable();
    //ReadCharacterStat();
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
            ItemInfos[uid] = ItemTable(uid, name, price);

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
                CharacterStats[(int)row[0].value<int>()]
                    = CharacterStat{
                    (int)row[0].value<int>(),   // index
                    (std::string)row[1].to_string(),         // name
                    (int)row[2].value<int>(),   // hp
                    (int)row[3].value<int>(),   // stamina
                    (int)row[4].value<int>(),   // strength
                    (int)row[5].value<int>(),   // speed
                    (int)row[6].value<int>(),   // attack
                    (int)row[7].value<int>(),   // headAttack
                    (int)row[8].value<int>(),   // jumpKick
                    (int)row[9].value<int>(),   // powerAttack
                    (int)row[10].value<int>(),  // walkSpeed
                    (int)row[11].value<int>(),  // runSpeed
                    (int)row[12].value<int>(),  // rollSpeed
                };
            }

            idx++;
        }
    }
    catch (const xlnt::exception& e) {
        std::cerr << "Excel File Load Fail: " << e.what() << std::endl;
    }

}

void TableManager::ReadFITHModeTable()
{
    try {
        xlnt::workbook wb;
        wb.load("GameData/FITH_Mode.xlsx");

        int idx = 0;
        int sheetIdx = 1;

        for (int i = GameCode::FITH_Team_battle_One; i <= GameCode::FITH_Team_battle_Three; ++i) {
            xlnt::worksheet ws = wb.sheet_by_index(i);

            for (auto row : ws.rows(false)) {
                if (idx == variableNameIdx) {
                    idx++;
                    continue;
                }

                if (!row.empty()) {
                    FITH_Data[(GameCode)i]
                        = FITH{
                        row[FITH_Field::Play_Time].value<int>(),
                        row[FITH_Field::Player_Spawn_Time].value<int>(),
                        row[FITH_Field::Team_Life_Count].value<int>(),
                        row[FITH_Field::Bomb_Spawn_Count].value<int>(),
                        row[FITH_Field::Bomb_Spawn_Time].value<int>(),
                        row[FITH_Field::Bomb_Delay_Time].value<int>(),
                        row[FITH_Field::Bomb_Spawn_Location_MinX].value<float>(),
                        row[FITH_Field::Bomb_Spawn_Location_MaxX].value<float>(),
                        row[FITH_Field::Bomb_Spawn_Location_MinY].value<float>(),
                        row[FITH_Field::Bomb_Spawn_Location_MaxY].value<float>(),
                        row[FITH_Field::Weapon_Spawn_Time].value<int>(),
                        row[FITH_Field::Weapon_Spawn_Count].value<int>(),
                        row[FITH_Field::Block_Spawn_Count].value<int>(),
                        row[FITH_Field::Block_Spawn_Time].value<int>(),
                        row[FITH_Field::Block_Spawn_Location_MinX].value<float>(),
                        row[FITH_Field::Block_Spawn_Location_MaxX].value<float>(),
                        row[FITH_Field::Block_Spawn_Location_MinY].value<float>(),
                        row[FITH_Field::Block_Spawn_Location_MaxY].value<float>(),
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
                MapData[MapCode::TEST].GetBombSpawnIndexes().push_back({ colCnt, lineCnt });
            }break;
            case 'R': {
                MapData[MapCode::TEST].GetPlayerSpawnIndexes(TeamCode::RED).push_back({colCnt, lineCnt});
            }break;
            case 'B': {
                MapData[MapCode::TEST].GetPlayerSpawnIndexes(TeamCode::BLUE).push_back({ colCnt, lineCnt });
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
