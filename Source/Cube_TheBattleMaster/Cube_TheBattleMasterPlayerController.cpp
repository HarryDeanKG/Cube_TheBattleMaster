// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "Player_Cube.h"
#include "Engine/World.h"
#include "Cube_TheBattleMasterPlayerController.h"

ACube_TheBattleMasterPlayerController::ACube_TheBattleMasterPlayerController()
{
	bShowMouseCursor = true;
	bEnableClickEvents = true;
	bEnableTouchEvents = true;
	DefaultMouseCursor = EMouseCursor::Crosshairs;
	//GetWorld()->SpawnActor<APlayer_Cube>(FVector(0, 0, 0), FRotator(0, 0, 0));
}


