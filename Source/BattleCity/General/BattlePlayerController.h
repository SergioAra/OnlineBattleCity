// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "BattlePlayerController.generated.h"

class ABattleCharacter;

UCLASS()
class BATTLECITY_API ABattlePlayerController : public APlayerController
{
	GENERATED_BODY()

	UPROPERTY(Replicated, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	ABattleCharacter* BattleCharacter;

	bool bMovingForward;

	bool bMovingRight;

public:

	ABattlePlayerController();

protected:

	virtual void BeginPlay() override;

	virtual void SetupInputComponent() override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void OnPossess(APawn* InPawn) override;

	void MoveForward(float Value);
	void MoveRight(float Value);

	UFUNCTION(Server, Reliable)
	void Fire();
	virtual void Fire_Implementation();

	
	
};
