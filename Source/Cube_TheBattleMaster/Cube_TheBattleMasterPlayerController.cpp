// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "Cube_TheBattleMasterPlayerController.h"
#include "Blueprint/UserWidget.h"
#include "UObject/ConstructorHelpers.h"

ACube_TheBattleMasterPlayerController::ACube_TheBattleMasterPlayerController()
{
	bShowMouseCursor = true;
	bEnableClickEvents = true;
	bEnableTouchEvents = true;
	DefaultMouseCursor = EMouseCursor::Crosshairs;

	
	
}



void ACube_TheBattleMasterPlayerController::BeginPlay()
{
	Super::BeginPlay();

	/*static ConstructorHelpers::FClassFinder<UUserWidget> TestFinder(TEXT("/Game/Widgets/ActionInterface"));
	TSubclassOf<class UUserWidget> TestClass = TestFinder.Class;

	UUserWidget* Test = CreateWidget<UUserWidget>(this, TestClass);

	Test->AddToViewport();
*/
	//ActionInterface_Instance->AddToViewport();

	//if (ActionInterface != nullptr)
	//{
	//	//ActionInterface_Instance = CreateWidget<UUserWidget>(GetWorld(), ActionInterface);
	//	if (ActionInterface_Instance != nullptr)
	//	{
	//		ActionInterface_Instance->AddToViewport();
	//	}
	//}
}