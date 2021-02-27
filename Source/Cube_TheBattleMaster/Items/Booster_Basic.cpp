// Fill out your copyright notice in the Description page of Project Settings.

#include "Booster_Basic.h"
#include "../Cube_TheBattleMasterPawn.h"


ABooster_Basic::ABooster_Basic() {

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

	BlockMesh->SetStaticMesh(ConstructorStatics.PlaneMesh.Get());

	BaseMaterial = ConstructorStatics.BaseMaterial.Get();

	WeaponName = "Booster";

	ActionName = "Boost";

	bCanBeUsedWithMovement = false;

	ActionItteration = 0;

	bEndAction = true;
}

void ABooster_Basic::DoAction(bool bMainPhase, FVector Direction) {
	ACube_TheBattleMasterPawn* PlayerPawn = Cast<ACube_TheBattleMasterPawn>(GetOwner());
	APlayer_Cube* Cube = PlayerPawn->MyCube;
	
	if (Cube->E_TurnStateEnum == ETurnState::TS_InitiateActions) { BoostFireOn(); }
}

void ABooster_Basic::EndAction()
{
	UE_LOG(LogTemp, Warning, TEXT("EndAction!!! Boost"));
	BoostFireOff();
}

void ABooster_Basic::SetActionInMotion() { HighlightBlocks(true); }

void ABooster_Basic::UnSetActionInMotion() { HighlightBlocks(false); }


void ABooster_Basic::HighlightBlocks(bool bHighlight) {
	ACube_TheBattleMasterPawn* PlayerPawn = Cast<ACube_TheBattleMasterPawn>(GetOwner());

	float dummySpeed = PlayerPawn->MyCube->Replicated_Speed;
	PlayerPawn->MyCube->Replicated_Speed += AdditionalSpeed;
	PlayerPawn->HighlightMoveOptions(PlayerPawn->MyCube->BlockOwner, bHighlight);
	
}

void ABooster_Basic::ResetAction() {
	//BoostFireOff();
}