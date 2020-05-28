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
	
	//WidgetComponents
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TSubclassOf<class UUserWidget> ActionInterface;

	UPROPERTY()
	class UUserWidget* ActionInterface_Instance;
	
	//Block properties
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite)
	class ACube_TheBattleMasterBlock* CurrentBlockFocus;

	UPROPERTY(Replicated, EditAnyWhere)
	APlayer_Cube* MyCube;

	bool bDead = false;

public:

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;

	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	virtual void CalcCamera(float DeltaTime, struct FMinimalViewInfo& OutResult) override;

	void CameraMove(AActor * Cube);

	/* Test functions for UI buttons */

	UFUNCTION(BlueprintCallable)
		void Movement_Test();

	UPROPERTY(BlueprintReadWrite)
	bool bMovement = false;

	UFUNCTION(BlueprintCallable)
		void Attack_Test();

	UPROPERTY(BlueprintReadWrite)
	bool bAttack = false;
	
protected:

	/* Functions for use by the pawns */


	/* This is the main function that dictates what happens on a mouse click*/
	void TriggerClick();

	/* This looks at the curser and extracts what block it is on (but nothing else yet) */
	void TraceForBlock(const FVector& Start, const FVector& End, bool bDrawDebugHelpers);

	/* Places the cube on the map and then tells the server */
	void SetCube(ACube_TheBattleMasterPawn* Test);

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_SetCube(ACube_TheBattleMasterPawn* Test);

	/* When a cube is on a block it must be flagged as occupied and the server must be told */
	void ToggleOccupied(ACube_TheBattleMasterBlock* Block, bool Bon);

	UFUNCTION(Reliable, Server)
	void Server_ToggleOccupied(ACube_TheBattleMasterBlock * Block, bool Bon);

	/* Changes the colour of blocks based on the cube's speed. This is only for the pawn to know, but there was some overlap with other functions that required server to know movement */
	void HighlightMoveOptions(ACube_TheBattleMasterPawn* Pawn, ACube_TheBattleMasterBlock* Block, bool Bmove);

	UFUNCTION(Reliable, Server)
	void Server_HighlightMoveOptions(ACube_TheBattleMasterPawn* Pawn, ACube_TheBattleMasterBlock* Block, bool Bmove);
	
	/* Destroy the cube, then destory them server wide */
	void CubeDestroy();

	UFUNCTION(Reliable, Server)
	void Server_CubeDestroy();
	
	/** Returns Camera subobject **/
	FORCEINLINE class UCameraComponent* GetCamera() const { return OurCamera; }
};
