// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "BattleGameState.generated.h"


class UUserWidget;

UCLASS()
class BATTLECITY_API ABattleGameState : public AGameStateBase
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Teams", meta = (AllowPrivateAccess = true))
	TMap<int32, FLinearColor> TeamColors;

	UPROPERTY(EditDefaultsOnly, Category = "Widget")
	TSubclassOf<UUserWidget> UserWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widget", meta = (AllowPrivateAccess = "true"))
	UUserWidget* UserWidget;

public:
	ABattleGameState();

	FLinearColor GetTeamColor(int32 TeamId);

	void ActorDied(AActor* Actor);

protected:

	virtual void BeginPlay() override;

	void SetupWidget();

	UFUNCTION(BlueprintImplementableEvent)
	void PlayFadeAnimation(int32 LosingTeamIndex);

	void EndLevel(int32 LosingTeamIndex);

	void RestartLevel();

	void DisableLosingTeam(int32 LosingTeamIndex);

	UFUNCTION(NetMulticast, Reliable)
	void DisableCharacter(ABattleCharacter* CharacterToDisable);
	virtual void DisableCharacter_Implementation(ABattleCharacter* CharacterToDisable);

	
};
