// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

//#include "D:/Unreal/UE_4.26Chaos/Engine/Plugins/FX/Niagara/Source/Niagara/Public/NiagaraComponent.h"

#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"

//#include "GeometryCollection/GeometryCollectionObject.h"
//#include "GeometryCollection/GeometryCollectionComponent.h"
//#include "GeometryCollection/StaticMeshSimulationComponent.h"
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
		
	//UPROPERTY(Category = Block, VisibleDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	//class UStaticMeshSimulationComponent* Test;

	UPROPERTY(Category = Decal, VisibleDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	UMaterialInterface* LaserHitDecal;

public:	
	// Sets default values for this actor's properties
	AWall_Actor();

	//Niagara components
	UPROPERTY(EditAnywhere)
	UNiagaraSystem* NiagaraParticleHit;

	UPROPERTY(EditAnywhere)
	UNiagaraSystem* NiagaraLaserHit;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	//virtual void Tick(float DeltaTime) override;

	float Base_Health;

	UPROPERTY(Replicated)
	float Replicated_Health;

	void AttachToSelect(USceneComponent * Parent, const FAttachmentTransformRules & AttachmentRules, FName SocketName);

	UFUNCTION()
	void Damage(AActor * DamagedActor, float Damage, const UDamageType * DamageType, AController * InstigatedBy, AActor * DamageCauser);
	UFUNCTION()
	void PointDamage(AActor* DamagedActor, float Damage, class AController* InstigatedBy, FVector HitLocation, class UPrimitiveComponent* FHitComponent, FName BoneName, FVector ShotFromDirection, const class UDamageType* DamageType, AActor* DamageCauser);
	//void PointDamage(AActor * DamagedActor, float BaseDamage, const FVector & HitFromDirection, const FHitResult & HitInfo, AController * EventInstigator, AActor * DamageCauser, TSubclassOf < class UDamageType > DamageTypeClass);


	float DefaultDamage;

	UPROPERTY(Replicated)
	bool bTemporary = false;

	bool IsDestroyed;
	/** Pointer to white material used on the focused block */


	UFUNCTION()
	void Trigger(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
	
	UPROPERTY()
	class UMaterial* BaseMaterial;

};
