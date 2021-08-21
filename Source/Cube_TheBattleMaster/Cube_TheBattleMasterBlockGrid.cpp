// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "Cube_TheBattleMasterBlockGrid.h"
#include "Cube_TheBattleMasterBlock.h"
#include "Components/TextRenderComponent.h"
#include "Net/UnrealNetwork.h"
#include "Engine/World.h"

#define LOCTEXT_NAMESPACE "PuzzleBlockGrid"

ACube_TheBattleMasterBlockGrid::ACube_TheBattleMasterBlockGrid()
{
	
	bReplicates = false;

	// Create dummy root scene component
	DummyRoot = CreateDefaultSubobject<USceneComponent>(TEXT("Dummy0"));
	RootComponent = DummyRoot;

	// Set defaults
	Size = 30;
	BlockSpacing = 60.f;

	StartPositions.Add(FVector(0, 0, 0));
	StartPositions.Add(FVector(0, 29, 29));
	StartPositions.Add(FVector(0, -29, -29));
}


void ACube_TheBattleMasterBlockGrid::BeginPlay()
{
	SetActorRelativeLocation(FVector(100.f, 200.f, 0.f));

	Super::BeginPlay();

	

}


void ACube_TheBattleMasterBlockGrid::MakeTheGrid() {

	/*if (GetLocalRole() != ROLE_Authority) {
	}*/
	float SizeOfTheHexX = 54.f;
	float SizeOfTheHexY = 35.5f;

	// Number of blocks up from center
	const int32 NumBlocks = 10;

	// Loop to spawn each block
	for (int32 X = -NumBlocks; X <= NumBlocks; X++) {
		int32 Y1 = FMath::Max(-NumBlocks, -X - NumBlocks);
		int32 Y2 = FMath::Min(NumBlocks, -X + NumBlocks);
		for (int32 Y = Y1; Y <= Y2; Y++)
		{
			int32 Z = -X - Y;
			//UE_LOG(LogTemp, Warning, TEXT("Coords: X-> %d, Y->%d, Z->%d"), X, Y, Z);

			int32 XOffset = SizeOfTheHexX * (3 / 2 * X + 0 * Y);
			int32 YOffset = SizeOfTheHexY * (sqrt(3) / 2 * X + sqrt(3) * Y);
			// Make position vector, offset from Grid location
			FVector BlockLocation = FVector(XOffset, YOffset, 0) + GetActorLocation();
			//UE_LOG(LogTemp, Warning, TEXT("Happening"));

			// Spawn a block
			ACube_TheBattleMasterBlock* NewBlock = GetWorld()->SpawnActor<ACube_TheBattleMasterBlock>(BlockLocation, FRotator(0, 0, 0));
			if (NewBlock != nullptr) {
				NewBlock->Coordinates = FVector(X, Y, Z);
				NewBlock->OwningGrid = this;
				NewBlock->BlockPosition = BlockLocation;


				//NewBlock->SetEnergyVariables(FMath::RandRange(0.f, 1000.f));
				//Add To Grid reference
				Grid.Add(FVector(X, Y, Z), NewBlock);
				GridReference.Add(BlockLocation, FVector(X, Y, Z));
			}
		}
		//}
	}


	//			/*For highlighting the starting positions, but this needs to be up to the pawn to call in the "start game"*/
	//			//for (FVector2D DummyPosition : StartPositions) {
	//			//	if (DummyPosition == FVector2D(X, Y)) { NewBlock ->CanMove(true); }
	//			//}
}

void ACube_TheBattleMasterBlockGrid::GetForwardDirection(ACube_TheBattleMasterBlock* CenterBlock, int32 RungLevel, TArray<ACube_TheBattleMasterBlock*> &OutBlocks, int Direction){

	//int N = RungLevel;
	//int SelectedDirection = Direction;
	int SelectedDirection = Direction % 6;
	//UE_LOG(LogTemp, Warning, TEXT("Selected Direction: %d"), SelectedDirection);

	ACube_TheBattleMasterBlock* FocusBlock;

	for (int Increment = 0; Increment <= RungLevel; Increment++) {

		int x;
		int y;
		int z;

		switch (SelectedDirection)
		{
		case 4:
			x = CenterBlock->Coordinates.X + Increment;
			y = CenterBlock->Coordinates.Y - Increment;
			z = CenterBlock->Coordinates.Z;
			break;
		case 1:
			x = CenterBlock->Coordinates.X - Increment;
			y = CenterBlock->Coordinates.Y + Increment;
			z = CenterBlock->Coordinates.Z;
			break;
		case 5:
			x = CenterBlock->Coordinates.X + Increment;
			y = CenterBlock->Coordinates.Y;
			z = CenterBlock->Coordinates.Z - Increment;
			break;
		case 2:
			x = CenterBlock->Coordinates.X - Increment;
			y = CenterBlock->Coordinates.Y;
			z = CenterBlock->Coordinates.Z + Increment;
			break;
		case 0:
			x = CenterBlock->Coordinates.X;
			y = CenterBlock->Coordinates.Y + Increment;
			z = CenterBlock->Coordinates.Z - Increment;
			break;
		case 3:
			x = CenterBlock->Coordinates.X;
			y = CenterBlock->Coordinates.Y - Increment;
			z = CenterBlock->Coordinates.Z + Increment;
			break;
		default:
			 x = CenterBlock->Coordinates.X;
			 y = CenterBlock->Coordinates.Y;
			 z = CenterBlock->Coordinates.Z;
			break;
		}

		if (Grid.Contains(FVector(x, y, z))) {
			FocusBlock = Grid[FVector(x, y, z)];
			OutBlocks.Add(FocusBlock);
			//UE_LOG(LogTemp, Warning, TEXT("Previous block: %s"), *FocusBlock->GetActorLocation().ToString());

		}
	}
	

}


