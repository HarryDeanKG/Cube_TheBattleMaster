// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Cube_TheBattleMasterPawn.generated.h"

UCLASS(config=Game)
class ACube_TheBattleMasterPawn : public APawn
{
	GENERATED_UCLASS_BODY()

public:

<<<<<<< HEAD
	virtual void BeginPlay() override;


=======
>>>>>>> parent of 881ec9e... Server client block making
	virtual void Tick(float DeltaSeconds) override;

	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	virtual void CalcCamera(float DeltaTime, struct FMinimalViewInfo& OutResult) override;

<<<<<<< HEAD
	//UFUNCTION(Client, Reliable, WithValidation)
	void Server_SetCube();

	void SetCube();



protected:
	
	
	void TriggerClick();

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_TriggerClick();

	UPROPERTY(Replicated, EditAnyWhere)
	APlayer_Cube* MyCube;

=======
protected:
	
	void TriggerClick();
>>>>>>> parent of 881ec9e... Server client block making
	void TraceForBlock(const FVector& Start, const FVector& End, bool bDrawDebugHelpers);

	UPROPERTY(EditInstanceOnly, BlueprintReadWrite)
	class ACube_TheBattleMasterBlock* CurrentBlockFocus;
};
