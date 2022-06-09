// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/BattleWall.h"
#include "Actors/BattleProjectile.h"

// Sets default values
ABattleWall::ABattleWall()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	RootComponent = MeshComp;
	MeshComp->SetCollisionResponseToAllChannels(ECR_Overlap);
	MeshComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
	MeshComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	bReplicates = true;

}

// Called when the game starts or when spawned
void ABattleWall::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		MeshComp->OnComponentBeginOverlap.AddDynamic(this, &ABattleWall::OnBeginOverlap);
	}
}

// Called every frame
void ABattleWall::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABattleWall::OnBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ABattleProjectile* MyProjectile = Cast<ABattleProjectile>(OtherActor);
	if (MyProjectile)
	{
		HandleImpact();
	}
}

void ABattleWall::HandleImpact()
{
}

