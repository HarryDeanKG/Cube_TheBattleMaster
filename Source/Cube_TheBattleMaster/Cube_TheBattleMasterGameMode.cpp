// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "Cube_TheBattleMasterGameMode.h"
#include "Cube_TheBattleMasterPlayerController.h"
#include "Cube_TheBattleMasterBlockGrid.h"
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
	//time = 0.f;
}

//void ACube_TheBattleMasterGameMode::BeginPlay() {
////	
////	//for (TObjectIterator<ACube_TheBattleMasterPawn> Pawn; Pawn; ++Pawn) {
////	//	Pawn->SetCube(*Pawn);
////	//}
//
//
//
//
//
//}

void ACube_TheBattleMasterGameMode::InitiateGridBlocksEquality() {
	bool bFlag = false;

	int i = 0;
	for (TObjectIterator<ACube_TheBattleMasterBlockGrid> Grid2; Grid2; ++Grid2) {
		for (auto Elems : Grid2->Grid) {
			i++;
			if (Elems.Value->BasicEnergy == 0) {
				bFlag = true;
			}
			break;
		}
	}
	if (i != GetNumPlayers() + 1) { bFlag = false; }

	if (bFlag) {
		TMap<int, float> testerMap;
		for (TObjectIterator<ACube_TheBattleMasterBlockGrid> Grid2; Grid2; ++Grid2) {
			i = 0;
			for (auto Elems : Grid2->Grid)
			{
				if (Elems.Value->BasicEnergy != 0) {
					testerMap.Add(i, Elems.Value->BasicEnergy);
				}
				else {
					Elems.Value->SetEnergyVariables(testerMap[i]);
				}
				i++;
			}
		}
	}
}

void ACube_TheBattleMasterGameMode::Tick(float DeltaSeconds) {
	Super::Tick(DeltaSeconds);
	
	//Initalize the GRIDS
	InitiateGridBlocksEquality();
	

	////////

	//int32 i = 0;
	//for (TObjectIterator<ACube_TheBattleMasterPawn> PlayerPawn; PlayerPawn; ++PlayerPawn) {
	//	if (PlayerPawn->CubeSelected) {
	//		UE_LOG(LogTemp, Warning, TEXT("%s: %s"), *PlayerPawn->GetName(), PlayerPawn->CubeSelected->bReady ? TEXT("True") : TEXT("False"));
	//		if (PlayerPawn->CubeSelected->bReady) {
	//			i++;
	//		}
	//	}
	//}
	//if (GetNumPlayers() <= i) { BeginTheGame(); 			UE_LOG(LogTemp, Warning, TEXT("GAMESTART"));
	//}


	//if (ROLE_Authority == GetLocalRole()) {
	//	for (TObjectIterator<ACube_TheBattleMasterPawn> PlayerPawn; PlayerPawn; ++PlayerPawn) {
	//		if (PlayerPawn->MyCube) {					
	//			FString One = PlayerPawn->MyCube->GetName();
	//			One.RemoveAt(0, One.Len() - 1);
	//			FString Two = PlayerPawn->GetName();
	//			Two.RemoveAt(0, Two.Len() - 1);
	//			if ( One == Two) {
	//				//UE_LOG(LogTemp, Warning, TEXT("%d: Player %s has cube %s"), GetLocalRole(), *PlayerPawn->GetName(), *PlayerPawn->MyCube->GetName())
	//			}
	//		}
	//	}
	//}
	//time += DeltaSeconds;

	/*Do the actions that are preset*/
	
	if (bDoActions) {

		if (bNextTurn) {
			for (TObjectIterator<ACube_TheBattleMasterPawn> PlayerPawn; PlayerPawn; ++PlayerPawn){
				
				if (PlayerPawn->MyCube) {
					//UE_LOG(LogTemp, Warning, TEXT("Player name: %s"), *PlayerPawn->GetName());

					//PlayerPawn->MyCube->bReady = false;
					//PlayerPawn->MyCube->time = 0.f;

					

					if (PlayerPawn->M_ActionStructure.Contains(doAction)) {
						
						//PlayerPawn->MyCube->InitiateMovementAndAction();
						//PlayerPawn->DoAction(doAction);
					}
					//UE_LOG(LogTemp, Warning, TEXT("doAction %d"), doAction);
					if (doAction > 3) {
						//UE_LOG(LogTemp, Warning, TEXT("test"));
						//PlayerPawn->ResetEverything(false);
						bDoActions = false;
						PlayerPawn->MyCube->E_TurnStateEnum = ETurnState::TS_SelectActions;
						PlayerPawn->MyCube->SetReplicatingMovement(false);
						PlayerPawn->bReady = false;

						//PlayerPawn->MyCube->BlockOwner = PlayerPawn->GetBlockFromPosition(PlayerPawn->MyCube->GetActorLocation());

						PlayerPawn->ClearVars();
						PlayerPawn->Reset_Buttons_test();
						PlayerPawn->UpdateActions();
					}
					else {
						//UE_LOG(LogTemp, Warning, TEXT("DoAction: %d - %s"), doAction, PlayerPawn->M_ActionStructure.Contains(doAction) ? TEXT("True") : TEXT("False")); 
						if (PlayerPawn->M_ActionStructure.Contains(doAction)) {
							//PlayerPawn->MyCube->bReady = false;
							//PlayerPawn->MyCube->time = 0.f;
							PlayerPawn->DoAction(doAction);
						}
					}
				}
			}
			if (!bDoActions) { doAction = 0; E_TurnStateEnum = ETurnState::TS_SelectActions;}
			else { doAction++; }
			bNextTurn = false;
		}

		/*Replace with the timer function that dictates speed etc...*/
		bNextTurn = true;

		for (TObjectIterator<ACube_TheBattleMasterPawn> PlayerPawn; PlayerPawn; ++PlayerPawn) {
			if (PlayerPawn->MyCube) {
				if (!PlayerPawn->MyCube->bReady) { bNextTurn = false; }
				//UE_LOG(LogTemp, Warning, TEXT("Player %s is ready: %s"), *PlayerPawn->GetName(), PlayerPawn->MyCube->bReady ? TEXT("True") : TEXT("False"));
			}
		}
		//for (TObjectIterator<APlayer_Cube> Cube; Cube; ++Cube) {
		//		if (Cube->bReady) { bNextTurn = false; }
		//		UE_LOG(LogTemp, Warning, TEXT("Cube %s is ready: %s"), *Cube->GetName(), Cube->bReady ? TEXT("True") : TEXT("False"));
		//}

	}
}

