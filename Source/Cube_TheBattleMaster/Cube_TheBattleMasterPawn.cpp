// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.


#include "Cube_TheBattleMasterPawn.h"
#include "Cube_TheBattleMasterBlock.h"
#include "Cube_TheBattleMasterBlockGrid.h"
#include "Cube_TheBattleMasterGameMode.h"
#include "Player_Cube.h"
#include "ItemBase.h"
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
		ConstructorHelpers::FObjectFinderOptional<UMaterialInstance> VariedCubeMaterial;
		ConstructorHelpers::FObjectFinderOptional<UMaterialInstance> BaseMaterial;
		ConstructorHelpers::FObjectFinderOptional<UMaterialInstance> PathMakerMaterial;
		ConstructorHelpers::FObjectFinderOptional<UMaterialInstance> RadiusMaterial;
		FConstructorStatics()
			: PlaneMesh(TEXT("/Game/Puzzle/Meshes/Cylinder_pivotBottom.Cylinder_pivotBottom"))
			//, RingMesh(TEXT("/Game/Puzzle/Meshes/SM_PathMarker.SM_PathMarker"))
			, RingMesh(TEXT("/Game/Puzzle/Meshes/Cylinder_pivotBottom.Cylinder_pivotBottom"))
			, BasicPlaneMesh(TEXT("/Game/Puzzle/Meshes/BasicPlaneMesh.BasicPlaneMesh"))
			, VariedCubeMaterial(TEXT("/Game/Puzzle/Meshes/BaseMaterial_Inst.BaseMaterial_Inst"))
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
	OurCameraSpringArm->SetRelativeLocationAndRotation(FVector(200.0f, 0.0f, 0.0f), FRotator(0.0f, 0.0f, 0.0f));
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
	if (MyCube->BlockOwner) { UE_LOG(LogTemp, Warning, TEXT("BlockOwner %s"), *MyCube->BlockOwner->GetName()); }
	if (MyCube) { HighlightMoveOptions(MyCube->BlockOwner, bToggle); }
}

void ACube_TheBattleMasterPawn::RotationMovement_Test(bool bToggle, bool bFlip)
{

	bRotation = bToggle;

	if (bFlip) { GlobalFlip = -1.f; }
	else { GlobalFlip = 0.f; }
}


void ACube_TheBattleMasterPawn::Attack_Test(FString Name, bool bToggle)
{
	if (MyCube) { HighlightAttackOptions(MyCube->BlockOwner, bToggle, 8, 8, true, true); }
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

}

void ACube_TheBattleMasterPawn::NoAction() {
	FAction_Struct NothingAction;
	FMovement_Struct NoMove;
	NothingAction.Action_Name = "Do Nothings";
	NoMove.Movement_Name = "Stationary";
	if (M_MovementList.Contains(ActionNumb)) {
		NothingAction.Action_Position = M_MovementList.FindRef(ActionNumb).MoveDirection;
		NothingAction.MovementDuringAction = M_MovementList.FindRef(ActionNumb).MoveDirection;
	}
	else {
		NothingAction.MovementDuringAction = FVector(0.f);
		
		NoMove.MoveDirection = FVector(0.f);

		AddMovementToList(NoMove);
	}


	AddActionToList(NothingAction);
}


void ACube_TheBattleMasterPawn::SetInMotionSelectedAction(AItemBase* dummyItem) {
	//UE_LOG(LogTemp, Warning, TEXT("YEAH!"));
	SelectedItem = dummyItem;
	dummyItem->SetActionInMotion();
}

void ACube_TheBattleMasterPawn::UnSetInMotionSelectedAction(AItemBase* dummyItem) {
	//UE_LOG(LogTemp, Warning, TEXT("YEAH!"));
	
	dummyItem->UnSetActionInMotion();
	SelectedItem = NULL;
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
	
	if (SelectedItem) { /*UE_LOG(LogTemp, Warning, TEXT("Item: %s"), *SelectedItem->ActionName);*/ }
	if (MyCube) {
		//UE_LOG(LogTemp, Warning, TEXT("Cube: %s"), *MyCube->GetName());
		if (MyCube->BlockOwner){ /*UE_LOG(LogTemp, Warning, TEXT("Block: %s"), *MyCube->BlockOwner->GetName());*/ }
	}

	//SetAction(SelectedItem->ActionName, MyCube->BlockOwner->GetActorLocation());
	SetAction(SelectedItem->ActionName, MyCube->GetActorLocation());

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
		//UE_LOG(LogTemp, Warning, TEXT("%d"), MoveNumb);
		FMovement_Struct NoMove;
		NoMove.Movement_Name = "Stationary";
		NoMove.MoveDirection = FVector(0.f);

		M_MovementList.Add(MoveNumb, NoMove);

		FAction_Struct NothingAction = M_ActionStructure.FindRef(MoveNumb);

		NothingAction.Action_Position = M_MovementList.FindRef(ActionNumb).MoveDirection;
		NothingAction.MovementDuringAction = M_MovementList.FindRef(ActionNumb).MoveDirection;
		
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
	
	//UE_LOG(LogTemp, Warning, TEXT("Path length %d"), Path.Num());

	RefreshEnergy();

	for (auto& Block : Path) {
		//UE_LOG(LogTemp, Warning, TEXT("Cube position: %s"), *Block->GetActorLocation().ToString());
	}
	if (Path.Num() > 0) {
		//UE_LOG(LogTemp, Warning, TEXT("MoveBack %s"), *(Path[0]->GetActorLocation()-GetActorLocation()).ToString());
		Movement(Path[0]->GetActorLocation());
		MyCube->SetActorLocation(Path[0]->GetActorLocation());
		MyCube->BlockOwner = GetBlockFromPosition(MyCube->GetActorLocation());
	}
	if (MyCube->StartPosition != FVector(0.f)) { 
		//MyCube->SetActorLocation(MyCube->StartPosition); 
		//StartingBlock = GetBlockFromPosition(MyCube->GetActorLocation());
		//UE_LOG(LogTemp, Warning, TEXT("MovingBack to block %s"), *StartingBlock->GetName());

	}

	MyCube->SetActorRotation(MyCube->StartRotation);

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
		const FVector* DummyVector = BlockGrid->Grid.FindKey(Block);
		BlockGrid->Grid.FindRef(FVector(DummyVector->X, DummyVector->Y, DummyVector->Z))->HighlightPath(false);
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

	if (RemoveDecals.Num() != 0) {
		for (auto Elems : RemoveDecals) { Elems->Destroy(); }
		RemoveDecals.Empty();
	}
}


