#pragma once
#include <ctime>

#pragma pack (push, 1)

#define Shop_Category_Sheet 0
#define Shop_List_Sheet 1

enum class Shop_Category_Field {
	index = 0,
	Category_Name,
	Category_Description,
	Number_Goods,
	Number_Discountable,
	Reroll_Term,
	open_date,
	close_date
};

enum class Shop_List_Field {
	index = 0,
	Category_Index,
	Item,
	Item_Grade,
	Item_Amount,
	Currency_ID,
	Price,
	Currency2_ID,
	Price2,
	Purchase_Limit,
	Discount_Rate_Min,
	Discount_Rate_Max,
	open_date,
	close_date
};

struct Shop_Goods {
	int index = 0;
	int Category_Index = 0;
	int Item = 0;
	int Item_Grade = 0;
	int Item_Amount = 0;
	int Currency_ID = 0;
	int Price = 0;
	int Currency2_ID = 0;
	int	Price2 = 0;
	int	Purchase_Limit = 0;
	int	Discount_Rate_Min = 0;
	int	Discount_Rate_Max = 0;
	std::tm	open_date = {};
	std::tm close_date = {};

};

struct Shop_Categoty {
	int index = 0;
	int	Number_Goods = 0;
	int	Number_Discountable = 0;
	int	Reroll_Term = 0;
	std::tm	open_date = {};
	std::tm close_date = {};

	std::unordered_map<int, Shop_Goods> goods;
};
#pragma pack(pop) 