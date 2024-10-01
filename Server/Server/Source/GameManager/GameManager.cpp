#pragma once
#include "../Server/Server.h"
#include "GameManager.h"

GameManager::GameManager(Server* server, GameMode gameMode)
{
    mServer = server;
    mDB = mServer->GetDB();
    mTimer = mServer->GetTimer();
    mTableManager = mServer->GetTableManager();
    mPacketSender = mServer->GetPacketSender();

    mGameMode = gameMode;
}
