// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../ItemBase.h"
#include "Booster_Basic.generated.h"

/**
 * 
 */
UCLASS()
class CUBE_THEBATTLEMASTER_API ABooster_Basic : public AItemBase
{
	GENERATED_BODY()
	
	virtual void BeginPlay() override;
public:

	float AdditionalSpeed = 6;
	ABooster_Basic();

	virtual void DoAction(bool bMainPhase, FVector Direction) override;

	virtual void EndAction() override;

	virtual void SetActionInMotion() override;

	virtual void UnSetActionInMotion() override;

	virtual void ResetAction() override;


	UPROPERTY()
	UParticleSystem* EmitterTemplate;

	UPROPERTY()
	UParticleSystemComponent* EmitterTemplateComponent;

	UFUNCTION(NetMulticast, Reliable)
	void BoostFireOn();

	UFUNCTION(NetMulticast, Reliable)
	void BoostFireOff();

	void HighlightBlocks(bool bHighlight);


};
