// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/CapsuleComponent.h"

#include "SmallMunition.generated.h"

UCLASS()
class CUBE_THEBATTLEMASTER_API ASmallMunition : public AActor
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
	ASmallMunition();

protected:
	// Called when the game starts or when spawned
	//virtual void BeginPlay() override;

	//Called on construction
	virtual void OnConstruction(const FTransform & Transform) override;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	// create trigger capsule
	UPROPERTY(VisibleAnywhere, Category = "Trigger Capsule")
	class UCapsuleComponent* TriggerCapsule;

	//UFUNCTION()
	//void BeginOverlap( AActor* OverlappedActor, AActor* OtherActor );

	//UFUNCTION()
	//void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

public:	

	//Set the variables used in calculations
	/*UPROPERTY(EditAnywhere)
	FVector Velocity = FVector(100.f);*/

	FVector StartPosition;
	float StartTime;

	//Properties of the projectile
	float Speed;

	UPROPERTY(BlueprintReadWrite)
	FVector Direction= FVector(0.f);

	/** Returns DummyRoot subobject **/
	FORCEINLINE class USceneComponent* GetDummyRoot() const { return DummyRoot; }
	/** Returns BlockMesh subobject **/
	FORCEINLINE class UStaticMeshComponent* GetBlockMesh() const { return BlockMesh; }
};
