// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MyUserWidget.generated.h"



/**
 * 
 */
UCLASS(ABSTRACT)
class MAGICBATTLE_API UMyUserWidget : public UUserWidget
{
	GENERATED_BODY()

	virtual bool Initialize() override;

	UPROPERTY(meta = (BindWidget))
		class UButton* HostGame;

	UPROPERTY(meta = (BindWidget))
		class UButton* JoinGame;

	UPROPERTY(meta = (BindWidget))
		class UCheckBox* bGhost;

	UPROPERTY(meta = (BindWidget))
		class UEditableTextBox* ServerName;

	UFUNCTION()
		void StartButtonClicked();

	UFUNCTION()
		void JoinButtonClicked();

//	UPROPERTY(meta = (BindWidget))
//		class UTextBlock* MyScore;
//
//	UPROPERTY(meta = (BindWidget))
//		class UProgressBar* HealthPoint;
//public:
//	void SetWidgetScore(int score);
//	void SetWidgetHP(float healtPoint);

	//UFUNCTION()
	//	void StartButtonClicked();
};
