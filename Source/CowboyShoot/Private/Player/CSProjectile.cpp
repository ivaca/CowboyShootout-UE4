// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/CSProjectile.h"

#include "CowboyShoot/CowboyShootCharacter.h"

// Sets default values
ACSProjectile::ACSProjectile()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;


	SphereComponent = CreateDefaultSubobject<USphereComponent>("SphereComponent");
	RootComponent = SphereComponent;
	SphereComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	SphereComponent->SetCollisionResponseToAllChannels(ECR_Block);


	//ProjectileMesh->SetupAttachment(GetRootComponent());
	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>("ProjectileMovementComponent");
	bReplicates = true;
	SphereComponent->SetIsReplicated(true);
	ProjectileMovementComponent->SetIsReplicated(true);
	
}

// Called when the game starts or when spawned
void ACSProjectile::BeginPlay()
{
	Super::BeginPlay();
	SetLifeSpan(5.0f);
	check(SphereComponent);
	SphereComponent->OnComponentBeginOverlap.AddDynamic(this, &ACSProjectile::OnProjectileHit);
	ProjectileMovementComponent->Velocity = GetActorForwardVector() * ProjectileMovementComponent->InitialSpeed;
	
}


void ACSProjectile::OnProjectileHit( UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
 int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
    OnHit.Broadcast(OtherActor);
	Destroy();
}

// Called every frame
void ACSProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
