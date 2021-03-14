// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../CustomFunctions.h"
#include "../ItemBase.h"
#include "../ShieldSegment.h"
#include "Shield_Basic.generated.h"

/**
 * 
 */
UCLASS()
class CUBE_THEBATTLEMASTER_API AShield_Basic : public AItemBase
{
	GENERATED_BODY()
	
public:


	AShield_Basic();

	virtual void DoAction(bool bMainPhase, FVector Direction) override;

	virtual void SetActionInMotion() override;

	virtual void UnSetActionInMotion() override;

	virtual void ResetAction() override;


	//Local variables and functions for this actor to this actor
	// Toggle shild on and off - default should be off so false.
	bool bShield;

	//Choise Whether shield is directed or over the top
	bool bDirectional;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Projectile Type")
	TSubclassOf <AShield_Basic> ShieldClass;

	UFUNCTION(Server, Reliable)
	void Server_ShieldOn(); //ACube_TheBattleMasterPawn* Pawn
	void ShieldOn();

	UFUNCTION(Server, Reliable)
	void Server_ShieldOff();
	void ShieldOff();

	void MakeShield(bool bIsReplicating);

	UFUNCTION(NetMulticast, Reliable)
	void ShieldToggle();

	TArray<AShieldSegment*> T_ShieldSegments;

	TArray<AShieldSegment*> T_HiddenShieldSegments;


};