void ACube_TheBattleMasterPawn::ActionSelected_Implementation(){
	UpdateActions();
	if (ActionNumb == 4) { Confirm_Actions(); }
}

void ACube_TheBattleMasterPawn::BeginPlay()
{
	Super::BeginPlay();

	DynamicBaseMaterial = RadiusMesh->CreateDynamicMaterialInstance(0, BaseMaterial);

}




void ACube_TheBattleMasterPawn::UpdateActions_Implementation(){

	if (MyCube) {
		MyCube->UpdateActionList(M_PossibleActions);
	}

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
		//UE_LOG(LogTemp, Warning, TEXT("Pawn name: %s"), *GetName());
		TraceForBlock(Start, End, false, !bArrow );

		//If we want an arrow lets tick that
		if (bArrow)	{ TraceForArrow(Start, Dir, false);	}
		else if (bRotation) { TraceForRotationCircle(Start, Dir, true); }
		//else if (ArrowMesh->IsVisible()) { ArrowMesh->SetVisibility(false); RadiusMesh->SetVisibility(false); }
		//else if (RadiusMesh->IsVisible()) { RadiusMesh->SetVisibility(true); }
		else { ArrowMesh->SetVisibility(false); RadiusMesh->SetVisibility(false);
	}

		//We want some Projectile motion tracer
		if (bCrossHair)	{ TraceForCrosshair(Start, End, true); }
		else { TraceForCrosshair(Start, End, false); }



		//UE_LOG(LogTemp, Warning, TEXT("Pawn name: %s"), *this->GetName());
		//if (CurrentBlockFocus) { UE_LOG(LogTemp, Warning, TEXT("Pawn name: %s and Block name: %s"), *GetName(), *CurrentBlockFocus->GetName()); }

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
	if (GetLocalRole() == ROLE_Authority) {
		UE_LOG(LogTemp, Warning, TEXT("TEST activated"));
		if (MyCube) {
			MyCube->SetActorRotation(FRotator(0.f, MyCube->GetActorRotation().Yaw + 45.f, 0.f));
			MyCube->StartRotation = MyCube->GetActorRotation();
		}
	}
	else { Server_Test(); }
}

void ACube_TheBattleMasterPawn::Server_Test_Implementation() { Test(); }

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
		//dummyActor->SetActorLocation(FVector(dummyPosition.X, dummyPosition.Y, dummyActor->GetActorLocation().Z));
		dummyActor->SetActorLocation(FVector(dummyPosition.X, dummyPosition.Y, 500.f));
	}
}

void ACube_TheBattleMasterPawn::RotateCubeOld(ACube_TheBattleMasterBlock* Block, bool bAction) 
{
	ACube_TheBattleMasterBlock* PreviousBlock;
	if (MyCube->BlockOwner) { PreviousBlock = MyCube->BlockOwner; }
	else { PreviousBlock = GetGrid()->Grid.FindRef(GetGrid()->GridReference.FindRef(MyCube->GetActorLocation())); }

	/* Just remove move fromn all actors*/
	TArray< ACube_TheBattleMasterBlock*> AllBlocks;
	GetGrid()->Grid.GenerateValueArray(AllBlocks);
	for (auto blocks : AllBlocks) { blocks->CanRotate(false); }

	if (bAction) {

		//TODO Need to change this to be relative, now it is an absolute rotation
		int Rot = round(FMath::RoundToDouble((Block->GetActorLocation() - PreviousBlock->GetActorLocation()).Rotation().Yaw)/10)*10;
		int difference = -MyCube->GetActorRotation().Yaw + Rot;
		//UE_LOG(LogTemp, Warning, TEXT("CurrentRotation %f, ROT %d, difference %d"), MyCube->GetActorRotation().Yaw, Rot, difference); 
		//UE_LOG(LogTemp, Warning, TEXT("CurrentRotation %s, NewRotation %s"), *(MyCube->GetActorRotation()).ToString(), *(GetActorRotation() + FRotator(0.f, difference, 0.f)).ToString());
		MyCube->SetActorRotation(MyCube->GetActorRotation()+FRotator(0.f, difference, 0.f));
		/*If move twice, make the last action null*/
		if (MoveNumb == ActionNumb + 1) {
			
			FAction_Struct NothingAction;
			NothingAction.Action_Name = "Do Nothings";
			NothingAction.Action_Position = M_MovementList.FindRef(ActionNumb).MoveDirection;
			NothingAction.MovementDuringAction = M_MovementList.FindRef(ActionNumb).MoveDirection;
			NothingAction.Rotation = M_MovementList.FindRef(ActionNumb).Rotation;
			M_ActionStructure.Add(ActionNumb, NothingAction);

			ActionNumb++;
		}

		FMovement_Struct DummyMovement;
		DummyMovement.Movement_Name = "Rotation";
		DummyMovement.MoveDirection = FVector(0.f);
		DummyMovement.Rotation = difference;
		DummyMovement.bMoveOrRotate = true;
		M_MovementList.Add(MoveNumb, DummyMovement);
		MoveNumb++;
		//UE_LOG(LogTemp, Warning, TEXT("tester mate"));
	
		RefreshMovement();
		ActionSelected();
	}

	SelectedItem = nullptr;
	UpdateActions();
	////UE_LOG(LogTemp, Warning, TEXT("Is ready? %s"), MyCube->bMove ? TEXT("True") : TEXT("False"));
}

