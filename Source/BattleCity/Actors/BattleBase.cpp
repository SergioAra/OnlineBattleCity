// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/BattleBase.h"
#include "General/BattleGameState.h"
#include "Components/TimelineComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ABattleBase::ABattleBase():
	TeamId(0),
	bInvulnerable(false)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	RootComponent = MeshComp;
	MeshComp->SetCollisionResponseToAllChannels(ECR_Overlap);
	MeshComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
	MeshComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	TimelineComp = CreateDefaultSubobject<UTimelineComponent>(TEXT("TimelineComp"));
	bReplicates = true;
}

// Called when the game starts or when spawned
void ABattleBase::BeginPlay()
{
	Super::BeginPlay();

	GameState = GetWorld()->GetGameState<ABattleGameState>();

	SetupDynamicMaterial();

	OnTakeAnyDamage.AddDynamic(this, &ABattleBase::OnTakeDamage);

	SetupTimeline();
}

void ABattleBase::SetupDynamicMaterial()
{
	if (!GameState)
	{
		return;
	}

	if (MeshComp->GetMaterial(0))
	{
		DynamicMaterial = UMaterialInstanceDynamic::Create(MeshComp->GetMaterial(0), this);
		MeshComp->SetMaterial(0, DynamicMaterial);
		DynamicMaterial->SetVectorParameterValue(TEXT("FloorColor"), GameState->GetTeamColor(TeamId));
	}
}

void ABattleBase::OnTakeDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
	UE_LOG(LogTemp, Warning, TEXT("Damaged"))

	bInvulnerable = true;
	SetCanBeDamaged(false);

	TimelineComp->PlayFromStart();
}

void ABattleBase::SetupTimeline()
{
	if (LinearColorCurve)
	{
		TimelineComp->SetIsReplicated(true);
		FOnTimelineLinearColor TimelineLinearColor;
		TimelineLinearColor.BindUFunction(this, TEXT("UpdateColor"));
		TimelineComp->AddInterpLinearColor(LinearColorCurve, TimelineLinearColor);

		FOnTimelineEvent TimelineFinished;
		TimelineFinished.BindUFunction(this, TEXT("TimelineFinished"));
		TimelineComp->SetTimelineFinishedFunc(TimelineFinished);
	}
}

void ABattleBase::UpdateColor(FLinearColor ColorOutput)
{
	if (DynamicMaterial)
	{
		DynamicMaterial->SetVectorParameterValue(TEXT("FloorColor"), ColorOutput);
	}
}

void ABattleBase::TimelineFinished()
{
	bInvulnerable = false;
	SetCanBeDamaged(true);
	if (DynamicMaterial)
	{
		DynamicMaterial->SetVectorParameterValue(TEXT("FloorColor"), GameState->GetTeamColor(TeamId));
	}
	UE_LOG(LogTemp, Warning, TEXT("No longer invulnerable"))
}

void ABattleBase::HandleDeath_Implementation()
{
	bInvulnerable = true;

	if (DeathEffect)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), DeathEffect, GetActorLocation());
	}

	if (DeathSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, DeathSound, GetActorLocation());
	}
}

// Called every frame
void ABattleBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

