// Fill out your copyright notice in the Description page of Project Settings.

#include "BasicMiner.h"
#include "../Cube_TheBattleMasterPawn.h"
#include "../Cube_TheBattleMasterBlockGrid.h"



ABasicMiner::ABasicMiner() {

	WeaponName = "Extractor";

	ActionName = "Mine";


	AttackRange = 1.f;
	AttackRangeMin = 1.f;

	ActionItteration = 0.f;

	bCanBeUsedWithMovement = false;
	bEndAction = true;
}

void ABasicMiner::BeginPlay() {

	Super::BeginPlay();

	MyOwner = Cast<ACube_TheBattleMasterPawn>(GetOwner());
}

void ABasicMiner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bGetEnergy) {

		TSet<AActor*> OverlappingActors;
		TSubclassOf<ACube_TheBattleMasterBlock> BlockClass;
		GetOverlappingActors(OverlappingActors, BlockClass);

		ACube_TheBattleMasterBlock* MyBlock = nullptr;
		float EnergyVal;
		for (auto Elems : OverlappingActors) {
			ACube_TheBattleMasterBlock* ElemsBlock = Cast< ACube_TheBattleMasterBlock>(Elems);
			if (!MyBlock) { MyBlock = ElemsBlock; EnergyVal = ElemsBlock->BasicEnergy; }
			else if (ElemsBlock->BasicEnergy < EnergyVal) { MyBlock = ElemsBlock; }
		}

		//MyBlock = MyOwner->GetBlockFromPosition(Cube->GetActorLocation());
		//Change to an overlap so that if we are moved it will still collect
		if (MyBlock) {
			float DummyEnergyVal = MyBlock->BasicEnergy * 0.1 / 5.7 * DeltaTime;
			//float DummyEnergyVal = MyBlock->BasicEnergy * 0.1 / 6 * DeltaTime;

			Cube->Replicated_BasicEnergy += DummyEnergyVal;
			
			for (TObjectIterator<ACube_TheBattleMasterBlockGrid> Grid; Grid; ++Grid) {
				for (auto Elems : Grid->Grid) {
					if (Elems.Value->GetName() == MyBlock->GetName()) {
						Elems.Value->SetEnergyVariables(MyBlock->BasicEnergy - DummyEnergyVal);
						break;
					}
				}
			}
		}
		
		//bGetEnergy = false;
	}
		
	
}

void ABasicMiner::DoAction(bool bMainPhase, FVector Direction) {
	UE_LOG(LogTemp, Warning, TEXT("DoAction"));

	//Extract(false);
	if (Cube->E_TurnStateEnum == ETurnState::TS_InitiateActions) {
		Extract(false);
		UE_LOG(LogTemp, Warning, TEXT("TS_InitiateActions"));

	}else if (Cube->E_TurnStateEnum == ETurnState::TS_SelectActions){
		Extract(true);
		UE_LOG(LogTemp, Warning, TEXT("TS_SelectActions"));

	}
}

void ABasicMiner::ResetAction() {

	Cube = MyOwner->MyCube;
	Cube->BasicEnergy = 0;

	/*if (Cube->BasicEnergy != Cube->Replicated_BasicEnergy) {
		Cube->BasicEnergy = Cube->Replicated_BasicEnergy;
	}*/
}

void ABasicMiner::SetActionInMotion() {
	UE_LOG(LogTemp, Warning, TEXT("Set action"));
	Extract(true);
	MyOwner->Confirmation_Button_Active(true);
	bRemovePotentialEnergy = true;
	
}

void ABasicMiner::UnSetActionInMotion() {
	UE_LOG(LogTemp, Warning, TEXT("unset action"));
	UnExtract();
	bRemovePotentialEnergy = false;
}

bool ABasicMiner::CheckDummyEnergyIsCorrect() {

	/*Cube = MyOwner->MyCube;
	ACube_TheBattleMasterBlock* MyBlock = MyOwner->GetBlockFromPosition(Cube->GetActorLocation());

	if (MyBlock->DummyEnergy == 0) { return true; }*/

	for (auto Elems : MyOwner->M_ActionStructure) { if (Elems.Value.Action_Name == "Mine") { return false; } }

	

	return true;
}

void ABasicMiner::Extract(bool bTemporary) {
	
	if (!Cube){	Cube = MyOwner->MyCube; }
	
	
	/*
	FVector Direction = Cube->GetActorLocation();
	FVector LocalDirection = FVector(Direction.X, Direction.Y, 0);

	if (MyOwner->GetServerGrid()->GridReference.Contains(LocalDirection))
	{
		return MyOwner->GetServerGrid()->Grid.FindRef(MyOwner->GetServerGrid()->GridReference.FindRef(LocalDirection));
	}*/

	float DummyEnergyVal ;
	//DummyEnergyVal = 100;
	if (bTemporary) {
		ACube_TheBattleMasterBlock* MyBlock = MyOwner->GetBlockFromPosition(Cube->GetActorLocation());
		if (CheckDummyEnergyIsCorrect()) { MyBlock->DummyEnergy = MyBlock->BasicEnergy; }

		DummyEnergyVal = MyBlock->DummyEnergy * 0.1;

		Cube->BasicEnergy += DummyEnergyVal;
		MyBlock->DummyEnergy -= DummyEnergyVal;
	}
	else {
		bGetEnergy = true;

		///////////
		/*
		DummyEnergyVal = MyBlock->BasicEnergy * 0.1;

		Cube->Replicated_BasicEnergy += DummyEnergyVal;
		//Cube->BasicEnergy += DummyEnergyVal;

		MyBlock->BasicEnergy -= DummyEnergyVal;
		*/
	}
	if (MyOwner) { MyOwner->RefreshEnergy(); }
	//else { Cube->MyPawn->RefreshEnergy(); }
	//FindOwner();
}

//void ABasicMiner::FindOwner() {
//	if (!MyOwner) { NetMulticast_FindOwner(); }
//}
//
//void ABasicMiner::NetMulticast_FindOwner_Implementation() {
//	if (MyOwner) {
//		UE_LOG(LogTemp, Warning, TEXT("Owner is %s"), *MyOwner->GetName());
//
//		MyOwner->RefreshEnergy();
//	}
//}

void ABasicMiner::UnExtract() {

	MyOwner->Confirmation_Button_Active(false);

	Cube = MyOwner->MyCube;
	ACube_TheBattleMasterBlock* MyBlock = MyOwner->GetBlockFromPosition(Cube->GetActorLocation());

	float DummyEnergyVal = MyBlock->DummyEnergy / 9.f;

	//DummyEnergyVal = 100;
	Cube->BasicEnergy -= DummyEnergyVal;
	MyBlock->DummyEnergy += DummyEnergyVal;

	MyOwner->RefreshEnergy();
}

void ABasicMiner::EndAction(){

	bGetEnergy = false;
}