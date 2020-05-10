// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "Cube_TheBattleMasterPawn.h"
#include "Cube_TheBattleMasterBlock.h"
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

void ACube_TheBattleMasterPawn::BeginPlay()
{
	Super::BeginPlay();

	//SetCube(this);

	/*AutoPossessPlayer = EAutoReceiveInput::Player0;*/
	
	
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



void ACube_TheBattleMasterPawn::SetCube(ACube_TheBattleMasterPawn* Test)
{
	if (Role < ROLE_Authority) {
		Server_SetCube(Test);
	}
	else {

		if (Test->MyCube == nullptr) {

			Test->MyCube = GetWorld()->SpawnActor<APlayer_Cube>(FVector(0, 0, 0), FRotator(0, 0, 0));
			Test->MyCube->SetOwner(this);
			Test->MyCube->Owner2 = this;

			/*for (TObjectIterator<APlayer_Cube> Cubes; Cubes; ++Cubes) {
				if (Cubes->GetOwner() == nullptr) { Cubes->Destroy(); }
			}*/
			
			//auto Camera = Test->MyCube->GetCamera();
			


			// Find the actor that handles control for the local player.
			
			APlayerController* OurPlayerController = UGameplayStatics::GetPlayerController(this, 0);
			if (OurPlayerController)
			{
				if ((OurPlayerController->GetViewTarget() != MyCube) && (MyCube != nullptr))
				{
					// Cut instantly to camera one.
					OurPlayerController->SetViewTargetWithBlend(MyCube);
				}
			}
		}
	}
	
}


bool ACube_TheBattleMasterPawn::Server_SetCube_Validate(ACube_TheBattleMasterPawn* Test) {
	if (Test->MyCube == nullptr){
		return true;
	}
	return false;
}

void ACube_TheBattleMasterPawn::Server_SetCube_Implementation(ACube_TheBattleMasterPawn* Test){ SetCube(Test); }



void ACube_TheBattleMasterPawn::TriggerClick()
{
	//if (Role < ROLE_Authority) {
	//	Server_TriggerClick();
	//}
	//SetCube();
	if (CurrentBlockFocus)
	{
		
		CurrentBlockFocus->HandleClicked();
		//MyCube->Movement(CurrentBlockFocus->BlockPosition);
		
		if (MyCube != nullptr) { 
			MyCube->Movement(CurrentBlockFocus->BlockPosition); 
			
		}
		else { SetCube(this); }
		//UE_LOG(LogTemp, Warning, TEXT("hit"))
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