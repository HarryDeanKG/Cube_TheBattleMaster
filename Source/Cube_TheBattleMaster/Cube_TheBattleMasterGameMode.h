// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Cube_TheBattleMasterGameMode.generated.h"

/** GameMode class to specify pawn and playercontroller */
UCLASS(minimalapi)
class ACube_TheBattleMasterGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ACube_TheBattleMasterGameMode();
	
	void TakeTurn();

	void EndGameCondition();

	UPROPERTY()
	int32 Game_turn;

	int32 Players_Ready;

};



