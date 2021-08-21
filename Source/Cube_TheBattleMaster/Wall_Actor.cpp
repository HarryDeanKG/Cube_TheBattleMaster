// Fill out your copyright notice in the Description page of Project Settings.



#include "Wall_Actor.h"
#include "Net/UnrealNetwork.h"

#include "Engine/DecalActor.h"
#include "Components/DecalComponent.h"

//#include "D:/Unreal/UE_4.26Chaos/Engine/Plugins/FX/Niagara/Source/Niagara/Public/NiagaraComponent.h"
//#include "D:/Unreal/UE_4.26Chaos/Engine/Plugins/FX/Niagara/Source/Niagara/Public/NiagaraFunctionLibrary.h"

#include "Kismet/KismetMathLibrary.h"
//#include "Chaos/Core.h"
//#include "GeometryCollection/GeometryCollectionObject.h"
//#include "GeometryCollection/GeometryCollectionComponent.h"

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
	
	BlockMesh->SetStaticMesh(ConstructorStatics.PlaneMesh.Get());
	BlockMesh->SetRelativeScale3D(FVector(0.055f, 0.05f, 0.45f));
	BlockMesh->SetRelativeRotation(FRotator(0.f, 90.f, 0.f));
	
	BlockMesh->SetRelativeLocation(FVector(0.f, -40.f, 0.f));
	BlockMesh->SetMaterial(0, ConstructorStatics.BaseMaterial.Get());
	BlockMesh->SetupAttachment(DummyRoot);
	BlockMesh->SetNotifyRigidBodyCollision(true);
	BlockMesh->SetGenerateOverlapEvents(false);

	//Niagra pointers
	static ConstructorHelpers::FObjectFinder< UNiagaraSystem> NiagaraParticleHit_Pointer(TEXT("/Game/BP_Classes/Misc/Shield/ImpactSparks_System"));
	static ConstructorHelpers::FObjectFinder< UNiagaraSystem> NiagaraLaserHit_Pointer(TEXT("/Game/BP_Classes/Misc/Wall/Redmark_System"));

	NiagaraParticleHit = NiagaraParticleHit_Pointer.Object;
	NiagaraLaserHit = NiagaraParticleHit_Pointer.Object;

	// Save a pointer to the orange material
	BaseMaterial = ConstructorStatics.BaseMaterial.Get();


	//OnTakeAnyDamage.AddDynamic(this, &AWall_Actor::ApplyDamage);

	static ConstructorHelpers::FObjectFinder<UMaterialInterface> LaserHitDecal_Pointer(TEXT("/Game/BP_Classes/Misc/Wall/BuiltHole_Decal"));
	LaserHitDecal = LaserHitDecal_Pointer.Object;
	check(LaserHitDecal);
	//Extra variables
	Base_Health = 100.0f;

	DefaultDamage = 1.f;

	//Extra (none nessessary) variables	

}

// Called when the game starts or when spawned
void AWall_Actor::BeginPlay()
{
	Super::BeginPlay();

	Replicated_Health = Base_Health;

	//OnTakeAnyDamage.AddDynamic(this, &AWall_Actor::Damage);
	OnTakePointDamage.AddDynamic(this, &AWall_Actor::PointDamage);
	
	BlockMesh->OnComponentHit.AddDynamic(this, &AWall_Actor::Trigger);

}

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

void AWall_Actor::Damage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{
	if (Damage <= 0) { return; }
	Replicated_Health = FMath::Clamp(Replicated_Health - Damage, 0.0f, Base_Health);
	if (Replicated_Health <= 0.f && !IsDestroyed) {
		IsDestroyed = true;
	}
	UE_LOG(LogTemp, Warning, TEXT("CurrentHealth is %f"), Replicated_Health);
}

