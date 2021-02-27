// Fill out your copyright notice in the Description page of Project Settings.

#include "Shield_Basic.h"
#include "../Player_Cube.h"
#include "../Cube_TheBattleMasterPawn.h"
#include "../Cube_TheBattleMasterBlock.h"
#include "../ShieldSegment.h"


AShield_Basic::AShield_Basic() {

	struct FConstructorStatics
	{
		ConstructorHelpers::FObjectFinderOptional<UStaticMesh> PlaneMesh;
		ConstructorHelpers::FObjectFinderOptional<UMaterial> BaseMaterial;
		FConstructorStatics()
			: PlaneMesh(TEXT("/Game/Puzzle/Meshes/PuzzleCube.PuzzleCube"))
			, BaseMaterial(TEXT("/Game/Puzzle/Meshes/BaseMaterial.BaseMaterial"))
		{
		}
	};
	static FConstructorStatics ConstructorStatics;

	BlockMesh->SetStaticMesh(ConstructorStatics.PlaneMesh.Get());

	BaseMaterial = ConstructorStatics.BaseMaterial.Get();

	WeaponName = "Shield";

	ActionName = "Shield On";

	bCanBeUsedWithMovement = true;

	ActionItteration = 0;

	bShield = false;

	bEndAction = false;

	bDirectional = true;
}

void AShield_Basic::DoAction(bool bMainPhase, FVector Direction) 
{	
	ACube_TheBattleMasterPawn* PlayerPawn = Cast<ACube_TheBattleMasterPawn>(GetOwner());
	APlayer_Cube* Cube = PlayerPawn->MyCube;

	if (Cube->E_TurnStateEnum == ETurnState::TS_InitiateActions) {
		if (ActionName == "Shield Off") { ShieldOff(); }
		else if (ActionName == "Shield On") { ShieldOn(); }
	}
	ShieldToggle();

	if (Cube->E_TurnStateEnum == ETurnState::TS_InitiateActions && GetLocalRole()<ROLE_Authority) {
		if (ActionName == "Shield Off")
		{
			ActionName = "Shield On";
		}
		else if (ActionName == "Shield On")
		{
			ActionName = "Shield Off";
		}
	}


	//if (bShield) {
	//	T_ShieldSegments.Empty();
	//}
}



void AShield_Basic::SetActionInMotion() {
	UE_LOG(LogTemp, Warning, TEXT("Set: %s"), *ActionName);
	ACube_TheBattleMasterPawn* PlayerPawn = Cast<ACube_TheBattleMasterPawn>(GetOwner());
	if (ActionName == "Shield Off")
	{
		ShieldOff();
	}
	else if(ActionName == "Shield On")
	{
		ShieldOn();
	}
	PlayerPawn->Confirmation_Button_Active(true);
	
}

void AShield_Basic::UnSetActionInMotion() {
	UE_LOG(LogTemp, Warning, TEXT("UnSet: %s"), *ActionName);
	ACube_TheBattleMasterPawn* PlayerPawn = Cast<ACube_TheBattleMasterPawn>(GetOwner());
	if (ActionName == "Shield Off")
	{
		ShieldOn();
	}
	else if (ActionName == "Shield On")
	{
		ShieldOff();
	}
	PlayerPawn->Confirmation_Button_Active(false);
	
}

void AShield_Basic::ShieldToggle_Implementation() {
	if (ActionName == "Shield On")
	{
		ActionName = "Shield Off";
	}
	else if (ActionName == "Shield Off")
	{
		ActionName = "Shield On";
	}
	
}

//void AShield_Basic::

void AShield_Basic::ShieldOn()
{
	ACube_TheBattleMasterPawn* PlayerPawn = Cast<ACube_TheBattleMasterPawn>(GetOwner());
	APlayer_Cube* Cube = PlayerPawn->MyCube;

	if (Cube->E_TurnStateEnum == ETurnState::TS_SelectActions) {
		if (ActionName == "Shield On") { MakeShield(false); }
		if (ActionName == "Shield Off") {
			//for (auto& Elems : T_ShieldSegments){
			for (TObjectIterator<AShieldSegment> Elems; Elems; ++Elems) {
				//if (Elems->GetOwner() == this) {
					Elems->SetActorHiddenInGame(false);
					//Elems->SetHidden(false);
					//Elems->SetActorLocation(GetActorLocation()+FVector(0.f, 0.f, 200.f));
				//Elems->SetActorRelativeLocation(-PlayerPawn->GetActorLocation());
					UE_LOG(LogTemp, Warning, TEXT("boooo"));
				//}
			}T_HiddenShieldSegments.Empty();
		}
	}
	if (Cube->E_TurnStateEnum == ETurnState::TS_InitiateActions) {
		if (GetLocalRole() < ROLE_Authority) { Server_ShieldOn(); }
		else { MakeShield(true); }
	}


}

void AShield_Basic::Server_ShieldOn_Implementation() { ShieldOn(); }

