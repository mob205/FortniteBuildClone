// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/PortalManager.h"

void UPortalManager::JoinGameSession()
{
	OnJoinStatusChanged.Broadcast("Searching for a game session...");
}
