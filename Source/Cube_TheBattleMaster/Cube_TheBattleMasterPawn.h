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

	UPROPERTY(Category = Block, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class USceneComponent* DummyRoot;

    //Camera componenet
	UPROPERTY(EditAnywhere)
	USpringArmComponent* OurCameraSpringArm;
	UCameraComponent* OurCamera;
<<<<<<< HEAD
<<<<<<< HEAD
	

	
	//Block properties
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite)
	class ACube_TheBattleMasterBlock* CurrentBlockFocus;

	UPROPERTY(Replicated, EditAnyWhere)
	APlayer_Cube* MyCube;

	bool bDead = false;
=======
>>>>>>> parent of 375cd1d... Buttons buttons who got the buttons
=======
>>>>>>> parent of 375cd1d... Buttons buttons who got the buttons

public:

	//virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;

	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	virtual void CalcCamera(float DeltaTime, struct FMinimalViewInfo& OutResult) override;

	void CameraMove(AActor * Cube);

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_SetCube(ACube_TheBattleMasterPawn* Test);

	void SetCube(ACube_TheBattleMasterPawn* Test);

	//UPROPERTY(EditAnyWhere)
	//FVector2D Vec_test;

	////UPROPERTY(EditAnyWhere)
	////FVector TheTest;

	//UPROPERTY(EditAnyWhere)
	//ACube_TheBattleMasterBlock* aaa;

	bool bDead = false;

protected:

	void ToggleOccupied(ACube_TheBattleMasterBlock* Block, bool Bon);

	UFUNCTION(Reliable, Server)
	void Server_ToggleOccupied(ACube_TheBattleMasterBlock * Block, bool Bon);

	void HighlightMoveOptions(ACube_TheBattleMasterPawn* Pawn, ACube_TheBattleMasterBlock* Block, bool Bmove);

	UFUNCTION(Reliable, Server)
	void Server_HighlightMoveOptions(ACube_TheBattleMasterPawn* Pawn, ACube_TheBattleMasterBlock* Block, bool Bmove);
	
	void TriggerClick();

	

	/*UFUNCTION(Client, Reliable)
	void Client_TriggerClick();*/

	void TraceForBlock(const FVector& Start, const FVector& End, bool bDrawDebugHelpers);

	UPROPERTY(EditInstanceOnly, BlueprintReadWrite)
	class ACube_TheBattleMasterBlock* CurrentBlockFocus;

	UPROPERTY(Replicated, EditAnyWhere)
	APlayer_Cube* MyCube;

	void DoDamage(APlayer_Cube* OwnedCube, APlayer_Cube* ToDamageCube);

	//bool IsInVacinity();

	void CubeDestroy();

	UFUNCTION(Reliable, Server)
	void Server_CubeDestroy();

	/** Returns Camera subobject **/
	FORCEINLINE class UCameraComponent* GetCamera() const { return OurCamera; }

public:
	/*Just a dummy functiont to reset the variables*/
	UFUNCTION(BlueprintImplementableEvent)
	void Reset_Buttons();

};
