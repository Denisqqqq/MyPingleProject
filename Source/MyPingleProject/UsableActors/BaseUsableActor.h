// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BaseUsableActor.generated.h"

UCLASS()
class MYPINGLEPROJECT_API ABaseUsableActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABaseUsableActor();

public:
	virtual void UseActor(AActor* ActorWhoUsed) {}

	UPROPERTY(VisibleAnywhere)
		bool bCanBeUsedByPlayer = true;


};
