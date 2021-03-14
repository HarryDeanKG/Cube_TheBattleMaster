// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.


#include "Cube_TheBattleMasterPawn.h"
#include "Cube_TheBattleMasterBlock.h"
#include "Cube_TheBattleMasterBlockGrid.h"
#include "Cube_TheBattleMasterGameMode.h"
#include "Player_Cube.h"
#include "Cube_TheBattleMasterPlayerController.h"

#include "Kismet/GameplayStatics.h"
#include "Math/Rotator.h"
#include "Kismet/KismetMathLibrary.h"
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

	struct FConstructorStatics
	{
		ConstructorHelpers::FObjectFinderOptional<UStaticMesh> PlaneMesh;
		ConstructorHelpers::FObjectFinderOptional<UStaticMesh> RingMesh;
		ConstructorHelpers::FObjectFinderOptional<UStaticMesh> BasicPlaneMesh;
		ConstructorHelpers::FObjectFinderOptional<UMaterialInstance> BaseMaterial;
		ConstructorHelpers::FObjectFinderOptional<UMaterialInstance> PathMakerMaterial;
		ConstructorHelpers::FObjectFinderOptional<UMaterialInstance> RadiusMaterial;
		FConstructorStatics()
			: PlaneMesh(TEXT("/Game/Puzzle/Meshes/Cylinder_pivotBottom.Cylinder_pivotBottom"))
			//, RingMesh(TEXT("/Game/Puzzle/Meshes/SM_PathMarker.SM_PathMarker"))
			, RingMesh(TEXT("/Game/Puzzle/Meshes/Cylinder_pivotBottom.Cylinder_pivotBottom"))
			, BasicPlaneMesh(TEXT("/Game/Puzzle/Meshes/BasicPlaneMesh.BasicPlaneMesh"))
			, BaseMaterial(TEXT("/Game/Puzzle/Meshes/RedMaterial.RedMaterial"))
			, PathMakerMaterial(TEXT("/Game/Puzzle/Meshes/OrangeMaterial_NoLighting.OrangeMaterial_NoLighting"))
			, RadiusMaterial(TEXT("/Game/Puzzle/Meshes/RadiusMaterial_Inst.RadiusMaterial_Inst"))
		{
		}
	};
	static FConstructorStatics ConstructorStatics;

	//Set up a dummy root system
	DummyRoot = CreateDefaultSubobject<USceneComponent>(TEXT("Dummy0"));
	RootComponent = DummyRoot;

	ArrowMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ArrowMesh0"));
	//BlockMesh->SetIsReplicated(true);
	ArrowMesh->SetupAttachment(DummyRoot);
	ArrowMesh->SetStaticMesh(ConstructorStatics.PlaneMesh.Get());
	ArrowMesh->SetWorldScale3D(FVector(0.5f, 0.5f, 0.5f));
	//ArrowMesh->SetRelativeLocation(FVector(0.f, 0.f, 25.f));
	ArrowMesh->SetMaterial(0, ConstructorStatics.BaseMaterial.Get());
	ArrowMesh->SetVisibility(false);
	
	RadiusMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RadiusMesh0"));
	//BlockMesh->SetIsReplicated(true);
	RadiusMesh->SetupAttachment(DummyRoot);
	RadiusMesh->SetStaticMesh(ConstructorStatics.BasicPlaneMesh.Get());
	RadiusMesh->SetWorldScale3D(FVector(0.5f, 0.5f, 0.5f));
	RadiusMesh->SetMaterial(0, ConstructorStatics.RadiusMaterial.Get());
	RadiusMesh->SetVisibility(false);

	
	Ring = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("RingMesh0"));
	//Ring->RegisterComponent();
	Ring->SetupAttachment(DummyRoot);
	Ring->SetStaticMesh(ConstructorStatics.RingMesh.Get());
	Ring->SetMaterial(0, ConstructorStatics.PathMakerMaterial.Get());
	Ring->SetCastShadow(false);
	Ring->SetCollisionProfileName(FName("NoCollision"));
	//Ring->SetVisibility(false);




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
	if (MyCube) { HighlightAttackOptions(MyCube->BlockOwner, bToggle, 8, 8, true); }
	WeaponName = Name;
}


