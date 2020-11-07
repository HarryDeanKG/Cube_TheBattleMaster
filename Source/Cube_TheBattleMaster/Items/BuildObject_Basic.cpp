// Fill out your copyright notice in the Description page of Project Settings.

#include "../Cube_TheBattleMasterPawn.h"
#include "BuildObject_Basic.h"

ABuildObject_Basic::ABuildObject_Basic() {

	//// Structure to hold one-time initialization
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

	BlockMesh->SetStaticMesh(ConstructorStatics.PlaneMesh.Get());
	
	BaseMaterial = ConstructorStatics.BaseMaterial.Get();

	WeaponName = "Build Object";

	ActionName = "Build Wall";

	bCanBeUsedWithMovement = false;
}

void ABuildObject_Basic::MakeTheWall(ACube_TheBattleMasterBlock* Block, bool bMainPhase) {

	const FRotator SpawnRotation = FRotator(0.f, 0.f, 0.f);

	const FVector SpawnLocation = FVector(0.f);
	
	ACube_TheBattleMasterPawn* PlayerPawn = Cast<ACube_TheBattleMasterPawn>(GetOwner());

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
	FVector Direction =  Block->GetActorLocation() - PlayerPawn->MyCube->GetActorLocation();
	Wall->AttachToSelect(Block->GetBlockMesh(), Trans, FindCorrectSlot(Direction));
	
	if (!bMainPhase) {
		Wall->bTemporary = true;
		PlayerPawn->FlagedForDeleation.Add(Wall);		
		PlayerPawn->ToggleBackList.Add(Block);
		Block->ToggleOccupied(true);
	} else{ 
		Wall->bTemporary = false; 
		Block->ToggleOccupied(true);
	}
}

void ABuildObject_Basic::DoAction(bool bMainPhase, ACube_TheBattleMasterBlock* Block) {
	UE_LOG(LogTemp, Warning, TEXT("TestWallAction!!"));
	MakeTheWall(Block, bMainPhase);
	ACube_TheBattleMasterPawn* PlayerPawn = Cast<ACube_TheBattleMasterPawn>(GetOwner());

	PlayerPawn->HighlightAttackOptions(PlayerPawn->MyCube->BlockOwner, false, 1, false);
}

void ABuildObject_Basic::ResetAction() {

	for (TObjectIterator<AWall_Actor> Elems; Elems; ++Elems) {
		if (Elems->bTemporary) {
			Elems->Destroy();
		}
	}
}

void ABuildObject_Basic::SetActionInMotion() {
	//Super::SetActionInMotion();
	UE_LOG(LogTemp, Warning, TEXT("Wall Action!!"));

	HighlightBlocks(true);
}

void ABuildObject_Basic::UnSetActionInMotion() {

	HighlightBlocks(false);
}


void ABuildObject_Basic::HighlightBlocks(bool bHighlight) {
	ACube_TheBattleMasterPawn* PlayerPawn = Cast<ACube_TheBattleMasterPawn>(GetOwner());
	
	PlayerPawn->HighlightAttackOptions(PlayerPawn->MyCube->BlockOwner, bHighlight, 1, false);
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