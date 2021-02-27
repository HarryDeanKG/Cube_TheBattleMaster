// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../ItemBase.h"
#include "Launcher_Basic.generated.h"

/**
 * 
 */
UCLASS()
class CUBE_THEBATTLEMASTER_API ALauncher_Basic : public AItemBase
{
	GENERATED_BODY()

public:

	ALauncher_Basic();

	virtual void SetActionInMotion() override;

	virtual void UnSetActionInMotion() override;

	void HighlightBlocks(bool bHighlight);


	virtual void DoAction(bool bMainPhase, FVector Direction) override;

};
