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

	CamSpeed = 300.0f;
	
}


void ACube_TheBattleMasterPawn::Movement_Test()
{
	if (!bReady) {
		HighlightMoveOptions(this, MyCube->BlockOwner, true);
		UE_LOG(LogTemp, Warning, TEXT("MOVE!"))
	}
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
	// bind movement axes
	PlayerInputComponent->BindAxis(TEXT("Forward"), this, &ACube_TheBattleMasterPawn::OnMoveForward);
	PlayerInputComponent->BindAxis(TEXT("Right"), this, &ACube_TheBattleMasterPawn::OnMoveRight);

}

void ACube_TheBattleMasterPawn::OnMoveForward(float value)
{
	if (value != 0.0f)
	{
		float deltaTime = GetWorld()->GetDeltaSeconds();
		if (GetOwner()) {
			APlayerController* OurPlayerController = Cast<APlayerController>(GetOwner());

			auto dummyActor = OurPlayerController->GetViewTarget();

			FVector CurrentLocation = dummyActor->GetActorLocation();
			dummyActor->SetActorLocation(CurrentLocation + FVector(CamSpeed*value * deltaTime, 0.f, 0.f));
			//FVector CurrentLocation = OurCameraSpringArm->AddRelativeLocation();
			//OurCameraSpringArm->SetWorldLocation(FVector(CamSpeed*value * deltaTime, 0.f, 0.f));

		}
	}
}

void ACube_TheBattleMasterPawn::OnMoveRight(float value)
{
	if (value != 0.0f)
	{
		// we move perpendicular to the controller view direction
		const FRotator rot = GetControlRotation();
		const FVector dir = FRotationMatrix(rot).GetScaledAxis(EAxis::Y);

		float deltaTime = GetWorld()->GetDeltaSeconds();

		//UE_LOG(LogTemp, Warning, TEXT("CameraLocation: %s"), *GetCamera()->GetRelativeLocation().ToString());
		UE_LOG(LogTemp, Warning, TEXT("Movement: %f"), value);
		//OurCamera->SetRelativeLocation(CameraLocation+FVector(CamSpeed * value * deltaTime, 0, 0));
		
		//UE_LOG(LogTemp, Warning, TEXT("CameraLocation: %s"), *OurCamera->GetComponentLocation().ToString());
		if (GetOwner()) {
			APlayerController* OurPlayerController = Cast<APlayerController>(GetOwner());
			
			auto dummyActor = OurPlayerController->GetViewTarget();

			FVector CurrentLocation = dummyActor->GetActorLocation();
			dummyActor->SetActorLocation(CurrentLocation+FVector(0.f, CamSpeed*value * deltaTime, 0.f));
		}
		
	}
}

void ACube_TheBattleMasterPawn::Movement(FVector dummyPosition) {
	if (MyCube) { MyCube->Movement(dummyPosition); }
	if (GetOwner()) {
		APlayerController* OurPlayerController = Cast<APlayerController>(GetOwner());
		auto dummyActor = OurPlayerController->GetViewTarget();
		dummyActor->SetActorLocation(FVector(dummyPosition.X, dummyPosition.Y, dummyActor->GetActorLocation().Z));
	}
}

void ACube_TheBattleMasterPawn::CalcCamera(float DeltaTime, struct FMinimalViewInfo& OutResult)
{
	Super::CalcCamera(DeltaTime, OutResult);

	OutResult.Rotation = FRotator(-90.0f, 0.0f, 0.0f);
}

void ACube_TheBattleMasterPawn::SetCube(ACube_TheBattleMasterPawn* Pawn)
{
	if (GetLocalRole() < ROLE_Authority) {
		Server_SetCube(Pawn);
	}
	else {
		if (Pawn->MyCube == nullptr) {
			FVector Position = FVector(1000, 500, 0);
			Pawn->MyCube = GetWorld()->SpawnActor<APlayer_Cube>(Position, FRotator(0, 0, 0));
			Pawn->MyCube->SetOwner(this);

			//TODO This sets the cube as movement so check if pawn is selected then move
			
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
	if (GetLocalRole() < ROLE_Authority)
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
	if (GetLocalRole() < ROLE_Authority) {
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

			if (CurrentBlockFocus->bMove && !bReady) {
				//if (MyCube->BlockOwner != nullptr) { HighlightMoveOptions(this, MyCube->BlockOwner, false); }

				MyCube->BlockOwner = CurrentBlockFocus;
				//MyCube->Movement(CurrentBlockFocus->BlockPosition);
				//SetActorLocation(CurrentBlockFocus->BlockPosition);
				Movement(CurrentBlockFocus->BlockPosition);
				Turn();

				//if (CurrentBlockFocus->bMove) { CurrentBlockFocus->Highlight(false); }
				//if (!CurrentBlockFocus->bIsOccupied) { CurrentBlockFocus->ToggleOccupied(true); }

				CurrentBlockFocus->ToggleOccupied(true);
				CurrentBlockFocus->CanMove(false);
				CurrentBlockFocus->ToggleOccupied(true);
				UE_LOG(LogTemp, Warning, TEXT("Testing"))
				//this->BP_ResetButtons();

				//HighlightMoveOptions(this, CurrentBlockFocus, true);

				//MyCube->ApplyDamage(MyCube, 10, MyCube);
				//UE_LOG(LogTemp, Warning, TEXT("Testing Damage. Helth is %f"), Replicated_Health);
			}
		}
		else { SetCube(this); }
	}
}

void ACube_TheBattleMasterPawn::Turn()
{
	if (GetLocalRole() < ROLE_Authority) { Server_Turn(); }
	else {
		ACube_TheBattleMasterGameMode* BaseGameMode = Cast<ACube_TheBattleMasterGameMode>(UGameplayStatics::GetGameMode(this));
		BaseGameMode->TakeTurn();
	}
	
	bReady = true;
}

void ACube_TheBattleMasterPawn::Server_Turn_Implementation() { Turn(); }

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
		
		bDead = true;
		MyCube->Destroy();
		if (GetLocalRole() < ROLE_Authority) {
			Server_CubeDestroy();
		}
		else {
			ACube_TheBattleMasterGameMode* BaseGameMode = Cast<ACube_TheBattleMasterGameMode>(UGameplayStatics::GetGameMode(this));
			BaseGameMode->EndGameCondition();
		}
		HighlightMoveOptions(this, MyCube->BlockOwner, false);
	}
}
//
//void ACube_TheBattleMasterPawn::BP_ResetButtons_Implementation()
//{
//	/*Empty*/
//	
//	/*FOutputDeviceNull ar;
//	GetLevel()->CallFunctionByNameWithArguments(TEXT("ResetButtons"), ar, NULL, true);*/
//	UE_LOG(LogTemp, Warning, TEXT("Reset!"));
//}

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