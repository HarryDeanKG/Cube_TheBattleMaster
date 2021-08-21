// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "Cube_TheBattleMasterBlock.h"
#include "Cube_TheBattleMasterBlockGrid.h"
#include "Player_Cube.h"
#include "UObject/ConstructorHelpers.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Net/UnrealNetwork.h"
#include "Materials/MaterialInstance.h"

ACube_TheBattleMasterBlock::ACube_TheBattleMasterBlock()
{
	bReplicates = false;
	// Structure to hold one-time initialization
	struct FConstructorStatics
	{
		ConstructorHelpers::FObjectFinderOptional<UStaticMesh> PlaneMesh;
		ConstructorHelpers::FObjectFinderOptional<UMaterial> BaseMaterial;
		ConstructorHelpers::FObjectFinderOptional<UMaterialInstance> BlueMaterial;
		ConstructorHelpers::FObjectFinderOptional<UMaterialInstance> RedMaterial;
		ConstructorHelpers::FObjectFinderOptional<UMaterialInstance> OrangeMaterial;
		ConstructorHelpers::FObjectFinderOptional<UMaterialInstance> YellowMaterial;
		ConstructorHelpers::FObjectFinderOptional<UMaterialInstance> PathMaterial;
		FConstructorStatics()
			: PlaneMesh(TEXT("/Game/Puzzle/Meshes/HexTile_mesh.HexTile_mesh"))
			, BaseMaterial(TEXT("/Game/Puzzle/Meshes/BaseMaterial.BaseMaterial"))
			, BlueMaterial(TEXT("/Game/Puzzle/Meshes/BlueMaterial.BlueMaterial"))
			, RedMaterial(TEXT("/Game/Puzzle/Meshes/RedMaterial.RedMaterial"))
			, OrangeMaterial(TEXT("/Game/Puzzle/Meshes/OrangeMaterial.OrangeMaterial"))
			, YellowMaterial(TEXT("/Game/Puzzle/Meshes/YellowMaterial.YellowMaterial"))
			, PathMaterial(TEXT("/Game/Puzzle/Meshes/PathMaterial.PathMaterial"))
		{
		}
	};
	static FConstructorStatics ConstructorStatics;

	// Create dummy root scene component
	DummyRoot = CreateDefaultSubobject<USceneComponent>(TEXT("Dummy0"));
	RootComponent = DummyRoot;

	// Create static mesh component
	BlockMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BlockMesh0"));
	BlockMesh->SetStaticMesh(ConstructorStatics.PlaneMesh.Get());
	BlockMesh->SetRelativeScale3D(FVector(0.6f,0.6f,0.25f));
	BlockMesh->SetRelativeLocation(FVector(0.f,0.f,0.f));
	BlockMesh->SetMaterial(0, ConstructorStatics.BlueMaterial.Get());
	BlockMesh->SetupAttachment(DummyRoot);

	//BlockMesh->OnClicked.AddDynamic(this, &ACube_TheBattleMasterBlock::BlockClicked);

	OverlapVol = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Overlap0"));

	OverlapVol->SetRelativeLocation(FVector(0.0f, 0.0f, 20.0f));
	OverlapVol->SetRelativeScale3D(FVector(1.0f));
	OverlapVol->SetCapsuleSize(20.f, 20.f, true);
	OverlapVol->SetGenerateOverlapEvents(true);
	OverlapVol->SetupAttachment(DummyRoot);
	OverlapVol->SetHiddenInGame(false);
	
	OverlapVol->SetCollisionProfileName("OverlapAll", false);
	//CapsuleComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Block);
	//CapsuleComponent->ComponentTags.Add("Intangable");

	// Save a pointer to the orange material
	BaseMaterial = ConstructorStatics.BaseMaterial.Get();
	BlueMaterial = ConstructorStatics.BlueMaterial.Get();
	RedMaterial = ConstructorStatics.RedMaterial.Get();
	OrangeMaterial = ConstructorStatics.OrangeMaterial.Get();
	YellowMaterial = ConstructorStatics.YellowMaterial.Get();


}

