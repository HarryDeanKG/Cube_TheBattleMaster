// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "Cube_TheBattleMasterPawn.h"
#include "Cube_TheBattleMasterBlock.h"
#include "Cube_TheBattleMasterBlockGrid.h"
#include "Player_Cube.h"
#include "Cube_TheBattleMasterPlayerController.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/PlayerController.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/UObjectIterator.h"


ACube_TheBattleMasterPawn::ACube_TheBattleMasterPawn(const FObjectInitializer& ObjectInitializer) 
	: Super(ObjectInitializer)
{
	AutoPossessPlayer = EAutoReceiveInput::Disabled;
	//SetReplicates(true);


}

//void ACube_TheBattleMasterPawn::BeginPlay()
//{
//	Super::BeginPlay();
//
//	//SetCube(this);
//
//	/*AutoPossessPlayer = EAutoReceiveInput::Player0;*/
//	
//	
//}

void ACube_TheBattleMasterPawn::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		FVector Start, Dir, End;
		PC->DeprojectMousePositionToWorld(Start, Dir);
		End = Start + (Dir * 8000.0f);
		TraceForBlock(Start, End, false);
	}
}

void ACube_TheBattleMasterPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction("TriggerClick", EInputEvent::IE_Pressed, this, &ACube_TheBattleMasterPawn::TriggerClick);
}

void ACube_TheBattleMasterPawn::CalcCamera(float DeltaTime, struct FMinimalViewInfo& OutResult)
{
	Super::CalcCamera(DeltaTime, OutResult);

	OutResult.Rotation = FRotator(-90.0f, -90.0f, 0.0f);
}

void ACube_TheBattleMasterPawn::CameraMove(APlayer_Cube* Cube) {
	//make sure owner is not null then because owner is a player controller just cast it
	if (GetOwner()) {
		APlayerController* OurPlayerController = Cast<APlayerController>(GetOwner());
		if ((OurPlayerController->GetViewTarget() != Cube) && (Cube != nullptr))
		{
			// Cut instantly to cube camera.
			OurPlayerController->SetViewTargetWithBlend(Cube, 0.75f);
		}
	}
}

void ACube_TheBattleMasterPawn::SetCube(ACube_TheBattleMasterPawn* Pawn)
{
	if (Role < ROLE_Authority) {
		Server_SetCube(Pawn);
	}
	else {
		if (Pawn->MyCube == nullptr) {
			Pawn->MyCube = GetWorld()->SpawnActor<APlayer_Cube>(FVector(0, 0, 0), FRotator(0, 0, 0));
			Pawn->MyCube->SetOwner(this);

			CameraMove(Pawn->MyCube);
		}
	}
}

bool ACube_TheBattleMasterPawn::Server_SetCube_Validate(ACube_TheBattleMasterPawn* Pawn) {
	if (Pawn->MyCube == nullptr){
		return true;
	}
	return false;
}

void ACube_TheBattleMasterPawn::Server_SetCube_Implementation(ACube_TheBattleMasterPawn* Pawn){ SetCube(Pawn); }

void ACube_TheBattleMasterPawn::TriggerClick()
{
	//if (Role < ROLE_Authority) {
	//	Server_TriggerClick();
	//}
	if (CurrentBlockFocus)
	{
		CurrentBlockFocus->HandleClicked();
		ACube_TheBattleMasterBlockGrid* BlockGrid;
		FVector2D Grid_Location;
		if (MyCube != nullptr) { 
			MyCube->Movement(CurrentBlockFocus->BlockPosition); 
			
			//UE_LOG(LogTemp, Warning, TEXT("Cube Position2: %s"), *TheTest.ToString());
			for (TObjectIterator<ACube_TheBattleMasterBlockGrid> test; test; ++test) 
			{
				Grid_Location = test->GridReference.FindRef(MyCube->GetActorLocation());
				//UE_LOG(LogTemp, Warning, TEXT("%s"), *test->GetName()); 
				//UE_LOG(LogTemp, Warning, TEXT("Cube actor location on block grid: %s"), *Vec_test.ToString());

				//break;
				BlockGrid = *test;
			}

			
			UE_LOG(LogTemp, Warning, TEXT("Grid_Location: %s"), *Grid_Location.ToString());
			ACube_TheBattleMasterBlock* SelectedBlock = BlockGrid->Grid.FindRef(Grid_Location);
			SelectedBlock->bOccupied = true;
			UE_LOG(LogTemp, Warning, TEXT("Block name on grid: %s"), *SelectedBlock->GetName());
			
			for (int32 X = Grid_Location.X-4; X < Grid_Location.X+4; X++) {
				for (int32 Y = Grid_Location.Y - 4; Y < Grid_Location.Y + 4; Y++) {
					BlockGrid->Grid.FindRef(Grid_Location)->bMove = true;
				}
			}
			
		}
		else { SetCube(this); }
	}
}

void ACube_TheBattleMasterPawn::TraceForBlock(const FVector& Start, const FVector& End, bool bDrawDebugHelpers)
{
	FHitResult HitResult;
	GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility);
	if (bDrawDebugHelpers)
	{
		DrawDebugLine(GetWorld(), Start, HitResult.Location, FColor::Red);
		DrawDebugSolidBox(GetWorld(), HitResult.Location, FVector(20.0f), FColor::Red);
	}
	if (HitResult.Actor.IsValid())
	{
		ACube_TheBattleMasterBlock* HitBlock = Cast<ACube_TheBattleMasterBlock>(HitResult.Actor.Get());
		if (CurrentBlockFocus != HitBlock)
		{
			if (CurrentBlockFocus)
			{
				CurrentBlockFocus->Highlight(false);
			}
			if (HitBlock)
			{
				HitBlock->Highlight(true);
			}
			CurrentBlockFocus = HitBlock;
		}
	}
	else if (CurrentBlockFocus)
	{
		CurrentBlockFocus->Highlight(false);
		CurrentBlockFocus = nullptr;
	}
}

void ACube_TheBattleMasterPawn::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ACube_TheBattleMasterPawn, MyCube);
}