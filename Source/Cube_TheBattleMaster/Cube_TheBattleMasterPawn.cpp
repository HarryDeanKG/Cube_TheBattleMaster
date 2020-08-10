// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "Cube_TheBattleMasterPawn.h"
#include "Cube_TheBattleMasterBlock.h"
#include "Cube_TheBattleMasterBlockGrid.h"
#include "Cube_TheBattleMasterGameMode.h"
#include "Player_Cube.h"
#include "SmallMunition.h"
#include "Cube_TheBattleMasterPlayerController.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/PlayerController.h"
#include "UObject/ConstructorHelpers.h"
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
	MoveSpeed = 100.0f;
	ActionNumb = 0;
	DoActionNumb = 0;

	M_PossibleActions.Add("Missles", "Attack");
	M_PossibleActions.Add("Short Attack", "Attack");

	M_PossibleActions.Add("Attack Drones", "Drone");
	M_PossibleActions.Add("Mine Drones", "Drone");

	M_PossibleActions.Add("Gas", "Utility");
	M_PossibleActions.Add("Shield", "Utility");

}

/*TODO: send actions to an action function, these are just to talk with blueprint*/
void ACube_TheBattleMasterPawn::Movement_Test(bool bToggle)
{
	if (MyCube) { HighlightMoveOptions(MyCube->BlockOwner, bToggle); }
}

void ACube_TheBattleMasterPawn::Attack_Test(bool bToggle)
{
	if (MyCube) { HighlightAttackOptions(MyCube->BlockOwner, bToggle); }

}


void ACube_TheBattleMasterPawn::Confirm_Actions_Implementation()
{
	bReady = !bReady;
	if (StartingBlock) { MoveCube(StartingBlock, false); }
	
}
//
void ACube_TheBattleMasterPawn::ActionSelected_Implementation(){}

//void ACube_TheBattleMasterPawn::BeginPlay()
//{
//	Super::BeginPlay();


//	//if (ActionInterface != nullptr)
//	//{
//	//	ActionInterface_Instance = CreateWidget<UUserWidget>(GetWorld(), ActionInterface);
//	//	if (ActionInterface_Instance != nullptr)
//	//	{
//	//		ActionInterface_Instance->AddToViewport();
//	//	}
//	//}
//
//	SetCube(this);
//
//	//AutoPossessPlayer = EAutoReceiveInput::Player0;
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
	PlayerInputComponent->BindAction("Destroy", EInputEvent::IE_Pressed, this, &ACube_TheBattleMasterPawn::CubeDestroy);
	// bind movement axes
	PlayerInputComponent->BindAxis(TEXT("Forward"), this, &ACube_TheBattleMasterPawn::OnMoveForward);
	PlayerInputComponent->BindAxis(TEXT("Right"), this, &ACube_TheBattleMasterPawn::OnMoveRight);
	PlayerInputComponent->BindAxis(TEXT("Inward"), this, &ACube_TheBattleMasterPawn::OnMoveIn);

	//space bar for quick test events
	PlayerInputComponent->BindAction("Test", EInputEvent::IE_Pressed, this, &ACube_TheBattleMasterPawn::Test);
}

