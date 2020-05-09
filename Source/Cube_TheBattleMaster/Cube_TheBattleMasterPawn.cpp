// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "Cube_TheBattleMasterPawn.h"
#include "Cube_TheBattleMasterBlock.h"
<<<<<<< HEAD
#include "Player_Cube.h"
#include "Cube_TheBattleMasterPlayerController.h"
=======

>>>>>>> parent of 881ec9e... Server client block making
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/PlayerController.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"
<<<<<<< HEAD
#include "Net/UnrealNetwork.h"
#include "UObject/UObjectIterator.h"
=======
>>>>>>> parent of 881ec9e... Server client block making

ACube_TheBattleMasterPawn::ACube_TheBattleMasterPawn(const FObjectInitializer& ObjectInitializer) 
	: Super(ObjectInitializer)
{
<<<<<<< HEAD

	AutoPossessPlayer = EAutoReceiveInput::Disabled;
	//SetReplicates(true);
}

void ACube_TheBattleMasterPawn::BeginPlay()
{
	Super::BeginPlay();

	//SetCube();
=======
	AutoPossessPlayer = EAutoReceiveInput::Player0;
	
>>>>>>> parent of 881ec9e... Server client block making
}

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


<<<<<<< HEAD
void ACube_TheBattleMasterPawn::SetCube()
{
	//if (Role < ROLE_Authority) {
	//	Server_SetCube();
	//}
	//if (MyCube == nullptr) {
	ACube_TheBattleMasterPlayerController* DummyControler = Cast<ACube_TheBattleMasterPlayerController>(GetController());
	DummyControler->MakeCube();
	MyCube = Cast<APlayer_Cube>(DummyControler->CubeArray.Last());
	MyCube->SetOwner(this);
	MyCube->Owner2 = this;

	
	for (TObjectIterator<APlayer_Cube> Cubes; Cubes; ++Cubes) {
		if (Cubes->GetOwner() == nullptr) { Cubes->Destroy(); }
	}


	//}
}

//
//bool ACube_TheBattleMasterPawn::Server_SetCube_Validate() {
//	
//	return false;
//}
//
//void ACube_TheBattleMasterPawn::Server_SetCube_Implementation()
//{
//	SetCube();
//}



void ACube_TheBattleMasterPawn::TriggerClick()
{
	if (Role < ROLE_Authority) {
		Server_TriggerClick();
	}
	if (CurrentBlockFocus)
	{
		CurrentBlockFocus->HandleClicked();
		if (MyCube != nullptr) { MyCube->Movement(CurrentBlockFocus->BlockPosition); }
		else { SetCube(); }
		UE_LOG(LogTemp, Warning, TEXT("hit"))
		
=======
void ACube_TheBattleMasterPawn::TriggerClick()
{
	if (CurrentBlockFocus)
	{
		CurrentBlockFocus->HandleClicked();
>>>>>>> parent of 881ec9e... Server client block making
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