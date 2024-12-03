#pragma once

//INSERT INTO AccountDB.dbo.ACCOUNT VALUES (?, ?, ?)
const wchar_t* InsertAccount_Query = L"INSERT INTO AccountDB.dbo.ACCOUNT VALUES (?, ?, ?)";

//INSERT INTO GameDB.dbo.UserInfo (AccountID, NickName) OUTPUT INSERTED.uid as uid VALUES (?, ?)
const wchar_t* InsertNewUser_Query = L"INSERT INTO GameDB.dbo.UserInfo (AccountID, NickName) OUTPUT INSERTED.uid as uid VALUES (?, ?)";

//INSERT INTO GameDB.dbo.UserGameRecords (UID) VALUES (?)
const wchar_t* InsertUserGameRecords_Query = L"INSERT INTO GameDB.dbo.UserGameRecords (UID) VALUES (?)";

//INSERT INTO GameDB.dbo.UserItem (owner_uid, itemCode, count, itemType) VALUES (?, ?, ?, ?)
const wchar_t* InsertUserItem_Query = L"INSERT INTO GameDB.dbo.UserItem (owner_uid, itemCode, count, itemType) VALUES (?, ?, ?, ?)";

//INSERT INTO GameDB.dbo.UserAttendance (user_UID, event_code, attendance_date, day_count) VALUES(?, ?, CAST(GETDATE() AS DATE), ?)
const wchar_t* InsertUserAttendance_Query = L"INSERT INTO GameDB.dbo.UserAttendance (user_UID, event_code, attendance_date, day_count) VALUES(?, ?, CAST(GETDATE() AS DATE), ?)";

// UPDATE GameDB.dbo.UserInfo SET ConnectionState = ? OUTPUT deleted.* WHERE AccountID = ?
const wchar_t* SelectUserInfoForLogin_Query = L"UPDATE GameDB.dbo.UserInfo \
							SET LastLoginTime = CAST(GETDATE() AS DATE), ConnectionState = ? \
							OUTPUT deleted.* \
							WHERE AccountID = ?";

//SELECT * FROM GameDB.dbo.UserInfo WHERE UID = ?
const wchar_t* SelectUserInfo_Query = L"SELECT * FROM GameDB.dbo.UserInfo WHERE UID = ?";

//SELECT count, itemCode FROM GameDB.dbo.UserItem WHERE owner_UID = ? AND itemType = ?
const wchar_t* SelectUserAllCurrency_Query = L"SELECT count, itemCode FROM GameDB.dbo.UserItem WHERE owner_UID = ? AND itemType = ?";

//SELECT * FROM GameDB.dbo.UserItem WHERE owner_UID = ? AND NOT count = ?
const wchar_t* SelectUserAllItems_Query = L"SELECT * FROM GameDB.dbo.UserItem WHERE owner_UID = ? AND NOT count = ?";

//SELECT DATALENGTH(CharacterCustomizing), CharacterCustomizing FROM GameDB.dbo.UserInfo WHERE UID = ?
const wchar_t* SelectCharacterCustomizing_Query = L"SELECT DATALENGTH(CharacterCustomizing), CharacterCustomizing FROM GameDB.dbo.UserInfo WHERE UID = ?";

//SELECT count FROM GameDB.dbo.UserItem WHERE owner_UID = ? AND ItemCode = ?
const wchar_t* SelectUserItemCount_Query = L"SELECT count FROM GameDB.dbo.UserItem WHERE owner_UID = ? AND ItemCode = ?";

//SELECT attendance_date, day_count, is_rewarded FROM GameDB.dbo.UserAttendance WHERE user_UID = ? AND event_code = ? ORDER BY day_count ASC
const wchar_t* SelectUserAttendanceEvent_Query = L"SELECT attendance_date, day_count, is_rewarded FROM GameDB.dbo.UserAttendance WHERE user_UID = ? AND event_code = ? ORDER BY day_count ASC";

const wchar_t* SelectUserAttendanceEventLatest_Query = L"SELECT TOP 1 attendance_date, day_count "
														L"FROM GameDB.dbo.UserAttendance "
														L"WHERE user_UID = ? AND event_code = ? "
														L"ORDER BY attendance_date DESC ";

//SELECT COUNT(*) FROM GameDB.dbo.UserAttendance WHERE user_UID = ? AND attendance_date = CAST(GETDATE() AS DATE)
const wchar_t* SelectUserAttendanceToday_Query = L"SELECT COUNT(*) FROM GameDB.dbo.UserAttendance WHERE user_UID = ? AND attendance_date = CAST(GETDATE() AS DATE)";

