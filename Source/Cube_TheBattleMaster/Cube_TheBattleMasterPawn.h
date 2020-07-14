// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Player_Cube.h"
#include "Cube_TheBattleMasterBlock.h"
#include "Cube_TheBattleMasterGameMode.h"
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

	float CamSpeed;

	// move the camera forward
	void OnMoveForward(float value);

	// move the camera right
	void OnMoveRight(float value);

	//move camera in
	void OnMoveIn(float value);

	//Block properties
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite)
	class ACube_TheBattleMasterBlock* CurrentBlockFocus;

	UPROPERTY(Replicated, EditAnyWhere)
	APlayer_Cube* MyCube;

	bool bDead = false;
	bool bReady = false;

	int32 ActionNumb;
	int32 DoActionNumb;

	/*List of Actions to undertake*/
	UPROPERTY(BlueprintReadWrite)
	TMap<int32, FString> M_Action_Name;

	UPROPERTY(BlueprintReadWrite)
	TMap<int32, FVector> M_Action_Pos;

	UPROPERTY()
	ACube_TheBattleMasterBlock* StartingBlock;

	/*Called to initiate what action is to be done*/
	void DoAction();

	void DoMove(FString Position, int32 MoveNumb);

	ACube_TheBattleMasterBlockGrid* GetGrid();

	ACube_TheBattleMasterGameMode* GameMode;

	UFUNCTION(Server, Reliable)
	void Server_TheGameMode();

public:

	//virtual void BeginPlay() override;


	UFUNCTION(BlueprintCallable)
	void Movement_Test(bool bToggle);

	UFUNCTION(BlueprintCallable)
	void Attack_Test();

	UFUNCTION(BlueprintImplementableEvent)
	void Reset_Buttons_test();

	void SetAction(FString ActionName, FVector DummyPosition);

	//UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	//void ActionSelected(bool bTest);
	

	virtual void Tick(float DeltaSeconds) override;

	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	virtual void CalcCamera(float DeltaTime, struct FMinimalViewInfo& OutResult) override;


	FVector2D CameraInput;

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_SetCube(ACube_TheBattleMasterPawn* Test);

	void SetCube(ACube_TheBattleMasterPawn* Test);

	void Movement(FVector dummyPosition);

	void ResetEverything();
protected:

	void ToggleOccupied(ACube_TheBattleMasterBlock* Block, bool Bon);

	UFUNCTION(Reliable, Server)
	void Server_ToggleOccupied(ACube_TheBattleMasterBlock * Block, bool Bon);

	void HighlightMoveOptions(ACube_TheBattleMasterPawn* Pawn, ACube_TheBattleMasterBlock* Block, bool Bmove);

	void TheGameMode();

	UFUNCTION(Reliable, Server)
	void Server_HighlightMoveOptions(ACube_TheBattleMasterPawn* Pawn, ACube_TheBattleMasterBlock* Block, bool Bmove);
	


	void TriggerClick();

	void MoveCube(ACube_TheBattleMasterBlock * Block, bool bAction);


	UFUNCTION(BlueprintCallable)
	void Turn();



	UFUNCTION(Reliable, Server)
	void Server_Turn();

	/*UFUNCTION(Client, Reliable)
	void Client_TriggerClick();*/

	void TraceForBlock(const FVector& Start, const FVector& End, bool bDrawDebugHelpers);

	void DoDamage(APlayer_Cube* OwnedCube, APlayer_Cube* ToDamageCube);

	//bool IsInVacinity();

	void CubeDestroy();

	UFUNCTION(Reliable, Server)
	void Server_CubeDestroy();



	/** Returns Camera subobject **/
	FORCEINLINE class UCameraComponent* GetCamera() const { return OurCamera; }

public:
	/*Just a dummy functiont to reset the variables*/
	
	/*UFUNCTION(BlueprintImplementableEvent)
	void BP_ResetButtons();*/
	//void BP_ResetButtons_Implementation();
};
