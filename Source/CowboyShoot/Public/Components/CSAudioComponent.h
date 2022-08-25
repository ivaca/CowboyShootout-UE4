// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/AudioComponent.h"
#include "CSAudioComponent.generated.h"

/**
 * 
 */

USTRUCT()
struct FSound
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere)
	USoundBase* Hit;
	UPROPERTY(EditAnywhere)
	USoundBase* Fire;
	UPROPERTY(EditAnywhere)
	USoundBase* Dead;
};


UCLASS()
class COWBOYSHOOT_API UCSAudioComponent : public UAudioComponent
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere)
	FSound Sounds;
	void OnDeath();
	
	UFUNCTION()
	void OnFire();
	
	void OnHit();
};