void ACube_TheBattleMasterPawn::Test() 
{ 
	UE_LOG(LogTemp, Warning, TEXT("TEST activated")); 

	//for (TObjectIterator<APlayer_Cube> DummyCube; DummyCube; ++DummyCube) { MyCube->bMove = true;  DummyCube->FinalPosition = FVector(0.f);  MyCube->E_TurnStateEnum = ETurnState::TS_InitiateActions; }
	
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

void ACube_TheBattleMasterPawn::OnRep_MyCuben()
{
	if (MyCube) {
		MyCube->SetOwningPawn(this);
	}
}

/*Movement function and move the camera to the cube*/
void ACube_TheBattleMasterPawn::Movement(FVector dummyPosition) {
	if (GetOwner()) {
		APlayerController* OurPlayerController = Cast<APlayerController>(GetOwner());
		auto dummyActor = OurPlayerController->GetViewTarget();
		dummyActor->SetActorLocation(FVector(dummyPosition.X, dummyPosition.Y, dummyActor->GetActorLocation().Z));
	}
}

void ACube_TheBattleMasterPawn::MoveCube(ACube_TheBattleMasterBlock* Block, bool bAction) {

	ACube_TheBattleMasterBlock* PreviousBlock;
	if (MyCube->BlockOwner) { PreviousBlock = MyCube->BlockOwner; }
	else { PreviousBlock = GetGrid()->Grid.FindRef(GetGrid()->GridReference.FindRef(MyCube->GetActorLocation())); }
	MyCube->BlockOwner = Block;

	if (!StartingBlock) { StartingBlock = PreviousBlock; }

	//ToggleOccupied(Block, false);
	if (PreviousBlock) { HighlightMoveOptions(PreviousBlock, false); }
	Block->CanMove(false);

	MyCube->bMove = true;
	//MyCube->bDoAction = true;
	Movement(Block->GetActorLocation());

	
	if (bAction) { SetAction("Movement", Block->BlockPosition - PreviousBlock->BlockPosition); }
	else { MyCube->StartPosition = MyCube->GetActorLocation(); }
}


void ACube_TheBattleMasterPawn::CalcCamera(float DeltaTime, struct FMinimalViewInfo& OutResult)
{
	Super::CalcCamera(DeltaTime, OutResult);

	OutResult.Rotation = FRotator(-90.0f, 0.0f, 0.0f);
}


void ACube_TheBattleMasterPawn::SetAction(FString ActionName, FVector DummyPosition) {
	//auto GameMode = Cast<ACube_TheBattleMasterGameMode>(UGameplayStatics::GetGameMode(this));
	// && GameMode->E_GameSectionEnum == EGameSection::GS_MidSection

	if (!bReady) {
		M_Action_Name.Add(ActionNumb, ActionName);
		M_Action_Pos.Add(ActionNumb, DummyPosition);
		ActionNumb++;
		ActionSelected();
		if (ActionNumb == 4) { Confirm_Actions(); }
	}/*UE_LOG(LogTemp, Warning, TEXT("Recorded Actions: %d"), ActionNumb);
	UE_LOG(LogTemp, Warning, TEXT("Ready? Actions: %s"), bReady? TEXT("true"): TEXT("false"));*/

}

void ACube_TheBattleMasterPawn::DoAction(int int_Action)
{
	MyCube->SetReplicatingMovement(true);

	String_Action = "Empty";

	if (M_Action_Name.Contains(int_Action) && M_Action_Pos.Contains(int_Action)) {
		String_Action = M_Action_Name[int_Action];
	}
	if (String_Action == "Movement") {
		ACube_TheBattleMasterBlockGrid* DummyGrid = GetGrid();
		ACube_TheBattleMasterBlock* DummyBlock = DummyGrid->Grid.FindRef(DummyGrid->GridReference.FindRef(MyCube->GetActorLocation() + M_Action_Pos[int_Action]));
		if (DummyGrid->GridReference.Contains(MyCube->GetActorLocation() + M_Action_Pos[int_Action])) {
			MoveCube(DummyBlock, false);
		}
	//	//Movement(GetActorLocation() + M_Action_Pos[int_Action]); 
	}
	else if (String_Action == "Attack") { AttackAction(this, M_Action_Pos[int_Action], false); }
	else {/*Do Nothing*/ }
	UE_LOG(LogTemp, Warning, TEXT("Do Action: %s Position: %s"), *String_Action,*(M_Action_Pos[int_Action]).ToString());
	MyCube->bDoAction = true;

}

void ACube_TheBattleMasterPawn::AttackAction(ACube_TheBattleMasterPawn* Pawn, FVector AttackDirection, bool bAction)
{
	if (!bReady){
		//const FRotator SpawnRotation = GetControlRotation();
		const FRotator SpawnRotation = FRotator(0, 0, 0);

		const FVector SpawnLocation = MyCube->GetActorLocation() ;

		ASmallMunition* Bullet = GetWorld()->SpawnActor<ASmallMunition>(SpawnLocation, FRotator(0, 0, 0));
		Bullet->SetOwner(MyCube);

		Bullet->Direction = AttackDirection;

		if (bAction) { HighlightAttackOptions(MyCube->BlockOwner, false); SetAction("Attack", AttackDirection); }

	}
	//If at the setting actions stage spawn locally
	//If doing the actions together then it needs to be on the server and replicated.
	else if (GetLocalRole() < ROLE_Authority) {
			Server_AttackAction(this, AttackDirection, false);
			const FRotator SpawnRotation = FRotator(0, 0, 0);

			const FVector SpawnLocation = MyCube->GetActorLocation();

			ASmallMunition* Bullet = GetWorld()->SpawnActor<ASmallMunition>(MyCube->GetActorLocation(), FRotator(0, 0, 0));
			Bullet->SetOwner(MyCube);
			//Bullet->SetReplicates(true);
			Bullet->Direction = AttackDirection;

		}
		else {

			//const FRotator SpawnRotation = GetControlRotation();
			const FRotator SpawnRotation = FRotator(0, 0, 0);

			const FVector SpawnLocation = MyCube->GetActorLocation();

			ASmallMunition* Bullet = GetWorld()->SpawnActor<ASmallMunition>(MyCube->GetActorLocation(), FRotator(0, 0, 0));
			Bullet->SetOwner(MyCube);
			Bullet->SetReplicates(true);
			Bullet->Direction = AttackDirection;
		}
	
}

void ACube_TheBattleMasterPawn::Server_AttackAction_Implementation(ACube_TheBattleMasterPawn* Pawn, FVector AttackDirection, bool bAction) { AttackAction(Pawn, AttackDirection, false); }

ACube_TheBattleMasterBlockGrid* ACube_TheBattleMasterPawn::GetGrid() 
{
	FString StopLoop = GetName();
	StopLoop.RemoveAt(0, GetName().Len() - 1);
	int32 Stoploop = FCString::Atoi(*StopLoop);

	int32 i = 0;
	for (TObjectIterator<ACube_TheBattleMasterBlockGrid> Grid; Grid; ++Grid) {
		if (i == Stoploop + 1) { return *Grid; }
		i++;
	}return nullptr;
}


void ACube_TheBattleMasterPawn::SetCube(ACube_TheBattleMasterPawn* Pawn, ACube_TheBattleMasterBlock* Block)
{
	if (GetLocalRole() < ROLE_Authority) { Server_SetCube(Pawn, Block); }
	else {
		if (!Pawn->MyCube) {

			//FVector SpawnLocation = Block->GetActorLocation() + FVector(0.f, 0.f, 500.f);
			//UE_LOG(LogTemp, Warning, TEXT("Spawn here: %s"), *SpawnLocation.ToString());
			//Pawn->MyCube = GetWorld()->SpawnActor<APlayer_Cube>(Block->GetActorLocation(), FRotator(0, 0, 0));
			//Pawn->MyCube->SetOwner(this);

			//Pawn->StartingBlock = Block;

			//Pawn->MoveCube(Block, false);
			//Pawn->Movement(MyCube->GetActorLocation());

			
			MyCube = GetWorld()->SpawnActor<APlayer_Cube>(Block->GetActorLocation(), FRotator(0, 0, 0));
			MyCube->SetOwner(this);

			StartingBlock = Block;

			MoveCube(Block, false);
			Movement(MyCube->GetActorLocation());
		}
	}//Pawn->CubeMade();
	CubeMade();
}

void ACube_TheBattleMasterPawn::Server_SetCube_Implementation(ACube_TheBattleMasterPawn* Pawn, ACube_TheBattleMasterBlock* Block){ SetCube(Pawn, Block); }


void ACube_TheBattleMasterPawn::ToggleOccupied(ACube_TheBattleMasterBlock* Block, bool Bon) {
	if (GetLocalRole() < ROLE_Authority)
	{
		Server_ToggleOccupied(Block, Bon);
	}
	//Block->bIsOccupied = Bon;
	//Block->ToggleOccupied(Bon);
}

void ACube_TheBattleMasterPawn::Server_ToggleOccupied_Implementation(ACube_TheBattleMasterBlock* Block, bool Bon) { ToggleOccupied(Block, Bon); }

void ACube_TheBattleMasterPawn::HighlightMoveOptions(ACube_TheBattleMasterBlock* Block, bool Bmove) {

	//UE_LOG(LogTemp, Warning, TEXT("MyCube position 1: %s"), *Pawn->MyCube->GetActorLocation().ToString())

	if (GetLocalRole() < ROLE_Authority) {
		//UE_LOG(LogTemp, Warning, TEXT("MyCube position 2: %s"), *Pawn->MyCube->GetActorLocation().ToString())

		Server_HighlightMoveOptions(Block, Bmove);
	}
	else {
		
		APlayer_Cube* ThisCube = MyCube;
		ACube_TheBattleMasterBlockGrid* BlockGrid = GetGrid();
		//UE_LOG(LogTemp, Warning, TEXT("MyCube position 3: %s"), *Pawn->MyCube->GetActorLocation().ToString())

		FVector2D Grid_Location ;
		
		if (Block) { Grid_Location = BlockGrid->GridReference.FindRef(Block->GetActorLocation()); }
		else { Grid_Location = BlockGrid->GridReference.FindRef(MyCube->GetActorLocation()); }
		FVector DummyLocation;
		if (Block) { DummyLocation = Block->GetTargetLocation(); }
		else { DummyLocation = MyCube->GetActorLocation(); }
			
		for (int32 X = Grid_Location.X - ThisCube->Base_Speed; X <= Grid_Location.X + ThisCube->Base_Speed; X++) {
			for (int32 Y = Grid_Location.Y - ThisCube->Base_Speed; Y <= Grid_Location.Y + ThisCube->Base_Speed; Y++) {
				if (BlockGrid->Grid.Contains(FVector2D(X, Y))) {
					if (FVector2D(X, Y) == BlockGrid->GridReference.FindRef(DummyLocation)) {
						if (Block) {
							Block->CanMove(false);
							/*ToggleOccupied(Block, Bmove);*/
						}
					}
					else {
						BlockGrid->Grid.FindRef(FVector2D(X, Y))->CanMove(Bmove);
					}
				}
			}
		}
		
	}
}


void ACube_TheBattleMasterPawn::Server_HighlightMoveOptions_Implementation(ACube_TheBattleMasterBlock* Block, bool Bmove) { HighlightMoveOptions(Block, Bmove); }


void ACube_TheBattleMasterPawn::HighlightAttackOptions(ACube_TheBattleMasterBlock* Block, bool bToggle)
{
	if (GetLocalRole() < ROLE_Authority) {
		Server_HighlightAttackOptions(Block, bToggle);
	}
	else{
		
		ACube_TheBattleMasterBlockGrid* BlockGrid = GetGrid();
		FVector2D Grid_Location;

		if (Block) { Grid_Location = BlockGrid->GridReference.FindRef(Block->GetActorLocation()); }
		else { Grid_Location = BlockGrid->GridReference.FindRef(MyCube->GetActorLocation()); }

		for (int32 X = 0; X <= 8*MyCube->AttackRange; X++) {
			int32 Radius = MyCube->AttackRange;
			float Angle = 2 * 3.14159 * X / (8 * MyCube->AttackRange);
		
			Highlight_Block(FMath::RoundHalfFromZero(Grid_Location.X + Radius * FMath::Sin(Angle)), FMath::RoundHalfFromZero(Grid_Location.Y + Radius * FMath::Cos(Angle)), bToggle);

		}
	}
}
void ACube_TheBattleMasterPawn::Highlight_Block(int32 dummyX, int32 dummyY, bool bToggle)
{
	
	while (dummyX < 0) { ++dummyX; }
	while (dummyX >= GetGrid()->Size) { --dummyX; }
	while (dummyY < 0) { ++dummyY; }
	while (dummyY >= GetGrid()->Size) { --dummyY; }

	if (GetGrid()->Grid.Contains(FVector2D(dummyX, dummyY))) {
		GetGrid()->Grid.FindRef(FVector2D(dummyX, dummyY))->CanAttack(bToggle);
	}
}

void ACube_TheBattleMasterPawn::Server_HighlightAttackOptions_Implementation(ACube_TheBattleMasterBlock* Block, bool bToggle)
{
	HighlightAttackOptions(Block, bToggle);
}

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
			if (!bReady) {
				if (CurrentBlockFocus->bMove) { MoveCube(CurrentBlockFocus, true); this->Reset_Buttons_test();}
				else if (CurrentBlockFocus->bAttack) { AttackAction(this, CurrentBlockFocus->GetActorLocation() - MyCube->GetActorLocation(), true); this->Reset_Buttons_test(); }
			}
		}
		else { SetCube(this, CurrentBlockFocus); }
	}//ActionSelected();
}





