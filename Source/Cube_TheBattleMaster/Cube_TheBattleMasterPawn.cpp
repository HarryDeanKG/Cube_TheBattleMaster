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

	CamSpeed = 800.0f;
	ActionNumb = 0;

}

/*TODO: send actions to an action function, these are just to talk with blueprint*/
void ACube_TheBattleMasterPawn::Movement_Test()
{
	HighlightMoveOptions(this, MyCube->BlockOwner, true);
	UE_LOG(LogTemp, Warning, TEXT("MOVE!"));
	SetAction("Movement");
}

void ACube_TheBattleMasterPawn::Attack_Test()
{
	UE_LOG(LogTemp, Warning, TEXT("ATTACK!"));
	SetAction("Attack");
}



void ACube_TheBattleMasterPawn::SetAction(FString ActionName) {
	//auto GameMode = Cast<ACube_TheBattleMasterGameMode>(UGameplayStatics::GetGameMode(this));
	// && GameMode->E_GameSectionEnum == EGameSection::GS_MidSection
	if (!bReady) {
		M_Action.Add(ActionNumb, ActionName);
		ActionNumb++;
		if (ActionNumb == 4) { Turn(); }
	}UE_LOG(LogTemp, Warning, TEXT("Recorded Actions: %d"), ActionNumb);
	UE_LOG(LogTemp, Warning, TEXT("Ready? Actions: %s"), bReady? TEXT("true"): TEXT("false"));
	ActionSelected();
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
	PlayerInputComponent->BindAxis(TEXT("Inward"), this, &ACube_TheBattleMasterPawn::OnMoveIn);
}

/*Camera Movement*/
void ACube_TheBattleMasterPawn::OnMoveForward(float value)
{
	float MaxValue = 1840.0f;
	float MinValue = 100.0f;
	if (value != 0.0f)
	{
		float deltaTime = GetWorld()->GetDeltaSeconds();
		if (GetOwner()) {
			APlayerController* OurPlayerController = Cast<APlayerController>(GetOwner());

			auto dummyActor = OurPlayerController->GetViewTarget();

			FVector CurrentLocation = dummyActor->GetActorLocation();

			CurrentLocation.X = FMath::Clamp(CurrentLocation.X + CamSpeed * value * deltaTime, MinValue, MaxValue);
			dummyActor->SetActorLocation(CurrentLocation);
		}
	}
}

void ACube_TheBattleMasterPawn::OnMoveRight(float value)
{
	float MaxValue = 1940.0f;
	float MinValue = 200.0f;
	if (value != 0.0f)
	{
		float deltaTime = GetWorld()->GetDeltaSeconds();
		if (GetOwner()) {
			APlayerController* OurPlayerController = Cast<APlayerController>(GetOwner());
			
			auto dummyActor = OurPlayerController->GetViewTarget();

			FVector CurrentLocation = dummyActor->GetActorLocation();

			CurrentLocation.Y = FMath::Clamp(CurrentLocation.Y + CamSpeed * value * deltaTime, MinValue, MaxValue);
			dummyActor->SetActorLocation(CurrentLocation);
		}
	}
}

void ACube_TheBattleMasterPawn::OnMoveIn(float value)
{
	float MaxValue = 3000.0f;
	float MinValue = 200.0f;
	if (value != 0.0f)
	{
		float deltaTime = GetWorld()->GetDeltaSeconds();
		if (GetOwner()) {
			APlayerController* OurPlayerController = Cast<APlayerController>(GetOwner());

			auto dummyActor = OurPlayerController->GetViewTarget();

			FVector CurrentLocation = dummyActor->GetActorLocation();

			CurrentLocation.Z = FMath::Clamp(CurrentLocation.Z + CamSpeed * value * deltaTime, MinValue, MaxValue);
			dummyActor->SetActorLocation(CurrentLocation);
		}
	}
}

/*Movement function and move the camera to the cube*/
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

void ACube_TheBattleMasterPawn::DoAction(int32 int_Action)
{
	FString String_Action = "Empty";

	if (M_Action.Contains(int_Action)) {
		String_Action = M_Action[int_Action];
	}
	if (String_Action=="Movement"){DoMove("Up",1);}
	else if (String_Action == "Attack") {DoDamage(this->MyCube,this->MyCube);}
	else {/*Do Nothing*/}
	UE_LOG(LogTemp, Warning, TEXT("%s"), *String_Action);
	//ActionSelected();
}

