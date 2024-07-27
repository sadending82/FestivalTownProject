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

void TableManager::ReadItemTable()
{
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

void TableManager::ReadCharacterStat()
{
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
