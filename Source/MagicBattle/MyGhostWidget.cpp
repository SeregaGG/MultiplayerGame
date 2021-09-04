// Fill out your copyright notice in the Description page of Project Settings.


#include "MyGhostWidget.h"

#include "Components/ProgressBar.h"

bool UMyGhostWidget::Initialize()
{
	Super::Initialize();
	StaminaPoint->Percent = 0.2;
	/*FTimerHandle Handle;
	HealthPoint->Percent = 0.2;
	GetWorld()->GetTimerManager().SetTimer(Handle, this, &UMyUserWidgetCPP::SetWidgetScore, 1, true);
	this->AddToViewport();
	return true;*/
	return true;
}
