// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "GameFramework/RotatingMovementComponent.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"


#include "Cube_TheBattleMasterBlock.h"
#include "Cube_TheBattleMasterGameMode.h"
#include "Cube_TheBattleMasterPlayerController.h"
#include "SmallMunition.h"
#include "ItemBase.h"
//#include "AttackComponent.h"
#include "AttachmentComponent.h"

#include "Player_Cube.generated.h"


USTRUCT(BlueprintType)
struct FToDo_Struct
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	AItemBase* Item = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector Movement = FVector(0.f);;

	UPROPERTY()
	AActor* SelectedActor = nullptr;

	UPROPERTY()
	FVector SupplementaryVec = FVector(0.f);

	UPROPERTY()
	float Rotation = 0.f;
};


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

	UPROPERTY(Category = "PlayerCube", BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	class UProjectileMovementComponent* MoveComponent;

	UPROPERTY(Category = "PlayerCube", BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	class URotatingMovementComponent* RotMoveComponent;

public:	
	UPROPERTY(Category = "Attachments", BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	TMap <FString, AItemBase*>  M_SlotsRefference;


	// Sets default values for this actor's properties
	APlayer_Cube();

	void ReadyTheCube(bool bIsReady);

	UFUNCTION(Server, reliable)
	void Server_ReadyTheCube(bool bIsReady);


	//virtual void BeginPlay() override;

	ETurnState E_TurnStateEnum;
	virtual void OnConstruction(const FTransform & Transform) override;

	UFUNCTION(BlueprintCallable)
	void SetCameraView();


	void UpdateActionList(TMap<FString, FString>& OutMap);

	void InitiateMovementAndAction();

	UFUNCTION(Server, Reliable)
	void Server_InitiateMovementAndAction();

	AActor* FindANearByBlock();

	virtual void Tick(float DeltaSeconds) override;

	UFUNCTION()
	void DoTheRotation();

	UFUNCTION()
	void DoTheMovement();

	UFUNCTION(Server, Reliable)
	void Server_DoTheMovement();

	UFUNCTION()
	void DoTheAction();

	UPROPERTY(Replicated);
	float time = 0.0f;
	float ActionTimer = 6.0f;

	float EndTimer = 6.0f;


	void SetCubeVelocity(FVector Vel);

	UFUNCTION(Server, Reliable)
	void Server_SetCubeVelocity(FVector Vel);

	float PreviousSubtraction;
	FVector Vec_PreviousSubtraction;
	//Get The action to be done
	UPROPERTY(Replicated)
	FToDo_Struct Action;

	UFUNCTION(Server, Reliable)
	void Server_SetAction(AItemBase* Item, FVector Move, AActor* Actor, float Rot, FVector Vec);

	void SetAction(AItemBase* Item, FVector Move, AActor* Actor, float Rot, FVector Vec);


	//UPROPERTY(Replicated)
	bool bDoAction = false;

	bool bActionInitilize = false;
	float DoActionAtTime;

	UPROPERTY(BlueprintReadWrite, Replicated)
	bool bReady = true;

	UPROPERTY(Replicated)
	bool bMove=false;
	UPROPERTY(Replicated)
	bool bRotate = false;

	UPROPERTY(Replicated);
	bool bStartTimer = false;

	bool bHasMoved = false;

	UPROPERTY(Replicated)
	FVector StartPosition = FVector(0.0f);

	UPROPERTY(Replicated)
	FRotator StartRotation = FRotator(0.f);
	FRotator LocalRotation;

	FVector CurrentPosition;
	FVector FinalPosition;

	//Are these required when adding external rotation and veloctiy (i.e. attacks) so that we don't stop .. how do we want to play this?
	int CurrentRotation = 0;
	FVector CurrentVelocity = FVector(0.f);


	//Attack function
	UFUNCTION(Reliable, Server, WithValidation)
	void Server_Attack(ACube_TheBattleMasterPawn* Pawn, AItemBase* Item, FVector AttackPosition);

	void Attack(ACube_TheBattleMasterPawn* Pawn, AItemBase* Item, FVector AttackPosition);

	UFUNCTION(Reliable, Server)
	void Server_EndAction(AItemBase * Item);

	void EndAction(AItemBase * Item);

	UFUNCTION(BlueprintCallable) void OnRep_ChangeColour();
	
	UFUNCTION(BlueprintCallable)
	void SetCubeDetails(FImportedCube_Struct SCube, TMap<FName, TSubclassOf<AItemBase>> AttachedItems);

	UFUNCTION(Server, Reliable)
	void Server_DoTheRotation();

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

	UPROPERTY()
	class UMaterialInstanceDynamic* VariedCubeMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString ThisCubeName;

	/* All the base variables */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Base_Health;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Base_Speed=4;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Base_RotationSpeed = 0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 AttackRange = 7;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Base_Damage = 10.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Base_Armour = 1.0;

	UPROPERTY(BlueprintReadWrite)
	float BasicEnergy = 0.f;


	/* Replicated variables */

	UPROPERTY(Replicated, EditAnywhere)
	float Replicated_Health;

	UPROPERTY(Replicated, EditAnywhere)
	int32 Replicated_Speed;

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite)
	float Replicated_RotationSpeed = 0.5f;

	UPROPERTY(Transient, BlueprintReadWrite, EditAnywhere, ReplicatedUsing = OnRep_ChangeColour)
	FLinearColor CubeColour;

	UPROPERTY(Transient, BlueprintReadWrite, ReplicatedUsing = OnRep_ChangeEnergy)
	float Replicated_BasicEnergy = 0.f;

	UFUNCTION()	void OnRep_ChangeEnergy();

	UPROPERTY(EditAnywhere, Replicated)
	ACube_TheBattleMasterBlock* BlockOwner;

	/* GamePlay functions */

	UFUNCTION()
	void ApplyDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

	UFUNCTION()
	void ApplyRadialEffects(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, FVector Origin, FHitResult HitInfo, class AController* InstigatedBy, AActor* DamageCauser);

	bool bNeedsDamage = false;

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
