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
	

public:

	float AdditionalSpeed = 6;
	ABooster_Basic();

	virtual void DoAction(bool bMainPhase, FVector Direction) override;

	virtual void EndAction() override;

	virtual void SetActionInMotion() override;

	virtual void UnSetActionInMotion() override;

	virtual void ResetAction() override;

	UFUNCTION(BlueprintImplementableEvent)
	void BoostFireOn();

	UFUNCTION(BlueprintImplementableEvent)
	void BoostFireOff();

	void HighlightBlocks(bool bHighlight);

	/** Pointer to white material used on the focused block */
	UPROPERTY()
	class UMaterial* BaseMaterial;
};
