// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/BattleCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Camera/BattleGameCameraActor.h"
#include "General/BattlePlayerController.h"
#include "Actors/BattleProjectile.h"
#include "General/BattleGameState.h"
#include "Components/TimelineComponent.h"
#include "Components/AudioComponent.h"
#include "Net/UnrealNetwork.h"


// Sets default values
ABattleCharacter::ABattleCharacter():
	FireRate(1.f),
	TeamId(0),
	bInvulnerable(false)
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetupAttachment(RootComponent);

	TurretMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TurretMesh"));
	TurretMesh->SetupAttachment(MeshComp);

	TimelineComp = CreateDefaultSubobject<UTimelineComponent>(TEXT("TimelineComp"));

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;

	bReplicates = true;
	SetReplicateMovement(true);

}

// Called when the game starts or when spawned
void ABattleCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	CameraActor = Cast<ABattleGameCameraActor>(UGameplayStatics::GetActorOfClass(this, ABattleGameCameraActor::StaticClass()));

	GameState = GetWorld()->GetGameState<ABattleGameState>();

	SetupDynamicMaterial();

	OnTakeAnyDamage.AddDynamic(this, &ABattleCharacter::OnTakeDamage);

	SetupTimeline();

	SetAudioComponents();
}

void ABattleCharacter::SetupDynamicMaterial()
{
	if (!GameState)
	{
		return;
	}

	if (MeshComp->GetMaterial(0))
	{
		DynamicMaterial = UMaterialInstanceDynamic::Create(MeshComp->GetMaterial(0), this);
		MeshComp->SetMaterial(0, DynamicMaterial);
		TurretMesh->SetMaterial(0, DynamicMaterial);
		DynamicMaterial->SetVectorParameterValue(TEXT("GlowColor"), GameState->GetTeamColor(TeamId));
	}
}

void ABattleCharacter::OnTakeDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
	UE_LOG(LogTemp, Warning, TEXT("Damaged"))

	bInvulnerable = true;
	SetCanBeDamaged(false);

	TimelineComp->PlayFromStart();
}

void ABattleCharacter::SetupTimeline()
{
	if (LinearColorCurve)
	{
		TimelineComp->SetIsReplicated(true); //Sets to replicate all of the events
		FOnTimelineLinearColor TimelineLinearColor;
		TimelineLinearColor.BindUFunction(this, TEXT("UpdateColor"));
		TimelineComp->AddInterpLinearColor(LinearColorCurve, TimelineLinearColor);
		
		FOnTimelineEvent TimelineFinished;
		TimelineFinished.BindUFunction(this, TEXT("TimelineFinished"));
		TimelineComp->SetTimelineFinishedFunc(TimelineFinished);
	}
}

void ABattleCharacter::UpdateColor(FLinearColor ColorOutput)
{
	if (DynamicMaterial)
	{
		DynamicMaterial->SetVectorParameterValue(TEXT("GlowColor"), ColorOutput);
	}
}

void ABattleCharacter::TimelineFinished()
{
	if (InputEnabled())
	{
		bInvulnerable = false;
		SetCanBeDamaged(true);
	}
	
	if (DynamicMaterial)
	{
		DynamicMaterial->SetVectorParameterValue(TEXT("GlowColor"), GameState->GetTeamColor(TeamId));
	}
	UE_LOG(LogTemp, Warning, TEXT("No longer invulnerable"))
}

void ABattleCharacter::SetAudioComponents()
{
	if (IdleSound)
	{
		IdleSoundComponent = UGameplayStatics::SpawnSoundAttached(IdleSound, MeshComp);
		if (IdleSoundComponent)
		{
			IdleSoundComponent->SetActive(true);
			IdleSoundComponent->bAutoDestroy = false;
		}
		
	}

	if (MovingSound)
	{
		MovingSoundComponent = UGameplayStatics::SpawnSoundAttached(MovingSound, MeshComp);
		if (MovingSoundComponent)
		{
			MovingSoundComponent->SetActive(false);
			MovingSoundComponent->bAutoDestroy = false;
		}
		
	}
}

void ABattleCharacter::MovementSound()
{
	if (!MovingSoundComponent)
	{
		return;
	}

	bool bMoving = GetCharacterMovement()->Velocity.Size() > 0.f;

	if (MovingSoundComponent->IsActive() == bMoving)
	{
		// if already active or inactive, prevent it from being set to the same state
		return;
	}

	MovingSoundComponent->SetActive(bMoving);
	
}

void ABattleCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABattleCharacter, TeamId);
}

void ABattleCharacter::OnRep_TeamId()
{
	if (!GameState)
	{
		return;
	}

	if (!DynamicMaterial)
	{
		return;
	}

	DynamicMaterial->SetVectorParameterValue(TEXT("GlowColor"), GameState->GetTeamColor(TeamId));

}

void ABattleCharacter::FireFX_Implementation(FVector SpawnLocation)
{

	if (FireEffect)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), FireEffect, SpawnLocation, FRotator::ZeroRotator);
	}

	if (FireSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
	}
}

void ABattleCharacter::HandleDeath_Implementation()
{

	Disable();

	if (DeathEffect)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), DeathEffect, GetActorLocation());
	}

	if (DeathSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, DeathSound, GetActorLocation());
	}
}

void ABattleCharacter::Disable()
{
	DisableInput(nullptr);
	bInvulnerable = true;
}

// Called every frame
void ABattleCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	MovementSound();

}

// Called to bind functionality to input
void ABattleCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void ABattleCharacter::MoveForward(float Value)
{
	if (CameraActor && Value != 0.0f)
	{
		AddMovementInput(CameraActor->GetActorUpVector(), Value);
	}

}

void ABattleCharacter::MoveRight(float Value)
{
	if (CameraActor && Value != 0.0f)
	{
		AddMovementInput(CameraActor->GetActorRightVector(), Value);
	}

}

void ABattleCharacter::Fire_Implementation()
{
	if (bInvulnerable)
	{
		UE_LOG(LogTemp, Warning, TEXT("%s"),(bInvulnerable? TEXT("true"):TEXT("false")))
		return;
	}

	if (GetWorldTimerManager().IsTimerActive(FireRateTimerHandle))
	{
		return;
	}

	GetWorldTimerManager().SetTimer(FireRateTimerHandle, FireRate, false);
	

	FVector SpawnLocation = GetActorLocation() + GetActorForwardVector() * 75;

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	SpawnParams.Owner = this;

	ABattleProjectile* SpawnedProjectile = GetWorld()->SpawnActor<ABattleProjectile>(ProjectileClass, SpawnLocation, FRotator::ZeroRotator, SpawnParams);

	if (SpawnedProjectile)
	{
		SpawnedProjectile->SetMovementDirection(GetActorForwardVector());
	}
	
	FireFX(SpawnLocation);
}

AActor* ABattleCharacter::GetCameraActor() const
{
	return CameraActor;
}

