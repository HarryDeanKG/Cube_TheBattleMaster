// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Cube_TheBattleMasterBlock.h"
#include "Cube_TheBattleMasterBlockGrid.generated.h"

/** Class used to spawn blocks and manage score */
UCLASS(minimalapi)
class ACube_TheBattleMasterBlockGrid : public AActor
{
	GENERATED_BODY()

	/** Dummy root component */
	UPROPERTY(Category = Grid, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class USceneComponent* DummyRoot;


public:
	ACube_TheBattleMasterBlockGrid();

	/** Number of blocks along each side of grid */
	UPROPERTY(Category=Grid, EditAnywhere, BlueprintReadOnly)
	int32 Size;

	/* 0 - has no double ups
	1 - half the map
	2 - quaterthe map
	3 - third the map
	6 - All hex triangles are copyed
	*/
	UPROPERTY(Category = Grid, EditAnywhere, BlueprintReadOnly)
	int32 Symmetry;

	/** Spacing of blocks */
	UPROPERTY(Category=Grid, EditAnywhere, BlueprintReadOnly)
	float BlockSpacing;

	UPROPERTY(Category = Grid, EditAnywhere, BlueprintReadOnly)
	TMap<FVector, ACube_TheBattleMasterBlock *> Grid;
	
	UPROPERTY(Category = Grid, EditAnywhere)
	TMap<FVector, FVector> GridReference;

	TArray<FVector> StartPositions;



protected:
	// Begin AActor interface
	virtual void BeginPlay() override;

	// End AActor interface

public:
	UFUNCTION(BlueprintCallable)
	void MakeTheGrid();

	void GetForwardDirection(ACube_TheBattleMasterBlock * CenterBlock, int32 RungLevel, TArray<ACube_TheBattleMasterBlock*>& OutBlocks, int Direction);

	void GetSectorRing(ACube_TheBattleMasterBlock * CenterBlock, int32 RungLevel, float Direction, TArray<ACube_TheBattleMasterBlock*>& OutBlocks);

	UFUNCTION(BlueprintCallable)
	void GetRing(ACube_TheBattleMasterBlock* CenterBlock, int32 RungLevel, TArray<ACube_TheBattleMasterBlock*> &OutBlocks);

	void GetAreaWithoutCenter(ACube_TheBattleMasterBlock * CenterBlock, int32 RungLevel, TArray<ACube_TheBattleMasterBlock*>& OutBlocks);

	void GetAreaWithCenter(ACube_TheBattleMasterBlock * CenterBlock, int32 RungLevel, TArray<ACube_TheBattleMasterBlock*>& OutBlocks);

	ACube_TheBattleMasterBlock* GetCube(int X, int Y, int Z);


	void HighlightAreaWithCenter(ACube_TheBattleMasterBlock* CenterBlock, int32 RungLevel);

	void HighlightAreaWithoutCenter(ACube_TheBattleMasterBlock* CenterBlock, int32 RungLevel);

	/** Returns DummyRoot subobject **/
	FORCEINLINE class USceneComponent* GetDummyRoot() const { return DummyRoot; }

};



