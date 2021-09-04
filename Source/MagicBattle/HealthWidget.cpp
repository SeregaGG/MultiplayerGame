// Fill out your copyright notice in the Description page of Project Settings.


#include "HealthWidget.h"
#include <Components/ProgressBar.h>
#include <Components/TextBlock.h>

bool UHealthWidget::Initialize()
{
	Super::Initialize();
	
	/*FTimerHandle Handle;
	HealthPoint->Percent = 0.2;
	GetWorld()->GetTimerManager().SetTimer(Handle, this, &UMyUserWidgetCPP::SetWidgetScore, 1, true);
	this->AddToViewport();
	return true;*/
	return true;
}