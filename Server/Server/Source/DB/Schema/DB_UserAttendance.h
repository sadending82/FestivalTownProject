#pragma once
#pragma pack (push, 1)

enum class UserAttendance_Field {
	user_UID = 1,
	attendance_date,
	event_index,
	day_count,
	is_rewarded,
};


struct UserAttendance {
	int user_UID = 0;
	std::tm	attendance_date;
	int	event_index = 0;
	int	day_count = 0;
	int	is_rewarded = 0;
};

struct sDayAttendanceInfo {
	int day_number = 0;
	std::tm	attendance_date = {};
	bool is_rewarded = false;
};

#pragma pack(pop) 