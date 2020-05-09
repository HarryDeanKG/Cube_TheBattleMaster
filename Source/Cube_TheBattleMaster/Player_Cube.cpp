// Fill out your copyright notice in the Description page of Project Settings.


#include "Player_Cube.h"
#include "UObject/ConstructorHelpers.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Materials/MaterialInstance.h"
#include "Net/UnrealNetwork.h"

// Sets default values
APlayer_Cube::APlayer_Cube()
{
	bReplicateMovement = true;
	bReplicates = true;
	

	// Structure to hold one-time initialization
	struct FConstructorStatics
	{
		ConstructorHelpers::FObjectFinderOptional<UStaticMesh> PlaneMesh;
		ConstructorHelpers::FObjectFinderOptional<UMaterial> BaseMaterial;
		FConstructorStatics()
			: PlaneMesh(TEXT("/Game/Puzzle/Meshes/PuzzleCube.PuzzleCube"))
			, BaseMaterial(TEXT("/Game/Puzzle/Meshes/BaseMaterial.BaseMaterial"))
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
	BlockMesh->SetRelativeScale3D(FVector(0.25f, 0.25f, 0.25f));
	BlockMesh->SetRelativeLocation(FVector(0.f, 0.f, 125.f));
	BlockMesh->SetMaterial(0, ConstructorStatics.BaseMaterial.Get());
	BlockMesh->SetupAttachment(DummyRoot);

	// Save a pointer to the orange material
	BaseMaterial = ConstructorStatics.BaseMaterial.Get();

	Owner2 = GetOwner();
}



void APlayer_Cube::Movement(FVector MovePosition) {
	//if (Role < ROLE_Authority)
	//{
	//	Server_Movement(MovePosition);
	//	//SetActorLocation(MovePosition);
	////	UE_LOG(LogTemp, Warning, TEXT("MOVE"));
	//}
	SetActorLocation(MovePosition);
	//FVector test = GetActorLocation();

	//UE_LOG(LogTemp, Warning, TEXT("Test %s"), *test.ToString());
}

//bool APlayer_Cube::Server_Movement_Validate(FVector MovePosition) {
//	return true;
//}
//
//void APlayer_Cube::Server_Movement_Implementation(FVector MovePosition) {
//	Movement(MovePosition);
//}