void ACube_TheBattleMasterPawn::RotateCube(float RotationAngle, bool bAction)
{
	if (MoveNumb < 4) {
		if (bAction) {

			MyCube->SetActorRotation(MyCube->GetActorRotation() + FRotator(0.f, RotationAngle, 0.f));
			/*If move twice, make the last action null*/
			if (MoveNumb == ActionNumb + 1) {
				FAction_Struct NothingAction;
				NothingAction.Action_Name = "Do Nothings";
				NothingAction.Action_Position = M_MovementList.FindRef(ActionNumb).MoveDirection;
				NothingAction.MovementDuringAction = M_MovementList.FindRef(ActionNumb).MoveDirection;
				NothingAction.Rotation = M_MovementList.FindRef(ActionNumb).Rotation;
				AddActionToList(NothingAction);
			}

			FMovement_Struct DummyMovement;
			DummyMovement.Movement_Name = "Rotation";
			DummyMovement.MoveDirection = FVector(0.f);
			DummyMovement.Rotation = RotationAngle;
			DummyMovement.bMoveOrRotate = true;

			AddMovementToList(DummyMovement);
			//UE_LOG(LogTemp, Warning, TEXT("RotationAngle %f"), RotationAngle);

			RefreshMovement();
		}
		SelectedItem = nullptr;
		UpdateActions();
	}
}


void ACube_TheBattleMasterPawn::MoveCube(ACube_TheBattleMasterBlock* Block, bool bAction) {

	if (MoveNumb < 4) {
		ACube_TheBattleMasterBlock* PreviousBlock;
		if (MyCube->BlockOwner) { PreviousBlock = MyCube->BlockOwner; }
		else { PreviousBlock = GetGrid()->Grid.FindRef(GetGrid()->GridReference.FindRef(MyCube->GetActorLocation())); }
		MyCube->BlockOwner = Block;

		/* Just remove move fromn all actors*/
		TArray< ACube_TheBattleMasterBlock*> AllBlocks;
		GetGrid()->Grid.GenerateValueArray(AllBlocks);
		for (auto blocks : AllBlocks) { blocks->CanMove(false); }


		/*Move the camera*/
		//Movement(Block->GetActorLocation());
		//

		if (bAction) {
			if (MyCube->StartPosition == FVector(0.f)) {
				MyCube->StartPosition = MyCube->GetActorLocation();
			}

			MyCube->SetActorLocation(Block->GetActorLocation());

			/*If move twice, make the last action null*/
			if (MoveNumb == ActionNumb + 1) {
				FAction_Struct NothingAction;
				NothingAction.Action_Name = "Do Nothings";
				NothingAction.Action_Position = M_MovementList.FindRef(ActionNumb).MoveDirection;
				NothingAction.MovementDuringAction = M_MovementList.FindRef(ActionNumb).MoveDirection;
				NothingAction.Rotation = M_MovementList.FindRef(ActionNumb).Rotation;
				AddActionToList(NothingAction);
			}
			Highlight_Path(PreviousBlock, Block);

			float Rot = 0;

			if (SelectedItem)
			{
				FAction_Struct DummyAction;
				DummyAction.Action_Name = SelectedItem->ActionName;
				DummyAction.Action_Position = M_MovementList.FindRef(ActionNumb).MoveDirection;
				DummyAction.Item = SelectedItem;
				DummyAction.MovementDuringAction = Block->GetActorLocation() - PreviousBlock->GetActorLocation();
				Rot = roundf(180 + FMath::RoundToDouble((Block->GetActorLocation() - PreviousBlock->GetActorLocation()).Rotation().Yaw - MyCube->GetActorRotation().Yaw));

				DummyAction.Rotation = Rot;

				AddActionToList(DummyAction);

				RotateCube(Rot, true);
				M_MovementList[MoveNumb - 1].MoveDirection = Block->GetActorLocation() - PreviousBlock->GetActorLocation();
				M_MovementList[MoveNumb - 1].Rotation = Rot;
			}
			RefreshMovement();
		}

		if (MyCube) { Movement(MyCube->GetActorLocation()); }
		SelectedItem = nullptr;
		UpdateActions();
		////UE_LOG(LogTemp, Warning, TEXT("Is ready? %s"), MyCube->bMove ? TEXT("True") : TEXT("False"));
	}
}


void ACube_TheBattleMasterPawn::CalcCamera(float DeltaTime, struct FMinimalViewInfo& OutResult)
{
	Super::CalcCamera(DeltaTime, OutResult);

	OutResult.Rotation = FRotator(-90.0f, 0.0f, 0.0f);
}


