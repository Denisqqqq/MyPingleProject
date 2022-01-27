// Fill out your copyright notice in the Description page of Project Settings.


#include "MovePlatform.h"

AMovePlatform::AMovePlatform()
{
	VisualMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	VisualMesh->SetupAttachment(RootComponent);
}
void AMovePlatform::UseActor(AActor* ActorWhoUsed)
{
	if (MoveState == EMoveDirection::NoMove)
	{
		MoveState = EMoveDirection::MoveToDest;
	}

}

void AMovePlatform::BeginPlay()
{
	Super::BeginPlay();

	Startpos = GetActorLocation();
	Endpos = Startpos + (MoveVector * MoveDistance);

	MoveState = EMoveDirection::NoMove;
}

void AMovePlatform::Tick(float DeltaTime)
{

	Super::Tick(DeltaTime);

	if (MoveState != EMoveDirection::NoMove)
	{
		FVector CurPos = GetActorLocation();
		FVector DestPoint;
		FVector MoveDir;

		DestPoint = (MoveState == EMoveDirection::MoveToStart ? Startpos : Endpos);
		MoveDir = (MoveState == EMoveDirection::MoveToStart ? MoveVector * -1 : MoveVector);

		float Distance = FVector(DestPoint - CurPos).SizeSquared();
		if (Distance <= ((MoveSpeed * MoveSpeed) * DeltaTime))
		{
			SetActorLocation(DestPoint);
			MoveState = (MoveState == EMoveDirection::MoveToStart ? EMoveDirection::NoMove : EMoveDirection::MoveToStart);
		}
		else
		{
			CurPos += MoveDir * MoveSpeed * DeltaTime;
			SetActorLocation(CurPos);
		}
	}
}

