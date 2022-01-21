// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/TriggerBox.h"
#include "MyTriggerDeathBox.generated.h"

/**
 * 
 */
UCLASS()
class MYPINGLEPROJECT_API AMyTriggerDeathBox : public ATriggerBox
{
	GENERATED_BODY()

	AMyTriggerDeathBox();

	virtual void BeginPlay() override;

public:
	UFUNCTION()
	void OnOverlapBegin(AActor* OverlappedActor, AActor* OtherActor);

	
};
