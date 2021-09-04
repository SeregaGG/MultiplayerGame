// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GameFramework/PlayerController.h"
#include "MyPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class MAGICBATTLE_API AMyPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AMyPlayerController();

	FORCEINLINE UClass* GetPlayerPawnClass() { return MyPawnClass; }
	


protected:

	
	UFUNCTION(Reliable, NetMulticast)
	void DeterminePawnClass();
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION(Reliable, Server, WithValidation)
	virtual void ServerSetPawn(TSubclassOf<ACharacter> InPawnClass);

	
	UPROPERTY(Replicated)
	TSubclassOf<ACharacter> MyPawnClass;


	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "My Controller")
	TSubclassOf<ACharacter> PawnToUseA;
 
	/* Second Pawn Type To Use */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "My Controller")
	TSubclassOf<ACharacter> PawnToUseB;

	
	UFUNCTION(Reliable, Client)
	void CreateMyGhostWidget();
	
	UFUNCTION(Reliable, Client)
	void StaminaRegen(float Value);
	
public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	virtual void SetupInputComponent() override;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class TSubclassOf<class UMyGhostWidget> GhostWidgetClass;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated)
	class UMyGhostWidget* GhostWidget;
};
