// Fill out your copyright notice in the Description page of Project Settings.


#include "ShieldSegment.h"
#include "Player_Cube.h"
#include "Cube_TheBattleMasterPawn.h"
#include "Components/CapsuleComponent.h"
#include "Components/TimelineComponent.h"
//#include "D:/Unreal/UE_4.26Chaos/Engine/Plugins/FX/Niagara/Source/Niagara/Public/NiagaraComponent.h"
//#include "D:/Unreal/UE_4.26Chaos/Engine/Plugins/FX/Niagara/Source/Niagara/Public/NiagaraFunctionLibrary.h"

//#include "../../Engine/Plugins/FX/Niagara/Source/Niagara/Public/NiagaraComponent.h"
#include "Engine.h"

// Sets default values
AShieldSegment::AShieldSegment()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//SetReplicatingMovement(false);

	//bReplicates = true;

	//bNetUseOwnerRelevancy = true;

	//bAlwaysRelevant = true;

	// Structure to hold one-time initialization
	struct FConstructorStatics
	{
		ConstructorHelpers::FObjectFinderOptional<UStaticMesh> PlaneMesh;
		ConstructorHelpers::FObjectFinderOptional<UMaterialInstance> BaseMaterial;
		FConstructorStatics()
			: PlaneMesh(TEXT("/Game/Puzzle/Meshes/HemiGeodesic.HemiGeodesic"))
			, BaseMaterial(TEXT("/Game/BP_Classes/Misc/Shield/M_BlueWarp_Dissolve_Inst.M_BlueWarp_Dissolve_Inst"))
		{
		}
	};
	static FConstructorStatics ConstructorStatics;

	// Create dummy root scene component
	DummyRoot = CreateDefaultSubobject<USceneComponent>(TEXT("Dummy0"));
	//DummyRoot->SetIsReplicated(true);
	RootComponent = DummyRoot;

	CapsuleComponent = CreateDefaultSubobject< UCapsuleComponent>(TEXT("Cap0"));
	CapsuleComponent->SetRelativeLocation(FVector(0.0f, 0.0f, 20.0f));
	CapsuleComponent->SetRelativeScale3D(FVector(10.0f, 10.0f, 2.5f));
	CapsuleComponent->SetGenerateOverlapEvents(true);
	CapsuleComponent->SetupAttachment(DummyRoot);
	CapsuleComponent->SetCollisionProfileName("OverlapAll", false);
	CapsuleComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	CapsuleComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Block);
	CapsuleComponent->ComponentTags.Add("Intangable");


	BlockMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BlockMesh0"));
	BlockMesh->SetStaticMesh(ConstructorStatics.PlaneMesh.Get());
	BlockMesh->SetRelativeScale3D(FVector(1.0f));
	BlockMesh->SetRelativeLocation(FVector(0.f));
	BlockMesh->SetMaterial(0, ConstructorStatics.BaseMaterial.Get());
	BlockMesh->SetupAttachment(DummyRoot);
	BlockMesh->SetNotifyRigidBodyCollision(true);
	BlockMesh->SetGenerateOverlapEvents(false);
	BlockMesh->SetCollisionProfileName("BlockAll");
	//BlockMesh->OnClicked.AddDynamic(this, &ACube_TheBattleMasterBlock::BlockClicked);

	static ConstructorHelpers::FObjectFinder< UCurveFloat> Curve(TEXT("/Game/BP_Classes/Misc/Shield/Curve_Amount"));
	check(Curve.Succeeded());
	Curve_Amount = Curve.Object;

	static ConstructorHelpers::FObjectFinder< UCurveFloat> CurveHit(TEXT("/Game/BP_Classes/Misc/Shield/Curve_Hit"));
	Curve_Hit = CurveHit.Object;

	static ConstructorHelpers::FObjectFinder< UCurveFloat> CurveLaser(TEXT("/Game/BP_Classes/Misc/Shield/Curve_Laser"));
	Curve_Laser = CurveLaser.Object;

	IsDestroyed = false;
	IsHit = false;
	//Set default niagara varables
	static ConstructorHelpers::FObjectFinder< UNiagaraSystem> NiagaraParticleHit_Pointer(TEXT("/Game/BP_Classes/Misc/Shield/ImpactSparks_System"));
	static ConstructorHelpers::FObjectFinder< UNiagaraSystem> NiagaraLaserHitExplosion_Pointer(TEXT("/Game/BP_Classes/Misc/Shield/FX_Explosion_System"));

	NiagaraParticleHit=NiagaraParticleHit_Pointer.Object;
	NiagaraLaserHitExplosion = NiagaraLaserHitExplosion_Pointer.Object;

	MaxHealth = 10.f;

	DefaultDamage = 1.f;
	DefaultImpulse = 1.f;

	Tags.Add("Shield");


	//OnTakeAnyDamage.AddDynamic(this, &AShieldSegment::ApplyDamage);


	//Extra variables
	//Base_Health = 100.0f;
	//Replicated_Health = Base_Health;

	//Extra (none nessessary) variables	

}

