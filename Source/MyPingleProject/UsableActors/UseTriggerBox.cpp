// Fill out your copyright notice in the Description page of Project Settings.


#include "UseTriggerBox.h"
#include "BaseUsableActor.h"
#include <GameFramework/Character.h>



AUseTriggerBox::AUseTriggerBox()
{

	OnActorBeginOverlap.AddDynamic(this, &AUseTriggerBox::OnOverlapBegin);
}

// Called when the game starts or when spawned
void AUseTriggerBox::BeginPlay()
{
	Super::BeginPlay();


}

void AUseTriggerBox::OnOverlapBegin(AActor* OverlappedActor, AActor* OtherActor)
{
	ACharacter* Character = Cast<ACharacter>(OtherActor);
	if (Character != nullptr && ActorToUse)
	{
		GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Red, TEXT("TRIGGERed!"));
		ActorToUse->UseActor(this);
	}
}