// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CustomFunctions.h"
#include "Engine/DecalActor.h"

#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"

//#include "AttackComponent.h"
#include "SmallMunition.h"
#include "Player_Cube.h"
#include "ItemBase.generated.h"


UCLASS()
class CUBE_THEBATTLEMASTER_API AItemBase : public AActor
{
	GENERATED_BODY()
private:
	/** Dummy root component */
	UPROPERTY(Category = Block, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class USceneComponent* DummyRoot;

	/** StaticMesh component for the clickable block */
	UPROPERTY(Category = Block, VisibleDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* BlockMesh;

	//UPROPERTY(Category = "Attack", BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	//class UAttackComponent* AttackComponent;



public:
	// Sets default values for this actor's properties
	AItemBase();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Enum)
	EAmmunitionType EAmmunitionType_Enum;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Enum)
	EAttachPoint EAttachPoint_Enum;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:

	virtual void Tick(float DeltaTime) override;


	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class APlayer_Cube* Cube;

	UPROPERTY(BlueprintReadWrite)
	class ACube_TheBattleMasterPawn* MyOwner;

	// Called every frame
	//virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	virtual void DoAction(bool bMainPhase, FVector Direction);

	//This is for highlighting blocks etc...
	UFUNCTION(BlueprintCallable)
	virtual void SetActionInMotion();

	//This is for un-highlighting blocks etc...
	UFUNCTION(BlueprintCallable)
	virtual void UnSetActionInMotion();

	//Does nothing right now but can be used to reset anything that has happend
	UFUNCTION(BlueprintCallable)
	virtual void ResetAction();

	UFUNCTION()
	void ApplyDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser);

	UFUNCTION()
	void ApplyRadialEffects(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, FVector Origin, FHitResult HitInfo, class AController* InstigatedBy, AActor* DamageCauser);


	UPROPERTY(Replicated)
	float Replicated_Health = 100.f;


	UFUNCTION(BlueprintCallable)
	virtual void EndAction();

	UFUNCTION()
	virtual void StartAction(FToDo_Struct Action);

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Default")
	FString WeaponName = "Weapon Default";

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Default")
	FString ActionName = "Attack";
	
	//Movement bools
	bool bCanBeUsedWithMovement;

	//How many times it does the action 
	int ActionItteration;

	// Variables for action range
	float AttackRange;
	float AttackRangeMin;
	//AMMO should be removed
	int AmmoCurrent;
	//Decide whether the item has something at the end (Currently only works for 0 itteration items).
	bool bEndAction;
	bool bStartAction;

	UPROPERTY(Replicated)
	FRotator DefaultRotation;

	EAmmunitionType AmmoType;

	//NEED TO REMOVE THIS To BE More GENERIC
	//UPROPERTY(Replicated, EditAnywhere, BlueprintReadOnly, Category = "Projectile Type")
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Projectile Type")
	TSubclassOf<ASmallMunition> SmallMunitionClass;


	//Equipment section: Get the socket name and attach it
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slots")
	FName EquipSocketName;

	UFUNCTION(BlueprintCallable, Category = "Slots")
	void SetSlotName(FName SlotName);
	

	//Equipment Functions
	void bEquip(bool bEquip);

	UFUNCTION(BlueprintCallable)
	void Equip();

	UFUNCTION(BlueprintCallable)
	void UnEquip();

	UFUNCTION()
	virtual bool IsInRange(AActor* SelectedActor);

	//Attach properties
	UPROPERTY(BlueprintReadWrite)
	EAttachPoint CurrentAttachPoint;

	TArray<EAttachPoint> AttachPoints;

	UPROPERTY(BlueprintReadWrite)
	FVector OutwardLocation;
	/** Pointer to white material used on the focused block */
	UPROPERTY()
	class UMaterial* BaseMaterial;

	/** Returns BlockMesh subobject **/
	FORCEINLINE class UStaticMeshComponent* GetBlockMesh() const { return BlockMesh; }

};
