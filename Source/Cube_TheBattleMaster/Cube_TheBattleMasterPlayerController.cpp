// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "Cube_TheBattleMasterPlayerController.h"
#include "Cube_TheBattleMasterGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/UserWidget.h"
#include "UObject/ConstructorHelpers.h"

ACube_TheBattleMasterPlayerController::ACube_TheBattleMasterPlayerController()
{
	bShowMouseCursor = true;
	bEnableClickEvents = true;
	bEnableTouchEvents = true;
	DefaultMouseCursor = EMouseCursor::Crosshairs;
}



//
//void ACube_TheBattleMasterPlayerController::BeginPlay()
//{
//	Super::BeginPlay();
//
//	/*static ConstructorHelpers::FClassFinder<UUserWidget> TestFinder(TEXT("/Game/Widgets/ActionInterface"));
//	TSubclassOf<class UUserWidget> TestClass = TestFinder.Class;
//
//	UUserWidget* Test = CreateWidget<UUserWidget>(this, TestClass);
//
//	Test->AddToViewport();
//*/
//	//ActionInterface_Instance->AddToViewport();
//
//	//if (ActionInterface != nullptr)
//	//{
//	//	//ActionInterface_Instance = CreateWidget<UUserWidget>(GetWorld(), ActionInterface);
//	//	if (ActionInterface_Instance != nullptr)
//	//	{
//	//		ActionInterface_Instance->AddToViewport();
//	//	}
//	//}
//}

ACube_TheBattleMasterGameMode * ACube_TheBattleMasterPlayerController::GetGameMode()
{
	return Cast<ACube_TheBattleMasterGameMode>(UGameplayStatics::GetGameMode(this));
}

EGameSection ACube_TheBattleMasterPlayerController::GetGameModeSection()
{
	ACube_TheBattleMasterGameMode* TheGameMode = Cast<ACube_TheBattleMasterGameMode>(UGameplayStatics::GetGameMode(this));

	return TheGameMode->E_GameSectionEnum;
	

	/*
	const UEnum* EnumPtr = FindObject<UEnum>(ANY_PACKAGE, TEXT("EGameSection"), true);
	if (!EnumPtr) return FString("Invalid");

	return EnumPtr->GetNameByValue((int64)EnumValue);*/

}

void ACube_TheBattleMasterPlayerController::ToggleGameModeSection(FString GameModeString) 
{
	ACube_TheBattleMasterGameMode* TheGameMode = Cast<ACube_TheBattleMasterGameMode>(UGameplayStatics::GetGameMode(this));
	if (TheGameMode->E_GameSectionEnum == EGameSection::GS_StartSection) { TheGameMode->E_GameSectionEnum = EGameSection::GS_MidSection; }
	else if (TheGameMode->E_GameSectionEnum == EGameSection::GS_MidSection) { TheGameMode->E_GameSectionEnum = EGameSection::GS_EndSection; }
	else if (TheGameMode->E_GameSectionEnum == EGameSection::GS_EndSection) { TheGameMode->E_GameSectionEnum = EGameSection::GS_StartSection; }
	else {/**/}
}