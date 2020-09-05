// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Cube_TheBattleMasterBlock.h"
#include "Cube_TheBattleMasterGameMode.h"
#include "Kismet/GameplayStatics.h"

#include "Cube_TheBattleMasterPlayerController.h"
#include "Player_Cube.generated.h"

UCLASS()
class CUBE_THEBATTLEMASTER_API APlayer_Cube : public AActor
{
	GENERATED_BODY()
	
	/** Dummy root component */
	UPROPERTY(Category = Block, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class USceneComponent* DummyRoot;

	/** StaticMesh component for the clickable block */
	UPROPERTY(Category = Block, VisibleDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* BlockMesh;

public:	
	// Sets default values for this actor's properties
	APlayer_Cube();

	//virtual void BeginPlay() override;

	ETurnState E_TurnStateEnum;
	virtual void OnConstruction(const FTransform & Transform) override;

	virtual void Tick(float DeltaSeconds) override;

	float time = 0.0f;
	float ActionTimer = 5.0f;

	UPROPERTY(Replicated)
	bool bDoAction = false;

	UPROPERTY(BlueprintReadWrite)
	bool bReady = false;

	bool bMove=false;
	bool bHasMoved = false;
	FVector StartPosition = FVector(0.0f);
	FVector CurrentPosition;
	FVector FinalPosition;
	UFUNCTION(Reliable, Server)
	void Server_Movement(FVector MovePosition);

	//Called by Server movement and vicaversa
	ACube_TheBattleMasterPawn* MyPawn;
	void SetOwningPawn(ACube_TheBattleMasterPawn * NewOwner);

	void Movement(FVector MovePosition);

	ACube_TheBattleMasterGameMode* BaseGameMode;

	/** Pointer to white material used on the focused block */
	UPROPERTY()
	class UMaterial* BaseMaterial;



	/* All the base variables */
	UPROPERTY(EditAnywhere)
	float Base_Health;

	UPROPERTY(EditAnywhere)
	int32 Base_Speed=4;

	UPROPERTY(EditAnywhere)
	int32 AttackRange = 7;

	UPROPERTY(EditAnywhere)
	float Base_Damage = 10.0;

	UPROPERTY(EditAnywhere)
	float Base_Armour = 1.0;

	/* Replicated variables */

	UPROPERTY(Replicated, EditAnywhere)
	float Replicated_Health;

	UPROPERTY(Replicated, EditAnywhere)
	int32 Replicated_Speed;

	UPROPERTY(EditAnywhere)
	ACube_TheBattleMasterBlock* BlockOwner;

	/* GamePlay functions */

	//void ApplyDamage(AActor* DamagedActor, float BaseDamage, AController* EventInstigator, AActor* DamageCauser, FDamageEvent DamageEvent);
	//void ApplyDamage(APlayer_Cube* DamagedActor, float BaseDamage, APlayer_Cube* DamageCauser);
	UFUNCTION()
	void ApplyDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	USpringArmComponent* OurCameraSpringArm;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UCameraComponent* OurCamera;

public:
	/** Returns DummyRoot subobject **/
	FORCEINLINE class USceneComponent* GetDummyRoot() const { return DummyRoot; }
	/** Returns BlockMesh subobject **/
	FORCEINLINE class UStaticMeshComponent* GetBlockMesh() const { return BlockMesh; }

	/** Returns Camera subobject **/
	FORCEINLINE class UCameraComponent* GetCamera() const { return OurCamera; }
};
