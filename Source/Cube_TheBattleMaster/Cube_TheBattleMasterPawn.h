// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Player_Cube.h"
#include "Cube_TheBattleMasterBlock.h"
#include "Cube_TheBattleMasterPawn.generated.h"

UCLASS(config=Game)
class ACube_TheBattleMasterPawn : public APawn
{
	GENERATED_UCLASS_BODY()

public:

	//virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;

	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	virtual void CalcCamera(float DeltaTime, struct FMinimalViewInfo& OutResult) override;

	void CameraMove(APlayer_Cube * Cube);

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_SetCube(ACube_TheBattleMasterPawn* Test);

	void SetCube(ACube_TheBattleMasterPawn* Test);

	//UPROPERTY(EditAnyWhere)
	//FVector2D Vec_test;

	////UPROPERTY(EditAnyWhere)
	////FVector TheTest;

	//UPROPERTY(EditAnyWhere)
	//ACube_TheBattleMasterBlock* aaa;

protected:

	void TriggerClick();

	//UFUNCTION(Server, Reliable, WithValidation)
	//void Server_TriggerClick();

	void TraceForBlock(const FVector& Start, const FVector& End, bool bDrawDebugHelpers);

	UPROPERTY(EditInstanceOnly, BlueprintReadWrite)
	class ACube_TheBattleMasterBlock* CurrentBlockFocus;

	UPROPERTY(Replicated, EditAnyWhere)
	APlayer_Cube* MyCube;

	
};
