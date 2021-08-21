// Fill out your copyright notice in the Description page of Project Settings.
#include "Player_Cube.h"
#include "Items/Booster_Basic.h"
#include "Cube_TheBattleMasterPawn.h"

#include "Components/PrimitiveComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Net/UnrealNetwork.h"
#include "Materials/MaterialInstance.h"
#include "GameFramework/GameStateBase.h"
//#include "Kismet/KismetMathLibrary.h"

// Sets default values
APlayer_Cube::APlayer_Cube()
{
	SetReplicatingMovement(true);
	//SetReplicatingMovement(false);

	//bReplicateMovement = true;
	bReplicates = true;

	bNetUseOwnerRelevancy = true;
	
	bAlwaysRelevant = true;
	//if (GetLocalRole() > ROLE_Authority) {
	//	NetUpdateFrequency = 10000;
	//}

	PrimaryActorTick.bCanEverTick = true;

	// Structure to hold one-time initialization
	struct FConstructorStatics
	{
		ConstructorHelpers::FObjectFinderOptional<UStaticMesh> PlaneMesh;
		ConstructorHelpers::FObjectFinderOptional<UMaterial> BaseMaterial;
		ConstructorHelpers::FObjectFinderOptional<UMaterialInstance> VariedCubeMaterial;
		FConstructorStatics()
			: PlaneMesh(TEXT("/Game/Puzzle/Meshes/PuzzleCube.PuzzleCube"))
			, VariedCubeMaterial(TEXT("/Game/Puzzle/Meshes/BaseMaterial_Inst.BaseMaterial_Inst"))
			, BaseMaterial(TEXT("/Game/Puzzle/Meshes/BaseMaterial.BaseMaterial"))
		{
		}
	};
	static FConstructorStatics ConstructorStatics;

	// Create dummy root scene component
	DummyRoot = CreateDefaultSubobject<USceneComponent>(TEXT("Dummy0"));
	//DummyRoot->SetIsReplicated(true);
	RootComponent = DummyRoot;
	
	//Create Movement component
	MoveComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("MoveComp0"));
	MoveComponent->StopMovementImmediately();
	MoveComponent->ProjectileGravityScale = 0.f;
	
	//MoveComponent->ShouldApplyGravity();

	//MoveComponent->InitializeComponent();
	//MoveComponent->RegisterComponent();
	//MoveComponent->SetIsReplicated(true);
	//MoveComponent->Velocity = FVector(0.f);
	//MoveComponent->SetupAttachment(DummyRoot);

	RotMoveComponent = CreateDefaultSubobject<URotatingMovementComponent>(TEXT("RotMoveComp0"));
	RotMoveComponent->RotationRate = FRotator(0.f);
	//RotMoveComponent->SetIsReplicated(true);
	//RotMoveComponent->SetupAttachment(DummyRoot);

	//Create Attack component
	//AttackComponent = CreateDefaultSubobject<UAttackComponent>(TEXT("BlockAttackComeponent0"));
	//AttackComponent->SetupAttachment(DummyRoot);

	//Create Attach componenet
	AttachmentComponent = CreateDefaultSubobject<UAttachmentComponent>(TEXT("Attachment Component"));
	check(AttachmentComponent);
	AttachmentComponent->RegisterComponent();

	// Create static mesh component
	BlockMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BlockMesh0"));
	//BlockMesh->SetIsReplicated(true);
	BlockMesh->SetStaticMesh(ConstructorStatics.PlaneMesh.Get());
	BlockMesh->SetRelativeScale3D(FVector(0.25f, 0.25f, 0.25f));
	BlockMesh->SetRelativeLocation(FVector(0.f, 0.f, 35.f));
	BlockMesh->SetMaterial(0, ConstructorStatics.VariedCubeMaterial.Get());
	BlockMesh->SetupAttachment(DummyRoot);
	
	

	// Save a pointer to the orange material
	BaseMaterial = ConstructorStatics.BaseMaterial.Get();

	//Create camera components
	OurCameraSpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraSpringArm"));
	OurCameraSpringArm->SetupAttachment(DummyRoot);
	OurCameraSpringArm->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, 0.0f), FRotator(90.0f, 0.0f, 0.0f));
	OurCameraSpringArm->TargetArmLength = 400.f;
	OurCameraSpringArm->bEnableCameraLag = true;
	OurCameraSpringArm->CameraLagSpeed = 3.0f;

	OurCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("GameCamera"));
	OurCamera->SetupAttachment(OurCameraSpringArm, USpringArmComponent::SocketName);
	OurCamera->SetRelativeLocationAndRotation(FVector(1500.0f, 0.0f, 0.0f), FRotator(0.f, 180.f, 0.f));

	OnTakeAnyDamage.AddDynamic(this, &APlayer_Cube::ApplyDamage);
	OnTakeRadialDamage.AddDynamic(this, &APlayer_Cube::ApplyRadialEffects);
	
	//DefineSlots
	//M_SlotsRefference.Add(FString("S_Slot1"), NULL);
	//M_SlotsRefference.Add(FString("S_Slot2"), NULL);

	

	//Extra variables
	Base_Health = 100.0f;
	Replicated_Health = Base_Health;
	Replicated_Speed = Base_Speed;

	//Extra (none nessessary) variables	
	
}

