// Fill out your copyright notice in the Description page of Project Settings.


#include "MyStepTrap.h"
#include <Components/SphereComponent.h>

// Sets default values
AMyStepTrap::AMyStepTrap()
{
	PrimaryActorTick.bCanEverTick = true;

	SphereRadius = 200.0f;

	MyCollision = CreateDefaultSubobject<USphereComponent>(TEXT("My Sphere Component"));
	MyCollision->InitSphereRadius(SphereRadius);
	RootComponent = MyCollision;

	VisibleMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("My Mesh"));
	VisibleMesh->SetupAttachment(RootComponent);

	MyCollision->OnComponentBeginOverlap.AddDynamic(this, &AMyStepTrap::OnOverlapBegin);


}

// Called when the game starts or when spawned
void AMyStepTrap::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AMyStepTrap::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
void AMyStepTrap::OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if ((OtherActor != nullptr) && (OtherActor != this) && (OtherComp != nullptr))
	{
		Destroy();
	}
}