void ACube_TheBattleMasterGameMode::TakeTurn()
{
	if (Game_turn < 1){/* End Game*/ }
	Players_Ready += 1;
	if (Players_Ready == GetNumPlayers()) { 
		if (E_GameSectionEnum == EGameSection::GS_StartSection) { E_GameSectionEnum = EGameSection::GS_MidSection; }
		E_TurnStateEnum = ETurnState::TS_InitiateActions;
		for (TObjectIterator<ACube_TheBattleMasterPawn> PlayerPawn; PlayerPawn; ++PlayerPawn) {
			//UE_LOG(LogTemp, Warning, TEXT("Player name: %s"), *PlayerPawn->GetName());

			if (PlayerPawn->MyCube) {
				PlayerPawn->MyCube->E_TurnStateEnum = ETurnState::TS_InitiateActions;
				PlayerPawn->MyCube->SetReplicatingMovement(true);
				//UE_LOG(LogTemp, Warning, TEXT("Player name: %s"), *PlayerPawn->GetName());

				//UE_LOG(LogTemp, Warning, TEXT("Player name: %s"), *PlayerPawn->MyCube->GetName());

			}
		}

		Game_turn -= 1; 
		Players_Ready = 0; 

		doAction = 0;
		bDoActions = true;
		bNextTurn = true;

	//	//E_TurnStateEnum = ETurnState::TS_PreSelection;
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
	/*E_GameSectionEnum = EGameSection::GS_MidSection;
	return true;*/

	int32 i = 0;
	for (TObjectIterator<ACube_TheBattleMasterPawn> PlayerPawn; PlayerPawn; ++PlayerPawn) {
		if (PlayerPawn->CubeSelected) {
			//UE_LOG(LogTemp, Warning, TEXT("%s: %s"), *PlayerPawn->GetName(), PlayerPawn->bReady ? TEXT("True") : TEXT("False"));
			if (PlayerPawn->bReady) {
				i++;
			}
		}
	}
	
	if (GetNumPlayers() <= i){
		E_GameSectionEnum = EGameSection::GS_MidSection;
		for (TObjectIterator<ACube_TheBattleMasterPawn> PlayerPawn; PlayerPawn; ++PlayerPawn) {
			if (PlayerPawn->CubeSelected) {
				//PlayerPawn->Waiting();
				//PlayerPawn->bReady = false;
				
				Cast<ACube_TheBattleMasterPlayerController>(PlayerPawn->GetOwner())->SetReadyToPlay();
				//Cast<ACube_TheBattleMasterPlayerController>(PlayerPawn->GetOwner());


				//PlayerPawn->CubeSelected->bReady = false;
				//PlayerPawn->CubeSelected->SetActorLocation(FVector(0.f));
				//PlayerPawn->CubeSelected->Destroy();
				//PlayerPawn->SpawnMap();
			}
		}
		return true;
	}		
	//else {  }
	return false;
}

