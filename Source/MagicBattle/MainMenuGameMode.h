// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "MainMenuGameMode.generated.h"

/**
 * 
 */
UCLASS()
class MAGICBATTLE_API AMainMenuGameMode : public AGameModeBase
{
	GENERATED_BODY()

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	AMainMenuGameMode();

	//UPROPERTY(EditAnywhere, BlueprintReadWrite)
	//	class TSubclassOf<class UMyUserWidget> UserWidgetClass;
	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	//	class UMyUserWidget* UserWidget;
};
