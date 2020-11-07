// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Wall_Actor.generated.h"

UCLASS()
class CUBE_THEBATTLEMASTER_API AWall_Actor : public AActor
{
	GENERATED_BODY()
	
	/** Dummy root component */
	UPROPERTY(Category = Block, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class USceneComponent* DummyRoot;

	/** StaticMesh component for the clickable block */
	UPROPERTY(Category = Block, VisibleDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* BlockMesh;

public:	
	// Sets default values for this actor's properties
	AWall_Actor();

protected:
	// Called when the game starts or when spawned
	//virtual void BeginPlay() override;

public:	
	// Called every frame
	//virtual void Tick(float DeltaTime) override;

	float Base_Health;
	//float Replicated_Health;

	void AttachToSelect(USceneComponent * Parent, const FAttachmentTransformRules & AttachmentRules, FName SocketName);

	void ApplyDamage(AActor * DamagedActor, float Damage, const UDamageType * DamageType, AController * InstigatedBy, AActor * DamageCauser);

	bool bTemporary;

	/** Pointer to white material used on the focused block */
	UPROPERTY()
	class UMaterial* BaseMaterial;

};
