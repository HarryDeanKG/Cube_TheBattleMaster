// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Cube_TheBattleMasterGameMode.generated.h"

/* Set the Enums for the game mode*/
UENUM(BlueprintType)		//"BlueprintType" is essential to include to get these enum types on the blueprint
enum class ETurnState : uint8
{
	TS_PreSelection 	UMETA(DisplayName = "PreSelection"),
	TS_SelectActions 	UMETA(DisplayName = "SelectActions"),
	TS_InitiateActions	UMETA(DisplayName = "InitiateActions"),
	TS_WaitAction		UMETA(DisplayName = "WaitAction")
};

UENUM(BlueprintType)
enum class EGameSection : uint8
{
	GS_StartSection UMETA(DisplayName = "StartSection"),
	GS_MidSection 	UMETA(DisplayName = "MidSection"),
	GS_EndSection	UMETA(DisplayName = "EndSection")
};

/** GameMode class to specify pawn and playercontroller */
UCLASS(minimalapi)
class ACube_TheBattleMasterGameMode : public AGameModeBase
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Enum)
	ETurnState E_TurnStateEnum;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Enum)
	EGameSection E_GameSectionEnum;

public:
	ACube_TheBattleMasterGameMode();

	UPROPERTY()
	TSubclassOf<class ACube_TheBattleMasterPawn> Starting_Pawn;
	
	//void BeginPlay() override;
	
	bool bDoActions;
	bool bNextTurn;
	int doAction = 0;
	virtual void Tick(float DeltaSeconds) override;

	void TakeTurn();

	//float time;

	void EndGameCondition();

	UPROPERTY()
	int32 Game_turn;

	int32 Players_Ready;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	bool BeginTheGame();

};



