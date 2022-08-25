// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Actor.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "CSProjectile.generated.h"



DECLARE_MULTICAST_DELEGATE_OneParam(FOnHit,  AActor*)

UCLASS()
class COWBOYSHOOT_API ACSProjectile : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ACSProjectile();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, Category="Weapon")
	UProjectileMovementComponent* ProjectileMovementComponent;
	
	
	UPROPERTY(EditAnywhere, Category="Weapon")
	UShapeComponent* SphereComponent;
public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	

	FOnHit OnHit;

	UPROPERTY(EditAnywhere)
	float Speed{200.0f};

	UFUNCTION()
	void OnProjectileHit(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);
};
