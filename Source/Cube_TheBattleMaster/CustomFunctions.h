// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "EngineMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "CustomFunctions.generated.h"


UENUM(BlueprintType)        //"BlueprintType" is essential to include
enum class EAttachPoint : uint8
{
	AP_Socket1    UMETA(DisplayName = "Side1"),
	AP_Socket2     UMETA(DisplayName = "Side2"),
	AP_Socket3		UMETA(DisplayName = "Side3"),
	AP_Socket4    UMETA(DisplayName = "Side4"),
	AP_Socket5     UMETA(DisplayName = "Side5"),
	AP_Socket6		UMETA(DisplayName = "Side6")
};

UENUM(BlueprintType)        //"BlueprintType" is essential to include
enum class EAmmunitionType : uint8
{
	AT_Rockets    UMETA(DisplayName = "Rockets"),
	AT_Bullets     UMETA(DisplayName = "Bullets"),
	AT_Energy		UMETA(DisplayName = "Energy")
};



USTRUCT(BlueprintType)
struct FAttachInfo_Struct
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool InUse = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EAttachPoint AttachPoint;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	AActor* ItemRef = nullptr;
};

USTRUCT(BlueprintType)
struct FAmmoInfo_Struct
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 AmmoCount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EAmmunitionType AmmoType;
};


UCLASS()
class CUBE_THEBATTLEMASTER_API UCustomFunctions : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Get Child Classes", Keywords = "Get Child Classes"), Category = Class)
	static TArray<UClass*> GetClasses(UClass* ParentClass);


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Enum)
	EAttachPoint EAttachPoint_Enum;

	UFUNCTION(BlueprintCallable, BlueprintPure = true)
	static FName GetSocketNameByPoint(EAttachPoint AttachPoint);
	//BlueprintNativeEvent
	UFUNCTION(BlueprintCallable, BlueprintPure = true)
	static EAttachPoint GetSocketPointByName(FName SocketName);

	
	/*UFUNCTION(BlueprintCallable)
	UClass* FindClass(const FString& ClassName);*/

};