void ACube_TheBattleMasterBlockGrid::GetSectorRing(ACube_TheBattleMasterBlock* CenterBlock, int32 RungLevel, float Direction, TArray<ACube_TheBattleMasterBlock*> &OutBlocks)
{
	TArray<ACube_TheBattleMasterBlock*> DummyOutBlocks;
	GetForwardDirection(CenterBlock, RungLevel, DummyOutBlocks, round(Direction));
	ACube_TheBattleMasterBlock* NewCenterBlock = DummyOutBlocks.Last();

	int SelectedInt = round(Direction - 2) < 0 ? round(Direction + 4) : round(Direction - 2);

	if (!FMath::IsNearlyEqual(FMath::Frac(Direction), 0.5f, 0.01f)) {

		GetForwardDirection(NewCenterBlock, RungLevel/2, OutBlocks, SelectedInt);
		
		SelectedInt = Direction + 2;
		GetForwardDirection(NewCenterBlock, RungLevel/2, OutBlocks, SelectedInt);
	}
	else{
		GetForwardDirection(NewCenterBlock, RungLevel, OutBlocks, SelectedInt);
	}

}


void ACube_TheBattleMasterBlockGrid::GetRing(ACube_TheBattleMasterBlock* CenterBlock, int32 RungLevel, TArray<ACube_TheBattleMasterBlock*> &OutBlocks)
{

	int N = RungLevel;

	ACube_TheBattleMasterBlock* FocusBlock;
	
	for (int i = 0; i < 6; i++) {
		TArray< ACube_TheBattleMasterBlock*> DummyArray;
		GetForwardDirection(CenterBlock, RungLevel, DummyArray, i);
		FocusBlock = DummyArray.Last();
		//OutBlocks.Add(DummyArray.Last());
		GetForwardDirection(FocusBlock, RungLevel - 1, OutBlocks, (i + 2) % 6);
	}
	//UE_LOG(LogTemp, Warning, TEXT("Length of ring: %d"), OutBlocks.Num());
}

ACube_TheBattleMasterBlock* ACube_TheBattleMasterBlockGrid::GetCube( int X, int Y, int Z)
{
	if (Grid.Contains(FVector(X, Y, Z))) {
		return Grid[FVector(X, Y, Z)];
	}
	return NULL;
}

void ACube_TheBattleMasterBlockGrid::GetAreaWithoutCenter(ACube_TheBattleMasterBlock* CenterBlock, int32 RungLevel, TArray<ACube_TheBattleMasterBlock*> &OutBlocks)
{
	GetAreaWithCenter(CenterBlock, RungLevel, OutBlocks);
	OutBlocks.RemoveSingle(CenterBlock);
	//OutBlocks.RemoveAll(CenterBlock);
	
}

void ACube_TheBattleMasterBlockGrid::GetAreaWithCenter(ACube_TheBattleMasterBlock* CenterBlock, int32 RungLevel, TArray<ACube_TheBattleMasterBlock*> &OutBlocks)
{

	CenterBlock->Coordinates.X;
	int N = RungLevel;
	CenterBlock->Coordinates.Y;
	CenterBlock->Coordinates.Z;

	int xmin = CenterBlock->Coordinates.X - N;
	int xmax = CenterBlock->Coordinates.X + N;

	int ymin = CenterBlock->Coordinates.Y - N;
	int ymax = CenterBlock->Coordinates.Y + N;

	int zmin = CenterBlock->Coordinates.Z - N;
	int zmax = CenterBlock->Coordinates.Z + N;
	for (int x = xmin; x <= xmax; x++) {
		int yymin = ymax < -x - zmin ? ymax : -x - zmin;
		int yymax = ymin > -x - zmax ? ymin : -x - zmax;
		for (int y = yymax; y <= yymin; y++) {
			int z = -x - y;
			if (Grid.Contains(FVector(x, y, z))) {
				OutBlocks.Add(Grid[FVector(x, y, z)]);
			}
		}
	}
}

void ACube_TheBattleMasterBlockGrid::HighlightAreaWithCenter(ACube_TheBattleMasterBlock* CenterBlock, int32 RungLevel){}

void ACube_TheBattleMasterBlockGrid::HighlightAreaWithoutCenter(ACube_TheBattleMasterBlock* CenterBlock, int32 RungLevel) {}

#undef LOCTEXT_NAMESPACE
