// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MyCharacter.h"
#include "GameFramework/Character.h"
#include "MyGhost.generated.h"

UCLASS()
class MAGICBATTLE_API AMyGhost : public ACharacter
{
	GENERATED_BODY()

	
public:
	// Sets default values for this character's properties
	AMyGhost();


protected:

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	/** Called for forwards/backward input */
	void MoveForward(float Value);

	/** Called for side to side input */
	void MoveRight(float Value);
	
	void ControllerYawInput(float Value);
	void ControllerPitchInput(float Value);

	bool SwapControllersWithHim(AMyCharacter* OtherCharacter);
		
public:	
	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

	UFUNCTION(Client, Reliable)
	void CallStealControl();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY()
	AMyCharacter* HitCharacter = nullptr;
	
	UFUNCTION(Server, Reliable)
	void StealControl();
	
	UFUNCTION(Server, Reliable)
	void GetControlBack();
	float StaminaPoints = 20.f;
	float ControlDuration = 5.f;
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
