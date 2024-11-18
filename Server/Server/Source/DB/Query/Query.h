#pragma once

const wchar_t* InsertAccount_Query = L"INSERT INTO ACCOUNT VALUES (?, ?, ?)";

const wchar_t* InsertNewUser_Query = L"INSERT INTO UserInfo (AccountID, NickName) OUTPUT INSERTED.uid as uid VALUES (?, ?)";

const wchar_t* InsertUserGameRecords_Query = L"INSERT INTO UserGameRecords (UID) VALUES (?)";

const wchar_t* InsertUserItem_Query = L"INSERT INTO UserItem (owner_uid, itemCode, count, itemType) VALUES (?, ?, ?, ?)";

const wchar_t* SelectUserInfoForLogin_Query = L"UPDATE UserInfo \
							SET ConnectionState = ? \
							OUTPUT deleted.* \
							WHERE AccountID = ?";

const wchar_t* SelectUserInfo_Query = L"SELECT * FROM UserInfo WHERE UID = ?";

const wchar_t* SelectUserAllCurrency_Query = L"SELECT count, itemCode FROM UserItem WHERE owner_UID = ? AND itemType = ?";

const wchar_t* SelectUserAllItems_Query = L"SELECT * FROM UserItem WHERE owner_UID = ? AND NOT count = ?";

const wchar_t* SelectCharacterCustomizing_Query = L"SELECT DATALENGTH(CharacterCustomizing), CharacterCustomizing FROM UserInfo WHERE UID = ?";

const wchar_t* SelectUserItemCount_Query = L"SELECT count FROM UserItem WHERE owner_UID = ? AND ItemCode = ?";

const wchar_t* UpdateUserConnectionState_Query = L"UPDATE UserInfo SET ConnectionState = ? WHERE UID = ?";

const wchar_t* UpdateUserPoint_Query = L"UPDATE UserInfo SET Point = Point + ? WHERE UID = ?";

const wchar_t* UpdateBattleRecords_Query = L"UPDATE UserGameRecords SET "
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

const wchar_t* UpdateUserItemCount_Query = L"UPDATE UserItem SET count = count + ? WHERE owner_UID = ? AND itemCode = ?";

const wchar_t* UpdateCharacterCustomizing_Query = L"UPDATE UserInfo SET CharacterCustomizing = ? WHERE UID = ?";

const wchar_t* UpsertUserItemCount_Query = L"MERGE INTO UserItem AS a "
											L"USING (SELECT 1 AS match) AS b "
											L"ON a.itemCode = ? AND a.owner_UID = ? "
											L"WHEN MATCHED THEN "
											L"UPDATE SET count = count + ? "
											L"WHEN NOT MATCHED THEN "
											L"INSERT (owner_UID, itemCode, count, itemType) VALUES (?, ?, ?, ?);";

const wchar_t* DeleteAcccount_Query = L"DELETE FROM Account WHERE ID = ?";

const wchar_t* DeleteUserInfo_Query = L"DELETE FROM UserInfo WHERE UID = ?";

const wchar_t* DeleteUserItem_Query = L"DELETE FROM UserItem WHERE Item_UID = (SELECT TOP 1 Item_UID FROM UserItem WHERE owner_UID = ? and ItemCode = ? ORDER BY Item_UID ASC)";

const wchar_t* CheckValidateLogin_Query = L"SELECT hashedPassword, salt FROM ACCOUNT WHERE ID = ?";