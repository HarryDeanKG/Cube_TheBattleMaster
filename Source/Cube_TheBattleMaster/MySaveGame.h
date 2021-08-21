// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "Player_Cube.h"
#include "ItemBase.h"
#include "MySaveGame.generated.h"

/**
 * 
 */
UCLASS()
class CUBE_THEBATTLEMASTER_API UMySaveGame : public USaveGame
{
	GENERATED_BODY()
	

public:

	UPROPERTY(BlueprintReadWrite)
	FName SaveSlotName;

	UPROPERTY(BlueprintReadWrite)
	FName CubeFunName;

	UPROPERTY(BlueprintReadWrite)
	FImportedCube_Struct SavedCube;
	
	//TSubclassOf<APlayer_Cube> SavedCube;
	//APlayer_Cube* SavedCube;

	UPROPERTY(BlueprintReadWrite)
	TMap<FName, TSubclassOf<AItemBase>> AllItemsAttached;

	//TMap<FName, AItemBase*> AllItemsAttached;
};
