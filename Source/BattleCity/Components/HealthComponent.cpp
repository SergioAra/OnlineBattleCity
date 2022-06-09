// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/HealthComponent.h"
#include "General/BattleGameState.h"

void UHealthComponent::DamageTaken(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* Instigator, AActor* DamageCauser)
{
	if (Damage <= 0.f) return;

	if (Health - Damage <= 0)
	{
		Health = 0;
		if (GameState)
		{
			GameState->ActorDied(DamagedActor);
		}
	}
	else
	{
		Health -= Damage;
	}
}

// Sets default values for this component's properties
UHealthComponent::UHealthComponent():
	MaxHealth(100.f),
	Health(0.f)
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UHealthComponent::BeginPlay()
{
	Super::BeginPlay();
	Health = MaxHealth;

	GetOwner()->OnTakeAnyDamage.AddDynamic(this, &UHealthComponent::DamageTaken);

	GameState = GetWorld()->GetGameState<ABattleGameState>();
	// ...
	
}


// Called every frame
void UHealthComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