void ACube_TheBattleMasterPawn::SetAction(FString ActionName, FVector Direction) {
	//UE_LOG(LogTemp, Warning, TEXT("GotHere"));

	if ( ActionNumb < 4) {
		FAction_Struct Actions;
		FVector DummyPosition;
		//UE_LOG(LogTemp, Warning, TEXT("Attack direction: %s"), *AttackDirection.ToString());
		//UE_LOG(LogTemp, Warning, TEXT("Direction: %s"), *Direction.ToString());

		if (Direction!=FVector(0.f)) { DummyPosition = Direction - MyCube->GetActorLocation(); }
		else { DummyPosition =  MyCube->GetActorLocation(); }
		//UE_LOG(LogTemp, Warning, TEXT("Direction: %s"), *DummyPosition.ToString());

		if (SelectedItem) {
			Actions.Action_Name = SelectedItem->ActionName;
			Actions.Action_Position = Direction;
			Actions.Action_Weapon = SelectedItem->WeaponName;
			Actions.Item = SelectedItem;
			if (CurrentMeshFocus) { Actions.SelectedActor = CurrentActorFocus; /*UE_LOG(LogTemp, Warning, TEXT("selected actor: %s"), *CurrentActorFocus->GetName());*/ }

			/*
			if the movement is at 4 then reset the movement, re do the path
			New function - redo action/movement
			*/

			if (M_MovementList.Contains(ActionNumb)) {
				//UE_LOG(LogTemp, Warning, TEXT("test"));
				if (!SelectedItem->bCanBeUsedWithMovement) {
					//UE_LOG(LogTemp, Warning, TEXT("test2"));
					//if (M_MovementList.FindRef(ActionNumb).MoveDirection != FVector(0.f)) {
						//UE_LOG(LogTemp, Warning, TEXT("test3"));

						FAction_Struct NothingAction;
						NothingAction.Action_Name = "Movement";
						NothingAction.MovementDuringAction = M_MovementList.FindRef(ActionNumb).MoveDirection;
						NothingAction.Action_Position = M_MovementList.FindRef(ActionNumb).MoveDirection;
						//M_ActionStructure.Add(ActionNumb, NothingAction);
						AddActionToList(NothingAction);
						//ActionNumb++;
						FMovement_Struct NoMovement;
						NoMovement.Movement_Name = "Stationary";
						NoMovement.MoveDirection = FVector(0.f);
					/*	M_MovementList.Add(ActionNumb, NoMovement);
						MoveNumb++;
						RefreshMovement();*/
						AddMovementToList(NoMovement);
					//}
				}
			}else{
				FMovement_Struct NoMovement;
				NoMovement.Movement_Name = "Stationary";
				NoMovement.MoveDirection = FVector(0.f);
				/*M_MovementList.Add(ActionNumb, NoMovement);
				MoveNumb++;*/
				
				AddMovementToList(NoMovement);
				//RefreshMovement();
			}
			Actions.MovementDuringAction = M_MovementList.FindRef(ActionNumb).MoveDirection;
			//M_ActionStructure.Add(ActionNumb, Actions);
			AddActionToList(Actions);
			
			//Actions.MovementDuringAction = M_MovementList.FindRef(ActionNumb);
			
	
			SelectedItem->DoAction(false, Direction);
			//UE_LOG(LogTemp, Warning, TEXT("Direction %s"), *Direction.ToString());
			//MyCube->Attack(this, SelectedItem, Direction);
			SelectedItem->UnSetActionInMotion();
			//if (bCrossHair) { SelectedItem->SetActorRotation(SelectedItem->DefaultRotation); }
			SelectedItem = NULL;
		}
		else {
			Actions.Action_Name = ActionName;
			//Actions.Action_Name = = "Do Nothing";

			//Actions.Action_Position = DummyPosition;
			//M_ActionStructure.Add(ActionNumb, Actions);
			AddActionToList(Actions);

		}
		//UE_LOG(LogTemp, Warning, TEXT("%d  -> %s"), ActionNumb, *ActionName);

		if (M_MovementList.Contains(ActionNumb)) {
			//UE_LOG(LogTemp, Warning, TEXT("%d  -> %s, move %s"), ActionNumb, *ActionName, *M_MovementList[ActionNumb].MoveDirection.ToString());
		}
		//ActionNumb++;
		//ActionSelected();
		
		//Unhighlight an actor if highlighted
		if (CurrentMeshFocus) {
			CurrentMeshFocus->SetRenderCustomDepth(false);
			CurrentMeshFocus = nullptr;
		}
	}
}


void ACube_TheBattleMasterPawn::AddActionToList(FAction_Struct Action) { 
	if (GetLocalRole() < ROLE_Authority) { Server_AddActionToList(Action); }
	M_ActionStructure.Add(ActionNumb, Action); ActionNumb++;
	if (GetLocalRole() != ROLE_Authority) { ActionSelected(); }
}

void ACube_TheBattleMasterPawn::AddMovementToList(FMovement_Struct Movement) { 
	if (GetLocalRole() < ROLE_Authority) { Server_AddMovementToList(Movement); }
	M_MovementList.Add(MoveNumb, Movement); MoveNumb++; 
	if (GetLocalRole() != ROLE_Authority) { RefreshMovement(); }
}

void ACube_TheBattleMasterPawn::Server_AddActionToList_Implementation(FAction_Struct Action) { AddActionToList(Action); }

void ACube_TheBattleMasterPawn::Server_AddMovementToList_Implementation(FMovement_Struct Movement) { AddMovementToList(Movement); }


void ACube_TheBattleMasterPawn::DoAction(int int_Action)
{
	if (GetLocalRole() < ROLE_Authority) {
		MyCube->SetReplicatingMovement(true);
		MyCube->E_TurnStateEnum = ETurnState::TS_InitiateActions;

		MyCube->Server_SetAction(M_ActionStructure[int_Action].Item,
			M_MovementList[int_Action].MoveDirection,
			M_ActionStructure[int_Action].SelectedActor,
			M_MovementList[int_Action].Rotation,
			M_ActionStructure[int_Action].Action_Position);

		for (int Elems = 0; Elems < 4; Elems++)
		{
			if (M_MovementList.Contains(Elems)) {
				//UE_LOG(LogTemp, Warning, TEXT("%d: Movement %s"), Elems, *M_MovementList[Elems].MoveDirection.ToString());
			}else{
				//UE_LOG(LogTemp, Warning, TEXT("%d: NOPE"), Elems);
			}
		}

		/*if (M_ActionStructure[int_Action].Item) { UE_LOG(LogTemp, Warning, TEXT("Pawm: Itme %s"), *M_ActionStructure[int_Action].Item->GetName()); }
		UE_LOG(LogTemp, Warning, TEXT("Pawm: Direction %s"), *M_MovementList[int_Action].MoveDirection.ToString());
		if (M_ActionStructure[int_Action].SelectedActor) { UE_LOG(LogTemp, Warning, TEXT("Pawm: Selected Actor %s"), *M_ActionStructure[int_Action].SelectedActor->GetName()); }
		UE_LOG(LogTemp, Warning, TEXT("Pawm: Rotation %f"), M_MovementList[int_Action].Rotation);*/

		MyCube->InitiateMovementAndAction();
	}
}


