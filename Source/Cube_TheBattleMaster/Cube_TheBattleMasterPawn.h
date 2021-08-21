// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Player_Cube.h"
#include "MySaveGame.h"
#include "Cube_TheBattleMasterPlayerController.h"
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

	UPROPERTY()
	AActor* SelectedActor = nullptr;

	UPROPERTY()
	FVector SecondSelectedPosition = FVector(0.f);

	UPROPERTY()
	float Rotation = 0;


};

USTRUCT(BlueprintType)
struct FMovement_Struct
{
	GENERATED_BODY()


	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Movement_Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector MoveDirection = FVector(0.f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bMoveOrRotate = false;

	UPROPERTY()
	AActor* SelectedActor = nullptr;

	UPROPERTY()
	float Rotation = 0;

	UPROPERTY()
	bool bIsRotationMovement = false;


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

	UPROPERTY(EditInstanceOnly, BlueprintReadWrite)
	class UStaticMeshComponent* CurrentMeshFocus;

	UPROPERTY(EditInstanceOnly, BlueprintReadWrite)
	class AActor* CurrentActorFocus;

	UFUNCTION(BlueprintCallable) void OnRep_MyCube();
	UPROPERTY(Transient, BlueprintReadWrite, EditAnywhere, ReplicatedUsing = OnRep_MyCube)
	class APlayer_Cube* MyCube;

	UPROPERTY(Transient, BlueprintReadWrite, EditAnywhere, ReplicatedUsing = OnRep_MyCube)
	class APlayer_Cube* CubeSelected;

	UPROPERTY(EditInstanceOnly, BlueprintReadWrite)
	UMySaveGame* ImportedSavedDetails;



	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Player Cube")
	TSubclassOf<APlayer_Cube> PlayerCubeClass;

	UPROPERTY(BlueprintReadWrite)
	AItemBase* SelectedItem;

	UFUNCTION(BlueprintImplementableEvent)
	void CubeMaterialUpdate();

	UFUNCTION(BlueprintImplementableEvent)
	void RefreshEnergy();
	
	bool bDead = false;

	UPROPERTY(Replicated, BlueprintReadWrite, EditAnywhere)
	bool bReady = false;

	int32 ActionNumb;
	int32 MoveNumb;
	int32 DoActionNumb;

	FString WeaponName = "default";

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void RefreshMovement();

	/*List of Actions to undertake*/
	UPROPERTY(BlueprintReadWrite)
	TMap<int32, FAction_Struct> M_ActionStructure;

	UPROPERTY(BlueprintReadWrite)
	TMap<int32, FMovement_Struct> M_MovementList;


	UPROPERTY(BlueprintReadWrite)
	TMap<FString, FString> M_PossibleActions;
	
	TArray<AActor*> FlagedForDeleation;
	TArray< ACube_TheBattleMasterBlock*> ToggleBackList;
	TArray<ACube_TheBattleMasterBlock*> Path;

	/*UPROPERTY(EditAnyWhere)
	ACube_TheBattleMasterBlock* StartingBlock;*/

	void SetAction(FString ActionName, FVector Direction);

	void AddActionToList(FAction_Struct Action);

	UFUNCTION(Reliable, Server)
	void Server_AddActionToList(FAction_Struct Action);

	void AddMovementToList(FMovement_Struct Movement);

	UFUNCTION(Server, Reliable)
	void Server_AddMovementToList(FMovement_Struct Movement);

	/*Called to initiate what action is to be done*/
	void DoAction(int int_Action);


	UFUNCTION()
	ACube_TheBattleMasterBlockGrid * GetServerGrid();

	//void AttackAction(ACube_TheBattleMasterPawn* Pawn, FString Name, FVector AttackDirection, bool bAction);

	UFUNCTION(BlueprintCallable)
	void Test();

	UFUNCTION(Server, Reliable)
	void Server_Test();

	UFUNCTION()
	ACube_TheBattleMasterBlockGrid* GetGrid();

	ACube_TheBattleMasterBlock* GetBlockFromPosition(FVector Direction);

	UPROPERTY(BlueprintReadWrite)
	ACube_TheBattleMasterBlockGrid* MyGrid;


public:

	UFUNCTION(BlueprintCallable)
	void Attack_Test(FString Name, bool bToggle);

	UFUNCTION(BlueprintCallable)
	void SetInMotionSelectedAction(AItemBase * dummyItem);

	UFUNCTION(BlueprintCallable)
	void UnSetInMotionSelectedAction(AItemBase * dummyItem);


	void ClearVars();

	UFUNCTION(BlueprintCallable)
	void StopMovement();

	UFUNCTION(BlueprintCallable)
	void NoAction();


	void ResetEverything(bool bResetPosition);

	UFUNCTION(BlueprintCallable)
	void Movement_Test(bool bToggle);

	UFUNCTION(BlueprintCallable)
	void RotationMovement_Test(bool bToggle, bool bFlip);

	float GlobalFlip = 0;

	UFUNCTION(BlueprintImplementableEvent)
	void Reset_Buttons_test();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void Ready_Button();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void Confirmation_Button();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void Confirmation_Button_Active(bool bOn);

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

	virtual void BeginPlay() override;

	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void UpdateActions();

	virtual void Tick(float DeltaSeconds) override;

	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	virtual void CalcCamera(float DeltaTime, struct FMinimalViewInfo& OutResult) override;


	FVector2D CameraInput;

//	UFUNCTION(Server, Reliable, WithValidation)

	UFUNCTION(Server, Reliable)
	void Server_SetCube(ACube_TheBattleMasterPawn* Test, ACube_TheBattleMasterBlock * Block, FImportedCube_Struct SCube, const TArray<FName>& Keys, const TArray<TSubclassOf<AItemBase>>& ItemClasses);
	
	UFUNCTION(BlueprintCallable)
	void SetCube(ACube_TheBattleMasterPawn * Pawn, ACube_TheBattleMasterBlock * Block, FImportedCube_Struct SCube, TMap<FName, TSubclassOf<AItemBase>> AttachedItems);

	UFUNCTION(BlueprintCallable)
	void SetCubeDetails(FImportedCube_Struct SCube, TMap<FName, TSubclassOf<AItemBase>> AttachedItems, APlayer_Cube * Cube);


	void Movement(FVector dummyPosition);

	void RotateCubeOld(ACube_TheBattleMasterBlock * Block, bool bAction);
	void RotateCube(float RotationAngle, bool bAction);

	float GlobalRoation = 0;

	void ToggleOccupied(ACube_TheBattleMasterBlock* Block, bool Bon);

	UFUNCTION(Reliable, Server)
	void Server_ToggleOccupied(ACube_TheBattleMasterBlock * Block, bool Bon);

	void HighlightMoveOptions(ACube_TheBattleMasterBlock * CenterBlock, bool Bmove);

	void HighlightRotationOptions(ACube_TheBattleMasterBlock * CenterBlock, bool Bmove);


	float GetQuadrent(float YawActor, float PitchActor);
	
	void HighlightAttackOptions(ACube_TheBattleMasterBlock * Block, bool bToggle, int minDistance, int maxDistance, bool bAttackImmutables, bool bCross);

	void Highlight_Path(ACube_TheBattleMasterBlock * Start, ACube_TheBattleMasterBlock * End);

	void Highlight_PathBlock(int32 X, int32 Y, int32 Z);

protected:

	void TriggerClick();

	void MoveCube(ACube_TheBattleMasterBlock * Block, bool bAction);

	UFUNCTION(BlueprintCallable)
	void Turn();

	UFUNCTION(Reliable, Server)
	void Server_Turn();

public:
	UPROPERTY(BlueprintReadWrite)
	bool bCrossHair = false;
	bool bArrow = false;
	bool bRotation = false;

	FVector ArrowEnd;
	float rangeEnd = 0.f;

	void TraceForBlock(const FVector& Start, const FVector& End, bool bDrawDebugHelpers, bool bHighlight);

	void HighlightAnActorNotABlock(const FVector & Start, const FVector & End);

	UFUNCTION()
	void TraceForCrosshair(const FVector & Start, const FVector & End, bool bToggleOn);
	FVector AttackDirection;
	
	void TraceForArrow(FVector Start, const FVector& End, bool bDrawDebugHelpers);

	void TraceForRotationCircle(FVector Start, const FVector & Direction, bool bDrawDebugHelpers);

	UPROPERTY(Category = Trace, VisibleDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* ArrowMesh;

	UPROPERTY(Category = Trace, VisibleDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* RadiusMesh;

	UPROPERTY(Category = Trace, VisibleDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	class UInstancedStaticMeshComponent* Ring;
	

	/** Pointer to white material used on the focused block */
	UPROPERTY()
	class UMaterial* BaseMaterial;

	/*UPROPERTY()
	class UMaterialInstance* RadiusMaterial;*/

protected:
	UPROPERTY()
	class UMaterialInstanceDynamic* DynamicBaseMaterial;

	UPROPERTY()
	class UMaterialInstanceDynamic* VariedCubeMaterial;

	void DoDamage(APlayer_Cube* OwnedCube, APlayer_Cube* ToDamageCube);

	//bool IsInVacinity();


	void CubeDestroy();

	UFUNCTION(Reliable, Server)
	void Server_CubeDestroy();



	/** Returns Camera subobject **/
	FORCEINLINE class UCameraComponent* GetCamera() const { return OurCamera; }

public:

	TArray<ADecalActor*> RemoveDecals;

	/*Just a dummy functiont to reset the variables*/
	
	/*UFUNCTION(BlueprintImplementableEvent)
	void BP_ResetButtons();*/
	//void BP_ResetButtons_Implementation();
};
