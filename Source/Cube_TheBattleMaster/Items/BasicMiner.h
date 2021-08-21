// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../ItemBase.h"
#include "BasicMiner.generated.h"

/**
 * 
 */
UCLASS()
class CUBE_THEBATTLEMASTER_API ABasicMiner : public AItemBase
{
	GENERATED_BODY()

		ABasicMiner();

public:
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	virtual void DoAction(bool bMainPhase, FVector Direction) override;

	virtual void SetActionInMotion() override;

	virtual void UnSetActionInMotion() override;

	virtual void EndAction() override;

	bool CheckDummyEnergyIsCorrect();

	void Extract(bool bTemporary);

	bool bGetEnergy = false;
	//void FindOwner();

	//UFUNCTION(NetMulticast, Reliable)
	//void NetMulticast_FindOwner();

	void UnExtract();

	virtual void ResetAction() override;


	//Local Variables
	bool bRemovePotentialEnergy;
};
