// Fill out your copyright notice in the Description page of Project Settings.

#include "SmallMunition.h"
#include "Player_Cube.h"
#include "UObject/ConstructorHelpers.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Net/UnrealNetwork.h"
#include "Materials/MaterialInstance.h"
#include "Kismet/GameplayStatics.h"


// Sets default values
ASmallMunition::ASmallMunition()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	

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
	BlockMesh->SetRelativeScale3D(FVector(0.15f));
	BlockMesh->SetRelativeLocation(FVector(0.f, 0.f, 125.f));
	BlockMesh->SetMaterial(0, ConstructorStatics.BaseMaterial.Get());
	BlockMesh->SetupAttachment(DummyRoot);

	// declare trigger capsule
	TriggerCapsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Trigger Capsule"));
	TriggerCapsule->InitCapsuleSize(0.15f, 0.15f);;
	TriggerCapsule->SetRelativeLocation(FVector(0.f, 0.f, 125.f));
	TriggerCapsule->SetCollisionProfileName(TEXT("Trigger"));
	TriggerCapsule->SetupAttachment(RootComponent);

	TriggerCapsule->OnComponentBeginOverlap.AddDynamic(this, &ASmallMunition::OnOverlapBegin);
//	OnActorBeginOverlap.AddDynamic(this, &ASmallMunition::BeginOverlap);

	Speed = 1000.0f;
}

// Called when the actor is spawned 
void ASmallMunition::OnConstruction(const FTransform & Transform)
{
	Super::OnConstruction(Transform);



//	BlockMesh->OnComponentHit.AddDynamic(this, &ASmallMunition::OnHit);

//	UE_LOG(LogTemp, Warning, TEXT("construction "));

	
}

// Called every frame
void ASmallMunition::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (true)
	{
		Position = FMath::VInterpConstantTo(GetActorLocation(), Direction * 10000.f, DeltaTime, Speed);
	}
	else { Position = GetActorLocation(); }
	SetActorLocation(Position);
}

//void ASmallMunition::BeginOverlap( AActor* OverlappedActor, AActor* OtherActor )
//{
//	UE_LOG(LogTemp, Warning, TEXT("Testing : Actor1 %s and Actor2 %s"), *OverlappedActor->GetName(), *OtherActor->GetName());
//	Destroy();
//}

//void ASmallMunition::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) {
//
//	AActor* MyOwner = GetOwner();
//	UE_LOG(LogTemp, Warning, TEXT("Testing "));
//
//	if (!MyOwner) { return; }
//
//	if (OtherActor && OtherActor != this && OtherActor != MyOwner) {
//		//AActor* DamagedActor, float BaseDamage, AController* EventInstigator, AActor* DamageCauser, TSubclassOf<class UDamageType> DamageTypeClass);
//		//UGameplayStatics::ApplyDamage(OtherActor, Damage, MyOwner->GetInstigatorController(), this, DamageType);
//		UGameplayStatics::ApplyDamage(OtherActor, 10.0f, MyOwner->GetInstigatorController(), this, nullptr);
//
//	}
//
//
//	Destroy();
//}

void ASmallMunition::OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//Owner is a nullptr when overlaping with the actual owner
	AActor* MyOwner = GetOwner();

	
	//Must have an owner, an interactive actor (that does not share the same actor, this is redundent right now), not itslef, and has to have a mesh to interact with i.e.BlockMesh0
	if (MyOwner && OtherActor && (OtherActor != GetOwner()) && (OtherActor != this) && OtherComp)
	{
		
		if (OtherActor->IsA(APlayer_Cube::StaticClass())){
			UE_LOG(LogTemp, Warning, TEXT("OnOverlap : Component1 %s, Actor2 %s, Component3 %s, int4 %d, bool5 %s"), *OverlappedComp->GetName(), *OtherActor->GetName(), *OtherComp->GetName(), OtherBodyIndex, bFromSweep ? TEXT("true") : TEXT("false"));
			//AActor* DamagedActor, float BaseDamage, AController* EventInstigator, AActor* DamageCauser, TSubclassOf<class UDamageType> DamageTypeClass);
	//		//UGameplayStatics::ApplyDamage(OtherActor, Damage, MyOwner->GetInstigatorController(), this, DamageType);
			UGameplayStatics::ApplyDamage(OtherActor, 10.0f, MyOwner->GetInstigatorController(), this, nullptr);

			Destroy();
		}
	}
}

void ASmallMunition::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ASmallMunition, Direction);

}