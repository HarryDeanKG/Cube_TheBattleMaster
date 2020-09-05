// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "Cube_TheBattleMasterGameMode.h"
#include "Cube_TheBattleMasterPlayerController.h"
#include "UObject/UObjectIterator.h"
#include "Player_Cube.h"
#include "Cube_TheBattleMasterPawn.h"
#include "UObject/ConstructorHelpers.h"
#include "TimerManager.h"

ACube_TheBattleMasterGameMode::ACube_TheBattleMasterGameMode()
{
	PrimaryActorTick.bCanEverTick = true;

	// no pawn by default
	//DefaultPawnClass = ACube_TheBattleMasterPawn::StaticClass();

	/*
	//FIND THE GOD DAMN BLUEPRINT 
	UClass* MyPawnBlueprintClass;

	static ConstructorHelpers::FClassFinder<APawn> MyPawnFinder(TEXT("/Game/BP_Classes/BP_Pawn"));
	MyPawnBlueprintClass = (UClass*)MyPawnFinder.Class;

	DefaultPawnClass = MyPawnBlueprintClass;*/

	// use our own player controller class
	PlayerControllerClass = ACube_TheBattleMasterPlayerController::StaticClass();

	E_GameSectionEnum = EGameSection::GS_StartSection;
	//E_GameSectionEnum = EGameSection::GS_MidSection;
	Game_turn = 200;

}

//void ACube_TheBattleMasterGameMode::BeginPlay() {
//	
//	//for (TObjectIterator<ACube_TheBattleMasterPawn> Pawn; Pawn; ++Pawn) {
//	//	Pawn->SetCube(*Pawn);
//	//}
//}

void ACube_TheBattleMasterGameMode::Tick(float DeltaSeconds) {
	Super::Tick(DeltaSeconds);
	
	/*Do the actions that are preset*/
	if (bDoActions) {
		
		/*Replace with the timer function that dictates speed etc...*/
		bNextTurn = true;
		
		int i = 0;
		for (TObjectIterator<ACube_TheBattleMasterPawn> PlayerPawn; PlayerPawn; ++PlayerPawn) {
			
			UE_LOG(LogTemp, Warning, TEXT("%d: Player %s is ready? %s"), i, *PlayerPawn->MyCube->GetName(), PlayerPawn->MyCube->bDoAction ? TEXT("True") : TEXT("False"));
			if (PlayerPawn->MyCube->bDoAction) { bNextTurn = false; }
			i++;
		}
		if (bNextTurn) {
			for (TObjectIterator<ACube_TheBattleMasterPawn> PlayerPawn; PlayerPawn; ++PlayerPawn){
				if (doAction > 3) {
					PlayerPawn->ResetEverything(); bDoActions = false; PlayerPawn->MyCube->E_TurnStateEnum = ETurnState::TS_SelectActions; PlayerPawn->MyCube->SetReplicatingMovement(false);
				}
				else{ if (PlayerPawn->M_Action_Name.Contains(doAction)) { PlayerPawn->DoAction(doAction); } }
			}if (!bDoActions) { doAction = 0; E_TurnStateEnum = ETurnState::TS_SelectActions; }
			else { doAction++; }
		}
	}
}

void ACube_TheBattleMasterGameMode::TakeTurn()
{
	if (Game_turn < 1){/* End Game*/ }
	Players_Ready += 1;
	if (Players_Ready == GetNumPlayers()) { 
		//if (E_GameSectionEnum == EGameSection::GS_StartSection) { E_GameSectionEnum = EGameSection::GS_MidSection; }
		E_TurnStateEnum = ETurnState::TS_InitiateActions;
		for (TObjectIterator<ACube_TheBattleMasterPawn> PlayerPawn; PlayerPawn; ++PlayerPawn) { PlayerPawn->MyCube->E_TurnStateEnum = ETurnState::TS_InitiateActions; PlayerPawn->MyCube->SetReplicatingMovement(true);
		}

		Game_turn -= 1; 
		Players_Ready = 0; 

		bDoActions = true;

		//E_TurnStateEnum = ETurnState::TS_PreSelection;
	}
	UE_LOG(LogTemp, Warning, TEXT("Game turn: %d"), Game_turn);
	UE_LOG(LogTemp, Warning, TEXT("Player ready: %d"), Players_Ready);
}

void ACube_TheBattleMasterGameMode::EndGameCondition()
{
	int32 dummy_count = 0;

	for (TObjectIterator<ACube_TheBattleMasterPawn> PlayerPawn; PlayerPawn; ++PlayerPawn) {
		if (PlayerPawn->bDead) {
			dummy_count++;
		}
	}
	if (dummy_count == GetNumPlayers() - 1) {/*EndGame*/
		UE_LOG(LogTemp, Warning, TEXT("EndGame"));
	}
}


bool ACube_TheBattleMasterGameMode::BeginTheGame_Implementation()
{
	//This is to circumvent the StartSection
	E_GameSectionEnum = EGameSection::GS_MidSection;
	return true;

	int32 i = 0;
	for (TObjectIterator<ACube_TheBattleMasterPawn> PlayerPawn; PlayerPawn; ++PlayerPawn) {
		if (PlayerPawn->CubeSelected) {
			//UE_LOG(LogTemp, Warning, TEXT("%s: %s"), *PlayerPawn->GetName(), PlayerPawn->CubeSelected->bReady ? TEXT("True") : TEXT("False"));
			if (PlayerPawn->CubeSelected->bReady) {
				i++;
			}
		}
	}
	
	if (GetNumPlayers() <= i){
		E_GameSectionEnum = EGameSection::GS_MidSection;
		for (TObjectIterator<ACube_TheBattleMasterPawn> PlayerPawn; PlayerPawn; ++PlayerPawn) {
			if (PlayerPawn->CubeSelected) {
				PlayerPawn->Waiting();
				PlayerPawn->bReady = false;
				PlayerPawn->CubeSelected->bReady = false;
				PlayerPawn->CubeSelected->SetActorLocation(FVector(0.f));
				PlayerPawn->CubeSelected->Destroy();
			}
		}
		return true;
	}
	else { return false; }
	
}

