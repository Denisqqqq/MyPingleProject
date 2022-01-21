// Fill out your copyright notice in the Description page of Project Settings.


#include "MyTriggerDeathBox.h"
#include "MyPingleProjectCharacter.h"
#include <Components/BoxComponent.h>
#include <DrawDebugHelpers.h>

AMyTriggerDeathBox::AMyTriggerDeathBox()
{
	OnActorBeginOverlap.AddDynamic(this, &AMyTriggerDeathBox::OnOverlapBegin);
}

void AMyTriggerDeathBox::BeginPlay()
{
	Super::BeginPlay();

	UBoxComponent* BoxComp = Cast<UBoxComponent>(GetCollisionComponent());

	if (BoxComp)
	{
		DrawDebugBox(GetWorld(), GetActorLocation(), BoxComp->GetScaledBoxExtent(), FColor::Red, true, -1, 0, 5);
	}
}
void AMyTriggerDeathBox::OnOverlapBegin(AActor* OverlappedActor, AActor* OtherActor)
{
	AMyPingleProjectCharacter* MyCharacter = Cast<AMyPingleProjectCharacter>(OtherActor);
	if (MyCharacter != nullptr)
	{
		GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Red, TEXT("YOU ARE DEAD"));
		MyCharacter->Death();
	}
}
