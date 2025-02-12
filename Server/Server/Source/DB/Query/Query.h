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

//INSERT INTO GameDB.dbo.UserEventReward (user_UID, event_code, attendance_date, day_count) VALUES(?, ?, CAST(GETDATE() AS DATE), ?)
const wchar_t* InsertUserEventReward_Query = L"INSERT INTO GameDB.dbo.UserEventReward (userUID, eventCode) VALUES(?, ?)";

const wchar_t* InsertUserPassReward_Query = L"INSERT INTO GameDB.dbo.UserPassReward (user_UID, pass_code, pass_type, level, is_rewarded) VALUES (?, ?, ?, ?, 1) ";

const wchar_t* SelectAccountCount_Query = L"SELECT COUNT(ID) FROM AccountDB.dbo.Account WHERE ID = ?";

// UPDATE GameDB.dbo.UserInfo SET ConnectionState = ? OUTPUT deleted.*, DATALENGTH(deleted.CharacterCustomizing) WHERE AccountID = ?
const wchar_t* SelectUserInfoForLogin_Query = L"UPDATE GameDB.dbo.UserInfo \
							SET LastLoginTime = CAST(GETDATE() AS DATE), ConnectionState = ? \
							OUTPUT deleted.* \
							WHERE AccountID = ?";

//SELECT * FROM GameDB.dbo.UserInfo WHERE UID = ?
const wchar_t* SelectUserInfo_Query = L"SELECT * FROM GameDB.dbo.UserInfo WHERE UID = ?";

//SELECT count, itemCode FROM GameDB.dbo.UserItem WHERE owner_UID = ? AND itemType = ?
const wchar_t* SelectUserAllCurrency_Query = L"SELECT count, itemCode FROM GameDB.dbo.UserItem WHERE owner_UID = ? AND itemType = 1";

//SELECT * FROM GameDB.dbo.UserItem WHERE owner_UID = ? AND NOT count = ?
const wchar_t* SelectUserAllItems_Query = L"SELECT * FROM GameDB.dbo.UserItem WHERE owner_UID = ? AND NOT count = ?";

//SELECT DATALENGTH(CharacterCustomizing), CharacterCustomizing FROM GameDB.dbo.UserInfo WHERE user_UID = ?
const wchar_t* SelectCharacterCustomizing_Query = L"SELECT * FROM GameDB.dbo.UserCustomizing WHERE user_UID = ?";

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

//SELECT is_rewarded FROM GameDB.dbo.UserAttendance WHERE user_UID = ? AND event_code = ? AND day_count = ?
const wchar_t* SelectUserAttendanceIsRewarded_Query = L"SELECT is_rewarded FROM GameDB.dbo.UserAttendance WHERE user_UID = ? AND event_code = ? AND day_count = ?";

const wchar_t* SelectUserMission_Query = L"DECLARE @DAILY_MISSION INT = 1 "
										L"SELECT * FROM GameDB.dbo.UserMission WHERE user_UID = ? "
										L"AND (mission_type <> @DAILY_MISSION "
										L"OR (mission_type = @DAILY_MISSION AND assigned_date = CAST(GETDATE() AS DATE)));";

//SELECT * FROM GameDB.dbo.UserPass WHERE user_UID = ? AND pass_code = ?
const wchar_t* SelectUserPass_Query = L"SELECT * FROM GameDB.dbo.UserPass WHERE user_UID = ? AND pass_code = ?";

const wchar_t* SelectUserPassReward_Query = L"SELECT * FROM GameDB.dbo.UserPassReward WHERE user_UID = ? AND pass_code = ?";

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

const wchar_t* UpsertCharacterCustomizing_Query = L"MERGE INTO GameDB.dbo.UserCustomizing AS Target "
													L"USING (VALUES (?, ?, ?, ?, ?, ?)) AS Source(uid, skin, head, face, back, emotion) "
													L"ON Target.user_uid = Source.uid "
													L"WHEN MATCHED THEN "
													L"UPDATE SET Target.skin = Source.skin, Target.head = Source.head, Target.face = Source.face, Target.back = Source.back, Target.emotion = Source.emotion "
													L"WHEN NOT MATCHED THEN "
													L"INSERT (user_UID, skin, head, face, back, emotion) VALUES (Source.uid, Source.skin, Source.head, Source.face, Source.back, Source.emotion);";

const wchar_t* UpsertUserItemCount_Query = L"MERGE INTO GameDB.dbo.UserItem AS Target "
											L"USING (SELECT 1 AS match) AS Source "
											L"ON Target.itemCode = ? AND Target.owner_UID = ? "
											L"WHEN MATCHED THEN "
											L"UPDATE SET count = count + ? "
											L"WHEN NOT MATCHED THEN "
											L"INSERT (owner_UID, itemCode, count, itemType) VALUES (?, ?, ?, ?);";

const wchar_t* UpsertUserCurrency_Query_Front = L"MERGE INTO GameDB.dbo.UserItem AS Target "
												L"USING (VALUES ";

const wchar_t* UpsertUserCurrency_Query_Back = L") AS Source(owner_UID, itemCode, count) "
												L"ON Target.owner_UID = Source.owner_UID AND (Target.itemCode = Source.itemCode OR Target.itemCode IS NULL) "
												L"WHEN MATCHED THEN "
												L"UPDATE SET Target.count = Target.count + Source.count "
												L"WHEN NOT MATCHED BY TARGET THEN "
												L"INSERT(owner_UID, itemCode, count, itemType) "
												L"VALUES(Source.owner_UID, Source.itemCode, Source.count, 1);";

