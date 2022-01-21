// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MyActorTransportCube.generated.h"

UCLASS()
class MYPINGLEPROJECT_API AMyActorTransportCube : public AActor
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* VisualMesh;

	UPROPERTY(EditAnyWhere)
		float Speed = 100;

	UPROPERTY(EditAnyWhere)
		AActor* StartPoint;

	UPROPERTY(EditAnyWhere)
		AActor* EndPoint;
public:	
	// Sets default values for this actor's properties
	AMyActorTransportCube();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	bool bMovedToEnd;

	FVector StartToEnd;
	FVector EndToStart;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
