// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "Cube_TheBattleMasterPawn.h"
#include "Cube_TheBattleMasterBlock.h"
#include "Cube_TheBattleMasterBlockGrid.h"
#include "Cube_TheBattleMasterGameMode.h"
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
#include "Misc/OutputDeviceNull.h"


<<<<<<< HEAD
<<<<<<< HEAD

=======
>>>>>>> parent of 375cd1d... Buttons buttons who got the buttons
=======
>>>>>>> parent of 375cd1d... Buttons buttons who got the buttons
ACube_TheBattleMasterPawn::ACube_TheBattleMasterPawn(const FObjectInitializer& ObjectInitializer) 
	: Super(ObjectInitializer)
{
	AutoPossessPlayer = EAutoReceiveInput::Disabled;
	//SetReplicates(true);

	//Set up a dummy root system
	DummyRoot = CreateDefaultSubobject<USceneComponent>(TEXT("Dummy0"));
	RootComponent = DummyRoot;


		//Create camera components
	OurCameraSpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraSpringArm"));
	OurCameraSpringArm->SetupAttachment(DummyRoot);
	//OurCameraSpringArm->SetRelativeLocationAndRotation(FVector(200.0f, 0.0f, 0.0f), FRotator(0.0f, 0.0f, 0.0f));
	OurCameraSpringArm->TargetArmLength = 400.f;
	OurCameraSpringArm->bEnableCameraLag = true;
	OurCameraSpringArm->CameraLagSpeed = 3.0f;

	OurCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("GameCamera"));
	OurCamera->SetupAttachment(OurCameraSpringArm, USpringArmComponent::SocketName);
}

<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
void ACube_TheBattleMasterPawn::Movement_Test()
{
	HighlightMoveOptions(this, MyCube->BlockOwner, true);
	UE_LOG(LogTemp, Warning, TEXT("MOVE!"))
}

void ACube_TheBattleMasterPawn::Attack_Test()
{
	UE_LOG(LogTemp, Warning, TEXT("ATTACK!"))
}

/*void ACube_TheBattleMasterPawn::BeginPlay()
{
	Super::BeginPlay();

	if (ActionInterface != nullptr)
	{
		ActionInterface_Instance = CreateWidget<UUserWidget>(GetWorld(), ActionInterface);
		if (ActionInterface_Instance != nullptr)
		{
			ActionInterface_Instance->AddToViewport();
		}
	}

	//SetCube(this);

	//AutoPossessPlayer = EAutoReceiveInput::Player0;
}*/	
=======
=======
>>>>>>> parent of 375cd1d... Buttons buttons who got the buttons
=======
>>>>>>> parent of 375cd1d... Buttons buttons who got the buttons
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
<<<<<<< HEAD
<<<<<<< HEAD
>>>>>>> parent of 375cd1d... Buttons buttons who got the buttons
=======
>>>>>>> parent of 375cd1d... Buttons buttons who got the buttons
=======
>>>>>>> parent of 375cd1d... Buttons buttons who got the buttons

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
	PlayerInputComponent->BindAction("Destroy", EInputEvent::IE_Pressed, this, &ACube_TheBattleMasterPawn::CubeDestroy);
}

void ACube_TheBattleMasterPawn::CalcCamera(float DeltaTime, struct FMinimalViewInfo& OutResult)
{
	Super::CalcCamera(DeltaTime, OutResult);

	OutResult.Rotation = FRotator(-90.0f, 0.0f, 0.0f);
}