/*This is all ready for DoAction to call it*/
void ACube_TheBattleMasterPawn::DoMove(FString Position, int32 MoveNumb) {
	ACube_TheBattleMasterBlockGrid* DummyGrid  = GetGrid();
	FVector2D CurrentPosition = DummyGrid->GridReference.FindRef(MyCube->BlockOwner->GetTargetLocation());
	FVector2D MovePosition;

	if (Position == "Up") { MovePosition = FVector2D(1, 0); }
	else if (Position == "Down") { MovePosition = FVector2D(-1, 0); }
	else if (Position == "Right") { MovePosition = FVector2D(0, 1); }
	else if (Position == "Down") { MovePosition = FVector2D(0, -1); }
	else { MovePosition = FVector2D(0, 0); }

	FVector2D SetPosition = CurrentPosition + MovePosition * MoveNumb;
	//UE_LOG(LogTemp, Warning, TEXT("%s : Go to %s "), (*Position, *SetPosition.ToString()));
	if (DummyGrid->Grid.Contains(SetPosition)) { Movement(DummyGrid->Grid.FindRef(SetPosition)->GetTargetLocation()); }
	else Movement(DummyGrid->Grid.FindRef(CurrentPosition)->GetTargetLocation()); 
}

ACube_TheBattleMasterBlockGrid* ACube_TheBattleMasterPawn::GetGrid() {
	FString StopLoop = GetName();
	StopLoop.RemoveAt(0, GetName().Len() - 1);
	int32 Stoploop = FCString::Atoi(*StopLoop);
	int32 i = 0;
	for (TObjectIterator<ACube_TheBattleMasterBlockGrid> Grid; Grid; ++Grid) {
		if (i == Stoploop + 1) { return *Grid; }
		i++;
	}return nullptr;
}


void ACube_TheBattleMasterPawn::SetCube(ACube_TheBattleMasterPawn* Pawn)
{
	if (GetLocalRole() < ROLE_Authority) {
		Server_SetCube(Pawn);
	}
	else {
		if (Pawn->MyCube == nullptr) {
			ACube_TheBattleMasterBlockGrid* ThisGrid = GetGrid();
			TArray<FVector> GridKeys;
			ThisGrid->GridReference.GenerateKeyArray(GridKeys);
			

			FVector dummyPosition = GridKeys[FMath::RandRange(0, GridKeys.Num())];
			//FVector Position = FVector(1000, 500, 0);
			Pawn->MyCube = GetWorld()->SpawnActor<APlayer_Cube>(dummyPosition, FRotator(0, 0, 0));
			Pawn->MyCube->SetOwner(this);
			Pawn->MyCube->Movement(dummyPosition);

			Movement(dummyPosition);
			ACube_TheBattleMasterBlock* Block = ThisGrid->Grid.FindRef(ThisGrid->GridReference.FindRef(dummyPosition));
			Block->CanMove(false);
			Block->ToggleOccupied(true);
			MyCube->BlockOwner = Block;
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

void ACube_TheBattleMasterPawn::TheGameMode() {
	if (GetLocalRole() < ROLE_Authority) { GameMode = Cast<ACube_TheBattleMasterGameMode>(UGameplayStatics::GetGameMode(this)); }
	else { Server_TheGameMode(); }
}

void ACube_TheBattleMasterPawn::Server_TheGameMode_Implementation() { 
	ACube_TheBattleMasterPlayerController* Test= Cast<ACube_TheBattleMasterPlayerController>(GetOwner());
	GameMode =  Test -> GetGameMode();
	//GameMode = Cast<ACube_TheBattleMasterGameMode>(UGameplayStatics::GetGameMode(this)); 
}

void ACube_TheBattleMasterPawn::TriggerClick()
{
	//GameMode = (ACube_TheBattleMasterGameMode*)GetWorld()->GetAuthGameMode();
	//GameMode->E_GameSectionEnum = EGameSection::GS_StartSection;

	if (CurrentBlockFocus && !bDead)
	{			
		CurrentBlockFocus->HandleClicked();
		if (MyCube != nullptr) {
			if (CurrentBlockFocus->bMove && !bReady) {
				MyCube->BlockOwner = CurrentBlockFocus;
				
				Movement(CurrentBlockFocus->BlockPosition);
				SetAction("Movement");

				CurrentBlockFocus->ToggleOccupied(true);
				CurrentBlockFocus->CanMove(false);
				CurrentBlockFocus->ToggleOccupied(true);

				Turn();
			}
		}
		else { SetCube(this); }
	}
}

void ACube_TheBattleMasterPawn::Turn()
{
	bReady = true;
	if (GetLocalRole() < ROLE_Authority) { Server_Turn(); }
	else {
		ACube_TheBattleMasterGameMode* BaseGameMode = Cast<ACube_TheBattleMasterGameMode>(UGameplayStatics::GetGameMode(this));
		BaseGameMode->TakeTurn();
		//ActionNumb = 0;
	}
	
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