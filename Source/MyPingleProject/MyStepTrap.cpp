// Fill out your copyright notice in the Description page of Project Settings.


#include "MyStepTrap.h"

// Sets default values
AMyStepTrap::AMyStepTrap()
{
	VisualMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	VisualMesh->SetupAttachment(RootComponent);

	OnActorBeginOverlap.AddDynamic(this, &AMyStepTrap::OnOverlapBegin);
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

void AMyStepTrap::OnOverlapBegin(AActor* OverlappedActor, AActor* OtherActor)
{
	AMyStepTrap* MyCharacter = Cast<AMyStepTrap>(OtherActor);
	MyCharacter->Destroy();
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

