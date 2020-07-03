// Fill out your copyright notice in the Description page of Project Settings.
#include "Player_Cube.h"
#include "UObject/ConstructorHelpers.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Net/UnrealNetwork.h"
#include "Materials/MaterialInstance.h"

// Sets default values
APlayer_Cube::APlayer_Cube()
{
	SetReplicatingMovement(true);
	//bReplicateMovement = true;
	bReplicates = true;
	
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

	//Extra variables
	Base_Health = 100.0f;
	Replicated_Health = Base_Health;

	//Extra (none nessessary) variables	
}

void APlayer_Cube::BeginPlay() {

	Super::BeginPlay();

	//OnTakeAnyDamage;

	//OnTakeAnyDamage.AddDynamic(this, &APlayer_Cube::OnTakeAnyDamage);

	/*AController* ThisPlayerContoler = Cast<AController>(this->GetOwner());
	ApplyDamage(this, 10, ThisPlayerContoler, this, FDamageEvent());*/
	
}

void APlayer_Cube::ApplyDamage(APlayer_Cube* DamagedActor, float BaseDamage, APlayer_Cube* DamageCauser)
{
	float Damage = BaseDamage;
	if (Damage > 0)
	{
		DamagedActor->Replicated_Health = FMath::Clamp(DamagedActor->Replicated_Health - Damage, 0.0f, DamagedActor->Base_Health);
	}
}


void APlayer_Cube::Movement(FVector MovePosition) {
	if (GetLocalRole() < ROLE_Authority) { Server_Movement(MovePosition); }
	SetActorLocation(MovePosition);

	/*ApplyDamage(this, 10, this);
	UE_LOG(LogTemp, Warning, TEXT("Testing Damage. Helth is %f"), Replicated_Health);*/
}

bool APlayer_Cube::Server_Movement_Validate(FVector MovePosition) {	return true; }

void APlayer_Cube::Server_Movement_Implementation(FVector MovePosition) { Movement(MovePosition); }


void APlayer_Cube::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(APlayer_Cube, Replicated_Health);

	DOREPLIFETIME(APlayer_Cube, Replicated_Speed);

}