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

AMainMenuPlayerController::AMainMenuPlayerController()
{
	bReplicates = true;
	PrimaryActorTick.bCanEverTick = true;
}

void AMainMenuPlayerController::BeginPlay()
{
	Super::BeginPlay();
	UWidgetBlueprintLibrary::SetInputMode_UIOnlyEx(this);
	//UWidgetBlueprintLibrary::SetInputMode_GameOnly(this);
	bShowMouseCursor = true;
	UserWidget = CreateWidget<UMyUserWidget>(GetWorld(), UserWidgetClass);
	UserWidget->AddToViewport();
}

void AMainMenuPlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}