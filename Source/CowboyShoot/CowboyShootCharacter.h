// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/CSAudioComponent.h"
#include "Components/CSCombatComponent.h"
#include "GameFramework/Character.h"
#include "CowboyShootCharacter.generated.h"


UENUM()
enum SOUNDS
{
	FIRE UMETA(DisplayName = "Fire Sound"),
	DEAD UMETA(DisplayName = "Dead Sound"),
	RELOAD UMETA(DisplayName = "Reload Sound"),
};

UCLASS(config=Game)
class ACowboyShootCharacter : public ACharacter
{
	GENERATED_BODY()


	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

	virtual void AddControllerPitchInput(float Val) override;
	virtual void AddControllerYawInput(float Val) override;


	UPROPERTY(Replicated)
	float PlayerYaw = 0;

	UPROPERTY(Replicated)
	float PlayerPitch = 0;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;


	virtual void BeginPlay() override;

	FTimerHandle DeathTimer;

	

	UFUNCTION(NetMulticast, Reliable)
	void OnDeathTimerTick();


	UPROPERTY( Replicated)
	bool IsDead{false};
public:
	ACowboyShootCharacter();
	void CameraRandomLook();
	bool IsPlayerDead() const
	{
		return IsDead;
	}



	void OnDead();


	int32 DeathTimerTickCount{0};

	UPROPERTY(EditAnywhere, Category="Weapon")
	TArray<USoundBase*> BaseSounds;

	UPROPERTY(EditAnywhere, Category="Weapon")
	float MaxRandAngle;
	UPROPERTY(EditAnywhere , Category="Weapon")
	float MinRandAngle;

	
	UFUNCTION(Server, Reliable)
	void OnFire();

UFUNCTION(Client, Reliable)
void OnReload();
	
	UFUNCTION(NetMulticast, Reliable)
	void OnFireMulticast();

	UFUNCTION(NetMulticast, Reliable)
	void OnHit();

	UPROPERTY(EditAnywhere)
	UCSCombatComponent* CombatComponent;

	UFUNCTION(BlueprintCallable)
	float GetPlayerYaw() { return PlayerYaw; };

	UFUNCTION(BlueprintCallable)
	float GetPlayerPitch() { return PlayerPitch; };


	UPROPERTY(EditAnywhere)
	UAnimMontage* DeadAnimMontage;

	UPROPERTY(EditAnywhere)
	UAnimMontage* FireAnimMontage;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseTurnRate;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseLookUpRate;

	UFUNCTION(Server, Reliable)
	void SetPlayerYawPitch(float Yaw, float Pitch);


protected:
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;


public:
	UFUNCTION(BlueprintCallable)
	UCSCombatComponent* GetCombatComponent() const { return CombatComponent; }

	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
};
