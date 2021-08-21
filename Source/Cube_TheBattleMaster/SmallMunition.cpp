// Fill out your copyright notice in the Description page of Project Settings.

#include "SmallMunition.h"
#include "Player_Cube.h"
#include "Cube_TheBattleMasterBlockGrid.h"
#include "Cube_TheBattleMasterPawn.h"
#include "Cube_TheBattleMasterPlayerController.h"
#include "UObject/ConstructorHelpers.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Net/UnrealNetwork.h"
#include "Materials/MaterialInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"

#include "Engine/DecalActor.h"
#include "Components/DecalComponent.h"

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
			: PlaneMesh(TEXT("/Game/Puzzle/Meshes/Basic_Sphere.Basic_Sphere"))
			, BaseMaterial(TEXT("/Game/Puzzle/Meshes/BaseMaterial.BaseMaterial"))
		{
		}
	};
	static FConstructorStatics ConstructorStatics;

	/* Create dummy root scene component */
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Dummy0"));
	//RootComponent = DummyRoot;

	CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent0"));
	CollisionComponent->InitSphereRadius(15.f);
	RootComponent = CollisionComponent;
	/* Create static mesh component */
	BlockMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BlockMesh0"));
	BlockMesh->SetStaticMesh(ConstructorStatics.PlaneMesh.Get());
	BlockMesh->SetRelativeScale3D(FVector(0.15f));
	BlockMesh->SetRelativeLocation(FVector(0.f));
	BlockMesh->SetMaterial(0, ConstructorStatics.BaseMaterial.Get());
	BlockMesh->SetupAttachment(RootComponent);

	/* Use this component to drive this projectile's movement.*/
	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
	ProjectileMovementComponent->SetUpdatedComponent(CollisionComponent);
	ProjectileMovementComponent->InitialSpeed = 618.0f;
	//ProjectileMovementComponent->MaxSpeed = 600.0f;
	ProjectileMovementComponent->bRotationFollowsVelocity = true;
	//ProjectileMovementComponent->bShouldBounce = true;
	//ProjectileMovementComponent->Bounciness = 0.3f;
	//ProjectileMovementComponent->ProjectileGravityScale = 0.0f;

	CollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &ASmallMunition::OnOverlapBegin);
	
	Speed = 618.0f;
	
	/*Explosion (What to do when it hits something)*/
	static ConstructorHelpers::FObjectFinder<UParticleSystem> PSClass(TEXT("/Game/FXVarietyPack/Particles/P_ky_explosion.P_ky_explosion"));
	//static ConstructorHelpers::FObjectFinder<UParticleSystem> PSClass(TEXT("/Game/FXVarietyPack/Particles/P_ky_fireBall.P_ky_fireBall"));
	if (PSClass.Object != NULL) {
		EmitterTemplate = PSClass.Object;
	}
	/*Decal*/
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> RedCircleDecal_Pointer(TEXT("/Game/BP_Classes/Weapons/RedCircle_Decal.RedCircle_Decal"));
	RedCircleDecal = RedCircleDecal_Pointer.Object;
}

// Called when the actor is spawned 
void ASmallMunition::OnConstruction(const FTransform & Transform)
{
	Super::OnConstruction(Transform);

	//Set the inital conditions 
	StartPosition = GetActorLocation();
	StartTime = GetWorld()->GetTimeSeconds();

//	BlockMesh->OnComponentHit.AddDynamic(this, &ASmallMunition::OnHit);
}

// Called every frame
void ASmallMunition::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (GetWorld()->GetTimeSeconds() > StartTime + 10) { Destroy(); }
}


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

	/*
	if has owner and the other actor is not the owner etc....

		if InitiateAction
			Spawn explosion
			if on the server
				Do damage to all things in radius
			Destroy this bullet
		else
			spawn a red spot decal
			add the decal to a removal list
			(The bullet isn't destroyed but is sorted out in tick)
	*/
