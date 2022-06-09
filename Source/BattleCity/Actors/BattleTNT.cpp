// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/BattleTNT.h"
#include "Actors/BattleProjectile.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ABattleTNT::ABattleTNT():
	ExplosionRange(200.f),
	ExplosionDamage(1.f)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	RootComponent = MeshComp;
	MeshComp->SetCollisionResponseToAllChannels(ECR_Overlap);
	MeshComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
	MeshComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	bReplicates = true;
}

// Called when the game starts or when spawned
void ABattleTNT::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		MeshComp->OnComponentBeginOverlap.AddDynamic(this, &ABattleTNT::OnBeginOverlap);
	}
}

void ABattleTNT::Destroyed()
{
	Super::Destroyed();

	if (ExplosionEffect)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplosionEffect, GetActorLocation());
	}

	if (ExplosionSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, ExplosionSound, GetActorLocation());
	}
}

// Called every frame
void ABattleTNT::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABattleTNT::OnBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ABattleProjectile* MyProjectile = Cast<ABattleProjectile>(OtherActor);
	if (!MyProjectile)
	{
		return;
	}

	TArray<AActor*> IgnoreActors;
	UGameplayStatics::ApplyRadialDamage(this, ExplosionDamage, GetActorLocation(), ExplosionRange, UDamageType::StaticClass(), IgnoreActors, OtherActor, (AController*)0, true);
	
	Destroy();

}

