// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "Cube_TheBattleMasterBlockGrid.h"
#include "Cube_TheBattleMasterBlock.h"
#include "Components/TextRenderComponent.h"
#include "Engine/World.h"

#define LOCTEXT_NAMESPACE "PuzzleBlockGrid"

ACube_TheBattleMasterBlockGrid::ACube_TheBattleMasterBlockGrid()
{
	// Create dummy root scene component
	DummyRoot = CreateDefaultSubobject<USceneComponent>(TEXT("Dummy0"));
	RootComponent = DummyRoot;

	// Create static mesh component
	ScoreText = CreateDefaultSubobject<UTextRenderComponent>(TEXT("ScoreText0"));
	ScoreText->SetRelativeLocation(FVector(-1000.f,-1000.f,-1000.f));
	ScoreText->SetRelativeRotation(FRotator(90.f,0.f,0.f));
	ScoreText->SetText(FText::Format(LOCTEXT("ScoreFmt", "Score: {0}"), FText::AsNumber(0)));
	ScoreText->SetupAttachment(DummyRoot);

	// Set defaults
	Size = 30;
	BlockSpacing = 60.f;

	StartPositions.Add(FVector2D(0, 0));
	StartPositions.Add(FVector2D(0, 29));
	StartPositions.Add(FVector2D(29, 29));
}


void ACube_TheBattleMasterBlockGrid::BeginPlay()
{
	Super::BeginPlay();

	// Number of blocks
	const int32 NumBlocks = Size * Size;

	// Loop to spawn each block
	for(int32 BlockIndex=0; BlockIndex<NumBlocks; BlockIndex++)
	{
		const float XOffset = (BlockIndex/Size) * BlockSpacing; // Divide by dimension
		const float YOffset = (BlockIndex%Size) * BlockSpacing; // Modulo gives remainder

		const float X = floor(BlockIndex/30);
		const float Y = BlockIndex % Size;

		// Make position vector, offset from Grid location
		const FVector BlockLocation = FVector(XOffset, YOffset, 0.f) + GetActorLocation();

		// Spawn a block
		ACube_TheBattleMasterBlock* NewBlock = GetWorld()->SpawnActor<ACube_TheBattleMasterBlock>(BlockLocation, FRotator(0,0,0));
		NewBlock->Coordinates = FVector2D(X, Y);
		
		Grid.Add(FVector2D(X, Y), NewBlock);
		GridReference.Add(BlockLocation, FVector2D(X, Y));
		// Tell the block about its owner
		if (NewBlock != nullptr)
		{
			NewBlock->OwningGrid = this;
			NewBlock->BlockPosition = BlockLocation;
			for (FVector2D DummyPosition : StartPositions) {
				if (DummyPosition == FVector2D(X, Y)) { NewBlock ->CanMove(true); }
			}
		}
	}
}


void ACube_TheBattleMasterBlockGrid::AddScore()
{
	// Increment score
	Score++;

	// Update text
	ScoreText->SetText(FText::Format(LOCTEXT("ScoreFmt", "Score: {0}"), FText::AsNumber(Score)));
}

#undef LOCTEXT_NAMESPACE