void AShield_Basic::MakeShield(bool bIsReplicating)
{
	bShield = true;
	ACube_TheBattleMasterPawn* PlayerPawn = Cast<ACube_TheBattleMasterPawn>(GetOwner());
	APlayer_Cube* Cube = PlayerPawn->MyCube;

	AShieldSegment* ShieldSegment;
	FRotator SpawnRotation = FRotator(0.f);
	FVector SpawnLocation = FVector(0.f, 0.f, 200.f);
	FAttachmentTransformRules Trans = FAttachmentTransformRules
	(
		EAttachmentRule::KeepRelative,
		EAttachmentRule::KeepRelative,
		EAttachmentRule::KeepRelative,
		true
	);
	
	if (ShieldClass) {
		ShieldSegment = GetWorld()->SpawnActor<AShieldSegment>(ShieldClass, SpawnLocation, SpawnRotation); 
	}
	else {
		ShieldSegment = GetWorld()->SpawnActor<AShieldSegment>(SpawnLocation, SpawnRotation); 
	}
	ShieldSegment->SetOwner(GetOwner());
	ShieldSegment->ItemOwner = this;
	//FName SlotName = "S_Side5";
	//if (bDirectional) {
		//EAttachPoint_Enum;
		//FName SlotName = UCustomFunctions::GetSocketNameByPoint(CurrentAttachPoint);
		ShieldSegment->SetActorRelativeScale3D(FVector(2.2f));
//		UE_LOG(LogTemp, Warning, TEXT("SlotName: %s"), *SlotName.ToString());
	//}

	//ShieldSegment->AttachToComponent(PlayerPawn->MyCube->GetBlockMesh(), Trans, SlotName);
	ShieldSegment->AttachToComponent(PlayerPawn->MyCube->GetBlockMesh(), Trans, GetAttachParentSocketName());

	//UE_LOG(LogTemp, Warning, TEXT("SlotName2: %s"), *ShieldSegment->GetAttachParentSocketName().ToString());
	
	
	ShieldSegment->SetReplicates(bIsReplicating);
	if (!bIsReplicating) { T_ShieldSegments.Add(ShieldSegment); }
	else { ShieldSegment->DoTheCurve(); //T_ShieldSegments.Add(ShieldSegment);
	}
}

void AShield_Basic::ShieldOff()
{
	ACube_TheBattleMasterPawn* PlayerPawn = Cast<ACube_TheBattleMasterPawn>(GetOwner());
	APlayer_Cube* Cube = PlayerPawn->MyCube;
	bShield = false;
	if (Cube->E_TurnStateEnum == ETurnState::TS_SelectActions) {
		if (ActionName == "Shield Off") {
			//UE_LOG(LogTemp, Warning, TEXT("Test2"));
			//for (auto& Elems : T_ShieldSegments) {
			for (TObjectIterator<AShieldSegment> Elems; Elems; ++Elems) {
				//if (Elems->ItemOwner == this) {
					//Elems->Destroy();
					Elems->SetReplicates(false);
					T_HiddenShieldSegments.Add(*Elems);

					Elems->SetActorHiddenInGame(true);
					//Elems->SetActorRelativeLocation(PlayerPawn->GetActorLocation());
					
				//}
			}
		}
		else if (ActionName == "Shield On") {
			//UE_LOG(LogTemp, Warning, TEXT("Test3"));

			for (auto& Elems : T_ShieldSegments) {
				//UE_LOG(LogTemp, Warning, TEXT("Test: %s"), *Elems->GetName());
				//Elems->SetActorHiddenInGame(false); 
				//GetWorld()->RemoveActor(Elems, false); 
				Elems->Destroy();
			}
			T_ShieldSegments.Empty();
		}
	}
	if (Cube->E_TurnStateEnum == ETurnState::TS_InitiateActions) { 
		if (GetLocalRole() < ROLE_Authority) { Server_ShieldOff(); }
		else {
			//for (auto& Elems : T_ShieldSegments) {
				for (TObjectIterator<AShieldSegment> Elems; Elems; ++Elems) {
					if (Elems->ItemOwner == this) {
						//GetWorld()->RemoveActor ( *Elems, false );
						//Elems->Destroy();
						Elems->LetsDestroy();
						//Elems->SetActorHiddenInGame(true);
					}
				}
		}
	}
}

void AShield_Basic::Server_ShieldOff_Implementation(){ ShieldOff(); }

void AShield_Basic::ResetAction() {
	ACube_TheBattleMasterPawn* PlayerPawn = Cast<ACube_TheBattleMasterPawn>(GetOwner());

	UE_LOG(LogTemp, Warning, TEXT("Reset Shield"));
	if (T_ShieldSegments.Num() != 0)
	{
		for (auto& Elems : T_ShieldSegments) { Elems->Destroy(); }// GetWorld()->RemoveActor(Elems, false);
	
		T_ShieldSegments.Empty();
	}
	if (T_HiddenShieldSegments.Num() != 0)
	{
		for (auto& Elems : T_HiddenShieldSegments) {
			Elems->SetActorHiddenInGame(false);
			Elems->SetReplicates(true);
			UE_LOG(LogTemp, Warning, TEXT("TEST"));
		}T_HiddenShieldSegments.Empty();
		//ActionName = "Shield On";
	}
	//else { ActionName = "Shield Off"; }
	ShieldToggle();
}