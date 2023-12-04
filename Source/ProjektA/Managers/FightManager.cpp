// Fill out your copyright notice in the Description page of Project Settings.


#include "FightManager.h"
#include "ProjektA/Characters/Human.h"
#include "Kismet/KismetMathLibrary.h"
#include "Camera/CameraComponent.h"

AFightManager::AFightManager()
{
	PrimaryActorTick.bCanEverTick = true;
	FightPosition = CreateDefaultSubobject<USceneComponent>(TEXT("Fight location"));
	SetRootComponent(FightPosition);
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(FightPosition);
}

void AFightManager::BeginPlay()
{
	Super::BeginPlay();
	
}

void AFightManager::PositionBoxers()
{
	if (!FirstBoxer || !SecondBoxer) return;

	FVector FirstBoxerTargetLocation = FightPosition->GetComponentLocation() + FVector(FightDistanceInCentimeters / 2.0f, // X
									   0.0f, // Y
									   FightPosition->GetComponentLocation().Z + 88.0f); // Z
	FRotator FirstBoxerTargetRotation = FRotator(0.f, // PITCH
										UKismetMathLibrary::FindLookAtRotation(FirstBoxer->GetActorLocation(), FightPosition->GetComponentLocation()).Yaw, //YAW
										0.f); // ROLL
	FirstBoxer->SetActorLocation(FirstBoxerTargetLocation);
	FirstBoxer->SetActorRotation(FirstBoxerTargetRotation);

	FVector SecondBoxerTargetLocation = FightPosition->GetComponentLocation() + FVector(-FightDistanceInCentimeters / 2.0f, // X
										0.0f, // Y
										FightPosition->GetComponentLocation().Z + 88.0f); // Z
	FRotator SecondBoxerTargetRotation = FRotator(0.f, // PITCH
										 UKismetMathLibrary::FindLookAtRotation(SecondBoxer->GetActorLocation(), // YAW
										 FightPosition->GetComponentLocation()).Yaw, 0.f); // ROLL
	SecondBoxer->SetActorLocation(SecondBoxerTargetLocation);
	SecondBoxer->SetActorRotation(SecondBoxerTargetRotation);
}

void AFightManager::BindBoxersDelegates()
{
	if (FirstBoxer)
	{
		FirstBoxer->OnMoveMade.AddDynamic(this, &AFightManager::OnBoxerMadeMove);
		FirstBoxer->OnBoxerKnockedOut.AddDynamic(this, &AFightManager::OnBoxerKnockedOut);
	}
	if (SecondBoxer)
	{
		SecondBoxer->OnMoveMade.AddDynamic(this, &AFightManager::OnBoxerMadeMove);
		SecondBoxer->OnBoxerKnockedOut.AddDynamic(this, &AFightManager::OnBoxerKnockedOut);
	}
}

void AFightManager::PerformBoxersFidgetMoves()
{
	FirstBoxer->PerformFidgetMove();
	SecondBoxer->PerformFidgetMove();
}

void AFightManager::OrderRivalPerformWinnerReaction(AHuman* Boxer)
{
	GetRival(Boxer)->PerformWinnerReaction();
}

void AFightManager::StartTheFight()
{
	bFightFinished = false;
	BindBoxersDelegates();
	BeginSequence();
}

void AFightManager::StopTheFight()
{
	bFightFinished = true;
}

void AFightManager::OnBoxerMadeMove(UMove* MoveAsset, bool bFinishSequence, AHuman* Boxer)
{
	if (bFinishSequence) BeginSequence();
	else GetRival(Boxer)->CallbackMakeMove(MoveAsset);
}

void AFightManager::OnBoxerKnockedOut(AHuman* Boxer)
{
	StopTheFight();

	FTimerHandle TimerHandle;
	FTimerDelegate TimerDelegate;
	TimerDelegate.BindUFunction(this, FName("OrderRivalPerformWinnerReaction"), Boxer);

	GetWorld()->GetTimerManager().SetTimer(TimerHandle, TimerDelegate, 2.4f, false);
}

AHuman* AFightManager::GetRival(AHuman* Boxer)
{
	if (Boxer == FirstBoxer) return SecondBoxer;
	else if (Boxer == SecondBoxer) return FirstBoxer;
	else return nullptr;
}

void AFightManager::BeginSequence()
{
	if (bFightFinished) return;
	
	MakeRandomOffensiveMove(DrawBoxer());
}

float AFightManager::GetAbsoluteAggressivenessAdvantage()
{
	float AggressivenessAdvantage = FMath::Abs(FirstBoxer->GetBoxerStats().Aggressiveness - SecondBoxer->GetBoxerStats().Aggressiveness);
	return AggressivenessAdvantage;
}

void AFightManager::MakeRandomOffensiveMove(AHuman* Boxer)
{
	Boxer->MakeMove(Boxer->ChooseRandomMove(Boxer->AttackMovesPool));
}

AHuman* AFightManager::DrawBoxer()
{
	bool bIsFirstBoxerMoreAggressive = FirstBoxer->GetBoxerStats().Aggressiveness > SecondBoxer->GetBoxerStats().Aggressiveness;
	float Chance = FMath::Clamp(0.5f + (bIsFirstBoxerMoreAggressive ? GetAbsoluteAggressivenessAdvantage() : (-1 * GetAbsoluteAggressivenessAdvantage())), 0.f, 1.f);

	if (UKismetMathLibrary::RandomBoolWithWeight(Chance)) return FirstBoxer;
	else return SecondBoxer;
}

void AFightManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

