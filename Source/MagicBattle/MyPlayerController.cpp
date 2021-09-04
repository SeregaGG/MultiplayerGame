// Fill out your copyright notice in the Description page of Project Settings.


#include "MyPlayerController.h"

#include <Blueprint/WidgetBlueprintLibrary.h>
#include <Kismet/GameplayStatics.h>

#include "GameFramework/PlayerController.h"

#include "MagicBattleGameModeBase.h"
#include "MyGameInstance.h"
#include "HealthWidget.h"
#include "MyGhost.h"
#include "MyGhostWidget.h"
#include "Components/ProgressBar.h"
#include "Net/UnrealNetwork.h"

AMyPlayerController::AMyPlayerController()
{
	PrimaryActorTick.bCanEverTick = true;

	/* Initialize The Values */
	PawnToUseA= nullptr;
	PawnToUseB= nullptr;
	bReplicates = true;
}

void AMyPlayerController::BeginPlay()
{
	Super::BeginPlay();
	DeterminePawnClass();
	UWidgetBlueprintLibrary::SetInputMode_GameOnly(this);
	//UWidgetBlueprintLibrary::SetInputMode_GameOnly(this);
	bShowMouseCursor = false;
	CreateMyGhostWidget();
}



void AMyPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	
}


void AMyPlayerController::CreateMyGhostWidget_Implementation()
{
	UMyGameInstance* MyMainMenu = Cast<UMyGameInstance>(GetGameInstance());
	if(MyMainMenu->bChoice)
	{
		GhostWidget = CreateWidget<UMyGhostWidget>(this,GhostWidgetClass);
		GhostWidget->AddToViewport();
	}
}

void AMyPlayerController::StaminaRegen_Implementation(float Value)
{
	AMyGhost* GhostRef = Cast<AMyGhost>(GetPawn());
	if(GhostRef)
	{
		if(GhostRef->StaminaPoints<80)
		{
			if(GhostWidget)
			{
				GhostRef->StaminaPoints+=Value;
				GhostWidget->StaminaPoint->SetPercent((GhostRef->StaminaPoints)/100);
			}
		}
	}
}


void AMyPlayerController::DeterminePawnClass_Implementation()
{
	if (IsLocalController()) //Only Do This Locally (NOT Client-Only, since Server wants this too!)
		{
		UMyGameInstance* MyMainMenu = Cast<UMyGameInstance>(GetGameInstance());
		/* Use PawnA if the Text File tells us to */
		if (MyMainMenu->bChoice)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("PawnToUseA")));
			ServerSetPawn(PawnToUseA);
			return;
		}
		/* Otherwise, Use PawnB :) */
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("PawnToUseB")));
		ServerSetPawn(PawnToUseB);
		return;
		}
}


bool AMyPlayerController::ServerSetPawn_Validate(TSubclassOf<ACharacter> InPawnClass)
{
	return true;
}
 
void AMyPlayerController::ServerSetPawn_Implementation(TSubclassOf<ACharacter> InPawnClass)
{
	MyPawnClass = InPawnClass;
 
	/* Just in case we didn't get the PawnClass on the Server in time... */
	GetWorld()->GetAuthGameMode()->RestartPlayer(this);
}


// Replication
void AMyPlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AMyPlayerController, MyPawnClass);
}


void AMyPlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	StaminaRegen(DeltaTime);

}
