// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BattleBase.generated.h"

class ABattleGameState;
class UTimelineComponent;

UCLASS()
class BATTLECITY_API ABattleBase : public AActor
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UStaticMeshComponent* MeshComp;

	UPROPERTY(EditInstanceOnly, Category = "Team")
	int32 TeamId;

	UPROPERTY(Transient)
	ABattleGameState* GameState;

	UPROPERTY(Transient)
	UMaterialInstanceDynamic* DynamicMaterial;

	bool bInvulnerable;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UTimelineComponent* TimelineComp;

	UPROPERTY(EditDefaultsOnly)
	UCurveLinearColor* LinearColorCurve;

	FTimerHandle FadeInTimerHandle;

	UPROPERTY(EditDefaultsOnly, Category = "FX")
	UParticleSystem* DeathEffect;

	UPROPERTY(EditDefaultsOnly, Category = "Sound")
	USoundBase* DeathSound;
	
public:	
	// Sets default values for this actor's properties
	ABattleBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void SetupDynamicMaterial();

	UFUNCTION()
	void OnTakeDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser);

	void SetupTimeline();

	UFUNCTION()
	void UpdateColor(FLinearColor ColorOutput);

	UFUNCTION()
	void TimelineFinished();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	FORCEINLINE int32 GetTeamId() const { return TeamId; }

	UFUNCTION(NetMulticast, Reliable)
	void HandleDeath();
	virtual void HandleDeath_Implementation();

};
