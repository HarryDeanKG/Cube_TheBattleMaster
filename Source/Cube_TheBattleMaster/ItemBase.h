// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AttackComponent.h"
#include "SmallMunition.h"
#include "CustomFunctions.h"

#include "ItemBase.generated.h"


UCLASS()
class CUBE_THEBATTLEMASTER_API AItemBase : public AActor
{
	GENERATED_BODY()

	/** Dummy root component */
	UPROPERTY(Category = Block, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class USceneComponent* DummyRoot;
public:
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
	// Called every frame
	//virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	virtual void DoAction(bool bMainPhase, ACube_TheBattleMasterBlock * Block);

	//This is for highlighting blocks etc...
	UFUNCTION(BlueprintCallable)
	virtual void SetActionInMotion();
	//This is for un-highlighting blocks etc...
	UFUNCTION(BlueprintCallable)
	virtual void UnSetActionInMotion();

	//Does nothing right now but can be used to reset anything that has happend
	UFUNCTION(BlueprintCallable)
	virtual void ResetAction();

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Default")
	FString WeaponName = "Weapon Default";

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Default")
	FString ActionName = "Attack";
	
	//Movement bools
	bool bCanBeUsedWithMovement;
	int32 MovementTicks;

	//AMMO should be removed
	int AmmoCurrent;

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
	
	void bEquip(bool bEquip);

	UFUNCTION(BlueprintCallable)
	void Equip();

	UFUNCTION(BlueprintCallable)
	void UnEquip();


	EAttachPoint CurrentAttachPoint;

	TArray<EAttachPoint> AttachPoints;


};