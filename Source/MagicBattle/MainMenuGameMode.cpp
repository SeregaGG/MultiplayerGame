// Fill out your copyright notice in the Description page of Project Settings.


#include "MainMenuGameMode.h"
#include "MyPawn.h"
#include "MyUserWidget.h"
#include <Engine/Engine.h>
#include <Components/WidgetComponent.h>
#include "Blueprint/UserWidget.h"

AMainMenuGameMode::AMainMenuGameMode()
{

}

void AMainMenuGameMode::BeginPlay()
{
	Super::BeginPlay();

	//DefaultPawnClass = AMyPawn::StaticClass();
}