ACube_TheBattleMasterBlockGrid* ACube_TheBattleMasterPawn::GetServerGrid()
{
	//UE_LOG(LogTemp, Warning, TEXT("MY Grid name: %s "), *MyGrid->GetName());
	int j = 0;
	for (TObjectIterator<ACube_TheBattleMasterBlockGrid> Grid; Grid; ++Grid) {
		return *Grid;
		MyGrid = *Grid;
	}
	return MyGrid;
}

ACube_TheBattleMasterBlockGrid* ACube_TheBattleMasterPawn::GetGrid() 
{
	//return GetServerGrid();
	//UE_LOG(LogTemp, Warning, TEXT("MY Grid name: %s "), *MyGrid->GetName());
	int j = 0;
	for (TObjectIterator<ACube_TheBattleMasterBlockGrid> Grid; Grid; ++Grid) {
		if (Cast<ACube_TheBattleMasterPlayerController>(GetOwner())->PlayerNumb == j) { return *Grid; }
		
		//UE_LOG(LogTemp, Warning, TEXT("GridName: %s, itteration %d, player num %d"), *Grid->GetName(), j, Cast<ACube_TheBattleMasterPlayerController>(GetOwner())->PlayerNumb);
		j++;
	/*	if (GetLocalRole() < ROLE_Authority){
			if (MyGrid->GetName() != Grid->GetName()) { Grid->Destroy(); }
		}else { if (MyGrid->GetName() == Grid->GetName()) { Grid->Destroy(); } }*/
		//if (!MyGrid) { 
			MyGrid = *Grid; 
		//}
	}
	return MyGrid;

	FString StopLoop = GetName();
	StopLoop.RemoveAt(0, GetName().Len() - 1);
	int32 Stoploop = FCString::Atoi(*StopLoop);
	//UE_LOG(LogTemp, Warning, TEXT("PlayerName: %s and number: %d"), *GetName(), Stoploop);
	//UE_LOG(LogTemp, Warning, TEXT("PlayerOwnerName: %s and number: %d"), *GetOwner()->GetName(), Stoploop);
	int32 i = 0;
	//ACube_TheBattleMasterBlockGrid* test = nullptr;
	for (TObjectIterator<ACube_TheBattleMasterBlockGrid> Grid; Grid; ++Grid) {
		//UE_LOG(LogTemp, Warning, TEXT("Grid name: %s at number %d"), *Grid->GetName(), i);
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

void ACube_TheBattleMasterPawn::Server_SetCube_Implementation(ACube_TheBattleMasterPawn* Pawn, ACube_TheBattleMasterBlock* Block, FImportedCube_Struct SCube, const TArray<FName>& Keys, const TArray<TSubclassOf<AItemBase>>& ItemClasses) {

	TMap<FName, TSubclassOf<AItemBase>> AttachedItems;
	for (int i = 0; i < ItemClasses.Num(); i++){AttachedItems.Add(Keys[i], ItemClasses[i]);	}
	SetCube(Pawn, Block, SCube, AttachedItems);
}

void ACube_TheBattleMasterPawn::SetCube(ACube_TheBattleMasterPawn* Pawn, ACube_TheBattleMasterBlock* Block, FImportedCube_Struct SCube, TMap<FName, TSubclassOf<AItemBase>> AttachedItems)
{
	TArray<FName> SlotNames;
	AttachedItems.GenerateKeyArray(SlotNames);

	TArray<TSubclassOf<AItemBase>> ItemClasses;
	AttachedItems.GenerateValueArray(ItemClasses);
	
	if (GetLocalRole() < ROLE_Authority) { Server_SetCube(Pawn, Block, SCube, SlotNames, ItemClasses); }
	else {

		FActorSpawnParameters params;
		if (Block) {
			//UE_LOG(LogTemp, Warning, TEXT("Pawn name: %s and Block name: %s"), *Pawn->GetName(), *Block->GetName());

			Pawn->MyCube = GetWorld()->SpawnActor<APlayer_Cube>(Block->GetActorLocation(), FRotator(0, 0, 0), params);
			Pawn->SetCubeDetails(SCube, AttachedItems, MyCube);

			Pawn->MyCube->SetOwner(this);
		}
	}

	CubeMade();
	if (MyCube) {
		Movement(MyCube->GetActorLocation());
		GetBlockFromPosition(MyCube->GetActorLocation());
	}
}

void ACube_TheBattleMasterPawn::SetCubeDetails(FImportedCube_Struct SCube, TMap<FName, TSubclassOf<AItemBase>> AttachedItems, APlayer_Cube* Cube)
{
	if (SCube.Name != "") {
		Cube->ThisCubeName = SCube.Name;
		Cube->Base_Health = SCube.BaseHealth;
		Cube->Base_Armour = SCube.BaseArmour;
		Cube->Base_Speed = SCube.BaseSpeed;
		Cube->GetBlockMesh()->CreateDynamicMaterialInstance(0, VariedCubeMaterial)->SetVectorParameterValue("BaseColor", SCube.Colours);
		Cube->CubeColour = SCube.Colours;
	}

	for (auto Elems : AttachedItems)
	{
		//UE_LOG(LogTemp, Warning, TEXT("Attached items %s"), *Elems.Value->GetName());
		TSubclassOf<AItemBase> ItemClass = Elems.Value;
		if (ItemClass && Cube)
		{
			FActorSpawnParameters params;
			AItemBase* Item = GetWorld()->SpawnActor<AItemBase>(ItemClass, FVector(0.f), FRotator(0.f), params);
			Item->Cube = Cube;
			Item->SetOwner(this);

			Cube->GetAttachComponent()->Equip(Item, Elems.Key);
		}
	}
}


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
}

void ACube_TheBattleMasterPawn::Server_ToggleOccupied_Implementation(ACube_TheBattleMasterBlock* Block, bool Bon) { ToggleOccupied(Block, Bon); }

void ACube_TheBattleMasterPawn::HighlightRotationOptions(ACube_TheBattleMasterBlock * CenterBlock, bool Bmove)
{
	ACube_TheBattleMasterBlock* PreviousBlock;

	if (CenterBlock) { PreviousBlock = CenterBlock; }
	else { PreviousBlock = GetBlockFromPosition(MyCube->GetActorLocation()); }

	TArray<ACube_TheBattleMasterBlock*> BlocksToHighlight;

	//Get rotation speed
	//highlihgt areas that can be moved to in 6 sec
	//Or just get the ring because easier?

	GetGrid()->GetRing(PreviousBlock, 1, BlocksToHighlight);

	for (auto Elems : BlocksToHighlight) { Elems->CanRotate(Bmove); }
}

void ACube_TheBattleMasterPawn::HighlightMoveOptions(ACube_TheBattleMasterBlock * CenterBlock, bool Bmove) {		

	ACube_TheBattleMasterBlock* PreviousBlock;
	TArray<ACube_TheBattleMasterBlock*> BlocksToHighlight;

	int32 speed = 1;
	float RotationSpeed = MyCube->Replicated_RotationSpeed;

	if (CenterBlock) { PreviousBlock = CenterBlock; }
	else { PreviousBlock = GetBlockFromPosition(MyCube->GetActorLocation()); }

	if(SelectedItem)
	{
		//Get the direction - Make sure it is 0 to 6 at 0.5 increments
		float Direction = (SelectedItem->GetActorForwardVector().Rotation().Yaw + 90.f) / 60.f;
		Direction = round(Direction * 2)  / 2;
		Direction = Direction >= 0 ? Direction : Direction + 6;
		//UE_LOG(LogTemp, Warning, TEXT("SelectedItemDirection %f"), Direction);
//
		speed = MyCube->Replicated_Speed;
		int StartScan = 0;
		
		if (!FMath::IsNearlyEqual(FMath::Frac(Direction), 0.5f, 0.01f)) {
			GetGrid()->GetForwardDirection(PreviousBlock, speed, BlocksToHighlight, Direction);
			StartScan = 1;		
			speed = speed-1/RotationSpeed;
		}
		else { speed = speed - 0.5 / RotationSpeed; }

		
		for (int i = StartScan; i< 3; i++) {		
			GetGrid()->GetForwardDirection(PreviousBlock, speed, BlocksToHighlight, (6 + int32(FMath::RoundToFloat(Direction + (1 - StartScan) * 0.5)) + i) % 6);
			GetGrid()->GetForwardDirection(PreviousBlock, speed, BlocksToHighlight, (6 + int32(FMath::RoundToFloat(Direction - (1 - StartScan) * 0.5)) - i) % 6);
			speed = speed - 1 / RotationSpeed;
		}
	}
	else{
		//UE_LOG(LogTemp, Warning, TEXT("PreviousBlock %s"), *PreviousBlock->GetName());
		for (int i = 0; i < 6; i++) {
			
			GetGrid()->GetForwardDirection(PreviousBlock, 1, BlocksToHighlight, i);
		}
	}
	for (auto Elems : BlocksToHighlight) {  Elems->CanMove(Bmove); }
}

float ACube_TheBattleMasterPawn::GetQuadrent(float YawActor, float PitchActor) {
	//All 
	if (FMath::IsNearlyEqual(PitchActor, 90.f, 0.1f)) { return 0; }
	//NONE 
	if (FMath::IsNearlyEqual(PitchActor, -90.f, 0.1f)) { return 100; }	
	//UP
	if (FMath::IsNearlyEqual(YawActor, 0.f, 0.1f)) { return PI/4; }
	//Right
	if (FMath::IsNearlyEqual(YawActor, 90.f, 0.1f) ) { return -PI/4; }
	//Down
	if (FMath::IsNearlyEqual(YawActor, 180.f, 0.1f) ) { return -3*PI/4; }
	if (FMath::IsNearlyEqual(YawActor, -180.f, 0.1f) ) { return -3 * PI / 4; }
	//Left
	if (FMath::IsNearlyEqual(YawActor, -90.f, 0.1f) ) { return 3*PI/4; }

	//default
	return 100; 
}
 
void ACube_TheBattleMasterPawn::HighlightAttackOptions(ACube_TheBattleMasterBlock* Block, bool bToggle, int minDistance, int maxDistance, bool bAttackImmutables, bool bCross)
{
	//bCrossHair = bCross;


	ACube_TheBattleMasterBlock* CenterBlock;
	if (Block) { CenterBlock = Block; }
	else { CenterBlock = GetBlockFromPosition(MyCube->GetActorLocation()); }

	//TODO GetRidOfThis for a top bot and other test
	float Qualifier;
	if (SelectedItem) { Qualifier = GetQuadrent(SelectedItem->GetActorRotation().Yaw, SelectedItem->GetActorRotation().Pitch); }
	else { Qualifier = 0; }

	float Direction = 0.f;
	if (SelectedItem) { Direction = (SelectedItem->GetActorForwardVector().Rotation().Yaw - 90.f) / 60.f; }
	Direction = round(Direction * 2) / 2;
	Direction = Direction >= 0 ? Direction : Direction + 6;

	TArray<ACube_TheBattleMasterBlock*> DummyBlocks;
	for (int i = minDistance; i <= maxDistance; i++) {
		if(Qualifier == 0) 	{ GetGrid()->GetRing(CenterBlock, i, DummyBlocks); }
		else { GetGrid()->GetSectorRing(CenterBlock, i, Direction, DummyBlocks); }
	}
	for (auto Elems : DummyBlocks) { Elems->CanAttack(bToggle, bAttackImmutables); }
}

void ACube_TheBattleMasterPawn::Highlight_Path(ACube_TheBattleMasterBlock* Start, ACube_TheBattleMasterBlock* End)
{
		int Distance = round(FVector::Distance(Start->GetActorLocation(), End->GetActorLocation())/60);

		float Direction = ((Start->GetActorLocation() - End->GetActorLocation()).Rotation().Yaw + 90.f) / 60.f;
		
		Direction = round(Direction * 2) / 2;
		Direction = Direction >= 0 ? Direction : Direction + 6;

		TArray<ACube_TheBattleMasterBlock*> DummyBlockList;
		GetGrid()->GetForwardDirection(Start, Distance-1, DummyBlockList, Direction);

		for (auto Elems : DummyBlockList) {
			Path.Add(Elems);
			Elems->HighlightPath(true);
		}
}
  

void ACube_TheBattleMasterPawn::TriggerClick()
{
	if (bArrow) { SelectedItem->UnSetActionInMotion();  SetAction("Attack", AttackDirection); this->Reset_Buttons_test(); }
	else if (bRotation) { RotateCube(GlobalRoation, true); this->Reset_Buttons_test(); bRotation = false; }
	else if (CurrentBlockFocus && !bDead)
	{
		CurrentBlockFocus->HandleClicked();
		if (MyCube != nullptr) {
			if (CurrentBlockFocus->bMove) { MoveCube(CurrentBlockFocus, true); this->Reset_Buttons_test();}

			if (CurrentBlockFocus->bAttack) { SetAction("Attack", CurrentBlockFocus->GetActorLocation());  this->Reset_Buttons_test(); }
		}
		else if(CurrentBlockFocus){ SetCube(this, CurrentBlockFocus, ImportedSavedDetails->SavedCube, ImportedSavedDetails->AllItemsAttached);}	
	}
	if (bCrossHair) { 
		SelectedItem->SetActorRotation(SelectedItem->DefaultRotation); 
	}
}

void ACube_TheBattleMasterPawn::Turn()
{
	for (auto Elems : M_ActionStructure) {

		if (M_MovementList.Contains(Elems.Key)) { /*UE_LOG(LogTemp, Warning, TEXT("%d: MovementDuringAction %s and MoveDirection %s"), Elems.Key, *Elems.Value.MovementDuringAction.ToString(), *M_MovementList[Elems.Key].MoveDirection.ToString()); */}
		//Elems.Value.MovementDuringAction = M_MovementList[Elems.Key].MoveDirection;


	}
	TArray<FMovement_Struct> DummyMove;
	M_MovementList.GenerateValueArray(DummyMove);
	TArray<FAction_Struct> DummyAction;
	M_ActionStructure.GenerateValueArray(DummyAction);
	//Server_SetActionAndMovementListOnServer(DummyMove, DummyAction);

	if (GetLocalRole() < ROLE_Authority) { Server_Turn(); }
	else {
		ACube_TheBattleMasterGameMode* BaseGameMode = Cast<ACube_TheBattleMasterGameMode>(UGameplayStatics::GetGameMode(this));
		BaseGameMode->TakeTurn();
	}	
}

void ACube_TheBattleMasterPawn::Server_Turn_Implementation() { Turn();}


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
		//UE_LOG(LogTemp, Warning, TEXT("Hit actor: %s, this pawn: %s"), *HitResult.Actor->GetName(), *this->GetName());

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
			//UE_LOG(LogTemp, Warning, TEXT("%s"), *HitResult.Actor->GetName());
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
	//UE_LOG(LogTemp, Warning, TEXT("Rotation %s"), *SelectedItem->GetActorRotation().ToString());

	//AttackDirection = UKismetMathLibrary::CreateVectorFromYawPitch(angle, 0.f, Dis) + SelectedItem->GetActorLocation();
	AttackDirection = UKismetMathLibrary::CreateVectorFromYawPitch(angle - SelectedItem->GetActorRotation().Yaw, 0.f, Dis);

	//UE_LOG(LogTemp, Warning, TEXT("direction: %s, Actor: %s"), *AttackDirection.ToString(), *SelectedItem->GetActorRotation().ToString());


	ArrowMesh->SetRelativeScale3D(FVector(0.05f, 0.05f, Dis));
	ArrowMesh->SetWorldLocation(SelectedItem->GetActorLocation());
	ArrowMesh->SetRelativeRotation(Dir);
	ArrowMesh->SetVisibility(true);

	RadiusMesh->SetRelativeScale3D(FVector(10*rangeEnd, 10*rangeEnd, 1.0f));
	RadiusMesh->SetWorldLocation(SelectedItem->GetActorLocation());
	//RadiusMesh->SetWorldRotation(SelectedItem->GetActorRotation());

	RadiusMesh->SetVisibility(true);
	DynamicBaseMaterial->SetScalarParameterValue("Alpha", 60.f/360.f);
	DynamicBaseMaterial->SetScalarParameterValue("CutOff", 0.9f);
	DynamicBaseMaterial->SetScalarParameterValue("InnerRadius", 0.f);
	DynamicBaseMaterial->SetScalarParameterValue("OuterRadius", 0.8f);
	DynamicBaseMaterial->SetScalarParameterValue("StartingAngle", ((SelectedItem->GetActorRotation().Pitch * 2 - SelectedItem->GetActorRotation().Yaw + 30.f)) / 360.f);
	//DynamicBaseMaterial->SetScalarParameterValue("StartingAngle", 30.f / 360.f);
	DynamicBaseMaterial->SetScalarParameterValue("RemoveArrow", 0.9f);
	DynamicBaseMaterial->SetScalarParameterValue("Flip", 1.f);

	

}