void ACube_TheBattleMasterBlock::OnConstruction(const FTransform &Transform) {

	Super::OnConstruction(Transform);

	DynamicBaseMaterial = BlockMesh->CreateDynamicMaterialInstance(0, BlueMaterial);

	OverlapVol->OnComponentBeginOverlap.AddDynamic(this, &ACube_TheBattleMasterBlock::BeginOverlap);
	
}
void ACube_TheBattleMasterBlock::OnRep_ChangeEnergy() {
	DynamicBaseMaterial->SetVectorParameterValue("BaseColor", FLinearColor(BasicEnergy / 1000.f, BasicEnergy / 1000.f, 1.f, 0.f));
//	//UE_LOG(LogTemp, Warning, TEXT("Basic Energy: %f"), BasicEnergy);
	DummyEnergy = BasicEnergy;
}


void ACube_TheBattleMasterBlock::SetEnergyVariables(float BEnergy) {

	//MultiCast_SetEnergyVariables(BEnergy);

	BasicEnergy = BEnergy;
	DynamicBaseMaterial->SetVectorParameterValue("BaseColor", FLinearColor(BasicEnergy / 1000.f, BasicEnergy / 1000.f, 1.f, 0.f));
	//UE_LOG(LogTemp, Warning, TEXT("Basic Energy: %f"), BasicEnergy);

	//Server_SetEnergyVariables(BEnergy);
	//DynamicBaseMaterial->SetVectorParameterValue("BaseColor", FLinearColor(BasicEnergy / 1000.f, BasicEnergy / 1000.f, 1.f, 0.f));
	/*if (GetLocalRole() < ROLE_Authority) { Server_SetEnergyVariables(BEnergy); }
	else {
		BasicEnergy = BEnergy;
		DynamicBaseMaterial->SetVectorParameterValue("BaseColor", FLinearColor(BasicEnergy / 1000.f, BasicEnergy / 1000.f, 1.f, 0.f));
		UE_LOG(LogTemp, Warning, TEXT("Basic Energy: %f"), BasicEnergy);
	}*/
}




void ACube_TheBattleMasterBlock::BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult) {

	if (OtherActor != this) {
		//UE_LOG(LogTemp, Warning, TEXT("%s: Overlap with: %s"), *this->GetName(), *OtherActor->GetName());
	}
}


void ACube_TheBattleMasterBlock::HandleClicked()
{
	// Check we are not already active
	if (!bIsActive)
	{
		//Unhighlight all other blocks
		for (TObjectIterator<ACube_TheBattleMasterBlock> Block; Block; ++Block) {
			Block->bIsActive = false;
			//Block->BlockMesh->SetMaterial(0, BlueMaterial);
		}

		bIsActive = true;
		// Change material
		//DynamicBaseMaterial->SetVectorParameterValue("BaseColor", FLinearColor(1.f, 0.f, 0.f));

		//BlockMesh->SetMaterial(0, OrangeMaterial);
		DynamicBaseMaterial->SetVectorParameterValue("BaseColor", FLinearColor(0.7f, 0.3f, 0.f, 0.f));

		UE_LOG(LogTemp, Warning, TEXT("Role : %d"), GetLocalRole());

		
	}
	//UE_LOG(LogTemp, Warning, TEXT("Coordingates : %s"), *Coordinates.ToString());

	//TArray< ACube_TheBattleMasterBlock*> test;
	//OwningGrid->GetRing(this, 3, test);

	//for (auto Elems : test) { Elems->Highlight(true); }
}


void ACube_TheBattleMasterBlock::ChangeColour(FLinearColor Colour) {

	//BlockMesh->SetMaterial(0, OrangeMaterial);


}


