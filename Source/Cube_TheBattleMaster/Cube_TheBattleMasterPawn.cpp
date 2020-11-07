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

	//M_PossibleActions.Add("Missles", "Attack");
	//M_PossibleActions.Add("Short Attack", "Attack");

	//M_PossibleActions.Add("Attack Drones", "Drone");
	//M_PossibleActions.Add("Mine Drones", "Drone");

	//M_PossibleActions.Add("Gas", "Utility");
	//M_PossibleActions.Add("Shield", "Utility");

}


/*TODO: send actions to an action function, these are just to talk with blueprint*/
void ACube_TheBattleMasterPawn::Movement_Test(bool bToggle)
{
	if (MyCube) { HighlightMoveOptions(MyCube->BlockOwner, bToggle); }
}

void ACube_TheBattleMasterPawn::Attack_Test(FString Name, bool bToggle)
{
	if (MyCube) { HighlightAttackOptions(MyCube->BlockOwner, bToggle, 8, true); }
	WeaponName = Name;
}


void ACube_TheBattleMasterPawn::StopMovement() {

	MoveCube(MyCube->BlockOwner, true);
	/*
	if (MoveNumb < 4) {
		M_MovementList.Add(MoveNumb, MyCube->GetActorLocation());
		MoveNumb++;
		RefreshMovement();
	}*/
}

void ACube_TheBattleMasterPawn::NoAction() {
	FAction_Struct NothingAction;
	NothingAction.Action_Name = "Do Nothings";
	//NothingAction.Action_Position = M_MovementList.FindRef(ActionNumb);
	//NothingAction.MovementDuringAction = M_MovementList.FindRef(ActionNumb);
	M_ActionStructure.Add(ActionNumb, NothingAction);

	ActionNumb++;
	ActionSelected();

}


void ACube_TheBattleMasterPawn::SetInMotionSelectedAction(AItemBase* dummyItem) {
	//UE_LOG(LogTemp, Warning, TEXT("YEAH!"));
	SelectedItem = dummyItem;
	dummyItem->SetActionInMotion();
}

//BUTTONS for interfaces
void ACube_TheBattleMasterPawn::Ready_Button_Implementation()
{
	
	ResetEverything(false);
	bReady = true;

	Turn();

}
void ACube_TheBattleMasterPawn::Cancel_Button_Implementation() {
	ResetEverything(true);

	ClearVars();

	//bReady = false;
}

void ACube_TheBattleMasterPawn::ClearVars() {
	M_ActionStructure.Empty();
	M_MovementList.Empty();
	UpdateActions();
	RefreshMovement();

	ActionNumb = 0;
	MoveNumb = 0;

	Path.Empty();

}

void ACube_TheBattleMasterPawn::Confirm_Actions_Implementation()
{
	while (MoveNumb < 4) {
		M_MovementList.Add(MoveNumb, FVector(0.f) );

		FAction_Struct NothingAction = M_ActionStructure.FindRef(MoveNumb);

		NothingAction.Action_Position = M_MovementList.FindRef(ActionNumb);
		NothingAction.MovementDuringAction = M_MovementList.FindRef(ActionNumb);
		
		MoveNumb++;
		UpdateActions();
		RefreshMovement();
	}
	this->Reset_Buttons_test();
	/*
	*/
	//ResetEverything(false);
	//bReady = true;
	//Ready_Button();
}
//

void ACube_TheBattleMasterPawn::ResetEverything(bool bResetPosition) {
	
	if (Path.Num() > 0) {
		Movement(Path[0]->GetActorLocation());
		MyCube->SetActorLocation(Path[0]->GetActorLocation());
	}

	for (auto& Elems : FlagedForDeleation) { if (Elems) { Elems->Destroy(); } }
	FlagedForDeleation.Empty();

	for (auto& Block : ToggleBackList) { Block->ToggleOccupied(false); }
	ToggleBackList.Empty();

	//UE_LOG(LogTemp, Warning, TEXT("Cube position: %s"), *MyCube->GetActorLocation().ToString());
	for (auto& Block : Path) { Block->HighlightPath(false); 	
		//UE_LOG(LogTemp, Warning, TEXT("Cube position: %s"), *Block->GetActorLocation().ToString());
	}
	if (bResetPosition) {
		Path.Empty();
	}
	/*
	if (bResetPosition) { if (StartingBlock) { MoveCube(StartingBlock, false); } }
	else { StartingBlock = NULL; this->Reset_Buttons_test(); }
	*/

	
	
}


void ACube_TheBattleMasterPawn::ActionSelected_Implementation(){
	UpdateActions();
	if (ActionNumb == 4) { Confirm_Actions(); }
}