void ACube_TheBattleMasterPawn::TraceForRotationCircle(FVector Start, const FVector& Direction, bool bDrawDebugHelpers)
{

	FHitResult HitResult;
	FCollisionQueryParams Params;

	TArray<AActor*> ActorList;
	ActorList.Add(MyCube);
	ActorList.Add(this);
	MyCube->GetAttachedActors(ActorList, false);
	Params.AddIgnoredActors(ActorList);
	
	FVector End = Start + (Direction * (100 - Start.Z) / Direction.Z);

	GetWorld()->LineTraceSingleByChannel(HitResult, GetActorLocation(), End, ECC_Visibility, Params);
	if (bDrawDebugHelpers)
	{
		DrawDebugLine(GetWorld(), MyCube->GetActorLocation(), End, FColor::Blue);
		DrawDebugSolidBox(GetWorld(), HitResult.Location, FVector(20.0f), FColor::Red);
	}

	float angle = (End - MyCube->GetActorLocation()).Rotation().Yaw - MyCube->GetActorRotation().Yaw;
	//UE_LOG(LogTemp, Warning, TEXT("angle %f"), angle);

	if (GlobalFlip == 0) { DynamicBaseMaterial->SetScalarParameterValue("Flip", 1.f); }
	else {
		DynamicBaseMaterial->SetScalarParameterValue("Flip", -1.f);
		angle = -angle;
	}

	angle = angle + 180;

	if (angle < 0) { angle = angle + 360; }
	if (angle > 360) { angle = angle - 360; }
	angle = 30.f*FMath::RoundFromZero((angle) / 30.f);
	
	
	if (GlobalFlip == 0) { GlobalRoation = angle; }
	else {
		GlobalRoation = -angle;
	}
	

	RadiusMesh->SetRelativeScale3D(FVector(5.f));
	RadiusMesh->SetWorldLocation(MyCube->GetActorLocation() + FVector(0.f, 0.f, 100.f));
	RadiusMesh->SetVisibility(true);

	DynamicBaseMaterial->SetScalarParameterValue("Alpha", angle / 360.f);
	DynamicBaseMaterial->SetScalarParameterValue("CutOff", 0.9f);
	DynamicBaseMaterial->SetScalarParameterValue("InnerRadius", 0.f);
	DynamicBaseMaterial->SetScalarParameterValue("OuterRadius", 0.7f);
	//DynamicBaseMaterial->SetScalarParameterValue("StartingAngle", (180.f - 30.f - MyCube->GetActorRotation().Yaw + MyCube->GetActorRotation().Pitch * 2 + 30) / 360.f);
	DynamicBaseMaterial->SetScalarParameterValue("StartingAngle", (180.f - MyCube->GetActorRotation().Yaw ) / 360.f);
	DynamicBaseMaterial->SetScalarParameterValue("RemoveArrow", 1.0f);


}

