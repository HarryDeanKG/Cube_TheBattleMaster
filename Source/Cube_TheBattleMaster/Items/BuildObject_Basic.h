// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../ItemBase.h"
#include "../Wall_Actor.h"
#include "BuildObject_Basic.generated.h"

/**
 * 
 */
UCLASS()
class CUBE_THEBATTLEMASTER_API ABuildObject_Basic : public AItemBase
{
	
	GENERATED_BODY()
	

public:

	ABuildObject_Basic();

	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = "Projectile Type")
	TSubclassOf < AWall_Actor > WallClass;
	
	//void MakeTheWall(FVector Location);

	void MakeTheWall(FVector  Direction, bool bMainPhase);

	

	virtual void DoAction(bool bMainPhase, FVector Direction) override;

	virtual void SetActionInMotion() override;

	virtual void UnSetActionInMotion() override;

	

	virtual void ResetAction() override;

	void HighlightBlocks(bool bHighlight);

	FName FindCorrectSlot(FVector Direction);


	/** Pointer to white material used on the focused block */
	UPROPERTY()
	class UMaterial* BaseMaterial;

};

