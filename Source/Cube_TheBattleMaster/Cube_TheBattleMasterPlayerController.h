// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Cube_TheBattleMasterPlayerController.generated.h"

/** PlayerController class used to enable cursor */
UCLASS()
class ACube_TheBattleMasterPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ACube_TheBattleMasterPlayerController();

	//virtual void BeginPlay() override;

	//WidgetComponents
	//UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	//TSubclassOf<class UUserWidget> ActionInterface;

	//UPROPERTY()
	//class UUserWidget* ActionInterface_Instance;
};