void AShieldSegment::TimelineProgress_Implementation(float Value)
{
	DynamicBaseMaterial->SetScalarParameterValue("Amount", Value);
	DynamicBaseMaterial->SetScalarParameterValue("Opacity2", 1-Value);
	CapsuleComponent->SetRelativeScale3D(FVector( Value/0.85  * 10, Value/0.85  * 10, 2.5f));
	UE_LOG(LogTemp, Warning, TEXT("Value is %f"), Value);

}

void AShieldSegment::TimelineProgress_Hit_Implementation(float Value)
{
	DynamicBaseMaterial->SetScalarParameterValue("Radius", Value*10);
	DynamicBaseMaterial->SetScalarParameterValue("Hardness", Value*0.75);
}

void AShieldSegment::TimelineProgress_Laser_Implementation(float Value)
{
	DynamicBaseMaterial->SetScalarParameterValue("Main_Emissivity", Value * 10000);
	BlockMesh->SetRelativeScale3D(FVector(1-Value));
}

// Called when the game starts or when spawned
void AShieldSegment::BeginPlay()
{
	Super::BeginPlay();
	CurrentHealth = MaxHealth;
	DynamicBaseMaterial = BlockMesh->CreateDynamicMaterialInstance(0, BaseMaterial);

	BlockMesh->OnComponentHit.AddDynamic(this, &AShieldSegment::Trigger);
	CapsuleComponent->OnComponentBeginOverlap.AddDynamic(this, &AShieldSegment::Overlap);
	CapsuleComponent->OnComponentEndOverlap.AddDynamic(this, &AShieldSegment::OverlapOver);

	//OnTakeAnyDamage.AddDynamic(this, &AShieldSegment::Damage);

	OnTakePointDamage.AddDynamic(this, &AShieldSegment::PointDamage);

	
	//if (Cube->E_TurnStateEnum == ETurnState::TS_SelectActions) {
		//DynamicBaseMaterial->SetScalarParameterValue("Amount", 1.0f);
	//}
	//else { 
	DynamicBaseMaterial->SetScalarParameterValue("Amount", 0.0f);
	//}
		
	if (Curve_Amount)
	{
		FOnTimelineFloat TimelineProgress;
		TimelineProgress.BindUFunction(this, FName("TimelineProgress"));
		CurveTimeline.AddInterpFloat(Curve_Amount, TimelineProgress);
		
	}
	if (Curve_Hit) {
		FOnTimelineFloat TimelineProgress_Hit;
		TimelineProgress_Hit.BindUFunction(this, FName("TimelineProgress_Hit"));
		CurveTimeline_Hit.AddInterpFloat(Curve_Hit, TimelineProgress_Hit);
	}
	if (Curve_Laser) {
		FOnTimelineFloat TimelineProgress_Laser;
		TimelineProgress_Laser.BindUFunction(this, FName("TimelineProgress_Laser"));
		CurveTimeline_Laser.AddInterpFloat(Curve_Laser, TimelineProgress_Laser);
	}
}

void AShieldSegment::DoTheCurve(){ CurveTimeline.ReverseFromEnd(); }

// Called every frame
void AShieldSegment::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	CurveTimeline.TickTimeline(DeltaTime);
	CurveTimeline_Hit.TickTimeline(DeltaTime);
	CurveTimeline_Laser.TickTimeline(DeltaTime);

	if (IsDestroyed) {
		if (Curve_Amount) { if (!CurveTimeline.IsPlaying() && !CurveTimeline_Laser.IsPlaying()) { Destroy(); } }
		else { Destroy(); }
	}
	if (IsHit) 
	{
		if (Curve_Hit) { if (!CurveTimeline_Hit.IsPlaying())
			{ 
				DynamicBaseMaterial->SetScalarParameterValue("Radius", 0);
				DynamicBaseMaterial->SetScalarParameterValue("Hardness", 1);
				IsHit = false;
			}
		}
	}
}

