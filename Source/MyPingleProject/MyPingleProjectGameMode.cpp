// Copyright Epic Games, Inc. All Rights Reserved.

#include "MyPingleProjectGameMode.h"
#include "MyPingleProjectHUD.h"
#include "MyPingleProjectCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "GameFramework/GameModeBase.h"

AMyPingleProjectGameMode::AMyPingleProjectGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPersonCPP/Blueprints/FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	// use our custom HUD class
	HUDClass = AMyPingleProjectHUD::StaticClass();
}

void AMyPingleProjectGameMode::RespawnAfterDeath()
{
	bool IsDead = true;
	if (IsDead)
	{
		RestartPlayer(GetWorld()->GetFirstPlayerController());
	}
}
