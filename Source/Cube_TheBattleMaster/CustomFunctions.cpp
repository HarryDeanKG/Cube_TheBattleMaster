// Fill out your copyright notice in the Description page of Project Settings.


#include "CustomFunctions.h"
#include "Player_Cube.h"

TArray<UClass*> UCustomFunctions::GetClasses(UClass* ParentClass)
{
	TArray<UClass*> Results;

	// get our parent blueprint class
	const FString ParentClassName = ParentClass->GetName();

	UObject* ClassPackage = ANY_PACKAGE;
	//UClass* ParentBPClass = FindObject<UClass>(ClassPackage, *ParentClassName);
	//UClass* ParentBPClass = StaticLoadClass(UClass::StaticClass(), nullptr, *ParentClassName, nullptr, LOAD_None, nullptr);

	FString ClassName = FStringClassReference(ParentClass).ToString();

	UClass* ParentBPClass = StaticLoadClass(UObject::StaticClass(), nullptr, *ClassName, nullptr, LOAD_None, nullptr);
	// iterate over UClass, this might be heavy on performance, so keep in mind..
	// better suggestions for a check are welcome
	for (TObjectIterator<UClass> It; It; ++It)
	{
		if (It->IsChildOf(ParentBPClass))
		{
			// It is a child of the Parent Class

			UE_LOG(LogTemp, Warning, TEXT("Class: %s"), *It->GetName());

			// make sure we don't include our parent class in the array (weak name check, suggestions welcome)
			if (It->GetName() != ParentClassName && !It->GetName().Contains("SKEL"))
			{
				Results.Add(*It);
			}
		}
	}
	return Results;
}

FName UCustomFunctions::GetSocketNameByPoint(EAttachPoint AttachPoint)
{
	FName PointName;
	switch (AttachPoint)
	{
	case EAttachPoint::AP_Socket1:
		PointName = "S_Side1";
		break;
	case EAttachPoint::AP_Socket2:
		PointName = "S_Side2";
		break;
	case EAttachPoint::AP_Socket3:
		PointName = "S_Side3";
		break;
	case EAttachPoint::AP_Socket4:
		PointName = "S_Side4";
		break;
	case EAttachPoint::AP_Socket5:
		PointName = "S_Side5";
		break;
	case EAttachPoint::AP_Socket6:
		PointName = "S_Side6";
		break;
	}
	return PointName;
}

EAttachPoint UCustomFunctions::GetSocketPointByName(FName SocketName)
{
	EAttachPoint AttachPoint = EAttachPoint::AP_Socket1;
	if (SocketName == "S_Side1") {
		AttachPoint = EAttachPoint::AP_Socket1;
	}else if (SocketName == "S_Side2") {
		AttachPoint = EAttachPoint::AP_Socket2;
	}
	else if (SocketName == "S_Side3") {
		AttachPoint = EAttachPoint::AP_Socket3;
	}
	else if (SocketName == "S_Side4") {
		AttachPoint = EAttachPoint::AP_Socket4;
	}
	else if (SocketName == "S_Side5") {
		AttachPoint = EAttachPoint::AP_Socket5;
	}
	else if (SocketName == "S_Side6") {
		AttachPoint = EAttachPoint::AP_Socket6;
	}
	return AttachPoint;
}

//UClass* UCustomFunctions::FindClass(const FString& ClassName) {
//	check(*ClassName);
//
//	UObject* ClassPackage = ANY_PACKAGE;
//
//	UClass* Result = StaticLoadClass(UObject::StaticClass(), nullptr, *ClassName, nullptr, LOAD_None, nullptr);
//
//	//UClass* blueprintClass = FindObject(ANY_PACKAGE, TEXT("/Game/Blueprints/GameState/GameMode_Parent.GameMode_Parent_C"));
//
//	//UClass* Result = FindObject<UClass>(ClassPackage, ClassName);
//
//	return nullptr;
//}