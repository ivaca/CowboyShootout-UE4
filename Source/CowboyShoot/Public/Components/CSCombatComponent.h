// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Player/CSProjectile.h"
#include "CSCombatComponent.generated.h"


DECLARE_MULTICAST_DELEGATE(FOnFire)


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class COWBOYSHOOT_API UCSCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UCSCombatComponent();


protected:
	virtual void BeginPlay() override;


public:
	UFUNCTION(Server, Reliable)
	void Fire();

	UPROPERTY(Replicated, BlueprintReadWrite, EditAnywhere)
	bool IsShooting;
	UPROPERTY(EditAnywhere, Replicated, BlueprintReadWrite, Category="Ammo")
	int32 RoundCount = 5;
	UPROPERTY(EditAnywhere)
	int32 MaxRoundCount = 5;

	UPROPERTY(Replicated)
	int32 CanReload = true;
	
	UFUNCTION(Server, Reliable)
	void Reload();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Ammo")
	float ReloadDelay = 0.3f;


	FTimerHandle ReloadTimerDelay;
	
	UFUNCTION(Server, Reliable)
	void AimStartServer();

	UFUNCTION(Server, Reliable)
	void AimStopServer();


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated)
	bool IsAiming;

	UFUNCTION(Server, Reliable)
	void SpawnProjectileServer(TSubclassOf<ACSProjectile> ProjectileClass, FTransform Transform);

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Weapon")
	FName WeaponMuzzleSocketName = "Socket_GunMuzzle";


	FOnFire OnFire;


	UFUNCTION(Server, Reliable)
	void OnHit(AActor* OtherActor);

	UPROPERTY(EditAnywhere)
	TSubclassOf<ACSProjectile> Projectile;
private:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
