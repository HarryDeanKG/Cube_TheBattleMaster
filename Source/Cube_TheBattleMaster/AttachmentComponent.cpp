// Fill out your copyright notice in the Description page of Project Settings.

#include "AttachmentComponent.h"
#include "CustomFunctions.h"


// Sets default values for this component's properties
UAttachmentComponent::UAttachmentComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	FAttachInfo_Struct Socket;
	Socket.AttachPoint = EAttachPoint::AP_Socket1;
	SocketStates.Add(Socket);
	Socket.AttachPoint = EAttachPoint::AP_Socket2;
	SocketStates.Add(Socket);
	Socket.AttachPoint = EAttachPoint::AP_Socket3;
	SocketStates.Add(Socket);
	Socket.AttachPoint = EAttachPoint::AP_Socket4;
	SocketStates.Add(Socket);
	Socket.AttachPoint = EAttachPoint::AP_Socket5;
	SocketStates.Add(Socket);
	Socket.AttachPoint = EAttachPoint::AP_Socket6;
	SocketStates.Add(Socket);

	FAmmoInfo_Struct Ammunition;
	Ammunition.AmmoType = EAmmunitionType::AT_Rockets;
	Ammunition.AmmoCount = 5;
	Ammo.Add(Ammunition);
	Ammunition.AmmoType = EAmmunitionType::AT_Bullets;
	Ammunition.AmmoCount = 20;
	Ammo.Add(Ammunition);
	Ammunition.AmmoType = EAmmunitionType::AT_Energy;
	Ammunition.AmmoCount = 50;
	Ammo.Add(Ammunition);

	// ...
}

void UAttachmentComponent::Equip(AItemBase* Item, FName SocketName)
{
	MarkSocketsUsed(UCustomFunctions::GetSocketPointByName(SocketName), Item);
	
	RemoveFromInventory(Item->GetClass());
	Item->SetSlotName(SocketName);
	Item->bEquip(true);
	UpdateInventoryStateUpdate.Broadcast();
}

void UAttachmentComponent::UnEquip(AItemBase* Item, FName SocketName)
{
	MarkSocketAvaliable(UCustomFunctions::GetSocketPointByName(SocketName));
	AddToInventory(Item->GetClass());
	Item->bEquip(false);
	UpdateInventoryStateUpdate.Broadcast();
	
}


void UAttachmentComponent::AddToInventory(TSubclassOf<AItemBase> Item)
{
	InventaryItems.Add(Item);
	UpdateInventoryStateUpdate.Broadcast();
}

void UAttachmentComponent::RemoveFromInventory(TSubclassOf<AItemBase> Item)
{
	//InventaryItems.Remove(Item);
	InventaryItems.RemoveSingle(Item);
	UpdateInventoryStateUpdate.Broadcast();

}

void UAttachmentComponent::MarkSocketsUsed(EAttachPoint Point, AItemBase* Item)
{
	int32 LocalInt;
	FAttachInfo_Struct LocalItem;
	//UE_LOG(LogTemp, Warning, TEXT("%s"), UEnum::GetDisplayValueAsText(Point.GetValue()).ToString());
	int32 i = 0;
	for (auto& sockets : SocketStates) {
//		UE_LOG(LogTemp, Warning, TEXT("Point %s"), *UCustomFunctions::GetSocketNameByPoint(Point).ToString());
//		UE_LOG(LogTemp, Warning, TEXT("Sockets %s"), *UCustomFunctions::GetSocketNameByPoint(sockets.AttachPoint).ToString());
		
		if (Point == sockets.AttachPoint) {
			LocalInt = i;
			LocalItem.AttachPoint = sockets.AttachPoint;
			LocalItem.InUse = true;
			LocalItem.ItemRef = Item;
			SocketStates[LocalInt] = LocalItem;
			break;
		}i++;
	}	
}

void UAttachmentComponent::MarkSocketAvaliable(EAttachPoint Point)
{
	int32 LocalInt;
	FAttachInfo_Struct LocalItem;

	int32 i = 0;
	for (auto& sockets : SocketStates) {
		if (Point == sockets.AttachPoint) {
			LocalInt = i;
			//LocalInt = SocketStates.IndexOfByKey(sockets);
			LocalItem.AttachPoint = sockets.AttachPoint;
			LocalItem.InUse = false;
			SocketStates[LocalInt] = LocalItem;
			break;
		}i++;
	}	
}

void UAttachmentComponent::AddAmmo(EAmmunitionType Type, int32 Amount)
{
	int32 LocalInt;
	FAmmoInfo_Struct LocalAmmoInfo;

	int32 i = 0;
	for (auto& ammunition : Ammo) {
		if (ammunition.AmmoType == Type){
			LocalInt = i;
			//LocalInt = SocketStates.IndexOfByKey(ammunition);
			LocalAmmoInfo.AmmoType = ammunition.AmmoType;
			LocalAmmoInfo.AmmoCount = ammunition.AmmoCount + Amount;
			
			Ammo[LocalInt] = LocalAmmoInfo;
			
			break;
		}i++;
	}	
}

void UAttachmentComponent::GetCurrentAmmo_Implementation(EAmmunitionType Type, int32 & int_Ammo)
{
	for (auto& ammunition : Ammo) {
		if (ammunition.AmmoType == Type) { int_Ammo = ammunition.AmmoCount; }
	}	

}
