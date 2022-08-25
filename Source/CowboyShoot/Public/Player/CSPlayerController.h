// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "CSPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class COWBOYSHOOT_API ACSPlayerController : public APlayerController
{
	GENERATED_BODY()

	ACSPlayerController();

public:
	UPROPERTY(EditAnywhere,  BlueprintReadWrite, Category="PlayerController")
	float InputYaw{2.5f};
	UPROPERTY(EditAnywhere,  BlueprintReadWrite, Category="PlayerController")
	float InputPitch{2.5f};
};
