// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Move.generated.h"

UENUM(BlueprintType)
enum class EMoveType : uint8
{
    EMT_Neutral UMETA(DisplayName = "Neutral"),
    EMT_Offensive UMETA(DisplayName = "Offensive"),
    EMT_Defensive UMETA(DisplayName = "Defensive")
};

USTRUCT(BlueprintType)
struct FMoveData {
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Visuals")
    FText MoveName = FText();
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Visuals")
    FText MoveDescription = FText();
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Visuals")
    UTexture2D* MoveThumbnail = nullptr;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Visuals")
    UAnimMontage* MoveMontage = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    EMoveType MoveType = EMoveType::EMT_Neutral;
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    float BaseStaminaCost = 0.f;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (EditCondition = "MoveType != EMoveType::EMT_Offensive"))
    float MontageDelay = 1.0f;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (EditCondition = "MoveType == EMoveType::EMT_Offensive"))
    float BaseDamage = 0.f;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (EditCondition = "MoveType == EMoveType::EMT_Offensive"))
    UMove* AppropriateHitReaction;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (EditCondition = "MoveType == EMoveType::EMT_Offensive"))
    UMove* AppropriateDefenceReaction;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (EditCondition = "MoveType == EMoveType::EMT_Offensive"))
    UMove* AppropriateKnockOutReaction;
};

UCLASS()
class PROJEKTA_API UMove : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FMoveData MoveData;
};
