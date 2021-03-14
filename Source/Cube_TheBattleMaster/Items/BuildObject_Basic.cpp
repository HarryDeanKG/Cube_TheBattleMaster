// Fill out your copyright notice in the Description page of Project Settings.

#include "BuildObject_Basic.h"
#include "../Cube_TheBattleMasterPawn.h"

ABuildObject_Basic::ABuildObject_Basic() {

	//// Structure to hold one-time initialization
	//struct FConstructorStatics
	//{
	//	ConstructorHelpers::FObjectFinderOptional<UStaticMesh> PlaneMesh;
	//	ConstructorHelpers::FObjectFinderOptional<UMaterial> BaseMaterial;
	//	FConstructorStatics()
	//		: PlaneMesh(TEXT("/Game/Puzzle/Meshes/PuzzleCube.PuzzleCube"))
	//		, BaseMaterial(TEXT("/Game/Puzzle/Meshes/BaseMaterial.BaseMaterial"))
	//	{
	//	}
	//};
	//static FConstructorStatics ConstructorStatics;

	//// Create dummy root scene component
	////DummyRoot = CreateDefaultSubobject<USceneComponent>(TEXT("Dummy0"));
	////RootComponent = DummyRoot;

	//// Create static mesh component
	//BlockMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BlockMesh0"));
	//BlockMesh->SetStaticMesh(ConstructorStatics.PlaneMesh.Get());
	//BlockMesh->SetRelativeScale3D(FVector(1.0f));
	//BlockMesh->SetRelativeLocation(FVector(0.f));
	//BlockMesh->SetMaterial(0, ConstructorStatics.BaseMaterial.Get());
	//BlockMesh->SetupAttachment(DummyRoot);
	////BlockMesh->OnClicked.AddDynamic(this, &ACube_TheBattleMasterBlock::BlockClicked);

	//// Save a pointer to the orange material
	//BaseMaterial = ConstructorStatics.BaseMaterial.Get();

	//BlockMesh->SetStaticMesh(ConstructorStatics.PlaneMesh.Get());
	
	// = ConstructorStatics.BaseMaterial.Get();

	WeaponName = "Build Object";

	ActionName = "Build Wall";

	bCanBeUsedWithMovement = false;

	AttackRange = 1;
}

void ABuildObject_Basic::MakeTheWall(FVector Direction, bool bMainPhase) {

	const FRotator SpawnRotation = FRotator(0.f, 0.f, 0.f);

	const FVector SpawnLocation = FVector(0.f);
	
	ACube_TheBattleMasterPawn* PlayerPawn = Cast<ACube_TheBattleMasterPawn>(GetOwner());
	ACube_TheBattleMasterBlock* Block = PlayerPawn->GetBlockFromPosition(Direction);

	AWall_Actor* Wall;
	if (WallClass) {
		Wall = GetWorld()->SpawnActor<AWall_Actor>(WallClass, SpawnLocation, SpawnRotation);
	}else{
		Wall = GetWorld()->SpawnActor<AWall_Actor>(SpawnLocation, SpawnRotation);
	}
	
	Wall->SetOwner(GetOwner());
	//Wall->SetReplicates(AttackRep);
	//Wall->Direction = AttackPosition;
	

	FAttachmentTransformRules Trans = FAttachmentTransformRules
	(
		EAttachmentRule::KeepRelative,
		EAttachmentRule::KeepRelative,
		EAttachmentRule::KeepRelative,
		true
	);

	//FAttachmentTransformRules::SnapToTargetIncludingScale
	FVector Dir =  Direction - PlayerPawn->MyCube->GetActorLocation();
	Wall->AttachToSelect(Block->GetBlockMesh(), Trans, FindCorrectSlot(Dir));
	
	if (!bMainPhase) {
		Wall->bTemporary = true;
		PlayerPawn->FlagedForDeleation.Add(Wall);		
		PlayerPawn->ToggleBackList.Add(Block);
		Block->ToggleOccupied(true);
	} else{ 
		Wall->bTemporary = false; 
		Wall->SetReplicates(true);
		//All blocks need to be toggled here
		for (TObjectIterator<ACube_TheBattleMasterPawn> Pawn; Pawn; ++Pawn) {
			Pawn->GetBlockFromPosition(Block->GetActorLocation())->ToggleOccupied(true);
		}
			
		//Block->ToggleOccupied(true);
	}
}

void ABuildObject_Basic::DoAction(bool bMainPhase, FVector Direction) {
	
	MakeTheWall(Direction, bMainPhase);
	HighlightBlocks(false);
}

void ABuildObject_Basic::ResetAction() {
	UE_LOG(LogTemp, Warning, TEXT("Reset wall"));
	
	for (TObjectIterator<AWall_Actor> Elems; Elems; ++Elems) {
		if (Elems->bTemporary) {
			Elems->Destroy();
		}
	}
}

void ABuildObject_Basic::SetActionInMotion() { HighlightBlocks(true); }

void ABuildObject_Basic::UnSetActionInMotion() { HighlightBlocks(false); }


void ABuildObject_Basic::HighlightBlocks(bool bHighlight) {
	ACube_TheBattleMasterPawn* PlayerPawn = Cast<ACube_TheBattleMasterPawn>(GetOwner());
	
	PlayerPawn->HighlightAttackOptions(PlayerPawn->MyCube->BlockOwner, bHighlight, AttackRange, AttackRange, false);
}

FName ABuildObject_Basic::FindCorrectSlot(FVector Direction) {

	if (Direction.X > 0 && Direction.Y == 0) { return FName("North"); }
	else if (Direction.X == 0 && Direction.Y > 0) { return FName("East"); }
	else if (Direction.X < 0 && Direction.Y == 0) { return FName("South"); }
	else if (Direction.X == 0 && Direction.Y < 0) { return FName("West"); }
	else if (Direction.X > 0 && Direction.Y > 0) { return FName("NE"); }
	else if (Direction.X > 0 && Direction.Y < 0) { return FName("NW"); }
	else if (Direction.X < 0 && Direction.Y > 0) { return FName("SE"); }
	else if (Direction.X < 0 && Direction.Y < 0) { return FName("SW"); }
	else { return FName("North"); }
}