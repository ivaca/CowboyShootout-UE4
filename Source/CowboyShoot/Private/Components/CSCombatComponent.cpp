// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/CSCombatComponent.h"

#include "DrawDebugHelpers.h"
#include "CowboyShoot/CowboyShootCharacter.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"


UCSCombatComponent::UCSCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}


void UCSCombatComponent::BeginPlay()
{
	Super::BeginPlay();
	checkf(Projectile, TEXT("Projectile class not found! Change it in Combatcomponent."))
}


void UCSCombatComponent::Reload_Implementation()
{
	if (RoundCount == MaxRoundCount) return;
	if (!CanReload) return;
	CanReload = false;
	GetWorld()->GetTimerManager().SetTimer(ReloadTimerDelay, [&]()
	{
		auto Character = Cast<ACowboyShootCharacter>(GetOwner());
		if (!Character) return;
		Character->OnReload();

		CanReload = true;
		RoundCount += 1;
		GetOwner()->GetWorldTimerManager().ClearTimer(ReloadTimerDelay);
	}, ReloadDelay, false);
}


void UCSCombatComponent::AimStartServer_Implementation()
{
	IsAiming = true;
	CanReload = false;
}

void UCSCombatComponent::AimStopServer_Implementation()
{
	IsAiming = false;
	CanReload = true;
}


void UCSCombatComponent::SpawnProjectileServer_Implementation(TSubclassOf<ACSProjectile> ProjectileClass,
                                                              FTransform Transform)
{
	auto SpawnedProjectile = GetWorld()->SpawnActor<ACSProjectile>(ProjectileClass, Transform);
	
	SpawnedProjectile->OnHit.AddUObject(this, &UCSCombatComponent::OnHit);
	
}


void UCSCombatComponent::
OnHit_Implementation(AActor* OtherActor)
{
	auto HittedActor = Cast<ACowboyShootCharacter>(OtherActor);
	if (!HittedActor)return;
	if (HittedActor->IsPlayerDead())return;

	HittedActor->OnHit();
}


void UCSCombatComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UCSCombatComponent, IsAiming);
	DOREPLIFETIME(UCSCombatComponent, RoundCount);
	DOREPLIFETIME(UCSCombatComponent, CanReload);
	DOREPLIFETIME(UCSCombatComponent, IsShooting);
}


void UCSCombatComponent::Fire_Implementation()
{
	if (RoundCount == 0) return;
	auto Character = Cast<ACowboyShootCharacter>(GetOwner());
	if (!Character) return;

	
	OnFire.Broadcast();
	IsShooting = true;
	
	--RoundCount;
	auto WeaponMuzzleLocation = Character->GetMesh()->GetSocketLocation(WeaponMuzzleSocketName);
	const FTransform SpawnTransform(Character->GetMesh()->GetSocketQuaternion(WeaponMuzzleSocketName),
	                                WeaponMuzzleLocation);
	SpawnProjectileServer(Projectile, SpawnTransform);

	FTimerHandle TimerDelay;
	GetWorld()->GetTimerManager().SetTimer(TimerDelay, [=]()
	{
		IsShooting = false;
		Character->CameraRandomLook();
	}, 0.2f, false);
}
