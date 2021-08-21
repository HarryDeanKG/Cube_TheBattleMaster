// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/TimelineComponent.h"
#include "GameFramework/Actor.h"
#include "ItemBase.h"
#include "Components/CapsuleComponent.h"


//#include "D:/Unreal/UE_4.26Chaos/Engine/Plugins/FX/Niagara/Source/Niagara/Public/NiagaraComponent.h"


#include "ShieldSegment.generated.h"

class UCurveFloat;

UCLASS()
class CUBE_THEBATTLEMASTER_API AShieldSegment : public AActor
{

	/** Dummy root component */
	UPROPERTY(Category = Block, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class USceneComponent* DummyRoot;

	UPROPERTY(Category = Block, VisibleDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* BlockMesh;

	UPROPERTY(Category = Block, VisibleDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	class UCapsuleComponent* CapsuleComponent;

	
	FTimeline CurveTimeline;
	UPROPERTY(Category = "Timeline", VisibleDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	UCurveFloat* Curve_Amount;

	FTimeline CurveTimeline_Hit;
	UPROPERTY(Category = "Timeline", VisibleDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	UCurveFloat* Curve_Hit;

	FTimeline CurveTimeline_Laser;
	UPROPERTY(Category = "Timeline", VisibleDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	UCurveFloat* Curve_Laser;

	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AShieldSegment();

	//Timeline stuff
	UFUNCTION()
	void DoTheCurve();


	UFUNCTION(NetMulticast, Reliable)
	void TimelineProgress(float Value);

	UFUNCTION(NetMulticast, Reliable)
	void TimelineProgress_Hit(float Value);

	UFUNCTION(NetMulticast, Reliable)
	void TimelineProgress_Laser(float Value);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	

//	UNiagaraComponent* FX_Niagra;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void AttachToSelect(USceneComponent * Parent, const FAttachmentTransformRules & AttachmentRules, FName SocketName);


	//////////////////////////
	AItemBase* ItemOwner;

	UFUNCTION()
	void Damage(AActor * DamagedActor, float Damage, const UDamageType * DamageType, AController * InstigatedBy, AActor * DamageCauser);

	UFUNCTION()
	void PointDamage(AActor * DamagedActor, float Damage, AController * InstigatedBy, FVector HitLocation, UPrimitiveComponent * FHitComponent, FName BoneName, FVector ShotFromDirection, const UDamageType * DamageType, AActor * DamageCauser);
	
	UFUNCTION()
	void Trigger(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	UFUNCTION()
	void Overlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	UFUNCTION()
	void OverlapOver(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	//void ApplyDamage();
	
	UFUNCTION()
	void LetsDestroy();

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite)
	bool IsDestroyed;

	UPROPERTY();
	bool IsHit;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite)
	float MaxHealth;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite)
	float CurrentHealth;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite)
	float DefaultDamage;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite)
	float DefaultImpulse;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite)
	bool IsTriggeredEnabled;

	/** Pointer to white material used on the focused block */
	UPROPERTY()
	class UMaterial* BaseMaterial;

	UFUNCTION(BlueprintCallable, NetMulticast, Reliable)
	void LaserHit(FVector ImpactPoint);

	//Niagara components
	UPROPERTY(EditAnywhere)
	UNiagaraSystem* NiagaraParticleHit;

	UPROPERTY(EditAnywhere)
	UNiagaraSystem* NiagaraLaserHitExplosion;

	/** Returns BlockMesh subobject **/
	FORCEINLINE class UStaticMeshComponent* GetBlockMesh() const { return BlockMesh; }

	//FORCEINLINE class UCapsuleComponent* GetCapsule() const { return CapsuleComponent; }

private:
	UPROPERTY()
	class UMaterialInstanceDynamic* DynamicBaseMaterial;
};
