// Fill out your copyright notice in the Description page of Project Settings.


#include "MyUserWidget.h"
#include "MyGameInstance.h"

#include <Components/Button.h>
#include <Components/ProgressBar.h>
#include <Components/TextBlock.h>
#include <Components/EditableTextBox.h>
#include <Components/CheckBox.h>
#include <Kismet/GameplayStatics.h>
#include <Blueprint/WidgetBlueprintLibrary.h>
#include <Engine/Player.h>

bool UMyUserWidget::Initialize()
{
	Super::Initialize();
	/*FTimerHandle Handle;
	HealthPoint->Percent = 0.2;
	GetWorld()->GetTimerManager().SetTimer(Handle, this, &UMyUserWidgetCPP::SetWidgetScore, 1, true);*/
	//this->AddToViewport();
	HostGame->OnClicked.AddDynamic(this, &UMyUserWidget::StartButtonClicked);
	JoinGame->OnClicked.AddDynamic(this, &UMyUserWidget::JoinButtonClicked);
	return true;
}

void UMyUserWidget::StartButtonClicked()
{
	
	UMyGameInstance* MyMainMenu = Cast<UMyGameInstance>(GetGameInstance());
	MyMainMenu->bChoice = bGhost->IsChecked();

	UGameplayStatics::OpenLevel(GetWorld(), "Untitled", true, FString("listen"));
}


void UMyUserWidget::JoinButtonClicked()
{
	UMyGameInstance* MyMainMenu = Cast<UMyGameInstance>(GetGameInstance());
	MyMainMenu->bChoice = bGhost->IsChecked();
	//UGameplayStatics::OpenLevel(this, "Untitled", true, FString("listen"));
	UGameplayStatics::OpenLevel(GetWorld(), FName::FName(ServerName->GetText().ToString()));
}
//void UMyUserWidget::SetWidgetScore(int score)
//{
//	MyScore->SetText(FText::FromString(FString::FromInt(score)));
//	//if (HealthPoint->Percent < 1)
//	//{
//	//	HealthPoint->SetPercent(HealthPoint->Percent + 0.01);
//	//}
//
//}
//
//void UMyUserWidget::SetWidgetHP(float healtPoint)
//{
//	HealthPoint->SetPercent(healtPoint / 100);
//}