void ACube_TheBattleMasterPawn::TraceForCrosshair(const FVector& Start, const FVector& End, bool bToggleOn)
{
	if (bToggleOn && SelectedItem) {
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
		float pitchNew = 0;
		for (float i = 39.0f; i < 95.1f; i++) {
			pitchNew = PI / 180.f* (i);
			newDistance = MaxVelocity * FMath::Cos(pitchNew) *(MaxVelocity*FMath::Sin(pitchNew) + sqrt(MaxVelocity*MaxVelocity*FMath::Sin(pitchNew)*FMath::Sin(pitchNew) + 2 * 980.f * SelectedItem->GetActorLocation().Z)) / 980.f;
			pitchNew = pitchNew * 180.f / PI;
			if (FMath::IsWithin(newDistance, distance - 10, distance + 10)) { break; }
			//UE_LOG(LogTemp, Warning, TEXT("End Pitch: %f, old distance %f, new distance %f"), alpha, distance, newDistance);
		}

		if (CurrentBlockFocus) {/*TODO  Test this to see if it is what we want*/
			if (CurrentBlockFocus->bAttack) {
				SelectedItem->SetActorRotation(FRotator(
					pitchNew,
					(EndPosition - StartPosition).Rotation().Yaw,
					SelectedItem->GetActorRotation().Roll));
			}/*TODO  DefaultRotation needs to be updated when the cube is rotated*/
		} else { SelectedItem->SetActorRotation(SelectedItem->DefaultRotation); }

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
			if (i + 1 < Ring->GetInstanceCount()) { Ring->UpdateInstanceTransform(i, InstTransfrom, true, true, true); }
			else { Ring->AddInstanceWorldSpace(InstTransfrom); }
		}for (int i = PredictResult.PathData.Num() - 2; i < Ring->GetInstanceCount(); i++) { Ring->RemoveInstance(i); }
	}
	else { for (int32 i = 0; i < Ring->GetInstanceCount(); i++) { Ring->RemoveInstance(i); } }
}



void ACube_TheBattleMasterPawn::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ACube_TheBattleMasterPawn, MyCube);
	DOREPLIFETIME(ACube_TheBattleMasterPawn, CubeSelected);
	DOREPLIFETIME(ACube_TheBattleMasterPawn, PlayerCubeClass);

	DOREPLIFETIME(ACube_TheBattleMasterPawn, bReady);
	


}