void APlayer_Cube::ReadyTheCube(bool bIsReady) {
	if (GetLocalRole() < ROLE_Authority) { Server_ReadyTheCube(bIsReady); }	
	bReady = bIsReady; 
}

void APlayer_Cube::Server_ReadyTheCube_Implementation(bool bIsReady) { ReadyTheCube(bIsReady); }

//void APlayer_Cube::BeginPlay() {
//
//	Super::BeginPlay();
//
//	//OnTakeAnyDamage;
//
//	//OnTakeAnyDamage.AddDynamic(this, &APlayer_Cube::OnTakeAnyDamage);
//
//	/*AController* ThisPlayerContoler = Cast<AController>(this->GetOwner());
//	ApplyDamage(this, 10, ThisPlayerContoler, this, FDamageEvent());*/
//	
//}

void APlayer_Cube::OnConstruction(const FTransform &Transform) {
	Super::OnConstruction(Transform);

	E_TurnStateEnum = ETurnState::TS_SelectActions;

	PrimaryActorTick.TickGroup = TG_PrePhysics;

	//BlockMesh->OnComponentBeginOverlap.AddDynamic(this, &APlayer_Cube::Overlap);

	//if (GetLocalRole() < ROLE_Authority){
	//	BlockMesh->SetSimulatePhysics(true);
	//}
	//else { BlockMesh->SetSimulatePhysics(false); }

	MyPawn = Cast<ACube_TheBattleMasterPawn>(GetOwner());
	CurrentPosition = FVector(0.f);
	StartRotation = GetActorRotation();

	//SetCameraView();
	//if (MyPawn) { Cast<ACube_TheBattleMasterPlayerController>(MyPawn->GetOwner())->SetViewTarget(Cast<AActor>(OurCamera)); }
	//GetCamera()->SetRelativeRotation(FRotator(0.f, 180.f, 0.f));

	//MyPawn->ClearVars();
	
}

void APlayer_Cube::SetCameraView() {
	//if (ROLE_Authority == GetLocalRole()) { Server_SetCameraView(); }
	if (MyPawn) { 
		
		Cast<ACube_TheBattleMasterPlayerController>(MyPawn->GetOwner())->SetViewTarget(Cast<AActor>(OurCamera)); 
	}
}
//
//void APlayer_Cube::MultiCast_SetCameraView_Implementation() {
//	return SetCameraView();
//}
//
//void APlayer_Cube::Server_SetCameraView_Implementation() {
//	return SetCameraView();
//}

//void APlayer_Cube::OnConstruction(const FTransform &Transform) {
//
//}

