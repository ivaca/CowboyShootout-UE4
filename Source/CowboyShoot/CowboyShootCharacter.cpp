// Copyright Epic Games, Inc. All Rights Reserved.

#include "CowboyShootCharacter.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Camera/CameraComponent.h"
#include "Components/AudioComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/GameSession.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

//////////////////////////////////////////////////////////////////////////
// ACowboyShootCharacter


void ACowboyShootCharacter::BeginPlay()
{
	Super::BeginPlay();
	check(CombatComponent);
	
	CombatComponent->OnFire.AddUObject(this, &ACowboyShootCharacter::OnFire);
}

void ACowboyShootCharacter::OnDeathTimerTick_Implementation()
{
	if (DeathTimerTickCount > 5)
	{
		GetWorldTimerManager().ClearTimer(DeathTimer);
		SetActorHiddenInGame(false);
		StopAnimMontage();
		DeathTimerTickCount = 0;
		auto LocalController = GetController();
		auto PlayerController = Cast<APlayerController>(LocalController);
		if (!PlayerController) return;
		EnableInput(PlayerController);
		IsDead = false;
		
	
		return;
	}
	
	SetActorHiddenInGame(!IsHidden());
	++DeathTimerTickCount;
	
}

void ACowboyShootCharacter::OnDead()
{
	IsDead = true;
	PlayAnimMontage(DeadAnimMontage);
	auto LocalController = GetController();
	auto PlayerController = Cast<APlayerController>(LocalController);
	if (!PlayerController) return;

	DisableInput(PlayerController);
	if (HasAuthority())
		GetWorldTimerManager().SetTimer(DeathTimer, this, &ACowboyShootCharacter::OnDeathTimerTick, 0.3f, true);
	
}

void ACowboyShootCharacter::OnReload_Implementation()
{
	UGameplayStatics::SpawnSoundAtLocation(GetWorld(), BaseSounds[RELOAD], GetActorLocation(), FRotator::ZeroRotator, 0.7f,
										   FMath::RandRange(0.9f, 1.1f));
}

ACowboyShootCharacter::ACowboyShootCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 5.f;
	BaseLookUpRate = 5.f;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;


	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(GetMesh());
	// Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = true;


	bReplicates = true;

	CombatComponent = CreateDefaultSubobject<UCSCombatComponent>("CombatComponent");
	CombatComponent->SetIsReplicated(true);
	
}


void ACowboyShootCharacter::OnHit_Implementation()
{
	UGameplayStatics::SpawnSoundAtLocation(GetWorld(), BaseSounds[FIRE], GetActorLocation(), FRotator::ZeroRotator, 0.7f,
	                                       FMath::RandRange(0.7f, 1.1f));
	
	OnDead();	
}

void ACowboyShootCharacter::OnFireMulticast_Implementation()
{
	PlayAnimMontage(FireAnimMontage);
	UGameplayStatics::SpawnSoundAtLocation(GetWorld(), BaseSounds[DEAD], GetActorLocation(), FRotator::ZeroRotator, 0.7f,
	                                       FMath::RandRange(0.7f, 1.1f));
}

void ACowboyShootCharacter::OnFire_Implementation()
{
	OnFireMulticast();
}


//////////////////////////////////////////////////////////////////////////
// Input


void ACowboyShootCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ACowboyShootCharacter, PlayerYaw);
	DOREPLIFETIME(ACowboyShootCharacter, PlayerPitch);
	DOREPLIFETIME(ACowboyShootCharacter, IsDead);

}


void ACowboyShootCharacter::SetPlayerYawPitch_Implementation(float Yaw, float Pitch)
{
	PlayerYaw = Yaw;
	PlayerPitch = Pitch;
}

void ACowboyShootCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);


	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &ACowboyShootCharacter::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp", this, &ACowboyShootCharacter::AddControllerPitchInput);


	// VR headset functionality
	PlayerInputComponent->BindAction("Shoot", IE_Pressed, CombatComponent, &UCSCombatComponent::Fire);
	PlayerInputComponent->BindAction("Reload", IE_Pressed, CombatComponent, &UCSCombatComponent::Reload);
	PlayerInputComponent->BindAction("Aim", IE_Pressed, CombatComponent, &UCSCombatComponent::AimStartServer);
	PlayerInputComponent->BindAction("Aim", IE_Pressed, this, &ACowboyShootCharacter::CameraRandomLook);
	PlayerInputComponent->BindAction("Aim", IE_Released, CombatComponent, &UCSCombatComponent::AimStopServer);
}

void ACowboyShootCharacter::CameraRandomLook()
{
	FMath::GetRandSeed();
	auto RandYaw = FMath::RandRange(MinRandAngle, MaxRandAngle);
	
	FMath::GetRandSeed();
	auto RandPitch = FMath::RandRange(MinRandAngle, MaxRandAngle);
	
	SetPlayerYawPitch(RandYaw, RandPitch);
	AddControllerPitchInput(RandYaw);
	AddControllerYawInput(RandPitch);
}



void ACowboyShootCharacter::AddControllerPitchInput(float Val)
{
	if (!FollowCamera) return;

	auto CameraRot = FollowCamera->GetRelativeTransform().Rotator();
	auto YawScale = GetWorld()->GetFirstPlayerController()->InputPitchScale;
	auto ClampedAngle = Val * BaseTurnRate * GetWorld()->GetDeltaSeconds();

	if (CameraRot.Pitch + ClampedAngle * YawScale < 20.0f && CameraRot.Pitch + ClampedAngle * YawScale > -20.0f)
	{
		PlayerPitch = FollowCamera->GetRelativeTransform().Rotator().Pitch;
		if (!HasAuthority())
			SetPlayerYawPitch(GetPlayerYaw(), PlayerPitch);
		Super::AddControllerPitchInput(ClampedAngle);
	}
}

void ACowboyShootCharacter::AddControllerYawInput(float Val)
{
	if (!FollowCamera) return;
	auto CameraRot = FollowCamera->GetRelativeTransform().Rotator();
	auto YawScale = GetWorld()->GetFirstPlayerController()->InputYawScale;
	auto ClampedAngle = Val * BaseTurnRate * GetWorld()->GetDeltaSeconds();

	if (CameraRot.Yaw + ClampedAngle * YawScale < 20.0f && CameraRot.Yaw + ClampedAngle * YawScale > -20.0f)
	{
		PlayerYaw = FollowCamera->GetRelativeTransform().Rotator().Yaw;
		if (!HasAuthority())
			SetPlayerYawPitch(PlayerYaw, GetPlayerPitch());
		Super::AddControllerYawInput(ClampedAngle);
	}
}