void AShieldSegment::AttachToSelect(USceneComponent * Parent,
	const FAttachmentTransformRules & AttachmentRules,
	FName SocketName) {
	AttachToComponent(Parent, AttachmentRules, SocketName);
}

void AShieldSegment::Damage(AActor * DamagedActor, float Damage, const UDamageType * DamageType, AController * InstigatedBy, AActor * DamageCauser)
{
	
		if (Damage <= 0) { return; }
		CurrentHealth = FMath::Clamp(CurrentHealth - Damage, 0.0f, MaxHealth);
		if (CurrentHealth <= 0.f && !IsDestroyed) {
			BeginDestroy();
		}
	
	UE_LOG(LogTemp, Warning, TEXT("CurrentHealth is %f"), CurrentHealth);
}

void AShieldSegment::PointDamage(AActor* DamagedActor, float Damage, class AController* InstigatedBy, FVector HitLocation, class UPrimitiveComponent* FHitComponent, FName BoneName, FVector ShotFromDirection, const class UDamageType* DamageType, AActor* DamageCauser)
{
	if (DamageCauser->ActorHasTag("Laser")) { LaserHit(HitLocation); }
	else {
		if (Damage <= 0) { return; }
		CurrentHealth = FMath::Clamp(CurrentHealth - Damage, 0.0f, MaxHealth);
		if (CurrentHealth <= 0.f && !IsDestroyed) {
			BeginDestroy();
		}
	}
}

void AShieldSegment::LetsDestroy() {
	if (Curve_Amount) {
		if (CurveTimeline.IsPlaying()) {
			CurveTimeline.Play();
		}
		else {
			CurveTimeline.PlayFromStart();
		}
	} IsDestroyed = true;
}

void AShieldSegment::Trigger(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit){

	if (OtherActor->ActorHasTag("Cube"))
	{
		//Cast<APlayer_Cube>(OtherActor)->DoDamge();
	}
	else if (OtherActor->ActorHasTag("Projectile"))
	{
		//UE_LOG(LogTemp, Warning, TEXT("Projectile Position: %s"), *Hit.ImpactPoint.ToString());
		DynamicBaseMaterial->SetVectorParameterValue("Location", Hit.ImpactPoint);

		if (Curve_Hit) { CurveTimeline_Hit.PlayFromStart(); }
		IsHit = true;

		FVector FX_Loaction = Hit.Location;
		FRotator FX_Rotation =  Hit.Normal.Rotation();
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), NiagaraParticleHit, FX_Loaction, FX_Rotation);
	}
	else if (OtherActor->ActorHasTag("Laser"))
	{
		UNiagaraComponent* FX_ExlosionComponenet = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), NiagaraLaserHitExplosion, BlockMesh->GetComponentLocation(), BlockMesh->GetComponentRotation());
		FX_ExlosionComponenet->SetNiagaraVariableVec3("Origin Offset", FVector(NormalImpulse.X/100, NormalImpulse.Y / 100, NormalImpulse.Z / 10+500));

		Destroy();
	}
}

void AShieldSegment::Overlap(UPrimitiveComponent * OverlappedComponent, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	if (OtherActor != this) {
		//UE_LOG(LogTemp, Warning, TEXT("Overlap!!!!!"));
		OtherComp->SetCollisionProfileName("OverlapAll");
		//OtherComp->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);

	}
}

void AShieldSegment::OverlapOver(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	//UE_LOG(LogTemp, Warning, TEXT("Overlap Over"));
	if (OtherActor != this) {
		OtherComp->SetCollisionProfileName("PhysicsActor");
	}
}


void AShieldSegment::LaserHit_Implementation(FVector ImpactPoint)
{
	UE_LOG(LogTemp, Warning, TEXT("damn"));


	UNiagaraComponent* FX_ExlosionComponenet = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), NiagaraLaserHitExplosion, BlockMesh->GetComponentLocation(), BlockMesh->GetComponentRotation());
	if (FX_ExlosionComponenet) {
		FX_ExlosionComponenet->SetNiagaraVariableVec3("Origin Offset", BlockMesh->GetComponentLocation() - ImpactPoint + FVector(0.f, 0.f, 500.f));
	}
	DynamicBaseMaterial->SetVectorParameterValue("Location", ImpactPoint);

	if (Curve_Laser) { CurveTimeline_Laser.PlayFromStart(); }

	IsDestroyed = true;
	
	if (ItemOwner) { ItemOwner->Destroy(); }



	//Destroy();
}


