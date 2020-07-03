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

	//E_GameSectionEnum = EGameSection::GS_StartSection;
	E_GameSectionEnum = EGameSection::GS_MidSection;
	Game_turn = 200;
}

void ACube_TheBattleMasterGameMode::BeginPlay() {
	
	for (TObjectIterator<ACube_TheBattleMasterPawn> Pawn; Pawn; ++Pawn) {
		Pawn->SetCube(*Pawn);
	}
}

void ACube_TheBattleMasterGameMode::TakeTurn()
{
	if (Game_turn < 1){/* End Game*/ }
	Players_Ready += 1;
	if (Players_Ready == GetNumPlayers()) { 
		if (E_GameSectionEnum == EGameSection::GS_StartSection) { E_GameSectionEnum = EGameSection::GS_MidSection; }
		E_TurnStateEnum = ETurnState::TS_InitiateActions;
		Game_turn -= 1; 
		Players_Ready = 0; 
		DoSetActions();
		for (TObjectIterator<ACube_TheBattleMasterPawn> PlayerPawn; PlayerPawn; ++PlayerPawn) { PlayerPawn->bReady = false; PlayerPawn->M_Action.Empty(); PlayerPawn->ActionNumb = 0; }
	}
	UE_LOG(LogTemp, Warning, TEXT("Game turn: %d"), Game_turn);
	UE_LOG(LogTemp, Warning, TEXT("Player ready: %d"), Players_Ready);
}

void ACube_TheBattleMasterGameMode::DoSetActions()
{
	for (int32 itr = 0; itr < 4; ++itr) {
		for (TObjectIterator<ACube_TheBattleMasterPawn> PlayerPawn; PlayerPawn; ++PlayerPawn) { PlayerPawn->DoAction(itr); }
	}
	/*Check actions are complete*/
	E_TurnStateEnum = ETurnState::TS_PreSelection;
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