void ACube_TheBattleMasterPawn::Turn()
{
	if (GetLocalRole() < ROLE_Authority) { Server_Turn(); }
	else {
		ACube_TheBattleMasterGameMode* BaseGameMode = Cast<ACube_TheBattleMasterGameMode>(UGameplayStatics::GetGameMode(this));
		BaseGameMode->TakeTurn();
	}	
}

void ACube_TheBattleMasterPawn::Server_Turn_Implementation() { Turn();}


void ACube_TheBattleMasterPawn::Ready_Button_Implementation()
{
	Turn();
}

void ACube_TheBattleMasterPawn::ResetEverything() {
	M_Action_Name.Empty();
	M_Action_Pos.Empty();
	ActionNumb = 0;
	bReady = false;

	StartingBlock=NULL;

	this->Reset_Buttons_test();

}


void ACube_TheBattleMasterPawn::Cancel_Button_Implementation() {
	M_Action_Name.Empty();
	M_Action_Pos.Empty();
	ActionNumb = 0;
	bReady = !bReady;
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
		
		bDead = true;
		MyCube->Destroy();
		if (GetLocalRole() < ROLE_Authority) {
			Server_CubeDestroy();
		}
		else {
			ACube_TheBattleMasterGameMode* BaseGameMode = Cast<ACube_TheBattleMasterGameMode>(UGameplayStatics::GetGameMode(this));
			BaseGameMode->EndGameCondition();
		}
		HighlightMoveOptions(MyCube->BlockOwner, false);
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