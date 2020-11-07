// Fill out your copyright notice in the Description page of Project Settings.


#include "Wall_Actor.h"

// Sets default values
AWall_Actor::AWall_Actor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	//SetReplicatingMovement(false);

	//bReplicates = true;

	//bNetUseOwnerRelevancy = true;

	//bAlwaysRelevant = true;

	// Structure to hold one-time initialization
	struct FConstructorStatics
	{
		ConstructorHelpers::FObjectFinderOptional<UStaticMesh> PlaneMesh;
		ConstructorHelpers::FObjectFinderOptional<UMaterial> BaseMaterial;
		FConstructorStatics()
			: PlaneMesh(TEXT("/Game/Puzzle/Meshes/TemplateWall.TemplateWall"))
			, BaseMaterial(TEXT("/Game/Puzzle/Meshes/BaseMaterial.BaseMaterial"))
		{
		}
	};
	static FConstructorStatics ConstructorStatics;

	// Create dummy root scene component
	DummyRoot = CreateDefaultSubobject<USceneComponent>(TEXT("Dummy0"));
	//DummyRoot->SetIsReplicated(true);
	RootComponent = DummyRoot;


	// Create static mesh component
	BlockMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BlockMesh0"));
	//BlockMesh->SetIsReplicated(true);
	BlockMesh->SetStaticMesh(ConstructorStatics.PlaneMesh.Get());
	BlockMesh->SetRelativeScale3D(FVector(0.055f, 0.05f, 0.15f));
	BlockMesh->SetRelativeRotation(FRotator(0.f, 90.f, 0.f));
	//BlockMesh->SetAbsoluteScale3D(FVector(0.15, 0.1, 0.4));
	BlockMesh->SetRelativeLocation(FVector(0.f, -40.f, 0.f));
	BlockMesh->SetMaterial(0, ConstructorStatics.BaseMaterial.Get());
	BlockMesh->SetupAttachment(DummyRoot);



	// Save a pointer to the orange material
	BaseMaterial = ConstructorStatics.BaseMaterial.Get();


	//OnTakeAnyDamage.AddDynamic(this, &AWall_Actor::ApplyDamage);


	//Extra variables
	Base_Health = 100.0f;
	//Replicated_Health = Base_Health;

	//Extra (none nessessary) variables	

}

// Called when the game starts or when spawned
//void AWall_Actor::BeginPlay()
//{
//	Super::BeginPlay();
//	
//}

// Called every frame
//void AWall_Actor::Tick(float DeltaTime)
//{
//	Super::Tick(DeltaTime);
//
//}

void AWall_Actor::AttachToSelect(USceneComponent * Parent,
	const FAttachmentTransformRules & AttachmentRules,
	FName SocketName) {
	AttachToComponent(Parent, AttachmentRules, SocketName);

}

void AWall_Actor::ApplyDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{

}

