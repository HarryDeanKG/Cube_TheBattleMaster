// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "Cube_TheBattleMasterGameMode.h"
#include "Cube_TheBattleMasterPlayerController.h"
#include "UObject/UObjectIterator.h"
#include "Player_Cube.h"
#include "Cube_TheBattleMasterPawn.h"

ACube_TheBattleMasterGameMode::ACube_TheBattleMasterGameMode()
{
	// no pawn by default
	DefaultPawnClass = ACube_TheBattleMasterPawn::StaticClass();
	// use our own player controller class
	PlayerControllerClass = ACube_TheBattleMasterPlayerController::StaticClass();
}


void ACube_TheBattleMasterGameMode::EndGameCondition()
{
	int32 dummy_count=0;
	
	for (TObjectIterator<ACube_TheBattleMasterPawn> PlayerPawn; PlayerPawn; ++PlayerPawn) {
		if (PlayerPawn->bDead) {
			dummy_count++;
		}
	}
	if (dummy_count == GetNumPlayers()-1) {/*EndGame*/
		UE_LOG(LogTemp, Warning, TEXT("EndGame"));
	}
}