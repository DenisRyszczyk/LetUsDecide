// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Human.generated.h"

class UMove;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnMoveMade, UMove*, MoveAsset, bool, bFinishSequence, AHuman*, Boxer);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnStaminaUpdated, float, NewStamina, float, OldStamina);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnHealthUpdated, float, NewHealth, float, OldHealth);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBoxerKnockedOut, AHuman*, Boxer);

USTRUCT(BlueprintType)
struct FBoxerStats
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Identity")
	FText FirstName = FText();
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Identity")
	FText Nickname = FText();
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Identity")
	FText SecondName = FText();
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Identity")
	UTexture2D* FaceThumbnail = nullptr;

	// Let's try to keep values between 0 and 1.

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float Toughness = 0.f; // Reduces damage received.
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float Aggressiveness = 0.5f; // Attacking frequency.
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float Timing = 0.f; // Hit and defence chance.				
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float Endurance = 0.f; // Stamina loss divider.
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float BaseHealthRegenPerSecond = 0.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float BaseStaminaRegenPerSecond = 0.f;
};

UCLASS()
class PROJEKTA_API AHuman : public ACharacter
{
	GENERATED_BODY()

public:
	AHuman();

	UPROPERTY(BlueprintAssignable)
	FOnMoveMade OnMoveMade;
	UPROPERTY(BlueprintAssignable)
	FOnStaminaUpdated OnStaminaUpdated;
	UPROPERTY(BlueprintAssignable)
	FOnHealthUpdated OnHealthUpdated;
	UPROPERTY(BlueprintAssignable)
	FOnBoxerKnockedOut OnBoxerKnockedOut;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FBoxerStats Stats = FBoxerStats();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float SkipMoveStaminaBoostValue;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UMove* SkipMoveAsset;

	UPROPERTY(BlueprintReadOnly)
	float Stamina = 1.0f;
	UPROPERTY(BlueprintReadOnly)
	float Health = 1.0f;
	UPROPERTY(BlueprintReadWrite)
	bool bKnockedOut = false;
	UFUNCTION()
	void AutoRegenerateHealthAndStamina();
	UFUNCTION()
	void SubtractStamina(float Value);
	UFUNCTION()
	void AddStamina(float Value);
	UFUNCTION()
	void SubtractHealth(UMove* MoveAsset);
	UFUNCTION()
	void AddHealth(float Value);

	UFUNCTION()
	void PlayMoveMontage(UMove* MoveAsset);
	UFUNCTION()
	void CallbackOnMoveMade(UMove* MoveAsset);
	UFUNCTION()
	void SetTimerForCallbackOnMoveMade(UMove* MoveAsset);

	UFUNCTION(BlueprintCallable)
	bool HasEnoughStamina(float StaminaCost);
	UFUNCTION()
	float GetFinalStaminaCost(UMove* MoveAsset);
	UFUNCTION()
	void SkipMove();

	UFUNCTION(BlueprintCallable)
	bool IsMoveOffensive(UMove* MoveAsset);

public:	
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<UMove*> AttackMovesPool;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<UMove*> DefenceMovesPool;
	UPROPERTY(EditAnywhere)
	TArray<UMove*> WinnerMovesPool;
	UPROPERTY(EditAnywhere)
	TArray<UMove*> FidgetMovesPool;
	
	UFUNCTION(BlueprintCallable)
	void AddMoveToPool(UMove* MoveAsset, UPARAM(ref) TArray<UMove*>& Pool);
	UFUNCTION(BlueprintCallable)
	void RemoveMoveFromPool(UMove* MoveAsset, UPARAM(ref) TArray<UMove*>& Pool);

	UFUNCTION()
	bool ShouldBoxerDefend();
	UFUNCTION(BlueprintCallable)
	UMove* ChooseRandomMove(UPARAM(ref) TArray<UMove*>& Pool);
	UFUNCTION(BlueprintCallable)
	void MakeMove(UMove* MoveAsset);
	UFUNCTION()
	void OrderMakeMove(UMove* MoveAsset);
	UFUNCTION(BlueprintCallable)
	void CallbackMakeMove(UMove* MoveAsset);
	UFUNCTION()
	bool CanDefendHit(UMove* MoveAsset);
	UFUNCTION()
	void TakeHit(UMove* MoveAsset);
	UFUNCTION()
	void DefendHit(UMove* MoveAsset);
	UFUNCTION()
	bool ShouldBeKnockedOut();
	UFUNCTION()
	void GetKnockedOut(UMove* MoveAsset);
	UFUNCTION(BlueprintCallable)
	void PerformWinnerReaction();
	UFUNCTION(BlueprintCallable)
	void PerformFidgetMove();
	UFUNCTION(BlueprintPure, BlueprintCallable)
	float GetStamina();
	UFUNCTION(BlueprintPure, BlueprintCallable)
	float GetHealth();
	UFUNCTION(BlueprintPure, BlueprintCallable)
	FBoxerStats GetBoxerStats();
	UFUNCTION(BlueprintCallable)
	void SetBoxerToughness(float Value);
};
