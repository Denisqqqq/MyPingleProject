// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include <Components/StaticMeshComponent.h>
#include "MyStepTrap.generated.h"

UCLASS()
class MYPINGLEPROJECT_API AMyStepTrap : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMyStepTrap();

	UPROPERTY(VisibleAnywhere)
		class USphereComponent* MyCollision;


	UPROPERTY(VisibleAnywhere)
		class UStaticMeshComponent* VisibleMesh;

	float SphereRadius;
	
	UFUNCTION()
	void OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
