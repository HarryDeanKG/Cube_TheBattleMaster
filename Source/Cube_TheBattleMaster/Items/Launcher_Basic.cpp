// Fill out your copyright notice in the Description page of Project Settings.

#include "Launcher_Basic.h"
#include "../Cube_TheBattleMasterPawn.h"

ALauncher_Basic::ALauncher_Basic() {

	WeaponName = "Basic Launcher";
	ActionName = "Directional Launch";

	bCanBeUsedWithMovement = true;
	bEndAction = true;
	bStartAction = true;
	AttackRange = 7;
	AttackRangeMin = 5;


	static ConstructorHelpers::FObjectFinder< UCurveFloat> Rotation(TEXT("/Game/BP_Classes/Weapons/Launcher/Rotation"));

	check(Rotation.Succeeded());
	Curve_Rotation = Rotation.Object;
}

void ALauncher_Basic::BeginPlay() {

	Super::BeginPlay();

	if (Curve_Rotation)
	{
		FOnTimelineFloat TimelineProgress;
		TimelineProgress.BindUFunction(this, FName("TimelineProgress"));
		CurveTimeline.AddInterpFloat(Curve_Rotation, TimelineProgress);
	}
}

void ALauncher_Basic::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (GetLocalRole() == ROLE_Authority) {
		CurveTimeline.TickTimeline(DeltaTime);
	}
}

void ALauncher_Basic::TimelineProgress_Implementation(float Value)
{
	if (GetLocalRole() == ROLE_Authority)
	{
		if (NewRotation != OldRotation) {
			SetActorRotation(FMath::LerpRange(OldRotation, NewRotation, Value));
		}
	}
}

void ALauncher_Basic::SetActionInMotion() {	HighlightBlocks(true); }

void ALauncher_Basic::UnSetActionInMotion() { HighlightBlocks(false); }

void ALauncher_Basic::HighlightBlocks(bool bHighlight) {
	ACube_TheBattleMasterPawn* PlayerPawn = Cast<ACube_TheBattleMasterPawn>(GetOwner());
	PlayerPawn->HighlightAttackOptions(PlayerPawn->MyCube->BlockOwner, bHighlight, AttackRangeMin, AttackRange, false, bHighlight);
}

void ALauncher_Basic::DoAction(bool bMainPhase, FVector Direction)
{
	ACube_TheBattleMasterPawn* PlayerPawn = Cast<ACube_TheBattleMasterPawn>(GetOwner());

	FActorSpawnParameters params;
	params.Owner = GetOwner();
	params.Instigator = GetInstigator();

	ASmallMunition* Projectile=NULL;
		
	if (bMainPhase) {
		if (GetLocalRole() == ROLE_Authority) 
		{
			if (SmallMunitionClass) {
				Projectile = GetWorld()->SpawnActor<ASmallMunition>(SmallMunitionClass, GetActorLocation(), GetActorRotation(), params);
			}
			else { Projectile = GetWorld()->SpawnActor<ASmallMunition>(GetActorLocation(), GetActorRotation(), params); }
			if (Projectile) {
				Projectile->SetReplicates(true);
				Projectile->SetReplicateMovement(true);
			}
		}		
	}
	else {
		SetLocationVars(Direction);
		SetActorRotation(NewRotation);
		if (SmallMunitionClass) {
			Projectile = GetWorld()->SpawnActor<ASmallMunition>(SmallMunitionClass, GetActorLocation(), GetActorRotation(), params);
		}
		else { Projectile = GetWorld()->SpawnActor<ASmallMunition>(GetActorLocation(), GetActorRotation(), params); }
	}

	float dummySpeed = sqrt(980.f*(AttackRange*60.f - 30.f));
	if (Projectile) {
		Projectile->FireInDirection(GetActorRotation().Vector());
		Projectile->Speed = dummySpeed;
	}
	if (!bMainPhase){ SetActorRotation(OldRotation); }
}

void ALauncher_Basic::SetLocationVars(FVector Direction) {
	UE_LOG(LogTemp, Warning, TEXT("Direction: %s"), *Direction.ToString());

	ACube_TheBattleMasterPawn* PlayerPawn = Cast<ACube_TheBattleMasterPawn>(GetOwner());
	bool bMainPhase = (PlayerPawn->MyCube->E_TurnStateEnum == ETurnState::TS_InitiateActions);

	if (!bMainPhase || bMainPhase && GetLocalRole() == ROLE_Authority)
	{
		float pitchNew;
		float newDistance;
		FVector StartPosition = GetActorLocation() + GetActorForwardVector() * 15.f;
		FVector EndPosition = Direction;
		float distance = sqrt((EndPosition.X - StartPosition.X) * (EndPosition.X - StartPosition.X) + (EndPosition.Y - StartPosition.Y) * (EndPosition.Y - StartPosition.Y));
		float MaxVelocity = sqrt(980.f * (AttackRange * 60.f - 30.f));

		UE_LOG(LogTemp, Warning, TEXT("MaxVelocity: %f"), MaxVelocity);

		for (float i = 39.0f; i < 95.1f; i++) {
			pitchNew = PI / 180.f * (i);
			newDistance = MaxVelocity * FMath::Cos(pitchNew) * (MaxVelocity * FMath::Sin(pitchNew) + sqrt(MaxVelocity * MaxVelocity * FMath::Sin(pitchNew) * FMath::Sin(pitchNew) + 2 * 980.f * GetActorLocation().Z)) / 980.f;
			pitchNew = pitchNew * 180.f / PI;
			if (FMath::IsWithin(newDistance, distance - 10, distance + 10)) { break; }
			//UE_LOG(LogTemp, Warning, TEXT("End Pitch: %f, old distance %f, new distance %f"), alpha, distance, newDistance);
		}
		NewRotation = FRotator(
			pitchNew,
			(Direction - GetActorLocation()).Rotation().Yaw,
			0.f);

		if (bMainPhase) {
			NewRotation.Pitch *= FMath::RandRange(0.95f, 1.05f);
			NewRotation.Yaw *= FMath::RandRange(0.95f, 1.05f);
		}
		OldRotation = GetActorRotation();
		UE_LOG(LogTemp, Warning, TEXT("NewVel: %s, OldVel: %s"), *NewRotation.ToString(), *OldRotation.ToString());

	}
}


void ALauncher_Basic::EndAction()
{
	if (Curve_Rotation) { CurveTimeline.ReverseFromEnd(); }

	UE_LOG(LogTemp, Warning, TEXT("Action has ended!"));
}

void ALauncher_Basic::StartAction(FToDo_Struct Action)
{
	SetLocationVars(Action.SupplementaryVec);

	if (Curve_Rotation && GetLocalRole() == ROLE_Authority) {
		CurveTimeline.PlayFromStart();
	}

	UE_LOG(LogTemp, Warning, TEXT("Action has Started!"));
}
