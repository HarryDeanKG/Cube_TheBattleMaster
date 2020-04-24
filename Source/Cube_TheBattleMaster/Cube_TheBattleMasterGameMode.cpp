// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "Cube_TheBattleMasterGameMode.h"
#include "Cube_TheBattleMasterPlayerController.h"
#include "Cube_TheBattleMasterPawn.h"

ACube_TheBattleMasterGameMode::ACube_TheBattleMasterGameMode()
{
	// no pawn by default
	DefaultPawnClass = ACube_TheBattleMasterPawn::StaticClass();
	// use our own player controller class
	PlayerControllerClass = ACube_TheBattleMasterPlayerController::StaticClass();
}