void ACube_TheBattleMasterPawn::CameraMove(AActor* dummyActor) {
	//make sure owner is not null then because owner is a player controller just cast it
	if (GetOwner()) {
		APlayerController* OurPlayerController = Cast<APlayerController>(GetOwner());
		if ((OurPlayerController->GetViewTarget() != dummyActor) && (dummyActor != nullptr))
		{
			// Cut instantly to cube camera.
			OurPlayerController->SetViewTargetWithBlend(dummyActor, 0.75f);
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

void ACube_TheBattleMasterPawn::ToggleOccupied(ACube_TheBattleMasterBlock* Block, bool Bon) {
	if (Role < ROLE_Authority)
	{
		Server_ToggleOccupied(Block, Bon);
	}
	Block->bIsOccupied = Bon;
	Block->ToggleOccupied(Bon);
}

void ACube_TheBattleMasterPawn::Server_ToggleOccupied_Implementation(ACube_TheBattleMasterBlock* Block, bool Bon) { ToggleOccupied(Block, Bon); }

void ACube_TheBattleMasterPawn::HighlightMoveOptions(ACube_TheBattleMasterPawn* Pawn, ACube_TheBattleMasterBlock* Block, bool Bmove) {
	
	APlayer_Cube* ThisCube = Pawn->MyCube;
	ACube_TheBattleMasterBlockGrid* BlockGrid;
	ACube_TheBattleMasterBlockGrid* DummyGrid;
	FVector2D Grid_Location;
	int32 size;
	if (Role < ROLE_Authority) {
		Server_HighlightMoveOptions(Pawn, Block, Bmove);
	}
	else {
		if (ThisCube != nullptr) {
			FString StopLoop = GetName();
			StopLoop.RemoveAt(0, GetName().Len()-1);
			int32 Stoploop = FCString::Atoi(*StopLoop);
			int32 i = 0;
			for (TObjectIterator<ACube_TheBattleMasterBlockGrid> Grid; Grid; ++Grid) {
				DummyGrid = *Grid;
				Grid_Location = DummyGrid->GridReference.FindRef(ThisCube->GetActorLocation());
				size = DummyGrid->Size;
				if (i != 0) { DummyGrid->Grid.FindRef(DummyGrid->GridReference.FindRef(Block->GetTargetLocation()))->ToggleOccupied(Bmove); }
				if (i == Stoploop+1) { BlockGrid = *Grid; }
				i++;
			}
			for (int32 X = Grid_Location.X - ThisCube->Base_Speed; X <= Grid_Location.X + ThisCube->Base_Speed; X++) {
				for (int32 Y = Grid_Location.Y - ThisCube->Base_Speed; Y <= Grid_Location.Y + ThisCube->Base_Speed; Y++) {
					if (BlockGrid->Grid.Contains(FVector2D(X, Y))) {
						if (FVector2D(X, Y)==BlockGrid->GridReference.FindRef(Block->GetTargetLocation())) { //ToggleOccupied(Block, Bmove);
						}
						else { BlockGrid->Grid.FindRef(FVector2D(X, Y))->CanMove(Bmove); }
					}
				}
			}
		}
	}
}

void ACube_TheBattleMasterPawn::Server_HighlightMoveOptions_Implementation(ACube_TheBattleMasterPawn* Pawn, ACube_TheBattleMasterBlock* Block, bool Bmove) { HighlightMoveOptions(Pawn, Block, Bmove); }

void ACube_TheBattleMasterPawn::TriggerClick()
{
	if (CurrentBlockFocus && !bDead)
	{			
		CurrentBlockFocus->HandleClicked();
		if (MyCube != nullptr) {

			if (CurrentBlockFocus->bMove || MyCube->BlockOwner == nullptr) {
				if (MyCube->BlockOwner != nullptr) { HighlightMoveOptions(this, MyCube->BlockOwner, false); }

				MyCube->BlockOwner = CurrentBlockFocus;
				MyCube->Movement(CurrentBlockFocus->BlockPosition);
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
				
				
				this->Reset_Buttons();
				//HighlightMoveOptions(this, CurrentBlockFocus, true);


				//MyCube->ApplyDamage(MyCube, 10, MyCube);
				//UE_LOG(LogTemp, Warning, TEXT("Testing Damage. Helth is %f"), Replicated_Health);
=======

				HighlightMoveOptions(this, CurrentBlockFocus, true);
>>>>>>> parent of 375cd1d... Buttons buttons who got the buttons
=======

				HighlightMoveOptions(this, CurrentBlockFocus, true);
>>>>>>> parent of 375cd1d... Buttons buttons who got the buttons
=======

				HighlightMoveOptions(this, CurrentBlockFocus, true);
>>>>>>> parent of 375cd1d... Buttons buttons who got the buttons
			}
		}
		else { SetCube(this); }
	}
}


//bool ACube_TheBattleMasterPawn::IsInVacinity() {
//	ACube_TheBattleMasterBlockGrid* BlockGrid;
//	
//	FString StopLoop = GetName();
//	StopLoop.RemoveAt(0, GetName().Len() - 1);
//	int32 Stoploop = FCString::Atoi(*StopLoop);
//	int32 i = 0;
//	for (TObjectIterator<ACube_TheBattleMasterBlockGrid> Grid; Grid; ++Grid) {
//		DummyGrid = *Grid;
//		Grid_Location = DummyGrid->GridReference.FindRef(ThisCube->GetActorLocation());
//		size = DummyGrid->Size;
//		if (i != 0) { DummyGrid->Grid.FindRef(DummyGrid->GridReference.FindRef(Block->GetTargetLocation()))->ToggleOccupied(Bmove); }
//		if (i == Stoploop + 1) { BlockGrid = *Grid; }
//		i++;
//	}
//
//
//	for (int32 X = Grid_Location.X - ThisCube->Base_Speed; X <= Grid_Location.X + ThisCube->Base_Speed; X++) {
//		for (int32 Y = Grid_Location.Y - ThisCube->Base_Speed; Y <= Grid_Location.Y + ThisCube->Base_Speed; Y++) {
//			if (BlockGrid->Grid.Contains(FVector2D(X, Y))) {
//				if (FVector2D(X, Y) == BlockGrid->GridReference.FindRef(Block->GetTargetLocation())) { //ToggleOccupied(Block, Bmove);
//				}
//				else { BlockGrid->Grid.FindRef(FVector2D(X, Y))->CanMove(Bmove); }
//			}
//		}
//	}
//}

void ACube_TheBattleMasterPawn::CubeDestroy() {
	
	if (MyCube != nullptr) {
		CameraMove(this);
		bDead = true;
		MyCube->Destroy();
		if (Role < ROLE_Authority) {
			Server_CubeDestroy();
		}
		else {
			ACube_TheBattleMasterGameMode* BaseGameMode = Cast<ACube_TheBattleMasterGameMode>(UGameplayStatics::GetGameMode(this));
			BaseGameMode->EndGameCondition();
		}
		HighlightMoveOptions(this, MyCube->BlockOwner, false);
	}
}

void ACube_TheBattleMasterPawn::Reset_Buttons()
{
	/*Empty*/
	
	
	/*FOutputDeviceNull ar;
	GetLevel()->CallFunctionByNameWithArguments(TEXT("ResetButtons"), ar, NULL, true);*/

}

void ACube_TheBattleMasterPawn::Server_CubeDestroy_Implementation() {
	MyCube->Destroy(); 
	ACube_TheBattleMasterGameMode* BaseGameMode = Cast<ACube_TheBattleMasterGameMode>(UGameplayStatics::GetGameMode(this));
	BaseGameMode->EndGameCondition();
}

void ACube_TheBattleMasterPawn::DoDamage(APlayer_Cube* OwnedCube, APlayer_Cube* ToDamageCube) {
	ToDamageCube->Replicated_Health -= OwnedCube->Base_Damage;
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