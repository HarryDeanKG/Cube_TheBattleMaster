// Fill out your copyright notice in the Description page of Project Settings.

#include "Booster_Basic.h"
#include "../Cube_TheBattleMasterPawn.h"


ABooster_Basic::ABooster_Basic() {

	static ConstructorHelpers::FObjectFinder<UParticleSystem> PSClass(TEXT("/Game/AdvancedMagicFX12/particles/P_ky_shot_fire.P_ky_shot_fire"));
	if (PSClass.Object != NULL) {
		EmitterTemplate = PSClass.Object;
	}

	
	GetBlockMesh();

	WeaponName = "Booster";

	ActionName = "Boost";

	bCanBeUsedWithMovement = false;

	ActionItteration = 0;

	bEndAction = true;
}

void ABooster_Basic::BeginPlay() {

	Super::BeginPlay();

	FTransform Transform;
	Transform.SetLocation(OutwardLocation);
	Transform.SetScale3D(FVector(0.1f));
	//EmitterTemplateComponent = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), EmitterTemplate, Transform);

	FAttachmentTransformRules Trans = FAttachmentTransformRules
	(
		EAttachmentRule::SnapToTarget,
		EAttachmentRule::SnapToTarget,
		EAttachmentRule::SnapToTarget,
		true
	);
	EmitterTemplateComponent = UGameplayStatics::SpawnEmitterAttached(EmitterTemplate, GetBlockMesh(), "S_Outward");
}
void ABooster_Basic::DoAction(bool bMainPhase, FVector Direction) {
	ACube_TheBattleMasterPawn* PlayerPawn = Cast<ACube_TheBattleMasterPawn>(GetOwner());
	if (!Cube) { Cube = PlayerPawn->MyCube; }
	
	if (Cube->E_TurnStateEnum == ETurnState::TS_InitiateActions) { BoostFireOn(); }
}

void ABooster_Basic::EndAction(){ BoostFireOff(); }
//
void ABooster_Basic::BoostFireOn_Implementation() { if (EmitterTemplateComponent) { EmitterTemplateComponent->SetRelativeScale3D(FVector(5.f)); } }
//
void ABooster_Basic::BoostFireOff_Implementation() { if (EmitterTemplateComponent) { EmitterTemplateComponent->SetRelativeScale3D(FVector(1.f)); } }


void ABooster_Basic::SetActionInMotion() { HighlightBlocks(true); }

void ABooster_Basic::UnSetActionInMotion() { HighlightBlocks(false); }


void ABooster_Basic::HighlightBlocks(bool bHighlight) {
	ACube_TheBattleMasterPawn* PlayerPawn = Cast<ACube_TheBattleMasterPawn>(GetOwner());

	int PreviousSpeed = PlayerPawn->MyCube->Replicated_Speed;
	
	PlayerPawn->MyCube->Replicated_Speed = AdditionalSpeed;
	PlayerPawn->HighlightMoveOptions(PlayerPawn->MyCube->BlockOwner, bHighlight);
	PlayerPawn->MyCube->Replicated_Speed = PreviousSpeed;
}

void ABooster_Basic::ResetAction() {
	//BoostFireOff();
}