//void ACube_TheBattleMasterPawn::BeginPlay()
//{
//	Super::BeginPlay();
//}

void ACube_TheBattleMasterPawn::UpdateActions_Implementation(){

	//M_PossibleActions.Empty();
	MyCube->UpdateActionList(M_PossibleActions);

	//UE_LOG(LogTemp, Warning, TEXT("Update"))

	//if (MyCube)
	//{
	//	for (auto& Elems : M_PossibleActions) {

	//		UE_LOG(LogTemp, Warning, TEXT("Key %s, Value %s"), *Elems.Key, *Elems.Value)
	//	}
	//}
}

void ACube_TheBattleMasterPawn::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);


	//UpdateActions();
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
	MyCube->bDoAction = true;
	//MyCube->bDoAction = true;
	Movement(Block->GetActorLocation());
	

	if (bAction) {
		
		//**If move twice, make the last action null**//
		if (MoveNumb  == ActionNumb + 1) {
			//SetAction("Movement", PreviousBlock);
			FAction_Struct NothingAction;
			NothingAction.Action_Name = "Do Nothings";
			NothingAction.Action_Position = M_MovementList.FindRef(ActionNumb);
			NothingAction.MovementDuringAction = M_MovementList.FindRef(ActionNumb);
			M_ActionStructure.Add(ActionNumb, NothingAction);

			ActionNumb++;
			//ActionSelected();
			
		}
		Highlight_Path(PreviousBlock, Block); 
		M_MovementList.Add(MoveNumb, Block->GetActorLocation() - PreviousBlock->GetActorLocation());
		MoveNumb++;

		RefreshMovement();

		ActionSelected();
	}
	else { MyCube->StartPosition = MyCube->GetActorLocation(); }
	//UE_LOG(LogTemp, Warning, TEXT("Is ready? %s"), MyCube->bMove ? TEXT("True") : TEXT("False"));
}


void ACube_TheBattleMasterPawn::CalcCamera(float DeltaTime, struct FMinimalViewInfo& OutResult)
{
	Super::CalcCamera(DeltaTime, OutResult);

	OutResult.Rotation = FRotator(0.0f, 0.0f, 0.0f);
}


void ACube_TheBattleMasterPawn::SetAction(FString ActionName, ACube_TheBattleMasterBlock* Block) {
	if (!bReady && ActionNumb < 4) {
		FAction_Struct Actions;
		FVector DummyPosition = Block->GetActorLocation() - MyCube->GetActorLocation();
		if (SelectedItem) {
			Actions.Action_Name = SelectedItem->ActionName;
			Actions.Action_Position = DummyPosition;
			Actions.Action_Weapon = SelectedItem->WeaponName;
			Actions.Item = SelectedItem;
			
			/*
			if the movement is at 4 then reset the movement, re do the path
			New function - redo action/movement
			*/

			if (M_MovementList.Contains(ActionNumb)) {
				if (!SelectedItem->bCanBeUsedWithMovement) {
					if (M_MovementList.FindRef(ActionNumb) != FVector(0.f)) {
						FAction_Struct NothingAction;
						NothingAction.Action_Name = "Movement";
						NothingAction.MovementDuringAction = M_MovementList.FindRef(ActionNumb);
						NothingAction.Action_Position = M_MovementList.FindRef(ActionNumb);
						M_ActionStructure.Add(ActionNumb, NothingAction);

						ActionNumb++;
						M_MovementList.Add(ActionNumb, FVector(0.f));
						MoveNumb++;
						RefreshMovement();
					}
				}
			}else{
				M_MovementList.Add(ActionNumb, FVector(0.f));
				MoveNumb++;
				RefreshMovement();
			}
			Actions.MovementDuringAction = M_MovementList.FindRef(ActionNumb);
			M_ActionStructure.Add(ActionNumb, Actions);
			
			
			//Actions.MovementDuringAction = M_MovementList.FindRef(ActionNumb);
			
			SelectedItem->DoAction(false, Block);
			SelectedItem = NULL;
		}
		else {
			Actions.Action_Name = ActionName;
			//Actions.Action_Name = = "Do Nothing";

			//Actions.Action_Position = DummyPosition;
			M_ActionStructure.Add(ActionNumb, Actions);
			

		}
		//UE_LOG(LogTemp, Warning, TEXT("%d  -> %s"), ActionNumb, *ActionName);

		ActionNumb++;
		ActionSelected();
		
	}
}

