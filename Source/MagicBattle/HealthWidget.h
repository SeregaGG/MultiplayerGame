// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HealthWidget.generated.h"

/**
 * 
 */
UCLASS()
class MAGICBATTLE_API UHealthWidget : public UUserWidget
{
	GENERATED_BODY()

	virtual bool Initialize() override;
public:
	
	UPROPERTY(meta = (BindWidget))
		class UTextBlock* MaxHealth;

	UPROPERTY(meta = (BindWidget))
		class UTextBlock* CurrentHealth;

	UPROPERTY(meta = (BindWidget))
		class UProgressBar* HealthPoint;

	float PlayerMaxHealth;
	float PlayerCurrentHealth;
};