void APlayer_Cube::UpdateActionList(TMap<FString, FString> &OutMap) {
	//There is a bool to remove actions if the cube moved this action

	M_SlotsRefference.Empty();
	TMap <FString, FString> M_dummyList;

	TArray<AActor*> OutArray;
	GetAttachedActors(OutArray);

	if (Cast<ACube_TheBattleMasterPawn>(GetOwner())->SelectedItem)
	{
		for (auto& Elem : OutArray) {
			if (Cast<ACube_TheBattleMasterPawn>(GetOwner())->SelectedItem == Elem)
			{
				AItemBase* W_Elem = Cast<AItemBase>(Elem);
				M_SlotsRefference.Add(W_Elem->WeaponName, W_Elem);
				M_dummyList.Add(W_Elem->WeaponName, W_Elem->ActionName);
			}
		}
	}
	else {

		if (Cast<ACube_TheBattleMasterPawn>(GetOwner())->MoveNumb < 4) {
			M_dummyList.Add("Move", "Cube");
			M_dummyList.Add("Rotation Wise", "Cube");
			M_dummyList.Add("Rotation Counter", "Cube");
			M_dummyList.Add("Stop", "Cube");
		}

		M_dummyList.Add("No Action", "Cube");


		for (auto& Elem : OutArray)
		{
			if (Elem->IsA(AItemBase::StaticClass())) {
				AItemBase* W_Elem = Cast<AItemBase>(Elem);

				//W_Elem->DoActoin(false, FVector(0.f));
				//FString ProjectileName = W_Elem->SmallMunitionClass->GetName();

				//If movement has been selected, and it has moved, do not count items that cannot be used

				if (Cast<ACube_TheBattleMasterPawn>(GetOwner())->M_MovementList.Num() > 3)
				{
					if (Cast<ACube_TheBattleMasterPawn>(GetOwner())->M_MovementList.FindRef(3).MoveDirection.GetAbs() == FVector(0.f)
						&& !W_Elem->bCanBeUsedWithMovement)
					{

						FString W_Name = W_Elem->WeaponName;

						int32 i = 0;
						while (M_dummyList.Contains(W_Name) || i > 10)
						{
							i = i + 1;

							W_Name = W_Elem->WeaponName + FString::FromInt(i);
							//W_Elem->WeaponName = W_Name;
						}
						M_SlotsRefference.Add(W_Name, W_Elem);
						M_dummyList.Add(W_Name, W_Elem->ActionName);
					}
				}
				else {
					FString W_Name = W_Elem->WeaponName;

					int32 i = 0;
					while (M_dummyList.Contains(W_Name) || i > 10)
					{
						i = i + 1;

						W_Name = W_Elem->WeaponName + FString::FromInt(i);
						//W_Elem->WeaponName = W_Name;
					}
					M_SlotsRefference.Add(W_Name, W_Elem);
					M_dummyList.Add(W_Name, W_Elem->ActionName);
				}
			}
		}
	}
	OutMap = M_dummyList;
	//UE_LOG(LogTemp, Warning, TEXT("test!"));
}

void APlayer_Cube::InitiateMovementAndAction()
{
	if (GetLocalRole() == ROLE_Authority) { Server_InitiateMovementAndAction(); }
	
		MyPawn = Cast<ACube_TheBattleMasterPawn>(GetOwner());
		//MyPawn->M_ActionStructure[MyPawn->DoActionNumb].Item;

		



		bDoAction = true;
		bStartTimer = true;


		ReadyTheCube(false);

		FinalPosition = GetActorLocation() + Action.Movement;
		/*if (bMove) { DoTheMovement(); }
		if (bRotate) { DoTheRotation(); }*/
		//UE_LOG(LogTemp, Warning, TEXT("StartPosition %s"), *StartPosition.ToString());
	
}

void APlayer_Cube::Server_InitiateMovementAndAction_Implementation() { time = 0.f; bStartTimer = true; }

