// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MainMenuPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class MAGICBATTLE_API AMainMenuPlayerController : public APlayerController
{
	GENERATED_BODY()
public:
	AMainMenuPlayerController();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class TSubclassOf<class UMyUserWidget> UserWidgetClass;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		class UMyUserWidget* UserWidget;
};
