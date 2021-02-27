// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Cube_TheBattleMasterBlock.h"
#include "Cube_TheBattleMasterGameMode.h"
#include "SmallMunition.h"
#include "ItemBase.h"
#include "AttackComponent.h"
#include "AttachmentComponent.h"
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

	/*UPROPERTY(Category = "Attack", BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	class UAttackComponent* AttackComponent;*/


	UPROPERTY(Category = "Attachments", BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	class UAttachmentComponent* AttachmentComponent;


public:	
	UPROPERTY(Category = "Attachments", BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	TMap <FString, AItemBase*>  M_SlotsRefference;


	// Sets default values for this actor's properties
	APlayer_Cube();

	//virtual void BeginPlay() override;

	ETurnState E_TurnStateEnum;
	virtual void OnConstruction(const FTransform & Transform) override;

	void UpdateActionList(TMap<FString, FString>& OutMap);

	void InitiateMovementAndAction();

	virtual void Tick(float DeltaSeconds) override;

	float time = 0.0f;
	float ActionTimer = 6.0f;

	//UPROPERTY(Replicated)
	bool bDoAction = false;

	bool bActionInitilize = false;
	float DoActionAtTime;

	UPROPERTY(BlueprintReadWrite)
	bool bReady;

	//UPROPERTY(Replicated)
	bool bMove=false;
	

	bool bHasMoved = false;
	FVector StartPosition = FVector(0.0f);
	FVector CurrentPosition;
	FVector FinalPosition;

	//Movement function
	UFUNCTION(Reliable, Server, WithValidation)
	void Server_Movement(FVector MovePosition);

	void Movement(FVector MovePosition);


	//Attack function
	UFUNCTION(Reliable, Server, WithValidation)
	void Server_Attack(ACube_TheBattleMasterPawn* Pawn, AItemBase* Item, FVector BlockPosition);

	void Attack(ACube_TheBattleMasterPawn* Pawn, AItemBase* Item, FVector BlockPosition);

	void EndAction(AItemBase * Item);


	UPROPERTY(Replicated, EditAnywhere, BlueprintReadOnly, Category = "Projectile Type")
	TSubclassOf<ASmallMunition> SmallMunitionClass;

	//Called by Server movement and vicaversa
	//UPROPERTY(Replicated)
	ACube_TheBattleMasterPawn* MyPawn ;
	void SetOwningPawn(ACube_TheBattleMasterPawn * NewOwner);

	

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

	FORCEINLINE class UAttachmentComponent* GetAttachComponent() const { return AttachmentComponent; }

	/** Returns Camera subobject **/
	FORCEINLINE class UCameraComponent* GetCamera() const { return OurCamera; }
};
