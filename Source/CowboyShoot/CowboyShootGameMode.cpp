// Copyright Epic Games, Inc. All Rights Reserved.

#include "CowboyShootGameMode.h"
#include "CowboyShootCharacter.h"
#include "Player/CSPlayerController.h"
#include "UObject/ConstructorHelpers.h"

ACowboyShootGameMode::ACowboyShootGameMode()
{
	DefaultPawnClass = ACowboyShootCharacter::StaticClass();
	PlayerControllerClass = ACSPlayerController::StaticClass();
}
