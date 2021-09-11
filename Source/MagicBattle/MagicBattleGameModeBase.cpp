// Copyright Epic Games, Inc. All Rights Reserved.


#include "MagicBattleGameModeBase.h"
#include "MyCharacter.h"
#include "MyGameInstance.h"
#include "MyPlayerController.h"
#include "GameFramework/GameSession.h"


AMagicBattleGameModeBase::AMagicBattleGameModeBase()
{
	// mb pass DefaultPawnClass and PlayerControllerClass as parameters?
	// now ther are hardcoded and can't be changed + you make those files coupled
	
	// set default pawn class to our Blueprinted character
	DefaultPawnClass = AMyCharacter::StaticClass();
	PlayerControllerClass = AMyPlayerController::StaticClass();
}

UClass* AMagicBattleGameModeBase::GetDefaultPawnClassForController_Implementation(AController* InController)
{
	/* Override Functionality to get Pawn from PlayerController */
	
	AMyPlayerController* MyController = Cast<AMyPlayerController>(InController);
	if (MyController)
	{
		return MyController->GetPlayerPawnClass();
	}
	/* If we don't get the right Controller, use the Default Pawn */
	
	return DefaultPawnClass;
}

void AMagicBattleGameModeBase::BeginPlay()
{
	Super::BeginPlay();
}