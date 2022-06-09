// Fill out your copyright notice in the Description page of Project Settings.


#include "General/BattleGameState.h"
#include "Actors/BattleBase.h"
#include "Character/BattleCharacter.h"
#include "Actors/BattleWall.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"


ABattleGameState::ABattleGameState()
{

}

FLinearColor ABattleGameState::GetTeamColor(int32 TeamId)
{
	if (TeamColors.Contains(TeamId))
	{
		return TeamColors[TeamId];
	}

	return FLinearColor::Black;
}

void ABattleGameState::ActorDied(AActor* Actor)
{
	ABattleBase* BattleBase = Cast<ABattleBase>(Actor);

	if (BattleBase)
	{
		UE_LOG(LogTemp, Warning, TEXT("Destroyed base"))
		BattleBase->HandleDeath();
		EndLevel(BattleBase->GetTeamId());
		return;
	}

	ABattleCharacter* BattleCharacter = Cast<ABattleCharacter>(Actor);
	if (BattleCharacter)
	{
		UE_LOG(LogTemp, Warning, TEXT("Destroyed character"))
		BattleCharacter->HandleDeath();
		EndLevel(BattleCharacter->GetTeamId());
		return;
	}

	ABattleWall* BattleWall = Cast<ABattleWall>(Actor);

	if (BattleWall)
	{
		BattleWall->Destroy();
	}
}

void ABattleGameState::BeginPlay()
{
	Super::BeginPlay();

	SetupWidget();
}

void ABattleGameState::SetupWidget()
{
	if (!UserWidgetClass)
	{
		return;
	}

	UserWidget = CreateWidget<UUserWidget>(UGameplayStatics::GetPlayerController(this, 0), UserWidgetClass);

	if (UserWidget)
	{
		UserWidget->AddToViewport();
	}
}


void ABattleGameState::EndLevel(int32 LosingTeamIndex)
{
	PlayFadeAnimation(LosingTeamIndex);
	FTimerHandle EndLevelTimerHandle;
	GetWorldTimerManager().SetTimer(EndLevelTimerHandle, this, &ABattleGameState::RestartLevel, 4.f);
	DisableLosingTeam(LosingTeamIndex);
}

void ABattleGameState::RestartLevel()
{
	UWorld* World = GetWorld();
	if (World)
	{
		World->ServerTravel(TEXT("/Game/Maps/BattleLevel"));
	}
}


void ABattleGameState::DisableLosingTeam(int32 LosingTeamIndex)
{
	TArray<AActor*> Actors;
	UGameplayStatics::GetAllActorsOfClass(this, ABattleCharacter::StaticClass(), Actors);

	for (AActor* Actor : Actors)
	{
		ABattleCharacter* Character = Cast<ABattleCharacter>(Actor);
		if (Character && Character->GetTeamId() == LosingTeamIndex)
		{
			Character->GetCharacterMovement()->DisableMovement();
			Character->SetIsInvulnerable(true);
		}
	}
}

void ABattleGameState::DisableCharacter_Implementation(ABattleCharacter* CharacterToDisable)
{
	if (CharacterToDisable)
	{
		CharacterToDisable->Disable();
	}
}