void AWall_Actor::PointDamage(AActor* DamagedActor, float Damage, class AController* InstigatedBy, FVector HitLocation, class UPrimitiveComponent* FHitComponent, FName BoneName, FVector ShotFromDirection, const class UDamageType* DamageType, AActor* DamageCauser)
{
	UE_LOG(LogTemp, Warning, TEXT("Point Damage"));

	if (DamageCauser->ActorHasTag("Laser"))
	{
		FVector FX_Loaction = HitLocation;
		FRotator FX_Rotation = ShotFromDirection.Rotation();
		FX_Rotation.Yaw += 180;

		FRotator test = (DamagedActor->GetActorLocation() - DamageCauser->GetActorLocation()).Rotation();
		//UE_LOG(LogTemp, Warning, TEXT("ShotDirection: %s, test Rotation: %s"),  *ShotFromDirection.Rotation().ToString(), *test.ToString());

		UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), NiagaraLaserHit, FX_Loaction, test);
		//UE_LOG(LogTemp, Warning, TEXT("Actor Rotation: %s, Function Rotation: %s. ShotDirection: %s"), *GetActorRotation().ToString(), *FX_Rotation.ToString(), *ShotFromDirection.Rotation().ToString());

		ADecalActor* decal = GetWorld()->SpawnActor<ADecalActor>(FX_Loaction, FX_Rotation+ FRotator(90.f));
		if (decal)
		{
			decal->SetDecalMaterial(LaserHitDecal);
			decal->SetLifeSpan(0.0f);
			decal->GetDecal()->DecalSize = FVector(4.0f, 4.0f, 4.0f);
		}
	}
}
void AWall_Actor::Trigger(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) {

	UE_LOG(LogTemp, Warning, TEXT("Triggered"));

	if (OtherActor->ActorHasTag("Cube"))
	{
		//Cast<APlayer_Cube>(OtherActor)->DoDamge();
	}
	else if (OtherActor->ActorHasTag("Projectile"))
	{
		//UE_LOG(LogTemp, Warning, TEXT("Projectile Position: %s"), *Hit.ImpactPoint.ToString());

		FVector FX_Loaction = Hit.Location;
		FRotator FX_Rotation = Hit.Normal.Rotation();
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), NiagaraParticleHit, FX_Loaction, FX_Rotation);

		//GetTransform();
		
		
		//ADecalActor* decal = GetWorld()->SpawnActor<ADecalActor>(UKismetMathLibrary::InverseTransformLocation(BlockMesh->GetRelativeTransform(), Hit.ImpactPoint), UKismetMathLibrary::InverseTransformRotation(GetTransform(), OtherComp->GetRelativeRotation()));
		ADecalActor* decal = GetWorld()->SpawnActor<ADecalActor>(Hit.Location, FRotator(90.f));
		if (decal)
		{
			decal->SetDecalMaterial(LaserHitDecal);
			decal->SetLifeSpan(0.0f);
			decal->GetDecal()->DecalSize = FVector(20.0f, 20.0f, 20.0f);
			//decal->GetDecal()->DecalSize = 100*OtherComp->GetRelativeScale3D();
			UE_LOG(LogTemp, Warning, TEXT("%s"), *OtherComp->GetRelativeScale3D().ToString());
		}

	}
	else if (OtherActor->ActorHasTag("Laser"))
	{
		FVector FX_Loaction = Hit.Location;
		FRotator FX_Rotation = Hit.Normal.Rotation();
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), NiagaraLaserHit, FX_Loaction, FX_Rotation);

		ADecalActor* decal = GetWorld()->SpawnActor<ADecalActor>(Hit.Location, FRotator(90.f));
		if (decal)
		{
			decal->SetDecalMaterial(LaserHitDecal);
			decal->SetLifeSpan(0.0f);
			decal->GetDecal()->DecalSize = FVector(10.0f, 10.0f, 10.0f);
			//decal->GetDecal()->DecalSize = 100*OtherComp->GetRelativeScale3D();
			UE_LOG(LogTemp, Warning, TEXT("%s"), *OtherComp->GetRelativeScale3D().ToString());
		}

	}

}



void AWall_Actor::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AWall_Actor, Replicated_Health);

	DOREPLIFETIME(AWall_Actor, bTemporary);

}