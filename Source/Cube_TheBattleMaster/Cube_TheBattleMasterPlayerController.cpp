// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "Cube_TheBattleMasterPlayerController.h"
#include "Cube_TheBattleMasterGameMode.h"
#include "Player_Cube.h"

#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

//#include "Blueprint/UserWidget.h"
#include "GameFramework/PlayerController.h"
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

	//Get all cubes the player owns
	//GenerateStructsFromJson("Games/Content/Puzzle/ImportData/ImportedCubes.json");
	T_CubesInInv = GenerateStructsFromJson("Puzzle/ImportData/ImportedCubes.json");

		//APlayer_Cube* Cube = APlayer_Cube::StaticClass()->GetDefaultObject<APlayer_Cube>();


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


TArray<FImportedCube_Struct> ACube_TheBattleMasterPlayerController::GenerateStructsFromJson(FString Path)
{
	TSharedRef<TJsonReader<TCHAR>> JsonReader = TJsonReaderFactory<TCHAR>::Create(JsonFullPath(Path));	
	TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject());

	TArray<FImportedCube_Struct> ImportedCubes;
	
	//FJsonSerializer::Deserialize(JsonReader, JsonObject);

	if (FJsonSerializer::Deserialize(JsonReader, JsonObject) && JsonObject.IsValid())
	{
		if (GEngine) {
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, TEXT("Deserialize"));
		}

	GenerateStructsFromJson(ImportedCubes, JsonObject);
	}
	else {
		if (GEngine) {
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, TEXT("Not Deserialize"));
		}
	}
	//GenerateStructsFromJson(ImportedCubes, JsonObject);
	return ImportedCubes;
}

/*
** private
*/

void ACube_TheBattleMasterPlayerController::GenerateStructsFromJson(
	TArray<FImportedCube_Struct> &CubeStructs,
	TSharedPtr<FJsonObject> JsonObject)
{
	FVector PrevEndPoint = FVector{ 0,0,0 };
	TArray<TSharedPtr<FJsonValue>> objArray = JsonObject->GetArrayField(TEXT("OwnedCubes"));

	for (int32 i = 0; i < objArray.Num(); i++)
	{
		TSharedPtr<FJsonValue> value = objArray[i];
		TSharedPtr<FJsonObject> json = value->AsObject();
	
		FImportedCube_Struct CubeStruct;

		CubeStruct.Name = json->GetStringField(TEXT("Name"));
		CubeStruct.BaseHealth = json->GetNumberField(TEXT("BaseHealth"));
		CubeStruct.BaseArmour = json->GetNumberField(TEXT("BaseArmour"));
		CubeStruct.BaseSpeed = json->GetNumberField(TEXT("BaseSpeed"));
		CubeStruct.Colours = ParseAsColourArray(json, FString("Colours"));

		//FImportedCube_Struct CubeStruct = FImportedCube_Struct::BuildImportedCube_Struct(
		//	CubeStruct_Name, CubeStruct_BaseHealth, CubeStruct_BaseArmour, CubeStruct_BaseSpeed, CubeStruct_Colours
		//);
  
		CubeStructs.Push(CubeStruct);
	}
}

FString ACube_TheBattleMasterPlayerController::JsonFullPath(FString Path)
{
	// Games/Content/****.json
	FString FullPath = FPaths::ProjectContentDir() + Path;
	FString JsonStr;

	FFileHelper::LoadFileToString(JsonStr, *FullPath);

	return JsonStr;
}

//FVector ACube_TheBattleMasterPlayerController::ParseAsVector(TSharedPtr<FJsonObject> json, FString KeyName)
//{
//	TArray<FString> ArrayJson;
//	json->TryGetStringArrayField(*KeyName, ArrayJson);
//	FVector Vector = FVector{ FCString::Atof(*ArrayJson[0]),
//						  FCString::Atof(*ArrayJson[1]),
//				  FCString::Atof(*ArrayJson[2]) };
//	return Vector;
//}

//FRotator ACube_TheBattleMasterPlayerController::ParseAsRotator(TSharedPtr<FJsonObject> json, FString KeyName)
//{
//	TArray<FString> ArrayJson;
//	json->TryGetStringArrayField(*KeyName, ArrayJson);
//	FRotator Rotator = FRotator{ FCString::Atof(*ArrayJson[0]),
//					 FCString::Atof(*ArrayJson[1]),
//							 FCString::Atof(*ArrayJson[2]) };
//	return Rotator;
//}

FLinearColor ACube_TheBattleMasterPlayerController::ParseAsColourArray(TSharedPtr<FJsonObject> json, FString KeyName)
{
	TArray<FString> ArrayJson;
	json->TryGetStringArrayField(*KeyName, ArrayJson);

	//const TArray<TSharedPtr<FJsonValue>> ArrayJson2;

	//auto ArrayJson2 = json->GetArrayField(*KeyName);
	//TArray<FLinearColor> ColourArray;

	FLinearColor Colour;
	
		
	Colour = FLinearColor{ FCString::Atof(*ArrayJson[0]),
					FCString::Atof(*ArrayJson[1]),
					FCString::Atof(*ArrayJson[2]) };
		//ColourArray.Add(Colour);
	
	return Colour;
}

void ACube_TheBattleMasterPlayerController::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ACube_TheBattleMasterPlayerController, PlayerNumb);
}