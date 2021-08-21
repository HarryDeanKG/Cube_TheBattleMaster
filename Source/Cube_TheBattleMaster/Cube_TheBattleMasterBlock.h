// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/CapsuleComponent.h"

#include "Cube_TheBattleMasterBlock.generated.h"

/** A block that can be clicked */
UCLASS(minimalapi)
class ACube_TheBattleMasterBlock : public AActor
{
	GENERATED_BODY()

	/** Dummy root component */
	UPROPERTY(Category = Block, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class USceneComponent* DummyRoot;

	/** StaticMesh component for the clickable block */
	UPROPERTY(Category = Block, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* BlockMesh;

	UPROPERTY(Category = Block, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UCapsuleComponent* OverlapVol;

protected:

	UPROPERTY()
	class UMaterialInstanceDynamic* DynamicBaseMaterial;

public:
	ACube_TheBattleMasterBlock();

	virtual void OnConstruction(const FTransform &Transform) override;

	UFUNCTION(BlueprintCallable) void OnRep_ChangeEnergy();

	UFUNCTION(BlueprintCallable)
	void SetEnergyVariables(float BEnergy);



	UFUNCTION()
	void BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	//
	//UPROPERTY( BlueprintReadWrite, EditAnywhere)
	UPROPERTY(Transient, BlueprintReadWrite, EditAnywhere, ReplicatedUsing = OnRep_ChangeEnergy)
	float BasicEnergy = 0.f;
	float DummyEnergy;

	/** Are we currently active? */
	bool bIsActive;

	/** Pointer to white material used on the focused block */
	UPROPERTY()
	class UMaterial* BaseMaterial;

	/** Pointer to blue material used on inactive blocks */
	UPROPERTY()
	class UMaterialInstance* BlueMaterial;

	/** Pointer to orange material used on active blocks */
	UPROPERTY()
	class UMaterialInstance* OrangeMaterial;

	/** Pointer to red material used on active blocks */
	UPROPERTY()
	class UMaterialInstance* RedMaterial;

	/** Pointer to yellow material used on active blocks */
	UPROPERTY()
	class UMaterialInstance* YellowMaterial;

	/** Pointer to pathing material used on active blocks */
	UPROPERTY()
	class UMaterialInstance* PathMaterial;

	void ChangeColour(FLinearColor Colour);

	/** Grid that owns us */
	UPROPERTY()
	class ACube_TheBattleMasterBlockGrid* OwningGrid;

	/** Handle the block being clicked */
/*	UFUNCTION()
	void BlockClicked(UPrimitiveComponent* ClickedComp, FKey ButtonClicked);*/


	void HandleClicked();

	void Highlight(bool bOn);

	void CanMove(bool bOn);

	void CanRotate(bool bOn);

	void CanAttack(bool bOn, bool bImmutable);

	void ToggleOccupied(bool bOn);

	void HighlightPath(bool bOn);

	
	

	//UFUNCTION(Server, Reliable)
	//void Server_ToggleOccupied(bool bOn);

	FVector BlockPosition;

	FVector Coordinates;
	//FVector2D Coordinates;

	UPROPERTY(Replicated)
	bool bIsOccupied = false;

	//UPROPERTY(Replicated)
	bool bMove = false;

	bool bRotation = false;

	bool bAttack = false;

	bool bIsPath = false; 

public:
	/** Returns DummyRoot subobject **/
	FORCEINLINE class USceneComponent* GetDummyRoot() const { return DummyRoot; }
	/** Returns BlockMesh subobject **/
	FORCEINLINE class UStaticMeshComponent* GetBlockMesh() const { return BlockMesh; }
};



