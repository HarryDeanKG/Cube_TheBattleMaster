// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../ItemBase.h"
#include "Weapon_Basic.generated.h"

/**
 * 
 */
UCLASS()
class CUBE_THEBATTLEMASTER_API AWeapon_Basic : public AItemBase
{
	GENERATED_BODY()
	
public:

	AWeapon_Basic();

	virtual void SetActionInMotion() override;

	virtual void UnSetActionInMotion() override;


	void HighlightBlocks(bool bHighlight);
};
