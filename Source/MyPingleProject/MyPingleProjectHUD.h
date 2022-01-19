// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once 

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "MyPingleProjectHUD.generated.h"

UCLASS()
class AMyPingleProjectHUD : public AHUD
{
	GENERATED_BODY()

public:
	AMyPingleProjectHUD();

	/** Primary draw call for the HUD */
	virtual void DrawHUD() override;

private:
	/** Crosshair asset pointer */
	class UTexture2D* CrosshairTex;

};

