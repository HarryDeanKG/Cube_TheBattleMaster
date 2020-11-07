// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Player_Cube.h"
#include "Cube_TheBattleMasterBlock.h"
#include "Cube_TheBattleMasterGameMode.h"
#include "Cube_TheBattleMasterPawn.generated.h"

USTRUCT(BlueprintType)
struct FAction_Struct
{
	GENERATED_BODY()


	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Action_Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector Action_Position = FVector(0.f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Action_Weapon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	AItemBase* Item = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector MovementDuringAction = FVector(0.f);;
};

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


	UFUNCTION(BlueprintCallable) void OnRep_MyCube();
	UPROPERTY(Transient, BlueprintReadWrite, EditAnywhere, ReplicatedUsing = OnRep_MyCube)
	class APlayer_Cube* MyCube;

	UPROPERTY(Transient, BlueprintReadWrite, EditAnywhere, ReplicatedUsing = OnRep_MyCube)
	class APlayer_Cube* CubeSelected;



	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Player Cube")
	TSubclassOf<APlayer_Cube> PlayerCubeClass;

	UPROPERTY(BlueprintReadWrite)
	AItemBase* SelectedItem;

	UFUNCTION(BlueprintImplementableEvent)
	void CubeMaterialUpdate();

	
	bool bDead = false;

	UPROPERTY(Replicated, BlueprintReadWrite, EditAnywhere)
	bool bReady = false;

	int32 ActionNumb;
	int32 MoveNumb;
	int32 DoActionNumb;

	FString WeaponName = "default";

	UFUNCTION(BlueprintImplementableEvent,BlueprintCallable)
	void RefreshMovement();

	/*List of Actions to undertake*/
	UPROPERTY(BlueprintReadWrite)
	TMap<int32, FAction_Struct> M_ActionStructure;

	UPROPERTY(BlueprintReadWrite)
	TMap<int32, FVector> M_MovementList;

	UPROPERTY(BlueprintReadWrite)
	TMap<FString, FString> M_PossibleActions;
	
	TArray<AActor*> FlagedForDeleation;
	TArray< ACube_TheBattleMasterBlock*> ToggleBackList;
	TArray<ACube_TheBattleMasterBlock*> Path;

	UPROPERTY(EditAnyWhere)
	ACube_TheBattleMasterBlock* StartingBlock;

	void SetAction(FString ActionName, ACube_TheBattleMasterBlock * Block);

	/*Called to initiate what action is to be done*/
	void DoAction(int int_Action);

	void AttackAction(ACube_TheBattleMasterPawn * Pawn, FString Name, ACube_TheBattleMasterBlock * Block, bool bAction);

	//void AttackAction(ACube_TheBattleMasterPawn* Pawn, FString Name, FVector AttackDirection, bool bAction);


	void Test();


	UFUNCTION()
	ACube_TheBattleMasterBlockGrid* GetGrid();

	ACube_TheBattleMasterBlock * GetBlockFromPosition(FVector Direction);


	ACube_TheBattleMasterBlockGrid* MyGrid;


public:

	UFUNCTION(BlueprintCallable)
	void Attack_Test(FString Name, bool bToggle);

	UFUNCTION(BlueprintCallable)
	void SetInMotionSelectedAction(AItemBase * dummyItem);

	void ClearVars();

	UFUNCTION(BlueprintCallable)
	void StopMovement();

	UFUNCTION(BlueprintCallable)
	void NoAction();

	void ResetEverything(bool bResetPosition);

	UFUNCTION(BlueprintCallable)
	void Movement_Test(bool bToggle);

	UFUNCTION(BlueprintImplementableEvent)
	void Reset_Buttons_test();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void Ready_Button();

	FString String_Action;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void Cancel_Button();

	//void SetAction(FString ActionName, FVector DummyPosition);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void ActionSelected();
	
	UFUNCTION(BlueprintNativeEvent)
	void Confirm_Actions();

	UFUNCTION(BlueprintImplementableEvent)
	void CubeMade();

	UFUNCTION(BlueprintImplementableEvent)
	void Waiting();

	//virtual void BeginPlay() override;

	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void UpdateActions();

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



	void ToggleOccupied(ACube_TheBattleMasterBlock* Block, bool Bon);

	UFUNCTION(Reliable, Server)
	void Server_ToggleOccupied(ACube_TheBattleMasterBlock * Block, bool Bon);

	void HighlightMoveOptions(ACube_TheBattleMasterBlock* Block, bool Bmove);

	UFUNCTION(Reliable, Server)
	void Server_HighlightMoveOptions(ACube_TheBattleMasterBlock* Block, bool Bmove);
	
	void HighlightAttackOptions(ACube_TheBattleMasterBlock* Block, bool bToggle, int distance, bool bAttackImmutables);

	UFUNCTION(Reliable, Server)
	void Server_HighlightAttackOptions(ACube_TheBattleMasterBlock* Block, bool bToggle, int distance, bool bAttackImmutables);

	void Highlight_Block(int32 dummyX, int32 dummyY, bool bToggle, bool bAttackImmutables);


protected:


	


	void Highlight_Path(ACube_TheBattleMasterBlock * Start, ACube_TheBattleMasterBlock * End);

	void Highlight_PathBlock(int32 X, int32 Y);

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
