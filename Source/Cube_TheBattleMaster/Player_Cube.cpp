// Fill out your copyright notice in the Description page of Project Settings.
#include "Player_Cube.h"
#include "Cube_TheBattleMasterPawn.h"
#include "UObject/ConstructorHelpers.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Net/UnrealNetwork.h"
#include "Materials/MaterialInstance.h"
#include "GameFramework/GameStateBase.h"

// Sets default values
APlayer_Cube::APlayer_Cube()
{
	//SetReplicatingMovement(true);
	SetReplicatingMovement(false);

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
		FConstructorStatics()
			: PlaneMesh(TEXT("/Game/Puzzle/Meshes/PuzzleCube.PuzzleCube"))
			, BaseMaterial(TEXT("/Game/Puzzle/Meshes/BaseMaterial.BaseMaterial"))
		{
		}
	};
	static FConstructorStatics ConstructorStatics;

	// Create dummy root scene component
	DummyRoot = CreateDefaultSubobject<USceneComponent>(TEXT("Dummy0"));
	//DummyRoot->SetIsReplicated(true);
	RootComponent = DummyRoot;
	

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
	BlockMesh->SetRelativeLocation(FVector(0.f, 0.f, 100.f));
	BlockMesh->SetMaterial(0, ConstructorStatics.BaseMaterial.Get());
	BlockMesh->SetupAttachment(DummyRoot);
	


	// Save a pointer to the orange material
	BaseMaterial = ConstructorStatics.BaseMaterial.Get();

	//Create camera components
	OurCameraSpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraSpringArm"));
	OurCameraSpringArm->SetupAttachment(DummyRoot);
	OurCameraSpringArm->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, 1800.0f), FRotator(-90.0f, 0.0f, 0.0f));
	OurCameraSpringArm->TargetArmLength = 400.f;
	OurCameraSpringArm->bEnableCameraLag = true;
	OurCameraSpringArm->CameraLagSpeed = 3.0f;

	OurCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("GameCamera"));
	OurCamera->SetupAttachment(OurCameraSpringArm, USpringArmComponent::SocketName);
	//OurCamera->SetIsReplicated(true);

	OnTakeAnyDamage.AddDynamic(this, &APlayer_Cube::ApplyDamage);

	
	//DefineSlots
	//M_SlotsRefference.Add(FString("S_Slot1"), NULL);
	//M_SlotsRefference.Add(FString("S_Slot2"), NULL);

	

	//Extra variables
	Base_Health = 100.0f;
	Replicated_Health = Base_Health;
	Replicated_Speed = Base_Speed;

	//Extra (none nessessary) variables	
	bMove = false;
}

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

	//if (GetLocalRole() < ROLE_Authority){
	//	BlockMesh->SetSimulatePhysics(true);
	//}
	//else { BlockMesh->SetSimulatePhysics(false); }
	MyPawn = Cast<ACube_TheBattleMasterPawn>(GetOwner());
	UE_LOG(LogTemp, Warning, TEXT("%f health left"), Base_Health);
	CurrentPosition = FVector(0.f);
	//MyPawn->ClearVars();
}

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
					if (Cast<ACube_TheBattleMasterPawn>(GetOwner())->M_MovementList.FindRef(3).GetAbs() == FVector(0.f)
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
}

void APlayer_Cube::InitiateMovementAndAction()
{
	MyPawn = Cast<ACube_TheBattleMasterPawn>(GetOwner());
	MyPawn->M_ActionStructure[MyPawn->DoActionNumb].Item;

	if (MyPawn->M_ActionStructure[MyPawn->DoActionNumb].Item) {
		DoActionAtTime = ActionTimer / (1 + MyPawn->M_ActionStructure[MyPawn->DoActionNumb].Item->ActionItteration);
	}
	
	time = 0.f;
	bReady = false;
}