void ACube_TheBattleMasterPawn::DoAction(int int_Action)
{
	MyCube->SetReplicatingMovement(true);

	String_Action = "Empty";
	//UE_LOG(LogTemp, Warning, TEXT("%d"), M_ActionStructure.Num())
	if (M_ActionStructure.Contains(int_Action)) {
		String_Action = M_ActionStructure[int_Action].Action_Name;
		
	}
	UE_LOG(LogTemp, Warning, TEXT("%d  -> %s"), int_Action, *String_Action);
	
	/*if (String_Action == "Movement") {
		ACube_TheBattleMasterBlockGrid* DummyGrid = GetGrid();
		ACube_TheBattleMasterBlock* DummyBlock = DummyGrid->Grid.FindRef(DummyGrid->GridReference.FindRef(MyCube->GetActorLocation() + M_ActionStructure[int_Action].Action_Position));
		if (DummyGrid->GridReference.Contains(MyCube->GetActorLocation() + M_ActionStructure[int_Action].Action_Position)) {
			//UE_LOG(LogTemp, Warning, TEXT("test"));
			MoveCube(DummyBlock, false);
		}
	//	//Movement(GetActorLocation() + M_Action_Pos[int_Action]); 
	}
	else 
	{
		FVector BlockPosition = M_ActionStructure[int_Action].Action_Position + MyCube->GetActorLocation();
		M_ActionStructure[int_Action].Item->DoAction(true, GetBlockFromPosition(BlockPosition));
	}*/
	

	ACube_TheBattleMasterBlockGrid* DummyGrid = GetGrid();
	ACube_TheBattleMasterBlock* DummyBlock = DummyGrid->Grid.FindRef(DummyGrid->GridReference.FindRef(MyCube->GetActorLocation() + M_MovementList[int_Action]));
	if (DummyGrid->GridReference.Contains(MyCube->GetActorLocation() + M_MovementList[int_Action])) {

		MoveCube(DummyBlock, false);
	}
	if (M_ActionStructure[int_Action].Item) {
		UE_LOG(LogTemp, Warning, TEXT("%d  -> %s"), int_Action, *M_ActionStructure[int_Action].Item->GetName());
		FVector BlockPosition = M_ActionStructure[int_Action].Action_Position + MyCube->GetActorLocation();
		M_ActionStructure[int_Action].Item->DoAction(true, GetBlockFromPosition(BlockPosition));
		
	}
	//UE_LOG(LogTemp, Warning, TEXT("Do Action: %s Position: %s"), *String_Action,*(M_ActionStructure[int_Action].Action_Position).ToString());
	MyCube->bDoAction = true;
	MyCube->bMove = true;
	//bReady = false
}

void ACube_TheBattleMasterPawn::AttackAction(ACube_TheBattleMasterPawn* Pawn, FString Name, ACube_TheBattleMasterBlock* Block, bool bAction)
{
	if (bAction) { 
		//HighlightAttackOptions(MyCube->BlockOwner, false); 
		SetAction("Attack", Block);
	}
	


	//MyCube->Attack(Name, Block);
}

ACube_TheBattleMasterBlockGrid* ACube_TheBattleMasterPawn::GetGrid() 
{
	
	FString StopLoop = GetName();
	StopLoop.RemoveAt(0, GetName().Len() - 1);
	int32 Stoploop = FCString::Atoi(*StopLoop);
	//UE_LOG(LogTemp, Warning, TEXT("PlayerName: %s and number: %d"), *GetName(), Stoploop);
	int32 i = 0;
	//ACube_TheBattleMasterBlockGrid* test = nullptr;
	for (TObjectIterator<ACube_TheBattleMasterBlockGrid> Grid; Grid; ++Grid) {
		//UE_LOG(LogTemp, Warning, TEXT("Grid name: %s"), *Grid->GetName());
		//test = *Grid;
		if (i == Stoploop ) { return *Grid; }
		i++;
	} //return test;
	return nullptr;
}

ACube_TheBattleMasterBlock* ACube_TheBattleMasterPawn::GetBlockFromPosition(FVector Direction) {

	FVector LocalDirection = FVector(Direction.X, Direction.Y, 0);

	if (GetGrid()->GridReference.Contains(LocalDirection))
	{
		return GetGrid()->Grid.FindRef(GetGrid()->GridReference.FindRef(LocalDirection));
	}
	return nullptr;
}


