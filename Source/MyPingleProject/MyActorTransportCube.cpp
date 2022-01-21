// Fill out your copyright notice in the Description page of Project Settings.


#include "MyActorTransportCube.h"

// Sets default values
AMyActorTransportCube::AMyActorTransportCube()
{
	VisualMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	VisualMesh->SetupAttachment(RootComponent);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeVisualAsset(TEXT("/Game/StarterContent/Shapes/Shape_Cube.Shape_Cube"));

	if (CubeVisualAsset.Succeeded())
	{
		VisualMesh->SetStaticMesh(CubeVisualAsset.Object);
		VisualMesh->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
	}
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AMyActorTransportCube::BeginPlay()
{
	Super::BeginPlay();

	SetActorLocation(StartPoint->GetActorLocation());
	bMovedToEnd = true;

	StartToEnd = EndPoint->GetActorLocation() - StartPoint->GetActorLocation();
	StartToEnd.Normalize();

	EndToStart = StartPoint->GetActorLocation() - EndPoint->GetActorLocation();
	EndToStart.Normalize();
}

// Called every frame
void AMyActorTransportCube::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FVector DestPoint;
	FVector MoveVec;
	if (bMovedToEnd)
	{
		DestPoint = EndPoint->GetActorLocation();
		MoveVec = StartToEnd;
	}
	else
	{
		DestPoint = StartPoint->GetActorLocation();
		MoveVec = EndToStart;
	}

	FVector CurrentLoc = GetActorLocation();

	float Distance = FVector(DestPoint - CurrentLoc).SizeSquared();
	if (Distance <= ((Speed * Speed) * DeltaTime))
	{
		SetActorLocation(DestPoint);
		bMovedToEnd = !bMovedToEnd;
	}
	else
	{
		SetActorLocation(CurrentLoc + ((MoveVec * Speed) * DeltaTime));
	}
}