void ASmallMunition::OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//Owner is a nullptr when overlaping with the actual owner
	AActor* MyOwner = GetOwner();
	ACube_TheBattleMasterPawn* MyPawn = Cast<ACube_TheBattleMasterPawn>(GetOwner());
	
	//To ignor
	TArray<AActor*> AttachedActors;
	MyPawn->MyCube->GetAttachedActors(AttachedActors);
	AttachedActors.Add(MyPawn->MyCube);

	ACube_TheBattleMasterPlayerController* MyPlayerController = Cast<ACube_TheBattleMasterPlayerController>(MyOwner->GetOwner());
	/*Must have an owner, an interactive actor (that does not share the same actor, this is redundent right now), not itslef, and has to have a mesh to interact with i.e.BlockMesh0 */
	if (MyOwner && OtherActor && (OtherActor != GetOwner()) && (OtherActor != this) && OtherComp && !AttachedActors.Contains(OtherActor))
	{
		/* We can use the munitions to track where the bullits will go but during the initiate attack it must do damage */
		if (MyPawn->MyCube->E_TurnStateEnum == ETurnState::TS_InitiateActions) {
			if (GetLocalRole() == ROLE_Authority) {
				MulticastRPCExplosion(OtherActor->GetActorLocation());

				TArray<AActor*> ActorsToIgnore;
				TSubclassOf <UDamageType> DamageType;
				UGameplayStatics::ApplyRadialDamage(
					GetWorld(),
					10.f,
					//SweepResult.ImpactPoint
					SweepResult.TraceEnd
					,
					100.f,
					DamageType,
					ActorsToIgnore,
					this,
					MyPlayerController,
					false,
					//ECC_Camera
					ECC_Visibility
				);
				
			/*	for (TObjectIterator<APlayer_Cube> itr; itr; ++itr) {
					if (itr->bNeedsDamage) { 
					UGameplayStatics::ApplyDamage(
						*itr,
						9.f,
						MyPlayerController,
						this,
						DamageType
						); 
					} 
				}*/

				//TArray<AActor*> AffectedActors;
				////TArray<AActor*> ActorsToIgnore;
				//const TArray < TEnumAsByte < EObjectTypeQuery > >  ObjectTypes;
				//UClass * ActorClassFilter = NULL;
				////UClass * ActorClassFilter = APlayer_Cube::StaticClass();
				//UKismetSystemLibrary::SphereOverlapActors
				//(
				//	GetWorld(),
				//	SweepResult.ImpactPoint,
				//	50.f,
				//	ObjectTypes,
				//	ActorClassFilter,
				//	ActorsToIgnore,
				//	AffectedActors
				//);
				//for (auto Cubes : AffectedActors) {
				//	UE_LOG(LogTemp, Warning, TEXT("Actors %s"), *Cubes->GetName());
				//	//UGameplayStatics::ApplyDamage(Cubes, 10.0f, MyOwner->GetInstigatorController(), this, nullptr);
				//	TArray<AActor*> OutActor;
				//	Cubes->GetAttachedActors(OutActor);
				//	for (auto AttachedItems : OutActor)
				//	{
				//		FHitResult HitResult;
				//		FVector Start = SweepResult.ImpactPoint;
				//		FVector End = AttachedItems->GetActorLocation();
				//		FCollisionQueryParams Params;
				//		if (GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, Params)) {
				//			//UGameplayStatics::ApplyDamage(AttachedItems, 10.0f, MyOwner->GetInstigatorController(), this, nullptr);
				//		}
				//	}
				//}
			} Destroy();
		}
		else
		{
			ADecalActor* decal = NULL;
			if (!AttachedActors.Contains(OtherActor)) {
				decal = GetWorld()->SpawnActor<ADecalActor>(SweepResult.ImpactPoint, FRotator(0.f));
			}
			if (decal)
			{
				decal->SetActorRotation(SweepResult.ImpactNormal.Rotation());
				decal->SetDecalMaterial(RedCircleDecal);
				decal->SetLifeSpan(0.0f);
				decal->GetDecal()->DecalSize = FVector(CollisionComponent->GetScaledSphereRadius());
				decal->GetDecal()->SetFadeScreenSize(0.f);
				MyPawn->RemoveDecals.Add(decal);
			}
		}
	}
}

void ASmallMunition::MulticastRPCExplosion_Implementation(FVector Location) {
	FTransform Transform;
	Transform.SetLocation(Location);

	UParticleSystemComponent* Explosion = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), EmitterTemplate, Transform);
	UE_LOG(LogTemp, Warning, TEXT("The explosion happened here: %s"), *Transform.ToString());
}

void ASmallMunition::FireInDirection(const FVector & ShootDirection)
{
	//UE_LOG(LogTemp, Warning, TEXT("Direction %s, item Rotation %s"), *ShootDirection.ToString(), *Cast<ACube_TheBattleMasterPawn>(GetOwner())->SelectedItem->GetActorRotation().ToString());
	/*ProjectileMovementComponent->InitialSpeed = Speed;
	UE_LOG(LogTemp, Warning, TEXT("Velocity: %s"), *ProjectileMovementComponent->Velocity.ToString());
	ProjectileMovementComponent->Velocity = ShootDirection * ProjectileMovementComponent->InitialSpeed;
	UE_LOG(LogTemp, Warning, TEXT("Velocity: %s"), *ProjectileMovementComponent->Velocity.ToString());
	UE_LOG(LogTemp, Warning, TEXT("Speed: %f"), Speed);*/

}

//void ASmallMunition::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
//{
//	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
//	DOREPLIFETIME(ASmallMunition, Direction);
//}