void ACube_TheBattleMasterPawn::StopMovement() {
	ACube_TheBattleMasterBlockGrid* BlockGrid = GetGrid();
	ACube_TheBattleMasterBlock* Block;

	if (!MyCube->BlockOwner) {
		Block = BlockGrid->Grid.FindRef(BlockGrid->GridReference.FindRef(MyCube->GetActorLocation()));
	}
	else { Block = MyCube->BlockOwner; }

	if (Block) {
		MoveCube(Block, true);
	}
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

	if (M_MovementList.Contains(ActionNumb)) {
		NothingAction.Action_Position = M_MovementList.FindRef(ActionNumb);
		NothingAction.MovementDuringAction = M_MovementList.FindRef(ActionNumb);
	}
	else {
		NothingAction.MovementDuringAction = FVector(0.f);
		
		M_MovementList.Add(ActionNumb, FVector(0.f));
		MoveNumb++;
		RefreshMovement();
	}

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
	UE_LOG(LogTemp, Warning, TEXT("READY!"));

	ResetEverything(false);
	bReady = true;

	Turn();

}
void ACube_TheBattleMasterPawn::Cancel_Button_Implementation() {
	ResetEverything(true);

	ClearVars();
	UE_LOG(LogTemp, Warning, TEXT("CANCEL!"));
	UE_LOG(LogTemp, Warning, TEXT("%d"), GetLocalRole());

	//bReady = false;
}

void ACube_TheBattleMasterPawn::Confirmation_Button_Implementation() {
	
	SetAction(SelectedItem->ActionName, MyCube->BlockOwner->GetActorLocation());
}

