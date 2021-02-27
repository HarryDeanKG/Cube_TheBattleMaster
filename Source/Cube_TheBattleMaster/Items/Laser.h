// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapon_Basic.h"
#include "../Cube_TheBattleMasterPawn.h"

#include "D:/Unreal/UE_4.26Chaos/Engine/Plugins/FX/Niagara/Source/Niagara/Public/NiagaraComponent.h"

#include "Laser.generated.h"

/**
 * 
 */
UCLASS()
class CUBE_THEBATTLEMASTER_API ALaser : public AWeapon_Basic
{
	GENERATED_BODY()
	
public:
	ALaser();

	ACube_TheBattleMasterPawn* OwningPawn;

	UPROPERTY(EditAnywhere)
	UNiagaraSystem* NiagaraMainLaser;

	UPROPERTY()
	UCapsuleComponent* HitTarget;

protected:
	virtual void BeginPlay() override;

public:
	//Variables
	UPROPERTY(EditAnywhere)
	float DistanceToLaserEnd;

	virtual void DoAction(bool bMainPhase, FVector Direction) override;

	virtual void SetActionInMotion() override;
	virtual void UnSetActionInMotion() override;

	UFUNCTION()
	void ShieldInteraction(AActor* ShieldActor);

};
