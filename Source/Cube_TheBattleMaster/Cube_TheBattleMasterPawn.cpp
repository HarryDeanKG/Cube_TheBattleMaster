// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "Cube_TheBattleMasterPawn.h"
#include "Cube_TheBattleMasterBlock.h"
#include "Player_Cube.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/PlayerController.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"
#include "Net/UnrealNetwork.h"

ACube_TheBattleMasterPawn::ACube_TheBattleMasterPawn(const FObjectInitializer& ObjectInitializer) 
	: Super(ObjectInitializer)
{

	AutoPossessPlayer = EAutoReceiveInput::Disabled;
	//bReplicates = true;
	SetReplicates(true);
}

void ACube_TheBattleMasterPawn::BeginPlay()
{
	Super::BeginPlay();


		
	MakeCube();
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


void ACube_TheBattleMasterPawn::MakeCube()
{
	if (Role < ROLE_Authority) {
		Server_MakeCube();
	}

	MyCube = GetWorld()->SpawnActor<APlayer_Cube>(FVector(0, 0, 0), FRotator(0, 0, 0));

	//float test = GetWorld()->GetNumPlayerControllers();
	//FString test = GetPlayerState()->GetName();
	//UE_LOG(LogTemp, Warning, TEXT("Cube Owner %s"), test);

	MyCube->SetOwner(this);
	//PlayerCube->bReplicateMovement =true;
	//PlayerCube->SetReplicates(true);
}

bool ACube_TheBattleMasterPawn::Server_MakeCube_Validate() {
	return true;
}

void ACube_TheBattleMasterPawn::Server_MakeCube_Implementation()
{
	MakeCube();
}


void ACube_TheBattleMasterPawn::TriggerClick()
{
	if (Role < ROLE_Authority) {
		Server_TriggerClick();

	}
	if (CurrentBlockFocus)
	{
		CurrentBlockFocus->HandleClicked();
		UE_LOG(LogTemp, Warning, TEXT("hit"))
		MyCube->Movement(CurrentBlockFocus->BlockPosition);
	}
}

bool ACube_TheBattleMasterPawn::Server_TriggerClick_Validate() {
	return true;
}

void ACube_TheBattleMasterPawn::Server_TriggerClick_Implementation()
{
	TriggerClick();
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

void ACube_TheBattleMasterPawn::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const {

	DOREPLIFETIME(ACube_TheBattleMasterPawn, MyCube);
}