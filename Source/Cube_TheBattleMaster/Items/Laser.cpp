// Fill out your copyright notice in the Description page of Project Settings.


#include "Laser.h"
#include "../Cube_TheBattleMasterBlock.h"
#include "../Cube_TheBattleMasterPawn.h"
#include "Kismet/KismetMathLibrary.h"



#include "DrawDebugHelpers.h"
#include "Engine/World.h"

ALaser::ALaser()
{
	//Main laser function
	static ConstructorHelpers::FObjectFinder< UNiagaraSystem> NiagaraMainLaser_Pointer(TEXT("/Game/BP_Classes/Weapons/LaserCenter_System"));

	NiagaraMainLaser = NiagaraMainLaser_Pointer.Object;

	//BlockMesh->SetRelativeRotation(FRotator(0.f, -90.f, 0.f));

	//Variables initalised

	DistanceToLaserEnd = 1739.0;
	AttackRange = 3.f;

	WeaponName = "Laser";
	ActionName = "Shoot Laser";

	Tags.Add("Laser");
	bCanBeUsedWithMovement = false;
	bRotateForHit = true;
	
}

void ALaser::BeginPlay() {
	Super::BeginPlay();
	//OwningPawn = Cast<ACube_TheBattleMasterPawn>(GetOwner());
	//

}

bool ALaser::IsInRange(AActor* SelectedActor) {
	if (FMath::IsWithinInclusive((SelectedActor->GetActorLocation() - GetActorLocation()).Rotation().Yaw, 
		GetActorRotation().Pitch * 2 + GetActorRotation().Yaw - 30.f, 
		GetActorRotation().Pitch * 2 + GetActorRotation().Yaw + 30.f)) 
		 { return true; }
	else { return false; }
}

void ALaser::DoAction(bool bMainPhase, FVector Direction) 
{
	FireLaser(bMainPhase, Direction);
}


void ALaser::FireLaser(bool bMainPhase, FVector Direction)
{
	FVector FX_Location = GetActorLocation();
	//FRotator FX_Rotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), Direction);
	FRotator FX_Rotation = Direction.ToOrientationRotator() + GetActorRotation();
	FVector2D  ChangeRotation = FVector2D(0.f);


	if (bMainPhase) { ChangeRotation = FMath::RandPointInCircle(2.f); }
	FX_Rotation += FRotator(ChangeRotation.Y, ChangeRotation.X, 0.f);



	FHitResult RV_Hit(ForceInit);

	FCollisionQueryParams RV_TraceParams = FCollisionQueryParams(FName(TEXT("RV_Trace")), true, this);
	RV_TraceParams.bTraceComplex = true;
	RV_TraceParams.bReturnPhysicalMaterial = false;
	RV_TraceParams.AddIgnoredActor(GetOwner());


	FVector AttackLocation = GetActorLocation() + UKismetMathLibrary::CreateVectorFromYawPitch(FX_Rotation.Yaw, FX_Rotation.Pitch, DistanceToLaserEnd);

	GetWorld()->LineTraceSingleByChannel(RV_Hit, GetActorLocation(), AttackLocation, ECC_Visibility, RV_TraceParams);


	//The length of the laser system mesh length
	float MeshLength = 1739.0;

	float lengthOfLaser;
	if (RV_Hit.bBlockingHit) {
		lengthOfLaser = RV_Hit.Distance / MeshLength;
		if (bMainPhase && GetLocalRole() == ROLE_Authority) {
			UE_LOG(LogTemp, Warning, TEXT("FIRE"));
			TSubclassOf<UDamageType> DamageType;
			UGameplayStatics::ApplyPointDamage(RV_Hit.GetActor(), 20.f, RV_Hit.ImpactNormal, RV_Hit, Cast<ACube_TheBattleMasterPlayerController>(GetOwner()->GetOwner()), this, DamageType);
			if (RV_Hit.GetActor()->ActorHasTag("Shield")) { ShieldInteraction(RV_Hit.GetActor()); }
		}
	}
	else { lengthOfLaser = DistanceToLaserEnd / MeshLength; }

	MultiCast_FireLaser(FX_Location, FX_Rotation, lengthOfLaser);


}

//void ALaser::Server_FireLaser_Implementation(UNiagaraComponent* FX_NiagaraMainLaserComponenet, float lengthOfLaser, FVector Direction) {
void ALaser::MultiCast_FireLaser_Implementation(FVector FX_Location, FRotator FX_Rotation, float lengthOfLaser) {

	UNiagaraComponent* FX_NiagaraMainLaserComponenet = UNiagaraFunctionLibrary::SpawnSystemAtLocation(this->GetWorld(), NiagaraMainLaser, FX_Location, FX_Rotation);

	if (FX_NiagaraMainLaserComponenet) {
		FX_NiagaraMainLaserComponenet->SetNiagaraVariableFloat("Scale Curve_Scale Factor_Z", lengthOfLaser);
		FX_NiagaraMainLaserComponenet->AddLocalRotation(FRotator(-90.f, 0.f, 0.f));
	}

}


void ALaser::SetActionInMotion()
{
	
	ACube_TheBattleMasterPawn* PlayerPawn = Cast<ACube_TheBattleMasterPawn>(GetOwner());
	PlayerPawn->bArrow = true;

	PlayerPawn->ArrowEnd = GetActorLocation();
	PlayerPawn->rangeEnd = AttackRange;

}

void ALaser::UnSetActionInMotion()
{
	Cast<ACube_TheBattleMasterPawn>(GetOwner())->bArrow = false;
}

void ALaser::ShieldInteraction(AActor* ShieldActor)
{
	//Do some damage to character
	TSubclassOf<UDamageType> DamageType;
	OwningPawn = Cast<ACube_TheBattleMasterPawn>(GetOwner());

	UGameplayStatics::ApplyDamage(OwningPawn->MyCube, 50.0f, Cast<ACube_TheBattleMasterPlayerController>(GetOwner()->GetOwner()), ShieldActor, DamageType);

	//Do some destruction effect

	Destroy();
}

