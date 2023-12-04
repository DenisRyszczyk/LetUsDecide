// Fill out your copyright notice in the Description page of Project Settings.


#include "Human.h"
#include "Move.h"
#include "Kismet/KismetMathLibrary.h"

AHuman::AHuman()
{
	PrimaryActorTick.bCanEverTick = true;

}

void AHuman::BeginPlay()
{
	Super::BeginPlay();
}

void AHuman::MakeMove(UMove* MoveAsset)
{
	if (!HasEnoughStamina(GetFinalStaminaCost(MoveAsset)) && IsMoveOffensive(MoveAsset))
	{
		SkipMove();
		return;
	}

	SubtractStamina(GetFinalStaminaCost(MoveAsset));

	PlayMoveMontage(MoveAsset);

	if (!IsMoveOffensive(MoveAsset)) SetTimerForCallbackOnMoveMade(MoveAsset);
	else OnMoveMade.Broadcast(MoveAsset, !IsMoveOffensive(MoveAsset), this);
}

void AHuman::OrderMakeMove(UMove* MoveAsset)
{
	FTimerDelegate TimerDelegate;
	FTimerHandle TimerHandle;
	TimerDelegate.BindUFunction(this, FName("MakeMove"), MoveAsset);

	if (MoveAsset->MoveData.MontageDelay == 0.0f) MakeMove(MoveAsset);
	else GetWorld()->GetTimerManager().SetTimer(TimerHandle, TimerDelegate, MoveAsset->MoveData.MontageDelay, false);
}

void AHuman::CallbackMakeMove(UMove* MoveAsset)
{
	if (ShouldBoxerDefend())
	{
		if (CanDefendHit(MoveAsset)) DefendHit(MoveAsset);
		else TakeHit(MoveAsset);
	}
	else TakeHit(MoveAsset);
}

bool AHuman::CanDefendHit(UMove* MoveAsset)
{
	return DefenceMovesPool.Contains(MoveAsset->MoveData.AppropriateDefenceReaction);
}

void AHuman::TakeHit(UMove* MoveAsset)
{
	SubtractHealth(MoveAsset);
	if (ShouldBeKnockedOut()) GetKnockedOut(MoveAsset);
	else OrderMakeMove(MoveAsset->MoveData.AppropriateHitReaction);
}

void AHuman::DefendHit(UMove* MoveAsset)
{
	OrderMakeMove(MoveAsset->MoveData.AppropriateDefenceReaction);
}

bool AHuman::ShouldBeKnockedOut()
{
	return Health <= 0.0f;
}

void AHuman::GetKnockedOut(UMove* MoveAsset)
{
	OrderMakeMove(MoveAsset->MoveData.AppropriateKnockOutReaction);
	bKnockedOut = true;
	OnBoxerKnockedOut.Broadcast(this);
}

void AHuman::PerformWinnerReaction()
{
	PlayMoveMontage(ChooseRandomMove(WinnerMovesPool));
}

void AHuman::PerformFidgetMove()
{
	PlayMoveMontage(ChooseRandomMove(FidgetMovesPool));
}

float AHuman::GetStamina()
{
	return Stamina;
}

float AHuman::GetHealth()
{
	return Health;
}

FBoxerStats AHuman::GetBoxerStats()
{
	return Stats;
}

void AHuman::SetBoxerToughness(float Value)
{
	Stats.Toughness = Value;
}

void AHuman::AutoRegenerateHealthAndStamina()
{
	float DeltaTime = GetWorld()->GetDeltaSeconds();

	AddStamina(Stats.BaseStaminaRegenPerSecond * DeltaTime);
	AddHealth(Stats.BaseHealthRegenPerSecond * DeltaTime);
}

void AHuman::SubtractStamina(float Value)
{
	Stamina = FMath::Clamp(Stamina - Value, 0.0f, 1.0f);
	OnStaminaUpdated.Broadcast(Stamina, Stamina + Value);
}

void AHuman::AddStamina(float Value)
{
	Stamina = FMath::Clamp(Stamina + Value, 0.0f, 1.0f);
	OnStaminaUpdated.Broadcast(Stamina, Stamina - Value);
}

void AHuman::SubtractHealth(UMove* MoveAsset)
{
	Health = FMath::Clamp(Health - MoveAsset->MoveData.BaseDamage * (1 - Stats.Toughness), 0.f, 1.0f);
	OnHealthUpdated.Broadcast(Health, Health + MoveAsset->MoveData.BaseDamage * (1 - Stats.Toughness));
}

void AHuman::AddHealth(float Value)
{
	Health = FMath::Clamp(Health + Value, 0.0f, 1.0f);
	OnHealthUpdated.Broadcast(Health, Health - Value);
}

bool AHuman::ShouldBoxerDefend()
{
	return UKismetMathLibrary::RandomBoolWithWeight(Stats.Timing);
}

UMove* AHuman::ChooseRandomMove(UPARAM(ref) TArray<UMove*>& Pool)
{
	if (!Pool.IsEmpty())
	{
		int32 Random = FMath::RandRange(0, Pool.Num() - 1);
		return Pool[Random];
	}
	else return nullptr;
}

void AHuman::PlayMoveMontage(UMove* MoveAsset)
{
	UAnimMontage* Montage = MoveAsset->MoveData.MoveMontage;
	UAnimInstance* AnimationBlueprint = GetMesh()->GetAnimInstance();

	if (AnimationBlueprint && Montage) AnimationBlueprint->Montage_Play(Montage);
}

void AHuman::CallbackOnMoveMade(UMove* MoveAsset)
{
	OnMoveMade.Broadcast(MoveAsset, !IsMoveOffensive(MoveAsset), this);
}

void AHuman::SetTimerForCallbackOnMoveMade(UMove* MoveAsset)
{
	FTimerDelegate TimerDelegate;
	FTimerHandle TimerHandle;
	TimerDelegate.BindUFunction(this, FName("CallbackOnMoveMade"), MoveAsset);
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, TimerDelegate, MoveAsset->MoveData.MoveMontage->GetPlayLength(), false);
}

bool AHuman::HasEnoughStamina(float StaminaCost)
{
	return Stamina >= StaminaCost;
}

float AHuman::GetFinalStaminaCost(UMove* MoveAsset)
{
	return MoveAsset->MoveData.BaseStaminaCost * (1 - Stats.Endurance);
}

void AHuman::SkipMove()
{
	AddStamina(SkipMoveStaminaBoostValue);
	if (SkipMoveAsset) OrderMakeMove(SkipMoveAsset);
}

bool AHuman::IsMoveOffensive(UMove* MoveAsset)
{
	return MoveAsset->MoveData.MoveType == EMoveType::EMT_Offensive;
}

void AHuman::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if(!bKnockedOut) AutoRegenerateHealthAndStamina();
}

void AHuman::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AHuman::AddMoveToPool(UMove* MoveAsset, UPARAM(ref) TArray<UMove*>& Pool)
{
	Pool.Add(MoveAsset);
}

void AHuman::RemoveMoveFromPool(UMove* MoveAsset, UPARAM(ref) TArray<UMove*>& Pool)
{
	if (Pool.Contains(MoveAsset)) Pool.RemoveSingle(MoveAsset);
}

