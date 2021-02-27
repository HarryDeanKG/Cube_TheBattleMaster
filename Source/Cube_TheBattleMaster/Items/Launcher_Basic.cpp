// Fill out your copyright notice in the Description page of Project Settings.

#include "Launcher_Basic.h"
#include "../Cube_TheBattleMasterPawn.h"

ALauncher_Basic::ALauncher_Basic() {

	//// Structure to hold one-time initialization


	WeaponName = "Basic Launcher";
	ActionName = "Directional Launch";

	bCanBeUsedWithMovement = true;

	AttackRange = 7;
	AttackRangeMin = 5;
}


void ALauncher_Basic::SetActionInMotion() {

	HighlightBlocks(true);
}

void ALauncher_Basic::UnSetActionInMotion() {

	HighlightBlocks(false);
}


void ALauncher_Basic::HighlightBlocks(bool bHighlight) {
	ACube_TheBattleMasterPawn* PlayerPawn = Cast<ACube_TheBattleMasterPawn>(GetOwner());

	PlayerPawn->HighlightAttackOptions(PlayerPawn->MyCube->BlockOwner, bHighlight, AttackRangeMin, AttackRange, false);
}

void ALauncher_Basic::DoAction(bool bMainPhase, FVector Direction)
{

}