// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ItemBase.h"
#include "CustomFunctions.h"
#include "AttachmentComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class CUBE_THEBATTLEMASTER_API UAttachmentComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Enum)
	EAttachPoint EAttachPoint_Enum;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Enum)
	EAmmunitionType EAmmunitionType_Enum;

	// Sets default values for this component's properties
	UAttachmentComponent();

public:

	UFUNCTION(BlueprintCallable)
	void Equip(AItemBase* Item, FName SocketName);

	UFUNCTION(BlueprintCallable)
	void UnEquip(AItemBase* Item, FName SocketName);

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Sockets")
	TArray<FAttachInfo_Struct> SocketStates;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Inventory")
	TArray<AItemBase*> InventaryItems;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Ammo")
	TArray<FAmmoInfo_Struct> Ammo;


	void MarkSocketsUsed(EAttachPoint Point, AItemBase* Item);

	void MarkSocketAvaliable(EAttachPoint Point);

	void AddAmmo(EAmmunitionType Type, int32 Amount);

	UFUNCTION(BlueprintCallable, BlueprintPure = true, BlueprintNativeEvent)
	void GetCurrentAmmo(EAmmunitionType Type, int32& int_Ammo);

	void SetEquipedWeapon(AItemBase*  Item, FName SocketName);

	void ClearEquipedWeapon(AItemBase* Item, FName SocketName);

	UFUNCTION(BlueprintCallable)
	void AddToInventory(AItemBase* Item);

	void RemoveFromInventory(AItemBase* Item);

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FUpdateInventoryStateUpdate);
	UPROPERTY(BlueprintAssignable, Category = "Interaction")
	FUpdateInventoryStateUpdate UpdateInventoryStateUpdate;
};