void APlayer_Cube::SetAction(AItemBase* Item, FVector Move, AActor* Actor, float Rot, FVector Vec) {

	if (GetLocalRole() < ROLE_Authority) { Server_SetAction(Item, Move, Actor, Rot, Vec); }
	else {//NOTE::4th action not moving or rotating?
		Action.Item = Item;
		Action.Movement = Move;
		Action.SelectedActor = Actor;
		Action.Rotation = Rot;
		Action.SupplementaryVec = Vec;

		if (Action.Rotation == 360.f || Action.Rotation == -360.f) { Action.Rotation = 0; }
		EndTimer = 6.f;

		bMove = Action.Movement != FVector(0.f);
		bRotate = Action.Rotation != 0;
		bDoAction = Item != NULL;
		time = 0;

		

		LocalRotation = GetActorRotation();
		LocalRotation.Yaw = roundf(GetActorRotation().Yaw * 0.1) * 10;
		if (LocalRotation.Yaw < 0) { LocalRotation.Yaw += 360.f; }

		if (bMove) { DoTheMovement(); }
		if (bRotate) { DoTheRotation(); }
		if (bDoAction) { DoActionAtTime = ActionTimer / (1 + Action.Item->ActionItteration); }

		if (Item) {
			UE_LOG(LogTemp, Warning, TEXT("Item %s, can be used with movement? %s"), *Item->GetName(), Item->bCanBeUsedWithMovement ? TEXT("Yes") : TEXT("No"));
		}
		UE_LOG(LogTemp, Warning, TEXT("Movement %s, move? %s"), *Move.ToString(), bMove ? TEXT("true") : TEXT("false"));
		if (Actor) { UE_LOG(LogTemp, Warning, TEXT("Actor %s"), *Actor->GetName()); }
		UE_LOG(LogTemp, Warning, TEXT("Rotation %f, move? %s"), Rot, bRotate ? TEXT("true") : TEXT("false"));
		UE_LOG(LogTemp, Warning, TEXT("Sup Vec %s"), *Action.SupplementaryVec.ToString());

		if (Item) {
			if (Item->bStartAction) { Item->StartAction(Action); }
		}
	}
}

void APlayer_Cube::Server_SetAction_Implementation(AItemBase* Item, FVector Move, AActor* Actor, float Rot, FVector Vec) {
	SetAction(Item, Move, Actor, Rot, Vec);
}

AActor* APlayer_Cube::FindANearByBlock() 
{


	TMap<AActor*, float > M_OverlappingActor;


	//return DummyKey[0];

	if (M_OverlappingActor.Num() == 0.f) {

		for (TObjectIterator<AActor> Elems; Elems; ++Elems) 
		{
			M_OverlappingActor.Add(*Elems, (Elems->GetActorLocation() - GetActorLocation()).GetAbsMax());
		}

			//OR
		//Find the last block that was touched
		//Return that block
	}
	else if (MoveComponent->Velocity.GetAbsMax() == 0) {

		TSet<AActor*> OverlappingActors;
		GetOverlappingActors(OverlappingActors);

		for (auto Elems : OverlappingActors) {
			float dummyFloat = ((MoveComponent->Velocity).GetSafeNormal()
				- (Elems->GetActorLocation() - GetActorLocation()).GetSafeNormal()
				).GetAbsMax();

			M_OverlappingActor.Add(Elems, dummyFloat);		
		}
	}
	else {
		TSet<AActor*> OverlappingActors;
		GetOverlappingActors(OverlappingActors);

		for (auto Elems : OverlappingActors) {
			M_OverlappingActor.Add(Elems, (Elems->GetActorLocation() - GetActorLocation()).GetAbsMax());
		}
	}
	
	M_OverlappingActor.ValueSort([](float A, float B) {	return A < B; });
	TArray<AActor*> DummyKey;
	M_OverlappingActor.GenerateKeyArray(DummyKey);

	SetActorLocation(DummyKey[0]->GetActorLocation());
	MoveComponent->Velocity = FVector(0.f);
	UE_LOG(LogTemp, Warning, TEXT("Go here %s"), *DummyKey[0]->GetActorLocation().ToString())
	return DummyKey[0];
}

