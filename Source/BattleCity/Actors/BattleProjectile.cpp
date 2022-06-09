// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/BattleProjectile.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ABattleProjectile::ABattleProjectile():
	MovementDirection(FVector(0.f)),
	ProjectileSpeed(150.f),
	ProjectileDamage(1.f)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	RootComponent = MeshComp;
	MeshComp->SetCollisionResponseToAllChannels(ECR_Overlap);
	MeshComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	bReplicates = true;
	SetReplicateMovement(true);

}

// Called when the game starts or when spawned
void ABattleProjectile::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		MeshComp->OnComponentBeginOverlap.AddDynamic(this, &ABattleProjectile::OnBeginOverlap);
	}
}

void ABattleProjectile::MoveProjectile(float DeltaTime)
{
	FVector NewLocation = GetActorLocation() + MovementDirection * ProjectileSpeed * DeltaTime;

	SetActorLocation(NewLocation);
}

// Called every frame
void ABattleProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	MoveProjectile(DeltaTime);

}

void ABattleProjectile::OnBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{

	AActor* OwnerActor = GetOwner();
	if (OwnerActor)
	{
		AController* OwnerController = OwnerActor->GetInstigatorController();

		if (OwnerController && OtherActor && OtherActor != this && OtherActor != OwnerActor)
		{
			UGameplayStatics::ApplyDamage(OtherActor, ProjectileDamage, OwnerController, this, UDamageType::StaticClass());
		}
	}

	Destroy();
}

