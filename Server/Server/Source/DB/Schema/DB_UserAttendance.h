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


	// order by attendance_date
	bool operator == (const sDayAttendanceInfo& other) const {
		if (attendance_date.tm_year == other.attendance_date.tm_year && attendance_date.tm_mon == other.attendance_date.tm_mon && attendance_date.tm_mday == other.attendance_date.tm_mday) {
			return true;
		}
		return false;
	}

	bool operator < (const sDayAttendanceInfo& other) const {
		if (attendance_date.tm_year != other.attendance_date.tm_year) return attendance_date.tm_year < other.attendance_date.tm_year;
		if (attendance_date.tm_mon != other.attendance_date.tm_mon) return attendance_date.tm_mon < other.attendance_date.tm_mon;
		return attendance_date.tm_mday < other.attendance_date.tm_mday;
	}
};

#pragma pack(pop) 