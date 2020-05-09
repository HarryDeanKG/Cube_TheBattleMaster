// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Player_Cube.generated.h"

UCLASS()
class CUBE_THEBATTLEMASTER_API APlayer_Cube : public AActor
{
	GENERATED_BODY()
	
	/** Dummy root component */
	UPROPERTY(Category = Block, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class USceneComponent* DummyRoot;

	/** StaticMesh component for the clickable block */
	UPROPERTY(Category = Block, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* BlockMesh;

public:	
	// Sets default values for this actor's properties
	APlayer_Cube();

/*
	UFUNCTION(Reliable, Server, WithValidation)
	void Server_Movement(FVector MovePosition);*/

	//Called by Server movement and vicaversa
	void Movement(FVector MovePosition);

	/** Pointer to white material used on the focused block */
	UPROPERTY()
	class UMaterial* BaseMaterial;

	UPROPERTY(EditAnywhere)
	AActor* Owner2;

public:
	/** Returns DummyRoot subobject **/
	FORCEINLINE class USceneComponent* GetDummyRoot() const { return DummyRoot; }
	/** Returns BlockMesh subobject **/
	FORCEINLINE class UStaticMeshComponent* GetBlockMesh() const { return BlockMesh; }

};
