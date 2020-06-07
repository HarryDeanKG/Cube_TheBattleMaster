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
	// Structure to hold one-time initialization
	struct FConstructorStatics
	{
		ConstructorHelpers::FObjectFinderOptional<UStaticMesh> PlaneMesh;
		ConstructorHelpers::FObjectFinderOptional<UMaterial> BaseMaterial;
		ConstructorHelpers::FObjectFinderOptional<UMaterialInstance> BlueMaterial;
		ConstructorHelpers::FObjectFinderOptional<UMaterialInstance> RedMaterial;
		ConstructorHelpers::FObjectFinderOptional<UMaterialInstance> OrangeMaterial;
		ConstructorHelpers::FObjectFinderOptional<UMaterialInstance> YellowMaterial;
		FConstructorStatics()
			: PlaneMesh(TEXT("/Game/Puzzle/Meshes/PuzzleCube.PuzzleCube"))
			, BaseMaterial(TEXT("/Game/Puzzle/Meshes/BaseMaterial.BaseMaterial"))
			, BlueMaterial(TEXT("/Game/Puzzle/Meshes/BlueMaterial.BlueMaterial"))
			, RedMaterial(TEXT("/Game/Puzzle/Meshes/RedMaterial.RedMaterial"))
			, OrangeMaterial(TEXT("/Game/Puzzle/Meshes/OrangeMaterial.OrangeMaterial"))
			, YellowMaterial(TEXT("/Game/Puzzle/Meshes/YellowMaterial.YellowMaterial"))
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
	BlockMesh->SetRelativeScale3D(FVector(0.25f,0.25f,0.25f));
	BlockMesh->SetRelativeLocation(FVector(0.f,0.f,25.f));
	BlockMesh->SetMaterial(0, ConstructorStatics.BlueMaterial.Get());
	BlockMesh->SetupAttachment(DummyRoot);
	//BlockMesh->OnClicked.AddDynamic(this, &ACube_TheBattleMasterBlock::BlockClicked);

	// Save a pointer to the orange material
	BaseMaterial = ConstructorStatics.BaseMaterial.Get();
	BlueMaterial = ConstructorStatics.BlueMaterial.Get();
	RedMaterial = ConstructorStatics.RedMaterial.Get();
	OrangeMaterial = ConstructorStatics.OrangeMaterial.Get();
	YellowMaterial = ConstructorStatics.YellowMaterial.Get();
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
		BlockMesh->SetMaterial(0, OrangeMaterial);

		// Tell the Grid
		if (OwningGrid != nullptr)
		{
			OwningGrid->AddScore();
		}
		
		//for (TObjectIterator<APlayer_Cube> PlayerCube; PlayerCube; ++PlayerCube) {
		//	
		//		PlayerCube->Movement(BlockPosition);
		//	
		//}
	}
}

void ACube_TheBattleMasterBlock::Highlight(bool bOn)
{
	// Do not highlight if the block has already been activated.
	if (bIsOccupied)
	{
		BlockMesh->SetMaterial(0, YellowMaterial);
	}
	else if (bOn)
	{
		BlockMesh->SetMaterial(0, BaseMaterial);
	}
	else if (bMove)
	{
		BlockMesh->SetMaterial(0, RedMaterial);
	}
	else
	{
		BlockMesh->SetMaterial(0, BlueMaterial);
	}
}

void ACube_TheBattleMasterBlock::CanMove(bool bOn)
{
	// Do not change if occupied 
	//and if it is within movement space.
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
	}
}

void ACube_TheBattleMasterBlock::ToggleOccupied(bool bOn) 
{
	//if (Role < ROLE_Authority) { Server_ToggleOccupied(bOn); }
	if (bOn) {
		BlockMesh->SetMaterial(0, YellowMaterial);
		bIsOccupied = true;
	}
	else {
		bIsOccupied = false;
		if (bMove) { BlockMesh->SetMaterial(0, RedMaterial); }
		else { BlockMesh->SetMaterial(0, BlueMaterial); }
	}
}



//void ACube_TheBattleMasterBlock::Server_ToggleOccupied_Implementation(bool bOn) { ToggleOccupied(bOn); }

void ACube_TheBattleMasterBlock::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ACube_TheBattleMasterBlock, bIsOccupied);
}