void APlayer_Cube::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (GetLocalRole() == ROLE_Authority) {
		//UE_LOG(LogTemp, Warning, TEXT("Time %f"), time);
		//UE_LOG(LogTemp, Warning, TEXT("Player-> %s  and %d, time: %f, Move: %s, Action: %s, Ready: %s"), *GetOwner()->GetName(), GetLocalRole(), time, bMove ? TEXT("True") : TEXT("False"), bDoAction ? TEXT("True") : TEXT("False"), bReady ? TEXT("True") : TEXT("False"));
	
		if (E_TurnStateEnum == ETurnState::TS_InitiateActions) {
			if (MyPawn) {MyPawn->RefreshEnergy();}

			//Are you moving
			//Do you not want to be moving (bMove)
			//Decrease velocity and make sure we finish on a hex

			if (MoveComponent->Velocity.GetAbsMax() != 0) {
				//UE_LOG(LogTemp, Warning, TEXT("Player-> %s, role %d , time: %f, startlocation %s, CurrentLocation-> %s , FinalPosition: %s, Subtraction: %s, bool %s"), *GetOwner()->GetName(), GetLocalRole(), time, *StartPosition.ToString(), *GetActorLocation().ToString(), *FinalPosition.ToString(), *(GetActorLocation() - FinalPosition).ToString(), (GetActorLocation() - FinalPosition) == FVector(0.f) ? TEXT("True") : TEXT("False"));
				//UE_LOG(LogTemp, Warning, TEXT("Startlocation %s, CurrentLocation-> %s , FinalPosition: %s, Difference %s"), *StartPosition.ToString(), *GetActorLocation().ToString(), *FinalPosition.ToString(), *(GetActorLocation() - FinalPosition).ToString());
				if (!bMove) {
					UE_LOG(LogTemp, Warning, TEXT("PleaseStop!"));


					AActor* FoundBlock = FindANearByBlock();

					FVector dummySubtraction = FoundBlock->GetActorLocation() - GetActorLocation();
					FVector changeError = dummySubtraction - Vec_PreviousSubtraction;
					Vec_PreviousSubtraction = dummySubtraction;

					MoveComponent->Velocity += 0.5f * (dummySubtraction + changeError / DeltaSeconds);

					//UE_LOG(LogTemp, Warning, TEXT("Velocity %s"), *MoveComponent->Velocity.ToString());
					//UE_LOG(LogTemp, Warning, TEXT("Final location %s, CurrentLocation-> %s, subtraction %s"), *dummyPosition.ToString(), *GetActorLocation().ToString(), *(dummyPosition- GetActorLocation()).ToString());

					if (FMath::IsWithinInclusive((GetActorLocation() - FoundBlock->GetActorLocation()).GetAbsMax(), 0.f, 2.f) &&
						FMath::IsWithinInclusive(MoveComponent->Velocity.GetAbsMax(), 0.f, 2.f)) {
						MoveComponent->Velocity = FVector(0.f);
						SetActorLocation(FoundBlock->GetActorLocation());
						BlockOwner = Cast<ACube_TheBattleMasterBlock>(FoundBlock);
					}
					//What if no overlap?
					//Change search for velocity?
				}else{				
					UE_LOG(LogTemp, Warning, TEXT(" CurrentLocation-> %s , FinalPosition: %s"),  *GetActorLocation().ToString(), *FinalPosition.ToString());
				}
			}
			//Are you rotating?
			//Do you want to not be?
			//Start stopping (find a place to stop)

			if (RotMoveComponent->RotationRate != FRotator(0))
			{
				if (!bRotate)
				{
					//UE_LOG(LogTemp, Warning, TEXT("PleaseStop rotating!"));
					float dummySubtraction = -(GetActorRotation().Yaw - 30.f * FMath::RoundToFloat((GetActorRotation().Yaw) / 30.f));
					float changeError = dummySubtraction - PreviousSubtraction;
					PreviousSubtraction = dummySubtraction;

					RotMoveComponent->RotationRate.Yaw += 0.55f * (dummySubtraction + changeError / DeltaSeconds);;
					//UE_LOG(LogTemp, Warning, TEXT("Actor Rotation %f, subtraction %f, change %f, rotation %f"), GetActorRotation().Yaw, dummySubtraction, dummyChange, RotMoveComponent->RotationRate.Yaw);

					if (FMath::IsWithin(RotMoveComponent->RotationRate.Yaw, -1.f, 1.f) && FMath::IsWithin(dummySubtraction, -1.f, 1.f))
					{
						RotMoveComponent->RotationRate = FRotator(0);
						//SetActorRotation(FRotator(0.f, 10 * roundf(GetActorRotation().Yaw / 10.f), 0.f));
						
						if (Action.Movement !=FVector(0.f)) {
							DoTheMovement(); EndTimer += time; bMove = true;
							if (Action.Item && !Action.Item->bCanBeUsedWithMovement) { bDoAction = true; }
						}
						//UE_LOG(LogTemp, Warning, TEXT("Move? %s"), bMove ? TEXT("Yes") : TEXT("No"));
					}
				}
				else {
					if (Action.Item) { bDoAction = Action.Item->bCanBeUsedWithMovement;	}
					Server_SetCubeVelocity(FVector(0.f));

					float ActorRotation = GetActorRotation().Yaw;
					if (ActorRotation < 0) { ActorRotation += 360.f; }

					//if (StartRotation.Yaw + Action.Rotation <= 0) { StartRotation.Yaw += 360.f; }

					//UE_LOG(LogTemp, Warning, TEXT("StartRotation %f, list rotation %f, Goto %f, actor rotation %f, too zero %f"), StartRotation.Yaw, Action.Rotation, StartRotation.Yaw + Action.Rotation, ActorRotation, StartRotation.Yaw + Action.Rotation - ActorRotation);
					//UE_LOG(LogTemp, Warning, TEXT("Rotate? %s"), bRotate ? TEXT("True") : TEXT("False"));
					if ((RotMoveComponent->RotationRate.Yaw > 0 && LocalRotation.Yaw + Action.Rotation - ActorRotation < 0) ||
						(RotMoveComponent->RotationRate.Yaw < 0 && LocalRotation.Yaw + Action.Rotation - ActorRotation > 0))
					{
						Action.Rotation = 0; 
						bRotate = false;
						StartRotation = GetActorRotation();
					}
				}
			}

			if (time < 6.f && EndTimer < time) { bMove = false; bRotate = false; if (Action.Item) { Action.Item->EndAction(); } }

			if (bDoAction) {
				//UE_LOG(LogTemp, Warning, TEXT("Player-> %s , time: %f"), *GetOwner()->GetName(), time);

				if (Action.Item) {
					
					if (Action.Item->ActionItteration != 0) {
						if (roundf(time * 10) / 10 == DoActionAtTime + 0.5) {
							//UE_LOG(LogTemp, Warning, TEXT("Supp vec %s"), *Action.SupplementaryVec.ToString());
							//UE_LOG(LogTemp, Warning, TEXT("Attack happened, attack time %f, time %f"), DoActionAtTime, time);
							Attack(MyPawn, Action.Item, Action.SupplementaryVec);
							//DoTheAction();

							DoActionAtTime = DoActionAtTime * 2;
							if (DoActionAtTime >= ActionTimer) { bDoAction = false;/* EndAction(MyPawn->M_ActionStructure[MyPawn->DoActionNumb].Item);*/ }
						}
					}
					else {
						//DoTheAction();
						//UE_LOG(LogTemp, Warning, TEXT("Supp vec %s"), *Action.SupplementaryVec.ToString());
						Attack(MyPawn,Action.Item,Action.SupplementaryVec);
						bDoAction = false;
					}
				}
				else { bDoAction = false; }
			}

			if (ActionTimer <= time) {
				if (Action.Item) {
					if (Action.Item->bEndAction) {
						EndAction(Action.Item);
					}
				}

			}
			if (6 <= time) {
				if (bRotate) { bRotate = false; }
				if (bMove) { bMove = false; }
			}
		}
		if (6 <= time) {
			ReadyTheCube(true);
			time = 0;
			bStartTimer = false;
		}
		time += DeltaSeconds;
	}
}