void ACube_TheBattleMasterBlock::Highlight(bool bOn)
{
	// Do not highlight if the block has already been activated.
	//BlockMesh->SetMaterial(0, OrangeMaterial);

	if (bOn)
	{
		//BlockMesh->SetMaterial(0, BaseMaterial);
		DynamicBaseMaterial->SetVectorParameterValue("BaseColor", FLinearColor(1.f, 1.f, 1.f, 0.f));

	}
	else if (bMove )
	{
		//BlockMesh->SetMaterial(0, RedMaterial);
		DynamicBaseMaterial->SetVectorParameterValue("BaseColor", FLinearColor(1.f, 0.f, 0.f, 0.f));

	}
	else if (bIsPath)
	{
		//BlockMesh->SetMaterial(0, PathMaterial);
		DynamicBaseMaterial->SetVectorParameterValue("BaseColor", FLinearColor(0.7f, 0.f, 0.5f, 0.f));

	}
	else if (bIsOccupied || bRotation)
	{
		//BlockMesh->SetMaterial(0, YellowMaterial);
		DynamicBaseMaterial->SetVectorParameterValue("BaseColor", FLinearColor(1.f, 1.f, 0.f, 0.f));

	}
	else if (bAttack )
	{
		//BlockMesh->SetMaterial(0, OrangeMaterial);
		DynamicBaseMaterial->SetVectorParameterValue("BaseColor", FLinearColor(0.7f, 0.3f, 0.f, 0.f));

	}
	else
	{
		//BlockMesh->SetMaterial(0, BlueMaterial);

		//DynamicBaseMaterial = BlockMesh->CreateDynamicMaterialInstance(0, BlueMaterial);
		DynamicBaseMaterial->SetVectorParameterValue("BaseColor", FLinearColor(BasicEnergy / 1000.f, BasicEnergy / 1000.f, 1.f, 0.f));
	}
}


void ACube_TheBattleMasterBlock::CanMove(bool bOn)
{
	//UE_LOG(LogTemp, Warning, TEXT("Role move : %d"), GetLocalRole());

	// Do not change if occupied 
	//and if it is within movement space.
	bMove = bOn;
	Highlight(false);
	//BlockMesh->SetMaterial(0, OrangeMaterial);
	

	/*
	if (bOn)
	{
		BlockMesh->SetMaterial(0, RedMaterial);
		bMove = true;
	}
	else if (bIsOccupied) {
		 BlockMesh->SetMaterial(0, YellowMaterial);
		 bMove = false;
	}
	else
	{
		BlockMesh->SetMaterial(0, BlueMaterial);
		bMove = false;
	}*/
}

void ACube_TheBattleMasterBlock::CanRotate(bool bOn)
{
	bRotation = bOn;
	Highlight(false);
}

void ACube_TheBattleMasterBlock::CanAttack(bool bOn, bool bIsImmutable)
{
	
	bool bHighlightAttack;
	if (bIsImmutable) { bHighlightAttack = bOn; }
	else { bHighlightAttack = (bOn && !bIsOccupied); }

	if (bHighlightAttack) { bAttack = true; 
	DynamicBaseMaterial->SetVectorParameterValue("BaseColor", FLinearColor(0.7f, 0.3f, 0.f, 0.f));

	}
	else { 
		bAttack = false; 
		DynamicBaseMaterial->SetVectorParameterValue("BaseColor", FLinearColor(BasicEnergy / 1000.f, BasicEnergy / 1000.f, 1.f, 0.f));
	}
}

void ACube_TheBattleMasterBlock::ToggleOccupied(bool bOn) 
{
	//if (Role < ROLE_Authority) { Server_ToggleOccupied(bOn); }
	if (bOn) {
		//BlockMesh->SetMaterial(0, YellowMaterial);
		bIsOccupied = true;
	}
	else {
		bIsOccupied = false;
		/*if (bMove) { BlockMesh->SetMaterial(0, RedMaterial); }
		else { BlockMesh->SetMaterial(0, BlueMaterial); }*/
	}
}

void ACube_TheBattleMasterBlock::HighlightPath(bool bOn) {
	bIsPath = bOn;
	Highlight(false);
}

//void ACube_TheBattleMasterBlock::Server_ToggleOccupied_Implementation(bool bOn) { ToggleOccupied(bOn); }

void ACube_TheBattleMasterBlock::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ACube_TheBattleMasterBlock, bIsOccupied);

	DOREPLIFETIME(ACube_TheBattleMasterBlock, BasicEnergy);

	//DOREPLIFETIME_CONDITION(ACube_TheBattleMasterBlock, bMove, COND_SkipOwner);

	
}