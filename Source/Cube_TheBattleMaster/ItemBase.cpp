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

	bReplicates = true;

	//// Structure to hold one-time initialization
	struct FConstructorStatics
	{
		ConstructorHelpers::FObjectFinderOptional<UStaticMesh> PlaneMesh;
		ConstructorHelpers::FObjectFinderOptional<UMaterial> BaseMaterial;
		FConstructorStatics()
			: PlaneMesh(TEXT("/Game/Puzzle/Meshes/ItemCube.ItemCube"))
			, BaseMaterial(TEXT("/Game/Puzzle/Meshes/BaseMaterial.BaseMaterial"))
		{
		}
	};
	static FConstructorStatics ConstructorStatics;
	BaseMaterial = ConstructorStatics.BaseMaterial.Get();

	// Create dummy root scene component
	DummyRoot = CreateDefaultSubobject<USceneComponent>(TEXT("Dummy0"));
	//DummyRoot->SetIsReplicated(true);
	RootComponent = DummyRoot;

	// Create static mesh component
	BlockMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BlockMesh0"));
	//BlockMesh->SetIsReplicated(true);
	BlockMesh->SetStaticMesh(ConstructorStatics.PlaneMesh.Get());
	BlockMesh->SetRelativeScale3D(FVector(0.25f, 0.25f, 0.25f));
	BlockMesh->SetRelativeLocation(FVector(30.f, 0.f, 0.f));
	//BlockMesh->SetRelativeRotation(FRotator(0.f, 90.f, 0.f));
	BlockMesh->SetMaterial(0, ConstructorStatics.BaseMaterial.Get());
	BlockMesh->SetupAttachment(DummyRoot);	

	OutwardLocation = BlockMesh->GetSocketLocation("S_Outward") * 0.25;

	AttackRange = 7.f;
	AttackRangeMin = 7.f;
	ActionItteration = 1;
	bEndAction = false;
}

// Called when the game starts or when spawned
void AItemBase::BeginPlay()
{
	Super::BeginPlay();
	//Required for character creation


	UE_LOG(LogTemp, Warning, TEXT("Owner Is %s"), *GetOwner()->GetName());
	if (Cast<ACube_TheBattleMasterPawn>(GetOwner())->MyCube) {
		APlayer_Cube* Cube = Cast<ACube_TheBattleMasterPawn>(GetOwner())->MyCube;

		Cube->GetAttachComponent()->AddToInventory(this);
	}
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
		);//FAttachmentTransformRules::SnapToTargetIncludingScale
		AttachToComponent(Cube->GetBlockMesh(), Trans , EquipSocketName);
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

bool AItemBase::IsInRange(AActor* SelectedActor) {
	//Takes the actor and decides whether the item can reach it or not
	UE_LOG(LogTemp, Warning, TEXT("Default in range"));
	return false;
}

void AItemBase::DoAction(bool bMainPhase, FVector Direction){
	UE_LOG(LogTemp, Warning, TEXT("Default Action for %s"), *GetName());
	UE_LOG(LogTemp, Warning, TEXT("Number!!!: %d"), GetLocalRole());

	/*const FRotator SpawnRotation = GetActorRotation();

	const FVector SpawnLocation = GetActorLocation();
	ASmallMunition* Bullet = GetWorld()->SpawnActor<ASmallMunition>(SmallMunitionClass, SpawnLocation, SpawnRotation);

	Bullet->SetOwner(GetOwner());
	Bullet->SetReplicates(bMainPhase);
	Bullet->Direction = Direction;

	ACube_TheBattleMasterPawn* PlayerPawn = Cast<ACube_TheBattleMasterPawn>(GetOwner());
	PlayerPawn->HighlightAttackOptions(PlayerPawn->MyCube->BlockOwner, false, AttackRangeMin, AttackRange, false);*/

}

void AItemBase::EndAction()
{
	UE_LOG(LogTemp, Warning, TEXT("Action has ended!"));
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
