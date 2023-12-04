// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FightManager.generated.h"

class AHuman;
class UMove;
class UCameraComponent;

UCLASS()
class PROJEKTA_API AFightManager : public AActor
{
	GENERATED_BODY()
	
public:	
	AFightManager();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	USceneComponent* FightPosition;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	UCameraComponent* Camera;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float FightDistanceInCentimeters = 100.0f;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	AHuman* FirstBoxer;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	AHuman* SecondBoxer;
	UFUNCTION(BlueprintCallable)
	void PositionBoxers();
	UFUNCTION()
	void BindBoxersDelegates();
	UFUNCTION(BlueprintCallable)
	void PerformBoxersFidgetMoves();
	UFUNCTION()
	void OrderRivalPerformWinnerReaction(AHuman* Boxer);

	UFUNCTION(BlueprintCallable)
	void StartTheFight();
	UFUNCTION(BlueprintCallable)
	void StopTheFight();
	UPROPERTY(BlueprintReadOnly)
	bool bFightFinished = false;

	UFUNCTION(BlueprintCallable)
	void OnBoxerMadeMove(UMove* MoveAsset, bool bFinishSequence, AHuman* Boxer);
	UFUNCTION(BlueprintCallable)
	void OnBoxerKnockedOut(AHuman* Boxer);
	UFUNCTION(BlueprintCallable)
	AHuman* GetRival(AHuman* Boxer);

	UFUNCTION(BlueprintCallable)
	void BeginSequence();
	UFUNCTION()
	float GetAbsoluteAggressivenessAdvantage();
	UFUNCTION()
	void MakeRandomOffensiveMove(AHuman* Boxer);
	UFUNCTION()
	AHuman* DrawBoxer();
public:	
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void OrderStartTheFight();
};