const wchar_t* UpsertUserMission_Query_Front = L"MERGE INTO GameDB.dbo.UserMission AS Target "
													L"USING (VALUES ";

const wchar_t* UpsertUserMission_Query_Back = L") AS Source(user_UID, mission_code, mission_type, mission_group, mission_step, progress, required_count, assigned_date, is_rewarded) "
												L"ON Target.user_UID = Source.user_UID AND Target.mission_code = Source.mission_code AND Target.assigned_date = Source.assigned_date "
												L"WHEN MATCHED THEN "
												L"UPDATE SET Target.progress = Source.progress, Target.is_rewarded = Source.is_rewarded "
												L"WHEN NOT MATCHED BY TARGET THEN "
												L"INSERT "
												L"VALUES(Source.user_UID, Source.mission_code, Source.mission_type, Source.mission_group, Source.mission_step, Source.progress, Source.required_count, Source.assigned_date, Source.is_rewarded);";


//UPDATE GameDB.dbo.UserAttendance SET is_rewarded = ? OUTPUT deleted.is_rewarded WHERE user_UID = ? AND event_code = ? AND day_count = ? 
const wchar_t* UpdateUserAttendanceIsRewarded_Query = L"UPDATE GameDB.dbo.UserAttendance SET is_rewarded = ? OUTPUT deleted.is_rewarded WHERE user_UID = ? AND event_code = ? AND day_count = ? ";

const wchar_t* UpdateGoldAndSelectUserAllCurrency_Query = L"UPDATE GameDB.dbo.UserItem "
														L"SET count = count + ? "
														L"WHERE itemCode = ? AND onwer_UID = ? "
														L"SELECT count, itemCode FROM GameDB.dbo.UserItem WHERE owner_UID = ? AND itemType = 1";

const wchar_t* UpdateUserEventReward_IsRewarded_Query
= L"MERGE INTO GameDB.dbo.UserEventReward AS Target "
	L"USING (Select ? AS userUID, ? AS eventCode) AS Source "
	L"ON Target.userUID = Source.userUID AND Target.eventCode = Source.eventCode "
	L"WHEN MATCHED AND Target.isRewarded = 0 THEN"
	L"UPDATE SET isRewarded = 1, rewardedDate = CAST(GETDATE() AS DATE)";


const wchar_t* UpsertUserPass_Query
= L"MERGE INTO GameDB.dbo.UserPass AS Target "
L"USING (VALUES (?, ?, ?, ?, ?, ?)) AS Source(user_UID, pass_code, pass_type, pass_level, pass_exp, daily_mission_exp) "
L"ON Target.user_UID = Source.user_UID AND Target.pass_code = Source.pass_code "
L"WHEN MATCHED THEN "
L"UPDATE SET Target.pass_exp = Source.pass_exp, Target.pass_level = Source.pass_level, Target.daily_mission_exp = Source.daily_mission_exp "
L"WHEN NOT MATCHED THEN "
L"INSERT VALUES(Source.user_UID, Source.pass_code, Source.pass_type, Source.pass_level, Source.pass_exp, Source.daily_mission_exp);";

const wchar_t* UpdateUserPassReward_isRewarded_Query
= L"UPDATE GameDB.dbo.UserItem SET is_rewarded = ? ";
														

//DELETE FROM AccountDB.dbo.Account WHERE ID = ?
const wchar_t* DeleteAcccount_Query = L"DELETE FROM AccountDB.dbo.Account WHERE ID = ?";

//DELETE FROM GameDB.dbo.UserInfo WHERE UID = ?
const wchar_t* DeleteUserInfo_Query = L"DELETE FROM GameDB.dbo.UserInfo WHERE UID = ?";

//DELETE FROM GameDB.dbo.UserGameRecords WHERE UID = ?
const wchar_t* DeleteUserGameRecords_Query = L"DELETE FROM GameDB.dbo.UserGameRecords WHERE UID = ?";

//DELETE FROM GameDB.dbo.UserItem WHERE Item_UID = (SELECT TOP 1 Item_UID FROM GameDB.dbo.UserItem WHERE owner_UID = ? and ItemCode = ? ORDER BY Item_UID ASC)
const wchar_t* DeleteUserItem_Query = L"DELETE FROM GameDB.dbo.UserItem WHERE Item_UID = (SELECT TOP 1 Item_UID FROM GameDB.dbo.UserItem WHERE owner_UID = ? and ItemCode = ? ORDER BY Item_UID ASC)";

//DELETE FROM GameDB.dbo.UserItem WHERE owner_UID = ?
const wchar_t* DeleteUserItemAll_Query = L"DELETE FROM GameDB.dbo.UserItem WHERE owner_UID = ?";

//DELETE FROM GameDB.dbo.UserAttendance WHERE user_UID = ?
const wchar_t* DeleteUserAttendanceAll_Query = L"DELETE FROM GameDB.dbo.UserAttendance WHERE user_UID = ?";

//DELETE FROM GameDB.dbo.UserAttendance WHERE attendance_date  < DATEADD(DAY, ?, GETDATE())
const wchar_t* DeleteUserAttendanceOutDated_Query = L"DELETE FROM GameDB.dbo.UserAttendance WHERE attendance_date  < DATEADD(DAY, ?, GETDATE())";

//SELECT hashedPassword, salt FROM AccountDB.dbo.Account WHERE ID = ?
const wchar_t* CheckValidateLogin_Query = L"SELECT hashedPassword, salt FROM AccountDB.dbo.Account WHERE ID = ?";