void APlayer_Cube::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
	//if (GetLocalRole() != ROLE_Authority) {
		//UE_LOG(LogTemp, Warning, TEXT("Player-> %s  and %d, time: %f, Move: %s, Action: %s, Ready: %s"), *GetOwner()->GetName(), GetLocalRole(), time, bMove ? TEXT("True") : TEXT("False"), bDoAction ? TEXT("True") : TEXT("False"), bReady ? TEXT("True") : TEXT("False"));
	//}
	
	
	if (bMove) {
		UE_LOG(LogTemp, Warning, TEXT("Player-> %s , time: %f, CurrentLocation-> %s , FinalPosition: %s"), *GetOwner()->GetName(), time, *GetActorLocation().ToString(), *FinalPosition.ToString());
	

		if (BlockOwner) { FinalPosition = BlockOwner->GetActorLocation(); }
		else { FinalPosition = GetActorLocation(); }

		if (E_TurnStateEnum == ETurnState::TS_InitiateActions) {
			Movement(FMath::VInterpConstantTo(GetActorLocation(), FinalPosition, DeltaSeconds, FVector::Distance(StartPosition, FinalPosition) / ActionTimer));
		}
		else if (E_TurnStateEnum == ETurnState::TS_SelectActions) { Movement(FinalPosition); }

		if (FinalPosition == GetActorLocation()) { bMove = false;  }
	}
	
	if (bDoAction) {
		if (E_TurnStateEnum == ETurnState::TS_InitiateActions) {
			//UE_LOG(LogTemp, Warning, TEXT("Player-> %s , time: %f"), *GetOwner()->GetName(), time);

			if (MyPawn->M_ActionStructure[MyPawn->DoActionNumb].Item->ActionItteration != 0) {
				if (roundf(time*10)/10  == DoActionAtTime + 0.5) {
					Attack(MyPawn, MyPawn->M_ActionStructure[MyPawn->DoActionNumb].Item,
						MyPawn->M_ActionStructure[MyPawn->DoActionNumb].Action_Position + this->GetActorLocation());

					DoActionAtTime = DoActionAtTime * 2;
					if (DoActionAtTime >= ActionTimer) { bDoAction = false;/* EndAction(MyPawn->M_ActionStructure[MyPawn->DoActionNumb].Item);*/}
				}
			}
			else {
				Attack(MyPawn, MyPawn->M_ActionStructure[MyPawn->DoActionNumb].Item,
					MyPawn->M_ActionStructure[MyPawn->DoActionNumb].Action_Position + this->GetActorLocation());
				bDoAction = false;
			}
		}
		else if (E_TurnStateEnum == ETurnState::TS_SelectActions) {
			Attack(MyPawn, MyPawn->M_ActionStructure[MyPawn->DoActionNumb].Item,
				MyPawn->M_ActionStructure[MyPawn->DoActionNumb].Action_Position + this->GetActorLocation());
			bDoAction = false;

		}
	}
	
	if (E_TurnStateEnum == ETurnState::TS_InitiateActions && ActionTimer <= time) {
		if (MyPawn) {
			if (MyPawn->M_ActionStructure[MyPawn->DoActionNumb].Item) {
				if (MyPawn->M_ActionStructure[MyPawn->DoActionNumb].Item->bEndAction) {
					EndAction(MyPawn->M_ActionStructure[MyPawn->DoActionNumb].Item);
				}
			}
		}
		if (!bMove && !bDoAction) { bReady = true; }
		//if (FinalPosition != GetActorLocation()) { SetActorLocation(FinalPosition); }
		if (bReady) { time = 0.f;  }
	}
	time += DeltaSeconds; 
}

void APlayer_Cube::ApplyDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{
	//float Damage = BaseDamage;
	if (E_TurnStateEnum == ETurnState::TS_SelectActions) { /*Do Nothing!!*/ }
	else if (E_TurnStateEnum == ETurnState::TS_InitiateActions) {
		if (Damage > 0 && GetLocalRole() < ROLE_Authority)
		{
			Replicated_Health = FMath::Clamp(Replicated_Health - Damage, 0.0f, Base_Health);
		}
		UE_LOG(LogTemp, Warning, TEXT("%s has %f health"), *GetName(), Replicated_Health);
	}
	UE_LOG(LogTemp, Warning, TEXT("DAMAGE!!"));

}


void APlayer_Cube::SetOwningPawn(ACube_TheBattleMasterPawn* NewOwner)
{
	if (MyPawn != NewOwner)
	{
		/*
		Instigator = NewOwner;
		MyPawn = NewOwner;*/
		// [Net Owner] for RPC Calls
		SetOwner(NewOwner);
	}
}

void APlayer_Cube::Movement(FVector MovePosition) {
	//UE_LOG(LogTemp, Warning, TEXT("MOVEMENT!!! for player %s"), *GetOwner()->GetName());
	if (E_TurnStateEnum == ETurnState::TS_InitiateActions && GetLocalRole() < ROLE_Authority) { Server_Movement(MovePosition); /* SetActorLocation(MovePosition);*/}
	else { SetActorLocation(MovePosition, true); /*UE_LOG(LogTemp, Warning, TEXT("MOVE! %s goes %s"), *GetName(), *MovePosition.ToString()); */ }
}

bool APlayer_Cube::Server_Movement_Validate(FVector MovePosition) {	return true; }

void APlayer_Cube::Server_Movement_Implementation(FVector MovePosition) { Movement(MovePosition); }

void APlayer_Cube::Attack(ACube_TheBattleMasterPawn* Pawn, AItemBase* Item, FVector BlockPosition) {
	/*bool AttackRep = false;
	if (E_TurnStateEnum == ETurnState::TS_InitiateActions) { AttackRep = true; }*/
	if (E_TurnStateEnum == ETurnState::TS_InitiateActions && GetLocalRole() < ROLE_Authority) { Server_Attack(Pawn, Item, BlockPosition); }
	else
	{
		Item->DoAction(true, BlockPosition);
	}
}

bool APlayer_Cube::Server_Attack_Validate(ACube_TheBattleMasterPawn* Pawn, AItemBase* Item, FVector BlockPosition) { return true; }

void APlayer_Cube::Server_Attack_Implementation(ACube_TheBattleMasterPawn* Pawn, AItemBase* Item, FVector BlockPosition) { Attack(Pawn, Item, BlockPosition); }

void APlayer_Cube::EndAction(AItemBase* Item) {
	UE_LOG(LogTemp, Warning, TEXT("TESTSETSETS"));
	Item->EndAction(); 
	//if (GetLocalRole() < ROLE_Authority) { Item->EndAction(); }
}

void APlayer_Cube::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(APlayer_Cube, Replicated_Health);

	DOREPLIFETIME(APlayer_Cube, Replicated_Speed);
	
	//DOREPLIFETIME(APlayer_Cube, MyPawn);
	//DOREPLIFETIME(APlayer_Cube, bDoAction);

	//DOREPLIFETIME(APlayer_Cube, bMove);

	//DOREPLIFETIME(APlayer_Cube, bReady);
}