//Generate cube functions
void ACube_TheBattleMasterPawn::OnRep_MyCube()
{
	if (MyCube) {
		MyCube->SetOwningPawn(this);
	}
}
void ACube_TheBattleMasterPawn::SetCube(ACube_TheBattleMasterPawn* Pawn, ACube_TheBattleMasterBlock* Block)
{
	if (GetLocalRole() < ROLE_Authority) { Server_SetCube(Pawn, Block); }
	else {
		FActorSpawnParameters params;

		Pawn->MyCube = GetWorld()->SpawnActor<APlayer_Cube>(PlayerCubeClass, Block->GetActorLocation(), FRotator(0, 0, 0), params);
		Pawn->MyCube->SetOwner(this);
			
		StartingBlock = Block;
		MoveCube(Block, false);
		Movement(MyCube->GetActorLocation());

		OnRep_MyCube();
	}CubeMade();
}

void ACube_TheBattleMasterPawn::Server_SetCube_Implementation(ACube_TheBattleMasterPawn* Pawn, ACube_TheBattleMasterBlock* Block){ SetCube(Pawn, Block); }

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

void ACube_TheBattleMasterPawn::Server_CubeDestroy_Implementation() {
	MyCube->Destroy();
	ACube_TheBattleMasterGameMode* BaseGameMode = Cast<ACube_TheBattleMasterGameMode>(UGameplayStatics::GetGameMode(this));
	BaseGameMode->EndGameCondition();
}


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
		FVector DummyLocation;
		if (Block) { 
			Grid_Location = BlockGrid->GridReference.FindRef(Block->GetActorLocation()); 
			DummyLocation = Block->GetTargetLocation();
		}
		else { 
			Grid_Location = BlockGrid->GridReference.FindRef(MyCube->GetActorLocation()); 
			DummyLocation = MyCube->GetActorLocation();
		}
		
		int32 dummySpeed = ThisCube->Base_Speed;
		for (int32 X =  - dummySpeed; X <=  dummySpeed; X++) {
			for (int32 Y =- dummySpeed; Y <=  dummySpeed; Y++) {
				if (BlockGrid->Grid.Contains(FVector2D(Grid_Location.X + X, Grid_Location.Y + Y))) {
					if ((X == -Y || X==Y  || X==0 || Y==0) && !(X==0 && Y==0)) {
					BlockGrid->Grid.FindRef(FVector2D(Grid_Location.X + X, Grid_Location.Y + Y))->CanMove(Bmove);
					}
				}
			}
		}
	}
}


void ACube_TheBattleMasterPawn::Server_HighlightMoveOptions_Implementation(ACube_TheBattleMasterBlock* Block, bool Bmove) { HighlightMoveOptions(Block, Bmove); }


void ACube_TheBattleMasterPawn::HighlightAttackOptions(ACube_TheBattleMasterBlock* Block, bool bToggle, int distance, bool bAttackImmutables)
{
	if (GetLocalRole() < ROLE_Authority) {
		Server_HighlightAttackOptions(Block, bToggle, distance, bAttackImmutables);
	}
	else{
		
		ACube_TheBattleMasterBlockGrid* BlockGrid = GetGrid();
		FVector2D Grid_Location;

		if (Block) { Grid_Location = BlockGrid->GridReference.FindRef(Block->GetActorLocation()); }
		else { Grid_Location = BlockGrid->GridReference.FindRef(MyCube->GetActorLocation()); }

		int32 Radius = distance;
		for (int32 X = 0; X <= 8*distance; X++) {
			float Angle = 2 * 3.14159 * X / (8 * distance);
			Highlight_Block(FMath::RoundHalfFromZero(Grid_Location.X + Radius * FMath::Sin(Angle)), FMath::RoundHalfFromZero(Grid_Location.Y + Radius * FMath::Cos(Angle)), bToggle, bAttackImmutables);
		}
	}
}
void ACube_TheBattleMasterPawn::Highlight_Block(int32 dummyX, int32 dummyY, bool bToggle, bool bAttackImmutables)
{
	
	while (dummyX < 0) { ++dummyX; }
	while (dummyX >= GetGrid()->Size) { --dummyX; }
	while (dummyY < 0) { ++dummyY; }
	while (dummyY >= GetGrid()->Size) { --dummyY; }

	if (GetGrid()->Grid.Contains(FVector2D(dummyX, dummyY))) {
		GetGrid()->Grid.FindRef(FVector2D(dummyX, dummyY))->CanAttack(bToggle, bAttackImmutables);
	}
}

void ACube_TheBattleMasterPawn::Server_HighlightAttackOptions_Implementation(ACube_TheBattleMasterBlock* Block, bool bToggle, int distance, bool bAttackImmutables)
{
	HighlightAttackOptions(Block, bToggle, distance, bAttackImmutables);
}

