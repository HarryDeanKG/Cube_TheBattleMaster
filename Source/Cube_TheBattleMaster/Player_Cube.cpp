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
	RootComponent = DummyRoot;

	// Create static mesh component
	BlockMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BlockMesh0"));
	BlockMesh->SetStaticMesh(ConstructorStatics.PlaneMesh.Get());
	BlockMesh->SetRelativeScale3D(FVector(0.25f, 0.25f, 0.25f));
	BlockMesh->SetRelativeLocation(FVector(0.f, 0.f, 125.f));
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

	OnTakeAnyDamage.AddDynamic(this, &APlayer_Cube::ApplyDamage);

	
	//Extra variables
	Base_Health = 100.0f;
	Replicated_Health = Base_Health;

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

	UE_LOG(LogTemp, Warning, TEXT("%f health left"), Base_Health);
	CurrentPosition = FVector(0.f);
}



void APlayer_Cube::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
	if (bMove) {
		/*if (time != 0.0f) { UE_LOG(LogTemp, Warning, TEXT("%s: %f"), *GetName(), GetWorld()->TimeSince(time)); }
		time = GetWorld()->GetGameState()->GetServerWorldTimeSeconds();*/

		if (BlockOwner) { FinalPosition = BlockOwner->GetActorLocation(); }
		else { FinalPosition = GetActorLocation(); }

		if (E_TurnStateEnum == ETurnState::TS_InitiateActions) {
			Movement(FMath::VInterpConstantTo(GetActorLocation(), FinalPosition, DeltaSeconds, FVector::Distance(StartPosition, FinalPosition) / ActionTimer));
		}
		else if (E_TurnStateEnum == ETurnState::TS_SelectActions) { Movement(FinalPosition); }

		if (FinalPosition == GetActorLocation()) { bMove = false; UE_LOG(LogTemp, Warning, TEXT("%s distance is done!"), *GetName()); }
	}
	if (bDoAction) {
		time += DeltaSeconds; 
		//UE_LOG(LogTemp, Warning, TEXT("%s time is %f and do action is %s"), *GetName(), time, bDoAction ? TEXT("True") : TEXT("False"));
		if (ActionTimer <= time) { bDoAction = false; time = 0.f; }
	}
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
		Instigator = NewOwner;
		MyPawn = NewOwner;
		// [Net Owner] for RPC Calls
		SetOwner(NewOwner);
	}
}

void APlayer_Cube::Movement(FVector MovePosition) {
	if (E_TurnStateEnum == ETurnState::TS_InitiateActions && GetLocalRole() < ROLE_Authority) { Server_Movement(MovePosition); SetActorLocation(MovePosition);}
	else { SetActorLocation(MovePosition); /*UE_LOG(LogTemp, Warning, TEXT("MOVE! %s goes %s"), *GetName(), *MovePosition.ToString()); */ }
}

//bool APlayer_Cube::Server_Movement_Validate(FVector MovePosition) {	return true; }

void APlayer_Cube::Server_Movement_Implementation(FVector MovePosition) { Movement(MovePosition); }



void APlayer_Cube::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(APlayer_Cube, Replicated_Health);

	DOREPLIFETIME(APlayer_Cube, Replicated_Speed);

}