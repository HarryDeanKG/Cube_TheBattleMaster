// Fill out your copyright notice in the Description page of Project Settings.

#include "ItemBase.h"
#include "Cube_TheBattleMasterPawn.h"
#include "Player_Cube.h"
#include "AttachmentComponent.h"
#include "Components/StaticMeshComponent.h"

// Sets default values
AItemBase::AItemBase()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Create dummy root scene component
	DummyRoot = CreateDefaultSubobject<USceneComponent>(TEXT("Dummy0"));
	//DummyRoot->SetIsReplicated(true);
	RootComponent = DummyRoot;

	//Create Attack component
	//AttackComponent = CreateDefaultSubobject<UAttackComponent>(TEXT("BlockAttackComeponent0"));
	//AttackComponent->SetupAttachment(DummyRoot);

	// Create static mesh component
	BlockMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BlockMesh0"));
	//BlockMesh->SetIsReplicated(true);
	//BlockMesh->SetStaticMesh(ConstructorStatics.PlaneMesh.Get());
	BlockMesh->SetRelativeScale3D(FVector(0.25f, 0.25f, 0.25f));
	BlockMesh->SetRelativeLocation(FVector(0.f, 0.f, 25.f));
	//BlockMesh->SetMaterial(0, ConstructorStatics.BaseMaterial.Get());
	BlockMesh->SetupAttachment(DummyRoot);

	

}

// Called when the game starts or when spawned
void AItemBase::BeginPlay()
{
	Super::BeginPlay();

	APlayer_Cube* Cube = Cast<ACube_TheBattleMasterPawn>(GetOwner())->MyCube;
	Cube->GetAttachComponent()->AddToInventory(this);
}

void AItemBase::SetSlotName(FName SlotName)
{
	EquipSocketName = SlotName;
}

void AItemBase::bEquip(bool bEquip)
{
	APlayer_Cube* Cube = Cast<ACube_TheBattleMasterPawn>(GetOwner())->MyCube;

	if (bEquip) {
		FAttachmentTransformRules Trans = FAttachmentTransformRules
		(
			EAttachmentRule::KeepRelative,
			EAttachmentRule::KeepRelative,
			EAttachmentRule::KeepRelative,
			true
		);
		AttachToComponent(Cube->GetBlockMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, EquipSocketName);
		//Cube->M_SlotsRefference.Add(this->GetName(), this);
		Cube->M_SlotsRefference.Add(this->WeaponName, this);
	}
	else {
		FDetachmentTransformRules Trans = FDetachmentTransformRules
		(
			EDetachmentRule::KeepRelative,
			EDetachmentRule::KeepRelative,
			EDetachmentRule::KeepRelative,
			true
		);
		DetachFromActor(Trans);
		//Cube->M_SlotsRefference.Remove(this->GetName());
		Cube->M_SlotsRefference.Add(this->WeaponName, this);
		
	}//UE_LOG(LogTemp, Warning, TEXT("%s"), *this->WeaponName); UE_LOG(LogTemp, Warning, TEXT("%s"), *this->GetName());
}

void AItemBase::Equip()
{
	bEquip(true);
}

void AItemBase::UnEquip()
{
	bEquip(false);
}


void AItemBase::DoAction(bool bMainPhase, ACube_TheBattleMasterBlock* Block){
	UE_LOG(LogTemp, Warning, TEXT("Default Action for %s"), *GetName());

	const FRotator SpawnRotation = FRotator(0, 0, 0);

	const FVector SpawnLocation = GetActorLocation();
	ASmallMunition* Bullet = GetWorld()->SpawnActor<ASmallMunition>(SmallMunitionClass, SpawnLocation, FRotator(0, 0, 0));

	Bullet->SetOwner(GetOwner());
	Bullet->SetReplicates(bMainPhase);
	Bullet->Direction = Block->GetActorLocation();

}

void AItemBase::SetActionInMotion()
{
	UE_LOG(LogTemp, Warning, TEXT("Default SetAction"));
}

void AItemBase::UnSetActionInMotion()
{
	UE_LOG(LogTemp, Warning, TEXT("Default SetAction"));
}

void AItemBase::ResetAction()
{
}
