// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MyPlayerController.h"
#include "MyCharacter.generated.h"

UCLASS()
class MAGICBATTLE_API AMyCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class UCameraComponent* FollowCamera;

public:
	// Sets default values for this character's properties
	AMyCharacter();
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/** Called for forwards/backward input */
	void MoveForward(float Value);

	/** Called for side to side input */
	void MoveRight(float Value);

	UFUNCTION(Server, Reliable)
	void Fire();
	void TakeFireDamage(float dmg);
	
	void UpdateUW();
	void Target();
	void StopTarget();
	virtual void Jump() override;
	virtual void StopJumping() override;
	void ControllerYawInput(float Value);
	void ControllerPitchInput(float Value);

	UPROPERTY(Replicated, VisibleAnywhere)
		class UWidgetComponent* HealthWidgetComp;

	TArray<class ACharacter*> TargetObjects;
	int TargetObjectsLengths = 0;
	class ACharacter* NearestTargetObject = nullptr;

	bool bTarget = false;

	float MaxHealth;

	float FireDmg;
	
	UPROPERTY(ReplicatedUsing=OnRep_CurrentHealth)
	float CurrentHealth;

	UFUNCTION()
	void OnRep_CurrentHealth();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
public:	
	// Called every frame
	
	virtual void Tick(float DeltaTime) override;
	
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
};
