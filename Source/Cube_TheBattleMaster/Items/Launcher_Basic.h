// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../ItemBase.h"
#include "Components/TimelineComponent.h"

#include "Launcher_Basic.generated.h"

/**
 * 
 */
UCLASS()
class CUBE_THEBATTLEMASTER_API ALauncher_Basic : public AItemBase
{
	GENERATED_BODY()


	FTimeline CurveTimeline;
	UPROPERTY(Category = "Timeline", VisibleDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	UCurveFloat* Curve_Rotation;


	UFUNCTION(NetMulticast, Reliable)
	void TimelineProgress(float Value);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:

	ALauncher_Basic();

	virtual void Tick(float DeltaTime) override;

	void SetLocationVars(FVector Direction);
	FRotator NewRotation;
	FRotator OldRotation;

	virtual void SetActionInMotion() override;

	virtual void UnSetActionInMotion() override;

	void HighlightBlocks(bool bHighlight);


	virtual void DoAction(bool bMainPhase, FVector Direction) override;

	virtual void EndAction() override;

	virtual void StartAction(FToDo_Struct Action) override;

	
};
