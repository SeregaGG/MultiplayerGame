// Fill out your copyright notice in the Description page of Project Settings.


#include "MainMenuPlayerController.h"

#include <Engine/Player.h>
#include <Blueprint/WidgetBlueprintLibrary.h>
#include <Kismet/GameplayStatics.h>

#include "GameFramework/PlayerController.h"


#include "MyUserWidget.h"
#include <Engine/Engine.h>
#include <Components/WidgetComponent.h>
#include "Blueprint/UserWidget.h"

AMainMenuPlayerController::AMainMenuPlayerController(bool _bReplicates = true, bool _bCanEverTick = true)
{
	bReplicates = _bReplicates;
	PrimaryActorTick.bCanEverTick = _bCanEverTick;
}

void AMainMenuPlayerController::BeginPlay(bool _bShowMouseCursor = true)
{
	// seems like to many responsobilities for one function
	Super::BeginPlay();
	UWidgetBlueprintLibrary::SetInputMode_UIOnlyEx(this);
	// UWidgetBlueprintLibrary::SetInputMode_GameOnly(this);
	bShowMouseCursor = _bShowMouseCursor;  // let's move this to default argument
	UserWidget = CreateWidget<UMyUserWidget>(GetWorld(), UserWidgetClass);
	UserWidget->AddToViewport();
}

void AMainMenuPlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}