void ACube_TheBattleMasterPawn::ClearVars() {
	UE_LOG(LogTemp, Warning, TEXT("ClearVars!"));

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
		UE_LOG(LogTemp, Warning, TEXT("%d"), MoveNumb)
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



void ACube_TheBattleMasterPawn::ResetEverything(bool bResetPosition) {
	
	UE_LOG(LogTemp, Warning, TEXT("Path length %d"), Path.Num());

	for (auto& Block : Path) {
		UE_LOG(LogTemp, Warning, TEXT("Cube position: %s"), *Block->GetActorLocation().ToString());
	}
	if (Path.Num() > 0) {
		UE_LOG(LogTemp, Warning, TEXT("MoveBack %s"), *(Path[0]->GetActorLocation()-GetActorLocation()).ToString());
		Movement(Path[0]->GetActorLocation());
		MyCube->SetActorLocation(Path[0]->GetActorLocation());
		MyCube->BlockOwner = GetBlockFromPosition(MyCube->GetActorLocation());
	}


	for (auto& Elems : M_ActionStructure) { if (Elems.Value.Item) { Elems.Value.Item->ResetAction(); } }
	FlagedForDeleation.Empty();
	/*for (auto& Elems : FlagedForDeleation) { if (Elems) { Elems->Destroy(); } }
	FlagedForDeleation.Empty();*/

	for (auto& Block : ToggleBackList) { Block->ToggleOccupied(false); }
	ToggleBackList.Empty();

	//UE_LOG(LogTemp, Warning, TEXT("Cube position: %s"), *MyCube->GetActorLocation().ToString());
	//Need to select the correct grid.
	for (auto& Block : Path) {
		ACube_TheBattleMasterBlockGrid* BlockGrid = GetGrid();
		const FVector2D* DummyFvector = BlockGrid->Grid.FindKey(Block);
		BlockGrid->Grid.FindRef(FVector2D(DummyFvector->X, DummyFvector->Y))->HighlightPath(false);
		//UE_LOG(LogTemp, Warning, TEXT("Cube position: %s"), *Block->GetActorLocation().ToString());
		//UE_LOG(LogTemp, Warning, TEXT("Block Grid Name: %s"), *BlockGrid->GetName());
		

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

void ACube_TheBattleMasterPawn::BeginPlay()
{
	Super::BeginPlay();

	DynamicBaseMaterial = RadiusMesh->CreateDynamicMaterialInstance(0, BaseMaterial);

	//Obj = GetWorld()->SpawnActor<AActor>(FVector(0.f), FRotator(0.f));
	/*int i = 0;
	int32 Stoploop = 1;
	for (TObjectIterator<ACube_TheBattleMasterBlockGrid> Grid; Grid; ++Grid) 
	{ 
		UE_LOG(LogTemp, Warning, TEXT("TESTESETESTRer %s"), *Grid->GetName());
		FString StopLoop = Grid->GetName();
		StopLoop.RemoveAt(0, Grid->GetName().Len() - 1);
		Stoploop = FCString::Atoi(*StopLoop);
		i++; 
	}*/
		
	

	if (IsLocallyControlled()) {
		MyGrid = GetWorld()->SpawnActor<ACube_TheBattleMasterBlockGrid>(FVector(100.f, 200.f, 0.f), FRotator(0.f, 0.f, 180.f));
	}
}

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

/*void ACube_TheBattleMasterPawn::BeginPlay()
{
	if (GetOwner()) {
		APlayerController* OurPlayerController = Cast<APlayerController>(GetOwner());

		OurPlayerController->SetViewTarget(this);
	}
}*/

void ACube_TheBattleMasterPawn::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);


	//UpdateActions();
	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		FVector Start, Dir, End;
		PC->DeprojectMousePositionToWorld(Start, Dir);
		End = Start + (Dir * 8000.0f);

		TraceForBlock(Start, End, false, !bArrow );

		//If we want an arrow lets tick that
		if (bArrow)	{ TraceForArrow(Start, Dir, false);	}
		else if (ArrowMesh->IsVisible()) { ArrowMesh->SetVisibility(false); RadiusMesh->SetVisibility(false); }
		//If we want some sort of circle to represent attack range ... do that
		if (bCrossHair)	{ TraceForCrosshair(Start, End, true); }
		//else { TraceForCrosshair(Start, End, false); }
		
		//UE_LOG(LogTemp, Warning, TEXT("barrow %s, bcrosshair %s"), bArrow ? TEXT("True") : TEXT("False"), bCrossHair ? TEXT("True") : TEXT("False"));
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
	if (PreviousBlock) { HighlightMoveOptions(PreviousBlock, false); MyCube->Replicated_Speed = MyCube->Base_Speed; }

	Block->CanMove(false);

	Movement(Block->GetActorLocation());
	

	if (bAction) {
		MyCube->SetActorLocation(Block->GetActorLocation());
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
		UE_LOG(LogTemp, Warning, TEXT("Move number %d, FVector %s"), MoveNumb, *(Block->GetActorLocation() - PreviousBlock->GetActorLocation()).ToString());

		MoveNumb++;
		
		if (SelectedItem)
		{
			FAction_Struct DummyAction;
			DummyAction.Action_Name = SelectedItem->ActionName;
			DummyAction.MovementDuringAction = M_MovementList.FindRef(ActionNumb);
			DummyAction.Action_Position = M_MovementList.FindRef(ActionNumb);
			DummyAction.Item = SelectedItem;

			M_ActionStructure.Add(ActionNumb, DummyAction);
			ActionNumb++;
		}

		RefreshMovement();
		ActionSelected();
	}
	else { MyCube->StartPosition = MyCube->GetActorLocation(); }
	//Change the speed back to the base speed ... may have errors latter but good for now.
	MyCube->Replicated_Speed = MyCube->Base_Speed;
	SelectedItem = nullptr;
	UpdateActions();
	//UE_LOG(LogTemp, Warning, TEXT("Is ready? %s"), MyCube->bMove ? TEXT("True") : TEXT("False"));
}


void ACube_TheBattleMasterPawn::CalcCamera(float DeltaTime, struct FMinimalViewInfo& OutResult)
{
	Super::CalcCamera(DeltaTime, OutResult);

	OutResult.Rotation = FRotator(-90.0f, 0.0f, 0.0f);
}


void ACube_TheBattleMasterPawn::SetAction(FString ActionName, FVector Direction) {
	if (!bReady && ActionNumb < 4) {
		FAction_Struct Actions;
		FVector DummyPosition;
		//UE_LOG(LogTemp, Warning, TEXT("Attack direction: %s"), *AttackDirection.ToString());
		//UE_LOG(LogTemp, Warning, TEXT("Direction: %s"), *Direction.ToString());

		if (Direction!=FVector(0.f)) { DummyPosition = Direction - MyCube->GetActorLocation(); }
		else { DummyPosition =  MyCube->GetActorLocation(); }
		//UE_LOG(LogTemp, Warning, TEXT("Direction: %s"), *DummyPosition.ToString());

		if (SelectedItem) {
			Actions.Action_Name = SelectedItem->ActionName;
			Actions.Action_Position = DummyPosition;
			Actions.Action_Weapon = SelectedItem->WeaponName;
			Actions.Item = SelectedItem;
			if (CurrentMeshFocus) { Actions.SelectedActor = CurrentActorFocus; UE_LOG(LogTemp, Warning, TEXT("selected actor: %s"), *CurrentActorFocus->GetName()); }

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
			
			SelectedItem->DoAction(false, Direction);
			SelectedItem->UnSetActionInMotion();
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
		
		//Unhighlight an actor if highlighted
		if (CurrentMeshFocus) {
			CurrentMeshFocus->SetRenderCustomDepth(false);
			CurrentMeshFocus = nullptr;
		}
	}
}

void ACube_TheBattleMasterPawn::DoAction(int int_Action)
{
	MyCube->SetReplicatingMovement(true);
	DoActionNumb = int_Action;
	String_Action = "Empty";
	/*MyCube->bReady = false;
	MyCube->time = 0.f;*/
	MyCube->InitiateMovementAndAction();
	//UE_LOG(LogTemp, Warning, TEXT("%d"), M_ActionStructure.Num())
	if (M_ActionStructure.Contains(int_Action)) {
		String_Action = M_ActionStructure[int_Action].Action_Name;
		
	}
	//UE_LOG(LogTemp, Warning, TEXT("%d  -> %s"), int_Action, *String_Action);
	
	ACube_TheBattleMasterBlockGrid* DummyGrid = GetGrid();
	ACube_TheBattleMasterBlock* DummyBlock = DummyGrid->Grid.FindRef(DummyGrid->GridReference.FindRef(MyCube->GetActorLocation() + M_MovementList[int_Action]));
	if (DummyGrid->GridReference.Contains(MyCube->GetActorLocation() + M_MovementList[int_Action])) {
		MoveCube(DummyBlock, false);
		MyCube->bMove = true;
	}
	TArray<AActor*> AttachedComponents;
	MyCube->GetAttachedActors(AttachedComponents);
	
	if (M_ActionStructure[int_Action].Item && AttachedComponents.Contains(M_ActionStructure[int_Action].Item)) {
		UE_LOG(LogTemp, Warning, TEXT("%d  -> %s"), int_Action, *M_ActionStructure[int_Action].Item->GetName());

		MyCube->bDoAction = true;
	}
	
	//UE_LOG(LogTemp, Warning, TEXT("Do Action: %s Position: %s"), *String_Action,*(M_ActionStructure[int_Action].Action_Position).ToString());
	
	
	//bReady = false
}

void ACube_TheBattleMasterPawn::AttackAction(ACube_TheBattleMasterPawn* Pawn, FString Name, ACube_TheBattleMasterBlock* Block, bool bAction)
{
	if (bAction) { 
		//HighlightAttackOptions(MyCube->BlockOwner, false); 
		SetAction("Attack", Block->GetActorLocation());
	}
	


	//MyCube->Attack(Name, Block);
}

ACube_TheBattleMasterBlockGrid* ACube_TheBattleMasterPawn::GetGrid() 
{
	//UE_LOG(LogTemp, Warning, TEXT("MY Grid name: %s "), *MyGrid->GetName());
	for (TObjectIterator<ACube_TheBattleMasterBlockGrid> Grid; Grid; ++Grid) {
	/*	if (GetLocalRole() < ROLE_Authority){
			if (MyGrid->GetName() != Grid->GetName()) { Grid->Destroy(); }
		}else { if (MyGrid->GetName() == Grid->GetName()) { Grid->Destroy(); } }*/
		if (!MyGrid) { MyGrid = *Grid; }
	}
	return MyGrid;

	FString StopLoop = GetName();
	StopLoop.RemoveAt(0, GetName().Len() - 1);
	int32 Stoploop = FCString::Atoi(*StopLoop);
	UE_LOG(LogTemp, Warning, TEXT("PlayerName: %s and number: %d"), *GetName(), Stoploop);
	UE_LOG(LogTemp, Warning, TEXT("PlayerOwnerName: %s and number: %d"), *GetOwner()->GetName(), Stoploop);
	int32 i = 0;
	//ACube_TheBattleMasterBlockGrid* test = nullptr;
	for (TObjectIterator<ACube_TheBattleMasterBlockGrid> Grid; Grid; ++Grid) {
		UE_LOG(LogTemp, Warning, TEXT("Grid name: %s at number %d"), *Grid->GetName(), i);
		//test = *Grid;
		if (i == 0 ) { return *Grid; }
		
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
		UpdateActions();
	}CubeMade(); 
	//if (MyCube)
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

	FVector2D Grid_Location;
	FVector DummyLocation;
	if (Block) { 
		Grid_Location = GetGrid()->GridReference.FindRef(Block->GetActorLocation());
		DummyLocation = Block->GetTargetLocation();
	}
	else { 
		Grid_Location = GetGrid()->GridReference.FindRef(MyCube->GetActorLocation());
		DummyLocation = MyCube->GetActorLocation();
	}
		
	int32 dummySpeed = MyCube->Replicated_Speed;
	for (int32 X =  - dummySpeed; X <=  dummySpeed; X++) {
		for (int32 Y =- dummySpeed; Y <=  dummySpeed; Y++) {
			if (GetGrid()->Grid.Contains(FVector2D(Grid_Location.X + X, Grid_Location.Y + Y))) {
				if ((X == -Y || X==Y  || X==0 || Y==0) && !(X==0 && Y==0)) {
					GetGrid()->Grid.FindRef(FVector2D(Grid_Location.X + X, Grid_Location.Y + Y))->CanMove(Bmove);
				}
			}
		}
	}
}

void ACube_TheBattleMasterPawn::HighlightAttackOptions(ACube_TheBattleMasterBlock* Block, bool bToggle, int minDistance, int maxDistance, bool bAttackImmutables)
{
	ACube_TheBattleMasterBlockGrid* BlockGrid = GetGrid();
	FVector2D Grid_Location;

	if (Block) { Grid_Location = BlockGrid->GridReference.FindRef(Block->GetActorLocation()); }
	else { Grid_Location = BlockGrid->GridReference.FindRef(MyCube->GetActorLocation()); }

	for (int32 distance = minDistance; distance <= maxDistance; distance++) {
		for (int32 X = 0; X <= 8 * distance; X++) {
			float Angle = 2 * 3.14159 * X / (8 * distance);
			Highlight_Block(FMath::RoundHalfFromZero(Grid_Location.X + distance * FMath::Sin(Angle)), FMath::RoundHalfFromZero(Grid_Location.Y + distance * FMath::Cos(Angle)), bToggle, bAttackImmutables);
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

void ACube_TheBattleMasterPawn::Highlight_Path(ACube_TheBattleMasterBlock* Start, ACube_TheBattleMasterBlock* End)
{
		ACube_TheBattleMasterBlockGrid* BlockGrid = GetGrid();

		FVector2D Start2d = BlockGrid->GridReference.FindRef(Start->GetActorLocation());
		FVector2D End2d = BlockGrid->GridReference.FindRef(End->GetActorLocation());

		if (End2d.X - Start2d.X == End2d.Y - Start2d.Y) {
			if (End2d.X > Start2d.X) {
				for (auto X = 0; X <= End2d.X - Start2d.X; X++) {
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
				for (auto X = 0; X <= End2d.X - Start2d.X; X++) {
					Highlight_PathBlock(Start2d.X + X, Start2d.Y - X);
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
				for (auto X = Start2d.X; X >= End2d.X; X--) {
					Highlight_PathBlock(X, Start2d.Y);
				}
			}
			else if (End2d.Y > Start2d.Y) {
				for (auto X = Start2d.Y; X <= End2d.Y; X++) {
					Highlight_PathBlock(Start2d.X, X);
				}
			}
			else if (End2d.Y < Start2d.Y) {
				for (auto X = Start2d.Y; X >= End2d.Y; X--) {
					Highlight_PathBlock(Start2d.X, X);
				}
			}
		}
	
		
}
  
void ACube_TheBattleMasterPawn::Highlight_PathBlock(int32 X, int32 Y)
{
	ACube_TheBattleMasterBlock* Block = nullptr;
	ACube_TheBattleMasterBlockGrid* BlockGrid = GetGrid();
	if (BlockGrid->Grid.Contains(FVector2D(X, Y))) {
		Block = BlockGrid->Grid.FindRef(FVector2D(X, Y));

	}
	Block = BlockGrid->Grid.FindRef(FVector2D(X, Y));
	Path.Add(Block);

	Block->HighlightPath(true);

}

void ACube_TheBattleMasterPawn::TriggerClick()
{
	if (bCrossHair){}
	if (bArrow) { SelectedItem->UnSetActionInMotion();  SetAction("Attack", AttackDirection); this->Reset_Buttons_test(); }
	else if (CurrentBlockFocus && !bDead)
	{			
		CurrentBlockFocus->HandleClicked();
		if (MyCube != nullptr) {
			if (!bReady) {
				if (CurrentBlockFocus->bMove) { MoveCube(CurrentBlockFocus, true); this->Reset_Buttons_test();}
				else if (CurrentBlockFocus->bAttack) { SetAction("Attack", CurrentBlockFocus->GetActorLocation());  this->Reset_Buttons_test(); }
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

void ACube_TheBattleMasterPawn::TraceForBlock(const FVector& Start, const FVector& End, bool bDrawDebugHelpers, bool bHighlight)
{
	FHitResult HitResult;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);

	GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, Params);
	if (bDrawDebugHelpers)
	{
		DrawDebugLine(GetWorld(), Start, HitResult.Location, FColor::Red);
		DrawDebugSolidBox(GetWorld(), HitResult.Location, FVector(20.0f), FColor::Red);
	}
	
	if (HitResult.Actor.IsValid())
	{
		//UE_LOG(LogTemp, Warning, TEXT("Hit actor: %s, start: %s, end: %s"), *HitResult.Actor->GetName(), *Start.ToString(), *End.ToString());
		//HitResult.Component->SetRenderCustomDepth(true);
		ACube_TheBattleMasterBlock* HitBlock = Cast<ACube_TheBattleMasterBlock>(HitResult.Actor.Get());
		if (CurrentBlockFocus != HitBlock)
		{
			if (bHighlight) {
				if (CurrentBlockFocus)
				{
					CurrentBlockFocus->Highlight(false);
				}
				if (HitBlock)
				{
					HitBlock->Highlight(true);
				}
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

/*Add the same highligh feature for other actors, namly their mesh 
- only works if the Post process volumne is used and settings are changed in project settings*/
void ACube_TheBattleMasterPawn::HighlightAnActorNotABlock(const FVector& Start, const FVector& End){
	FHitResult HitResult;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);
	
	GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, Params);

	//Don't hit the block!
	if (HitResult.Actor.Get() != CurrentBlockFocus) {
		if (HitResult.Component != CurrentMeshFocus && HitResult.Actor.Get() != MyCube)
		{
			if (CurrentMeshFocus)
			{
				CurrentMeshFocus->SetRenderCustomDepth(false);
			}
			if (HitResult.Component.Get())
			{
				HitResult.Component->SetRenderCustomDepth(true);
			}
			CurrentMeshFocus = Cast<UStaticMeshComponent>(HitResult.Component.Get());
			CurrentActorFocus = HitResult.Actor.Get();
		}
	}
	else {
		if (CurrentMeshFocus) {
			CurrentMeshFocus->SetRenderCustomDepth(false);
			CurrentMeshFocus = nullptr;
		}
	}
	
	//TODO on click unhighlight CurrentMeshFocus.
}

void ACube_TheBattleMasterPawn::TraceForArrow(FVector Start, const FVector& Direction, bool bDrawDebugHelpers)
{
	FHitResult HitResult;
	FCollisionQueryParams Params;

	TArray<AActor*> ActorList;
	ActorList.Add(MyCube);
	ActorList.Add(this);
	MyCube->GetAttachedActors(ActorList, false);
	Params.AddIgnoredActors(ActorList);

	FVector End = Start + (Direction * (100 - Start.Z) / Direction.Z);
	
	GetWorld()->LineTraceSingleByChannel(HitResult, SelectedItem->GetActorLocation(), End, ECC_Visibility, Params);
	if (bDrawDebugHelpers)
	{
		DrawDebugLine(GetWorld(), SelectedItem->GetActorLocation(), End , FColor::Blue);
		DrawDebugSolidBox(GetWorld(), HitResult.Location, FVector(20.0f), FColor::Red);
	}
	
	float Dis;
	float angle = (End - SelectedItem->GetActorLocation()).Rotation().Yaw;

	//UE_LOG(LogTemp, Warning, TEXT("angle: %f, in: %f, out: %f"), angle, SelectedItem->GetActorRotation().Pitch * 2 + SelectedItem->GetActorRotation().Yaw - 30.f, SelectedItem->GetActorRotation().Pitch * 2 + SelectedItem->GetActorRotation().Yaw + 30.f);

	//This may need to be changed, I think the +-180 kills this
	if (FMath::IsWithinInclusive(angle, SelectedItem->GetActorRotation().Pitch * 2 + SelectedItem->GetActorRotation().Yaw - 30.f, SelectedItem->GetActorRotation().Pitch * 2 + SelectedItem->GetActorRotation().Yaw + 30.f)) {
		
		HighlightAnActorNotABlock(Start, Start + (Direction * 8000.0f));
		if (HitResult.Actor.IsValid())
		{
			UE_LOG(LogTemp, Warning, TEXT("%s"), *HitResult.Actor->GetName());
			Dis = FVector::Dist2D(HitResult.Actor->GetActorLocation(), SelectedItem->GetActorLocation()) / 200;
			
			//HitResult.Component->SetRenderCustomDepth(true);
		}
		else {
			Dis = FVector::Dist2D(End, SelectedItem->GetActorLocation()) / 200;
		}
	}
	else{
		Dis = rangeEnd;
	}

	Dis = FMath::Clamp(Dis, 0.f, rangeEnd);
	angle = UKismetMathLibrary::ClampAngle(angle, SelectedItem->GetActorRotation().Pitch*2 + SelectedItem->GetActorRotation().Yaw - 30.f,  SelectedItem->GetActorRotation().Pitch*2 + SelectedItem->GetActorRotation().Yaw + 30.f);
	FRotator Dir = FRotator(-90.f, angle, 0.f);
	
	AttackDirection = UKismetMathLibrary::CreateVectorFromYawPitch(angle, 0.f, Dis)+ SelectedItem->GetActorLocation();
	//UE_LOG(LogTemp, Warning, TEXT("direction: %s, Actor: %s"), *AttackDirection.ToString(), *SelectedItem->GetActorRotation().ToString());


	ArrowMesh->SetRelativeScale3D(FVector(0.05f, 0.05f, Dis));
	ArrowMesh->SetWorldLocation(SelectedItem->GetActorLocation());
	ArrowMesh->SetRelativeRotation(Dir);
	ArrowMesh->SetVisibility(true);

	RadiusMesh->SetRelativeScale3D(FVector(10*rangeEnd, 10*rangeEnd, 1.0f));
	RadiusMesh->SetWorldLocation(SelectedItem->GetActorLocation());
	//RadiusMesh->SetWorldRotation(FRotator(0.f, -90.f, 0.f));

	RadiusMesh->SetVisibility(true);
	DynamicBaseMaterial->SetScalarParameterValue("Alpha", 60.f/360.f);
	DynamicBaseMaterial->SetScalarParameterValue("CutOff", 0.9f);
	DynamicBaseMaterial->SetScalarParameterValue("InnerRadius", 0.f);
	DynamicBaseMaterial->SetScalarParameterValue("OuterRadius", 0.8f);
	DynamicBaseMaterial->SetScalarParameterValue("StartingAngle", (-SelectedItem->GetActorRotation().Yaw+ SelectedItem->GetActorRotation().Pitch*2+30) / 360.f);
}

void ACube_TheBattleMasterPawn::TraceForCrosshair(const FVector& Start, const FVector& End, bool bToggleOn)
{
	FHitResult HitResult;

	TArray<AActor*> IgnorList;
	IgnorList.Add(MyCube);
	IgnorList.Add(SelectedItem);
	

	FVector StartPosition = SelectedItem->GetActorLocation() + SelectedItem->GetActorForwardVector() * 15.f;
	StartPosition.Z = 0.f;
	FVector EndPosition;
	if (CurrentBlockFocus) {
		EndPosition = CurrentBlockFocus->GetActorLocation();
		//UE_LOG(LogTemp, Warning, TEXT("bAttack? %s"), CurrentBlockFocus->bAttack ? TEXT("true") : TEXT("false"))

	}
	else { EndPosition = SelectedItem->GetActorLocation() + SelectedItem->GetActorForwardVector() * 1500; }

	//Attack range is the max range and each square is 60 units wide
	//V=sqrt(displacement times acceleration divided by sin(2theta))
	//gravity is -980
	float MaxVelocity = sqrt(980.f*((SelectedItem->AttackRange)*60.f - 30.f));

	float distance = sqrt((EndPosition.X - StartPosition.X)*(EndPosition.X - StartPosition.X) + (EndPosition.Y - StartPosition.Y)*(EndPosition.Y - StartPosition.Y));

	float newDistance;
	float pitchNew;
	for (float i = 39.0f; i < 95.1f; i++) {
		pitchNew = PI / 180.f* (i);
		newDistance = MaxVelocity * FMath::Cos(pitchNew) *(MaxVelocity*FMath::Sin(pitchNew) + sqrt(MaxVelocity*MaxVelocity*FMath::Sin(pitchNew)*FMath::Sin(pitchNew) + 2 * 980.f * SelectedItem->GetActorLocation().Z)) / 980.f;
		pitchNew = pitchNew * 180.f / PI;
		if (FMath::IsWithin(newDistance, distance - 10, distance + 10)) { break; }
		//UE_LOG(LogTemp, Warning, TEXT("End Pitch: %f, old distance %f, new distance %f"), alpha, distance, newDistance);
	}

	SelectedItem->SetActorRotation(FRotator(
		pitchNew,
		(EndPosition - StartPosition).Rotation().Yaw,
		SelectedItem->GetActorRotation().Roll));


	FPredictProjectilePathParams PredictParams;
	PredictParams.StartLocation = FVector(SelectedItem->GetActorLocation() + SelectedItem->GetActorForwardVector() * 15);
	//PredictParams.StartLocation.Z += 100;
	PredictParams.LaunchVelocity = FVector(SelectedItem->GetActorForwardVector() * MaxVelocity);
	PredictParams.bTraceWithChannel = true;
	PredictParams.bTraceWithCollision = true;
	PredictParams.ProjectileRadius = 5.1f;
	PredictParams.TraceChannel = ECC_Visibility;
	PredictParams.bTraceComplex = false;
	PredictParams.ActorsToIgnore = IgnorList;
	PredictParams.SimFrequency = 75.f;
	PredictParams.MaxSimTime = 2.f;
	//PredictParams.DrawDebugType = EDrawDebugTrace::ForOneFrame;
	
	
	FPredictProjectilePathResult PredictResult;

	if (CurrentBlockFocus) {
		if (CurrentBlockFocus->bAttack) {
			UGameplayStatics::PredictProjectilePath(GetWorld(), PredictParams, PredictResult);
		}
	}

	FTransform InstTransfrom;
	for (int i = 0; i < PredictResult.PathData.Num() - 2; i++) {
		FVector Difference = PredictResult.PathData[i + 1].Location - PredictResult.PathData[i].Location;
		FRotator Rotation = UKismetMathLibrary::MakeRotFromX(Difference);
		Rotation.Pitch += 90;
		InstTransfrom = FTransform(Rotation, PredictResult.PathData[i].Location, FVector(0.05f));
		if (i + 1 < Ring->GetInstanceCount()) {
			Ring->UpdateInstanceTransform(i, InstTransfrom, true, true, true);
			UE_LOG(LogTemp, Warning, TEXT("Position %d -> Location: %s"), i, *PredictResult.PathData[i].Location.ToString());
		}
		else {

			Ring->AddInstanceWorldSpace(InstTransfrom);
			//ProjectilePathDisplay = GetWorld()->SpawnActor<UChildActorComponent>(SelectedItem->GetActorLocation(), Rotation);
			//this->AddInstanceComponent(Ring);
			//Ring->RegisterComponent();
			UE_LOG(LogTemp, Warning, TEXT("Add instance")); 

		}
	}for (int i = PredictResult.PathData.Num() - 2; i < Ring->GetInstanceCount(); i++){ Ring->RemoveInstance(i); }
	
	//	UE_LOG(LogTemp, Warning, TEXT("Ring count %d"), Ring->GetInstanceCount());

	//	Ring->GetInstanceTransform(Ring->GetInstanceCount() - 1, InstTransfrom, true);
	//	Ring->UpdateInstanceTransform(Ring->GetInstanceCount() - 1, InstTransfrom, true, true, true);
		//bCrossHair = false;
	//}
	//GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility);
	
	//bCrossHair = false;

}


void ACube_TheBattleMasterPawn::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ACube_TheBattleMasterPawn, MyCube);
	DOREPLIFETIME(ACube_TheBattleMasterPawn, CubeSelected);

	DOREPLIFETIME(ACube_TheBattleMasterPawn, bReady);
	
}