//UPDATE GameDB.dbo.UserInfo SET ConnectionState = ? WHERE UID = ?
const wchar_t* UpdateUserConnectionState_Query = L"UPDATE GameDB.dbo.UserInfo SET ConnectionState = ? WHERE UID = ?";

//UPDATE GameDB.dbo.UserInfo SET Point = Point + ? WHERE UID = ?
const wchar_t* UpdateUserPoint_Query = L"UPDATE GameDB.dbo.UserInfo SET Point = Point + ? WHERE UID = ?";

const wchar_t* UpdateBattleRecords_Query = L"UPDATE GameDB.dbo.UserGameRecords SET "
											L" Kill_Count = Kill_Count + ? "
											L", Death_Count = Death_Count + ? "
											L", Point = Point + ? "
											L", Weapon_Kill_Count = Weapon_Kill_Count + ? "
											L", Punch_Kill_Count = Punch_Kill_Count + ? "
											L", Bomb_Count = Bomb_Count + ? "
											L", Groggy_Count = Groggy_Count + ? "
											L", Pick_Weapon_Count = Pick_Weapon_Count + ? "
											L", Pick_Bomb_Count = Pick_Bomb_Count + ? "
											L", Battle_Count = Battle_Count + ? "
											L", FITH_Team_Count = FITH_Team_Count + ? "
											L", FITH_Indiv_Count = FITH_Indiv_Count + ? "
											L", Victory_Count = Victory_Count + ? "
											L" WHERE UID = ?";

//UPDATE GameDB.dbo.UserItem SET count = count + ? WHERE owner_UID = ? AND itemCode = ?
const wchar_t* UpdateUserItemCount_Query = L"UPDATE GameDB.dbo.UserItem SET count = count + ? WHERE owner_UID = ? AND itemCode = ?";

//UPDATE GameDB.dbo.UserInfo SET CharacterCustomizing = ? WHERE UID = ?
const wchar_t* UpdateCharacterCustomizing_Query = L"UPDATE GameDB.dbo.UserInfo SET CharacterCustomizing = ? WHERE UID = ?";

const wchar_t* UpsertUserItemCount_Query = L"MERGE INTO GameDB.dbo.UserItem AS a "
											L"USING (SELECT 1 AS match) AS b "
											L"ON a.itemCode = ? AND a.owner_UID = ? "
											L"WHEN MATCHED THEN "
											L"UPDATE SET count = count + ? "
											L"WHEN NOT MATCHED THEN "
											L"INSERT (owner_UID, itemCode, count, itemType) VALUES (?, ?, ?, ?);";

//DELETE FROM AccountDB.dbo.Account WHERE ID = ?
const wchar_t* DeleteAcccount_Query = L"DELETE FROM AccountDB.dbo.Account WHERE ID = ?";

//DELETE FROM GameDB.dbo.UserInfo WHERE UID = ?
const wchar_t* DeleteUserInfo_Query = L"DELETE FROM GameDB.dbo.UserInfo WHERE UID = ?";

//DELETE FROM GameDB.dbo.UserItem WHERE Item_UID = (SELECT TOP 1 Item_UID FROM GameDB.dbo.UserItem WHERE owner_UID = ? and ItemCode = ? ORDER BY Item_UID ASC)
const wchar_t* DeleteUserItem_Query = L"DELETE FROM GameDB.dbo.UserItem WHERE Item_UID = (SELECT TOP 1 Item_UID FROM GameDB.dbo.UserItem WHERE owner_UID = ? and ItemCode = ? ORDER BY Item_UID ASC)";

//DELETE FROM GameDB.dbo.UserItem WHERE owner_UID = ?
const wchar_t* DeleteUserItemAll_Query = L"DELETE FROM GameDB.dbo.UserItem WHERE owner_UID = ?";

//DELETE FROM GameDB.dbo.UserAttendance WHERE user_UID = ?
const wchar_t* DeleteUserAttendanceAll_Query = L"DELETE FROM GameDB.dbo.UserAttend¤±nce WHERE user_UID = ?";

//DELETE FROM GameDB.dbo.UserAttendance WHERE attendance_date  < DATEADD(DAY, ?, GETDATE())
const wchar_t* DeleteUserAttendanceOutDated_Query = L"DELETE FROM GameDB.dbo.UserAttendance WHERE attendance_date  < DATEADD(DAY, ?, GETDATE())";

//SELECT hashedPassword, salt FROM AccountDB.dbo.Account WHERE ID = ?
const wchar_t* CheckValidateLogin_Query = L"SELECT hashedPassword, salt FROM AccountDB.dbo.Account WHERE ID = ?";