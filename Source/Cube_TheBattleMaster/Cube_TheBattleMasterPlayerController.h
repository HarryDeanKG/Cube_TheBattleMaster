// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Cube_TheBattleMasterGameMode.h"
#include "Player_Cube.h"

#include "Cube_TheBattleMasterPlayerController.generated.h"

USTRUCT(BlueprintType)
struct FImportedCube_Struct 
{

	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float BaseHealth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float BaseArmour;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int BaseSpeed;

	//TArray<FLinearColor> Colours;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FLinearColor Colours;

	//FImportedCube_Struct FImportedCube_Struct::BuildImportedCube_Struct(
	//	FName Name,
	//	float BaseHealth,
	//	float BaseArmour,
	//	int BaseSpeed,
	//	TArray<FLinearColor> Colours
	//);
};



/** PlayerController class used to enable cursor */
UCLASS()
class ACube_TheBattleMasterPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ACube_TheBattleMasterPlayerController();

	virtual void BeginPlay() override;

	ACube_TheBattleMasterGameMode* GetGameMode();

	EGameSection GetGameModeSection();

	void ToggleGameModeSection(FString GameModeString);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FImportedCube_Struct>  T_CubesInInv;

	
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void SetReadyToPlay();

	UPROPERTY(BlueprintReadWrite, Replicated)
	int PlayerNumb = 0;
	//Import Frpm JSON functions

	TArray<FImportedCube_Struct> GenerateStructsFromJson(FString Path);
private:

	void GenerateStructsFromJson(TArray<FImportedCube_Struct>& AwesomeStructs, TSharedPtr<FJsonObject> JsonObject);
	FString JsonFullPath(FString Path);

	FLinearColor ParseAsColourArray(TSharedPtr<FJsonObject> json, FString KeyName);


	

	//WidgetComponents
	//UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	//TSubclassOf<class UUserWidget> ActionInterface;

	//UPROPERTY()
	//class UUserWidget* ActionInterface_Instance;
};


