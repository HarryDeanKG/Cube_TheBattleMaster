// Fill out your copyright notice in the Description page of Project Settings.

#include "Launcher_Basic.h"
#include "../Cube_TheBattleMasterPawn.h"

ALauncher_Basic::ALauncher_Basic() {

	WeaponName = "Basic Launcher";
	ActionName = "Directional Launch";

	bCanBeUsedWithMovement = true;

	AttackRange = 7;
	AttackRangeMin = 5;
}


void ALauncher_Basic::SetActionInMotion() {

	HighlightBlocks(true);

	ACube_TheBattleMasterPawn* PlayerPawn = Cast<ACube_TheBattleMasterPawn>(GetOwner());
	//PlayerPawn->bCrossHair = true;

	//PlayerPawn->ArrowEnd = GetActorLocation();
	//PlayerPawn->rangeEnd = AttackRange;

}

void ALauncher_Basic::UnSetActionInMotion() {

	HighlightBlocks(false);
	Cast<ACube_TheBattleMasterPawn>(GetOwner())->bCrossHair = false;

}


void ALauncher_Basic::HighlightBlocks(bool bHighlight) {
	ACube_TheBattleMasterPawn* PlayerPawn = Cast<ACube_TheBattleMasterPawn>(GetOwner());

	PlayerPawn->HighlightAttackOptions(PlayerPawn->MyCube->BlockOwner, bHighlight, AttackRangeMin, AttackRange, false);
}

void ALauncher_Basic::DoAction(bool bMainPhase, FVector Direction)
{

}