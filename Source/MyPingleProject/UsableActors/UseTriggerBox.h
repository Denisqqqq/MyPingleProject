// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/TriggerBox.h"
#include "UseTriggerBox.generated.h"

/**
 * 
 */
UCLASS()
class MYPINGLEPROJECT_API AUseTriggerBox : public ATriggerBox
{
	GENERATED_BODY()

protected:

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	AUseTriggerBox();

	UFUNCTION()
		void OnOverlapBegin(AActor* OverlappedActor, AActor* OtherActor);

	UPROPERTY(EditAnyWhere)
		class ABaseUsableActor* ActorToUse;
	
};
