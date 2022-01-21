// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseUsableActor.h"
#include "MovePlatform.generated.h"

/**
 * 
 */

UENUM()
enum EMoveDirection
{
	MoveToDest,
	MoveToStart,
	NoMove

};
UCLASS()
class MYPINGLEPROJECT_API AMovePlatform : public ABaseUsableActor
{
	GENERATED_BODY()
	
public:
	AMovePlatform();

	virtual void UseActor(AActor* ActorWhoUsed) override;

	UPROPERTY(EditAnywhere)
		UStaticMeshComponent* VisualMesh;

	UPROPERTY(EditAnywhere)
		FVector MoveVector;

	UPROPERTY(EditAnywhere)
		float MoveDistance;

	UPROPERTY(EditAnywhere)
		float MoveSpeed;

	// Called every frame
	virtual void Tick(float DeltaTime) override;
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
private:

	FVector Startpos;
	FVector Endpos;
	EMoveDirection MoveState;
};
