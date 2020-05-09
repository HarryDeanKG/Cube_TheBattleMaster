// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "Player_Cube.h"
#include "Engine/World.h"
#include "Cube_TheBattleMasterPlayerController.h"

ACube_TheBattleMasterPlayerController::ACube_TheBattleMasterPlayerController()
{
	bShowMouseCursor = true;
	bEnableClickEvents = true;
	bEnableTouchEvents = true;
	DefaultMouseCursor = EMouseCursor::Crosshairs;
	
	//MakeCube();
}

void ACube_TheBattleMasterPlayerController::MakeCube()
{
	if (Role < ROLE_Authority) {
		Server_MakeCube();
	}
	auto Next_Cube=GetWorld()->SpawnActor<APlayer_Cube>(FVector(0, 0, 0), FRotator(0, 0, 0));

	CubeArray.Add(Next_Cube);
}

bool ACube_TheBattleMasterPlayerController::Server_MakeCube_Validate() {
	return true;
}

void ACube_TheBattleMasterPlayerController::Server_MakeCube_Implementation()
{
	MakeCube();
}