void APlayer_Cube::OnRep_ChangeEnergy() {
	if (MyPawn) { MyPawn->RefreshEnergy(); }
}

void APlayer_Cube::DoTheRotation()
{
	if (GetLocalRole() < ROLE_Authority) {Server_DoTheRotation();}
	else {
		if (Action.Item) {
			if (Action.Rotation > 180.f) { Action.Rotation -= 360.f; }
		}
		float Rotspeed = Replicated_RotationSpeed * 160.f;
		if (Action.Rotation == 0.f) {bRotate = false;}
		else {
			if (Action.Rotation > 0) {
				RotMoveComponent->RotationRate = FRotator(0.f, Rotspeed, 0.f);
			}
			else {
				RotMoveComponent->RotationRate = FRotator(0.f, -Rotspeed, 0.f);
			}
		}

		
		//bRotate = true;
		EndTimer =  FMath::Abs(Action.Rotation)/ Rotspeed;
		UE_LOG(LogTemp, Warning, TEXT("EndTimer Rotation %f"), EndTimer);
		bMove = false;
	}
}

void APlayer_Cube::Server_DoTheRotation_Implementation() {	DoTheRotation(); }

void APlayer_Cube::DoTheMovement(){
	if (GetLocalRole() < ROLE_Authority) { Server_DoTheMovement(); }
	else { 
		float Speed = Replicated_Speed;
		if (Action.Item)
		{
			if (Action.Item->IsA(ABooster_Basic::StaticClass())) {
				ABooster_Basic* BoosterItem = Cast<ABooster_Basic>(Action.Item);
				Speed = BoosterItem->AdditionalSpeed*5;
			}
		}
		if ( Action.Movement != FVector(0.f) ) {
			float spaces = FMath::RoundFromZero(10*Action.Movement.GetAbsMax()/60.f)/10.f;
			UE_LOG(LogTemp, Warning, TEXT("Spaces %f"), spaces)
			EndTimer = spaces;
			bMove = true;
			SetCubeVelocity( Action.Movement/(Action.Movement).GetAbsMax() * Speed *2.0f);
		}
	}
}

