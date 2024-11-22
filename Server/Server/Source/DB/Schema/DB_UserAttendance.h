#pragma once
#pragma pack (push, 1)

enum class UserItem_Field {
	user_UID = 1,
	attendance_date,
	event_index,
	day_count,
	is_rewarded,
};


struct UserItem {
	int user_UID = 0;
	int	attendance_date = 0;
	int	event_index = 0;
	int	day_count = 0;
	int	is_rewarded = 0;
};


#pragma pack(pop) 