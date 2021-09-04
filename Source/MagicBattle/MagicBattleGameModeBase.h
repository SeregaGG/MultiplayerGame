// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "MyCharacter.h"
#include "GameFramework/GameModeBase.h"
#include "MagicBattleGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class MAGICBATTLE_API AMagicBattleGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
	virtual UClass* GetDefaultPawnClassForController_Implementation(AController* InController) override;
public:
	AMagicBattleGameModeBase();

	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
};
