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
