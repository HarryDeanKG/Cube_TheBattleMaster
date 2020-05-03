// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "Cube_TheBattleMasterBlock.h"
#include "Cube_TheBattleMasterBlockGrid.h"
#include "Player_Cube.h"
#include "UObject/ConstructorHelpers.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Materials/MaterialInstance.h"


ACube_TheBattleMasterBlock::ACube_TheBattleMasterBlock()
{
	
	
	// Structure to hold one-time initialization
	struct FConstructorStatics
	{
		ConstructorHelpers::FObjectFinderOptional<UStaticMesh> PlaneMesh;
		ConstructorHelpers::FObjectFinderOptional<UMaterial> BaseMaterial;
		ConstructorHelpers::FObjectFinderOptional<UMaterialInstance> BlueMaterial;
		ConstructorHelpers::FObjectFinderOptional<UMaterialInstance> OrangeMaterial;
		FConstructorStatics()
			: PlaneMesh(TEXT("/Game/Puzzle/Meshes/PuzzleCube.PuzzleCube"))
			, BaseMaterial(TEXT("/Game/Puzzle/Meshes/BaseMaterial.BaseMaterial"))
			, BlueMaterial(TEXT("/Game/Puzzle/Meshes/BlueMaterial.BlueMaterial"))
			, OrangeMaterial(TEXT("/Game/Puzzle/Meshes/OrangeMaterial.OrangeMaterial"))
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
	BlockMesh->SetRelativeScale3D(FVector(1.f,1.f,0.25f));
	BlockMesh->SetRelativeLocation(FVector(0.f,0.f,25.f));
	BlockMesh->SetMaterial(0, ConstructorStatics.BlueMaterial.Get());
	BlockMesh->SetupAttachment(DummyRoot);
	//BlockMesh->OnClicked.AddDynamic(this, &ACube_TheBattleMasterBlock::BlockClicked);
	
	

	// Save a pointer to the orange material
	BaseMaterial = ConstructorStatics.BaseMaterial.Get();
	BlueMaterial = ConstructorStatics.BlueMaterial.Get();
	OrangeMaterial = ConstructorStatics.OrangeMaterial.Get();
}

//void ACube_TheBattleMasterBlock::BlockClicked(FKey ButtonClicked)
//{
//	HandleClicked();
//	//UE_LOG(LogTemp, Warning, TEXT("UPrimitiveComponent = %s"), *ClickedComp->GetName());
//	UE_LOG(LogTemp, Warning, TEXT("FKey = %s"), *ButtonClicked.ToString());
//	
//}



void ACube_TheBattleMasterBlock::HandleClicked()
{
	// Check we are not already active
	if (!bIsActive)
	{
		//Unhighlight all other blocks
		for (TObjectIterator<ACube_TheBattleMasterBlock> Block; Block; ++Block) {
			Block->bIsActive = false;
			Block->BlockMesh->SetMaterial(0, BlueMaterial);
		}

		bIsActive = true;
		// Change material
		BlockMesh->SetMaterial(0, OrangeMaterial);

		// Tell the Grid
		if (OwningGrid != nullptr)
		{
			OwningGrid->AddScore();
		}
		
		//UE_LOG(LogTemp, Warning, TEXT("Test %s"), GetOwner());

		/*for (TObjectIterator<APlayer_Cube> PlayerCube; PlayerCube; ++PlayerCube) {
			
			if (PlayerCube->GetOwner()) {
				UE_LOG(LogTemp, Warning, TEXT("Cube Owner %s"), *(PlayerCube->GetOwner()->GetName()));
			}
			if (GetOwner())
			{
				FString NameTest = GetOwner()->GetName();
				UE_LOG(LogTemp, Warning, TEXT("Block Owner %s"), *(GetOwner()->GetName()));
			}
			if (PlayerCube->GetOwner() == GetOwner()){
				UE_LOG(LogTemp, Warning, TEXT("hit"), PlayerCube->GetOwner());
				PlayerCube->Movement(BlockPosition);
			}
		}*/
	}
}

void ACube_TheBattleMasterBlock::Highlight(bool bOn)
{
	// Do not highlight if the block has already been activated.
	if (bIsActive)
	{
		return;
	}

	if (bOn)
	{
		BlockMesh->SetMaterial(0, BaseMaterial);
	}
	else
	{
		BlockMesh->SetMaterial(0, BlueMaterial);
	}
}