void APlayer_Cube::Server_DoTheMovement_Implementation() { DoTheMovement(); }

void APlayer_Cube::SetCubeVelocity(FVector Vel) {
	if (Vel == FVector(0.f)) { MoveComponent->StopMovementImmediately(); }
	MoveComponent->Velocity = Vel;
}

void APlayer_Cube::Server_SetCubeVelocity_Implementation(FVector Vel) { SetCubeVelocity(Vel); }




void APlayer_Cube::DoTheAction() {
	if (GetLocalRole() == ROLE_Authority){
	FVector AttackVector = Action.SupplementaryVec;
	if (Action.SelectedActor) {
		if (Action.Item->IsInRange(Action.SelectedActor)) {
			FVector End = Action.SelectedActor->GetActorLocation();
			End.Z = 100.f;
				//Attack(MyPawn, Action.Item, End);
				AttackVector = End;
				//Action.Item->DoAction(true, End);
				UE_LOG(LogTemp, Warning, TEXT("Attack actor %s"), *Action.SelectedActor->GetName());
			}
		}
		else {

			UE_LOG(LogTemp, Warning, TEXT("Attack in the direction %s"), *AttackVector.ToString());
			//Attack(MyPawn, Action.Item,	);
		}
		Action.Item->DoAction(true, AttackVector); }
}

void APlayer_Cube::ApplyDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{
	//float Damage = BaseDamage;
	//if (E_TurnStateEnum == ETurnState::TS_SelectActions) { /*Do Nothing!!*/ }
	//else if (E_TurnStateEnum == ETurnState::TS_InitiateActions) {
		//UE_LOG(LogTemp, Warning, TEXT("%s has %f damage"), *GetName(), Damage);



		if (Damage > 0 && GetLocalRole() == ROLE_Authority)
		{
			Replicated_Health = FMath::Clamp(Replicated_Health - Damage, 0.0f, Base_Health);
			//UE_LOG(LogTemp, Warning, TEXT("test"));
			bNeedsDamage = false;
		}
		UE_LOG(LogTemp, Warning, TEXT("%s has %f health"), *GetName(), Replicated_Health);


	//}
	//UE_LOG(LogTemp, Warning, TEXT("DAMAGE!!"));

}

void APlayer_Cube::ApplyRadialEffects(AActor* DamagedActor, float DamageReceived, const class UDamageType* DamageType, FVector Origin, FHitResult HitInfo, class AController* InstigatedBy, AActor* DamageCauser)
{

	UE_LOG(LogTemp, Warning, TEXT("Radial effects"));

	FVector Vel = -HitInfo.ImpactNormal * 100.f;
	MoveComponent->Velocity += Vel;
	if (EndTimer < time || !bMove) {
		EndTimer = time + 0.5;
		bMove = true;
	}
	//ApplyDamage(DamagedActor, DamageReceived, DamageType, InstigatedBy, DamageCauser);

}

void APlayer_Cube::SetOwningPawn(ACube_TheBattleMasterPawn* NewOwner)
{
	if (MyPawn != NewOwner)
	{
		
		//Instigator = NewOwner;
		//MyPawn = NewOwner;
		// [Net Owner] for RPC Calls
		SetOwner(NewOwner);
	}
}



