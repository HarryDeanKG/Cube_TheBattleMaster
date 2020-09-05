// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Player_Cube.h"
#include "SmallMunition.h"
#include "Cube_TheBattleMasterBlock.h"
#include "Cube_TheBattleMasterGameMode.h"
#include "Cube_TheBattleMasterPawn.generated.h"

//class ASmallMunitionBase;

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
	float MoveSpeed;

	// move the camera forward
	void OnMoveForward(float value);

	// move the camera right
	void OnMoveRight(float value);

	//move camera in
	void OnMoveIn(float value);

	//Block properties
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite)
	class ACube_TheBattleMasterBlock* CurrentBlockFocus;

	//UPROPERTY(Replicated, EditAnyWhere)
	//APlayer_Cube* MyCube;

	

	UFUNCTION(BlueprintCallable) void OnRep_MyCube();
	UPROPERTY(Transient, BlueprintReadWrite, EditAnywhere, ReplicatedUsing = OnRep_MyCube)
	class APlayer_Cube* MyCube;

	UPROPERTY(Transient, BlueprintReadWrite, EditAnywhere, ReplicatedUsing = OnRep_MyCube)
	class APlayer_Cube* CubeSelected;



	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Player Cube")
	TSubclassOf<APlayer_Cube> PlayerCubeClass;



	UFUNCTION(BlueprintImplementableEvent)
	void CubeMaterialUpdate();

	
	bool bDead = false;

	UPROPERTY(Replicated, BlueprintReadWrite, EditAnywhere)
	bool bReady = false;

	int32 ActionNumb;
	int32 DoActionNumb;

	/*List of Actions to undertake*/
	UPROPERTY(BlueprintReadWrite)
	TMap<int32, FString> M_Action_Name;

	UPROPERTY(BlueprintReadWrite)
	TMap<int32, FVector> M_Action_Pos;

	UPROPERTY()
	TMap<FString, FString> M_PossibleActions;

	UPROPERTY(EditAnyWhere)
	ACube_TheBattleMasterBlock* StartingBlock;

	/*Called to initiate what action is to be done*/
	void DoAction(int int_Action);

	void AttackAction(ACube_TheBattleMasterPawn* Pawn, FVector AttackDirection, bool bAction);

	UFUNCTION(Server, Reliable)
	void Server_AttackAction(ACube_TheBattleMasterPawn* Pawn, FVector AttackDirection, bool bAction);


	void Test();


	UFUNCTION()
	ACube_TheBattleMasterBlockGrid* GetGrid();


	ACube_TheBattleMasterBlockGrid* MyGrid;


public:

	/*Attack Section*/
	UFUNCTION(BlueprintCallable)
	void Attack_Test(bool bToggle);


	UPROPERTY(Replicated, EditAnywhere, BlueprintReadOnly, Category = "Projectile Type")
	TSubclassOf<ASmallMunition> SmallMunitionClass;

	UFUNCTION(BlueprintCallable)
	void Movement_Test(bool bToggle);


	UFUNCTION(BlueprintImplementableEvent)
	void Reset_Buttons_test();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void Ready_Button();

	void ResetEverything();
	FString String_Action;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void Cancel_Button();

	void SetAction(FString ActionName, FVector DummyPosition);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void ActionSelected();
	
	UFUNCTION(BlueprintNativeEvent)
	void Confirm_Actions();

	UFUNCTION(BlueprintImplementableEvent)
	void CubeMade();

	UFUNCTION(BlueprintImplementableEvent)
	void Waiting();

	//virtual void BeginPlay() override;

	

	virtual void Tick(float DeltaSeconds) override;

	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	virtual void CalcCamera(float DeltaTime, struct FMinimalViewInfo& OutResult) override;


	FVector2D CameraInput;

//	UFUNCTION(Server, Reliable, WithValidation)

	UFUNCTION(Server, Reliable)
	void Server_SetCube(ACube_TheBattleMasterPawn* Test, ACube_TheBattleMasterBlock * Block);

	UFUNCTION(BlueprintCallable)
	void SetCube(ACube_TheBattleMasterPawn * Pawn, ACube_TheBattleMasterBlock * Block);

	void Movement(FVector dummyPosition);

protected:

	void ToggleOccupied(ACube_TheBattleMasterBlock* Block, bool Bon);

	UFUNCTION(Reliable, Server)
	void Server_ToggleOccupied(ACube_TheBattleMasterBlock * Block, bool Bon);

	void HighlightMoveOptions(ACube_TheBattleMasterBlock* Block, bool Bmove);

	UFUNCTION(Reliable, Server)
	void Server_HighlightMoveOptions(ACube_TheBattleMasterBlock* Block, bool Bmove);
	
	void HighlightAttackOptions(ACube_TheBattleMasterBlock* Block, bool bToggle);

	UFUNCTION(Reliable, Server)
	void Server_HighlightAttackOptions(ACube_TheBattleMasterBlock* Block, bool bToggle);

	void Highlight_Block(int32 dummyX, int32 dummyY, bool bToggle);


	


	


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
