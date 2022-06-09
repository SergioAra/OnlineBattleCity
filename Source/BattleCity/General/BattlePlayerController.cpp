// Fill out your copyright notice in the Description page of Project Settings.


#include "General/BattlePlayerController.h"
#include "Character/BattleCharacter.h"
#include "Net/UnrealNetwork.h"

ABattlePlayerController::ABattlePlayerController():
	bMovingForward(false),
	bMovingRight(false)
{

}

void ABattlePlayerController::BeginPlay()
{
	Super::BeginPlay();


}

void ABattlePlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	InputComponent->BindAxis("MoveForward", this, &ABattlePlayerController::MoveForward);
	InputComponent->BindAxis("MoveRight", this, &ABattlePlayerController::MoveRight);
	InputComponent->BindAction("Fire", IE_Pressed, this, &ABattlePlayerController::Fire);
}

void ABattlePlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABattlePlayerController, BattleCharacter);

}

void ABattlePlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	BattleCharacter = Cast<ABattleCharacter>(InPawn);

	if (!BattleCharacter)
	{
		UE_LOG(LogTemp, Warning, TEXT("No player found on %s"), *GetName())
		return;
	}
}

void ABattlePlayerController::MoveForward(float Value)
{
	if (bMovingRight)
	{
		return;
	}

	bMovingForward = Value != 0;

	if (!BattleCharacter)
	{
		return;
	}

	BattleCharacter->MoveForward(Value);
}

void ABattlePlayerController::MoveRight(float Value)
{

	if (bMovingForward)
	{
		return;
	}

	bMovingRight = Value != 0;

	if (!BattleCharacter)
	{
		return;
	}

	BattleCharacter->MoveRight(Value);
}

void ABattlePlayerController::Fire_Implementation()
{
	if (!BattleCharacter)
	{
		return;
	}

	BattleCharacter->Fire();
}