void ACube_TheBattleMasterPawn::Highlight_Path(ACube_TheBattleMasterBlock* Start, ACube_TheBattleMasterBlock* End)
{
	ACube_TheBattleMasterBlockGrid* BlockGrid = GetGrid();

	//UE_LOG(LogTemp, Warning, TEXT("test %s, %s"), *Start->GetActorLocation().ToString(), *End->GetActorLocation().ToString());


	FVector2D Start2d = BlockGrid->GridReference.FindRef(Start->GetActorLocation());
	FVector2D End2d = BlockGrid->GridReference.FindRef(End->GetActorLocation());
	//UE_LOG(LogTemp, Warning, TEXT("Start: %f, %f"), Start2d.X, Start2d.Y);
	//UE_LOG(LogTemp, Warning, TEXT("End: %f, %f"), End2d.X, End2d.Y);

	if (End2d.X - Start2d.X == End2d.Y - Start2d.Y) {
		if (End2d.X > Start2d.X) {
			for (auto X = 0; X <= End2d.X - Start2d.X; X++) {
				//UE_LOG(LogTemp, Warning, TEXT("loop: %f"), X);
				Highlight_PathBlock(Start2d.X + X, Start2d.Y + X);
			}
		}
		else if (End2d.X < Start2d.X) {
			for (auto X = 0; X <= Start2d.X - End2d.X; X++) {
				Highlight_PathBlock(Start2d.X - X, Start2d.Y - X);
			}
		}
	}
	else if (End2d.X - Start2d.X == -End2d.Y + Start2d.Y) {
		if (End2d.Y < Start2d.Y) {
			for (auto X =0; X <= End2d.X - Start2d.X; X++) {
				Highlight_PathBlock(Start2d.X+ X, Start2d.Y - X);
			}
		}
		else if (End2d.Y > Start2d.Y) {
			for (auto X = 0; X <= Start2d.X - End2d.X; X++) {
				Highlight_PathBlock(Start2d.X - X, Start2d.Y + X);
			}
		}
	}
	else {
		if (End2d.X > Start2d.X) {
			for (auto X = Start2d.X; X <= End2d.X; X++) {
				Highlight_PathBlock(X, Start2d.Y);
			}
		}
		else if (End2d.X < Start2d.X) {
			for (auto X = End2d.X; X <= Start2d.X; X++) {
				Highlight_PathBlock(X, Start2d.Y);
			}
		}
		else if (End2d.Y > Start2d.Y) {
			for (auto X = Start2d.Y; X <= End2d.Y; X++) {
				Highlight_PathBlock(Start2d.X, X);
			}
		}
		else if (End2d.Y < Start2d.Y) {
			for (auto X = End2d.Y; X <= Start2d.Y; X++) {
				Highlight_PathBlock(Start2d.X, X);
			}
		}
	}
}

void ACube_TheBattleMasterPawn::Highlight_PathBlock(int32 X, int32 Y)
{
	ACube_TheBattleMasterBlockGrid* BlockGrid = GetGrid();
	if (BlockGrid->Grid.Contains(FVector2D(X, Y))) {
		ACube_TheBattleMasterBlock* Block = BlockGrid->Grid.FindRef(FVector2D(X, Y));
		Block->HighlightPath(true);

		Path.Add(Block);
	}
}

void ACube_TheBattleMasterPawn::TriggerClick()
{
	if (CurrentBlockFocus && !bDead)
	{			
		CurrentBlockFocus->HandleClicked();
		if (MyCube != nullptr) {
			if (!bReady) {
				if (CurrentBlockFocus->bMove) { MoveCube(CurrentBlockFocus, true); this->Reset_Buttons_test();}
				else if (CurrentBlockFocus->bAttack) { AttackAction(this, WeaponName, CurrentBlockFocus, true); this->Reset_Buttons_test(); }
			}
		}
		else { SetCube(this, CurrentBlockFocus); 
		}
	}//ActionSelected();
}





void ACube_TheBattleMasterPawn::Turn()
{
	for (auto Elems : M_ActionStructure) {
		Elems.Value.MovementDuringAction = M_MovementList[Elems.Key];
	}

	if (GetLocalRole() < ROLE_Authority) { Server_Turn(); }
	else {
		ACube_TheBattleMasterGameMode* BaseGameMode = Cast<ACube_TheBattleMasterGameMode>(UGameplayStatics::GetGameMode(this));
		BaseGameMode->TakeTurn();
	}	
}

void ACube_TheBattleMasterPawn::Server_Turn_Implementation() { Turn();}








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
	DOREPLIFETIME(ACube_TheBattleMasterPawn, CubeSelected);

	DOREPLIFETIME(ACube_TheBattleMasterPawn, bReady);
	
}