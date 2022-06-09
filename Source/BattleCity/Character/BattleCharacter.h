// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BattleCharacter.generated.h"

class ABattleGameCameraActor;
class ABattleProjectile;
class ABattleGameState;
class UTimelineComponent;

UCLASS()
class BATTLECITY_API ABattleCharacter : public ACharacter
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, Category = "Componets")
	UStaticMeshComponent* MeshComp;

	UPROPERTY(VisibleAnywhere, Category = "Componets")
	UStaticMeshComponent* TurretMesh;

	UPROPERTY(Transient)
	ABattleGameCameraActor* CameraActor;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Projectile", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<ABattleProjectile> ProjectileClass;

	FTimerHandle FireRateTimerHandle;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Projectile", meta = (AllowPrivateAccess = "true"))
	float FireRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing = OnRep_TeamId ,Category = "Team", meta = (ExposeOnSpawn = "true", AllowPrivateAccess = "true"))
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
	UParticleSystem* FireEffect;

	UPROPERTY(EditDefaultsOnly, Category = "Sound")
	USoundBase* FireSound;

	UPROPERTY(EditDefaultsOnly, Category = "Sound")
	USoundBase* IdleSound;

	UPROPERTY(EditDefaultsOnly, Category = "Sound")
	USoundBase* MovingSound;

	UPROPERTY(Transient)
	UAudioComponent* IdleSoundComponent;

	UPROPERTY(Transient)
	UAudioComponent* MovingSoundComponent;

	UPROPERTY(EditDefaultsOnly, Category = "FX")
	UParticleSystem* DeathEffect;

	UPROPERTY(EditDefaultsOnly, Category = "Sound")
	USoundBase* DeathSound;

public:
	// Sets default values for this pawn's properties
	ABattleCharacter();

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

	void SetAudioComponents();

	void MovementSound();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION()
	void OnRep_TeamId();

	UFUNCTION(NetMulticast, Reliable)
	void FireFX(FVector SpawnLocation);
	virtual void FireFX_Implementation(FVector SpawnLocation);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void MoveForward(float Value);
	void MoveRight(float Value);

	UFUNCTION(Server, Reliable)
	void Fire();
	virtual void Fire_Implementation();

	AActor* GetCameraActor() const;

	FORCEINLINE int32 GetTeamId() const { return TeamId; }
	FORCEINLINE void SetIsInvulnerable(bool bInInvulnerable) { bInvulnerable = bInInvulnerable; }

	UFUNCTION(NetMulticast, Reliable)
	void HandleDeath();
	virtual void HandleDeath_Implementation();

	void Disable();

};
