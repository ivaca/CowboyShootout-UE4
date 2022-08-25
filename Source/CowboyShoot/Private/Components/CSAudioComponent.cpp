// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/CSAudioComponent.h"

void UCSAudioComponent::OnDeath()
{
}

void UCSAudioComponent::OnFire()
{
	Sound = Sounds.Fire;
	PitchMultiplier = FMath::RandRange(0.7f, 1.3f);
	Play();
}


void UCSAudioComponent::OnHit()
{
	Sound = Sounds.Hit;
	Play();
}
