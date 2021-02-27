// Fill out your copyright notice in the Description page of Project Settings.

#include "Weapon_Basic.h"
#include "../Cube_TheBattleMasterPawn.h"


AWeapon_Basic::AWeapon_Basic() {

	//// Structure to hold one-time initialization


	WeaponName = "Basic Shooter";
	ActionName = "Directional Shot";

	bCanBeUsedWithMovement = true;
	AttackRange = 7.f;
	AttackRangeMin = 0.f;
}


void AWeapon_Basic::SetActionInMotion() {
	ACube_TheBattleMasterPawn* PlayerPawn = Cast<ACube_TheBattleMasterPawn>(GetOwner());
	PlayerPawn->bArrow = true;

	PlayerPawn->ArrowEnd = GetActorLocation();
	PlayerPawn->rangeEnd = AttackRange;
}

void AWeapon_Basic::UnSetActionInMotion() {

	Cast<ACube_TheBattleMasterPawn>(GetOwner())->bArrow = false;
}


void AWeapon_Basic::HighlightBlocks(bool bHighlight) {
	ACube_TheBattleMasterPawn* PlayerPawn = Cast<ACube_TheBattleMasterPawn>(GetOwner());

	PlayerPawn->HighlightAttackOptions(PlayerPawn->MyCube->BlockOwner, bHighlight, AttackRangeMin, AttackRange, false);
}