void APlayer_Cube::Attack(ACube_TheBattleMasterPawn* Pawn, AItemBase* Item, FVector AttackPosition) {
	UE_LOG(LogTemp, Warning, TEXT("Attack TESTSETSETS"));

	bool bMainPhase;
		
	if (E_TurnStateEnum == ETurnState::TS_InitiateActions) { bMainPhase = true; }
	else { bMainPhase = false; };

	if (E_TurnStateEnum == ETurnState::TS_InitiateActions && GetLocalRole() < ROLE_Authority) { Server_Attack(Pawn, Item, AttackPosition); }
	else { 
		Item->DoAction(bMainPhase, AttackPosition);
		//UE_LOG(LogTemp, Warning, TEXT("Attack pos %s"), *AttackPosition.ToString());
	}
}

bool APlayer_Cube::Server_Attack_Validate(ACube_TheBattleMasterPawn* Pawn, AItemBase* Item, FVector AttackPosition) { return true; }

void APlayer_Cube::Server_Attack_Implementation(ACube_TheBattleMasterPawn* Pawn, AItemBase* Item, FVector AttackPosition) { Attack(Pawn, Item, AttackPosition); }

void APlayer_Cube::EndAction(AItemBase* Item) {
	if (E_TurnStateEnum == ETurnState::TS_InitiateActions && GetLocalRole() < ROLE_Authority) { Server_EndAction(Item); }
	else {
		Item->EndAction();
		
		SetAction(NULL, FVector(0.f), NULL, 0.f, FVector(0.f));
	}
}

void APlayer_Cube::Server_EndAction_Implementation(AItemBase* Item) { EndAction(Item); }


void APlayer_Cube::OnRep_ChangeColour() {
	BlockMesh->CreateDynamicMaterialInstance(0, VariedCubeMaterial)->SetVectorParameterValue("BaseColor", CubeColour);
	UE_LOG(LogTemp, Warning, TEXT("Colour change!"));
	
}

void APlayer_Cube::SetCubeDetails(FImportedCube_Struct SCube, TMap<FName, TSubclassOf<AItemBase>> AttachedItems)
{
	if (SCube.Name != "") {
		ThisCubeName = SCube.Name;
		Base_Health = SCube.BaseHealth;
		Base_Armour = SCube.BaseArmour;
		Base_Speed = SCube.BaseSpeed;
		GetBlockMesh()->CreateDynamicMaterialInstance(0, VariedCubeMaterial)->SetVectorParameterValue("BaseColor", SCube.Colours);
		CubeColour = SCube.Colours;
	}

	for (auto Elems : AttachedItems)
	{
		TSubclassOf<AItemBase> ItemClass = Elems.Value;
		if (ItemClass)
		{
			FActorSpawnParameters params;
			AItemBase* Item = GetWorld()->SpawnActor<AItemBase>(ItemClass, FVector(0.f), FRotator(0.f), params);
			Item->Cube = this;
			Item->SetOwner(GetOwner());

			GetAttachComponent()->Equip(Item, Elems.Key);
		}
	}

}


void APlayer_Cube::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(APlayer_Cube, Replicated_Health);

	DOREPLIFETIME(APlayer_Cube, Replicated_Speed);

	DOREPLIFETIME(APlayer_Cube, Replicated_RotationSpeed);
	
	DOREPLIFETIME(APlayer_Cube, SmallMunitionClass);

	DOREPLIFETIME(APlayer_Cube, CubeColour);

	DOREPLIFETIME(APlayer_Cube, BlockOwner);

	DOREPLIFETIME(APlayer_Cube, StartPosition);
	
	DOREPLIFETIME(APlayer_Cube, StartRotation);

	DOREPLIFETIME(APlayer_Cube, bMove);

	DOREPLIFETIME(APlayer_Cube, bRotate);

	DOREPLIFETIME(APlayer_Cube, bReady);

	DOREPLIFETIME(APlayer_Cube, time);

	DOREPLIFETIME(APlayer_Cube, Replicated_BasicEnergy);

	DOREPLIFETIME(APlayer_Cube, Action);
}