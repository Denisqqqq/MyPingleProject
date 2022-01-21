// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Components/TimelineComponent.h"
#include "MyPingleProjectCharacter.generated.h"

class UInputComponent;
class USkeletalMeshComponent;
class USceneComponent;
class UCameraComponent;
class UMotionControllerComponent;
class UAnimMontage;
class USoundBase;

//�������� ���������� ���������� � ������� enum 
UENUM()
enum class EWallRunSide : uint8
{
	None,
	Left,
	Right
};

UCLASS(config=Game)
class AMyPingleProjectCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Pawn mesh: 1st person view (arms; seen only by self) */
	UPROPERTY(VisibleDefaultsOnly, Category=Mesh)
	USkeletalMeshComponent* Mesh1P;

	/** Gun mesh: 1st person view (seen only by self) */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	USkeletalMeshComponent* FP_Gun;

	/** Location on gun mesh where projectiles should spawn. */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	USceneComponent* FP_MuzzleLocation;

	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FirstPersonCameraComponent;


public:
	AMyPingleProjectCharacter();
	/*������������� ������� ����*/
	virtual void Tick(float DeltaSeconds) override;

	virtual void Jump() override;

	void Death();

	bool IsDead = false;

protected:
	virtual void BeginPlay();

public:
	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseLookUpRate;

	/** Gun muzzle's offset from the characters location */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Gameplay)
	FVector GunOffset;

	/** Projectile class to spawn */
	UPROPERTY(EditDefaultsOnly, Category=Projectile)
	TSubclassOf<class AMyPingleProjectProjectile> ProjectileClass;

	/** Sound to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Gameplay)
	USoundBase* FireSound;

	/** AnimMontage to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	UAnimMontage* FireAnimation;


protected:
	
	/** Fires a projectile. */
	void OnFire();

	/** Handles moving forward/backward */
	void MoveForward(float Val);

	/** Handles stafing movement, left and right */
	void MoveRight(float Val);

	/**
	 * Called via input to turn at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void TurnAtRate(float Rate);

	/**
	 * Called via input to turn look up/down at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void LookUpAtRate(float Rate);
	/*��� �������������� ���������� ������� � ���������*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "WallRun", meta = (UImin = 0.0f , ClampMin = 0.0f))
	float MaxWallRunTime = 0.3f;
	
	/*��� ������� � ����������� ���� ������� �� ������� � ������� ������ ��������*/ 
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "WallRun")
	UCurveFloat* CameraTiltCurve;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "WallRun")
	UCurveFloat* CameraTippingCurve;

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;
	// End of APawn interface

public:
	/** Returns Mesh1P subobject **/
	USkeletalMeshComponent* GetMesh1P() const { return Mesh1P; }
	/** Returns FirstPersonCameraComponent subobject **/
	UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }


private:
	//��������� ��������� �� ���� � FComponentsHitSignature
	UFUNCTION()
	void OnPlayerCapsuleHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	/*������� ����������� �� �� ������ ��������� ������ */
	void GetWallRunSideAndDirection(const FVector & HitNormal, EWallRunSide & OutSide, FVector & OutDirection)const;
	/*������� �������� �������� ������� ����������� �� �� ������ ��������� ������ */
	bool IsSurfaceWallRuneble(const FVector& SurfaceNormal)const;
	/*������� �������� �������� �������� ������� ����������� �� �� ������ ��������� ������ */
	bool AreRequiredKeysDown(EWallRunSide Side)const;

	float ForwardAxis = 0.0f;
	float RightAxis = 0.0f;
	/*3  ������ ���������� �� ��� �� ������ */
	void StartWallRun(EWallRunSide, const FVector& Direction);/*��������� ������� � ���������� */
	void StopWallRun();
	void UpdateWallrun();
 
	/*3 ������ ������� �������� ��� �������� � ������������ FTimeline*/
	FORCEINLINE void BeginCameraTilt() {CameraTiltTimeline.Play();}
	UFUNCTION()
	void UpdateCameraTilt(float Value);
	FORCEINLINE void EndCameraTilt() {CameraTiltTimeline.Reverse();}
	/*2 ������ FORCEINLINE �.� ������ ��������� ���� �������� � ��� �� ���������� �� ��� ���������� �������� ������� ����*/

	FORCEINLINE void BeginCameraTipping() { CameraTippingTimeline.Play(); }
	UFUNCTION()
	void UpdateCameraTipping(float Value);
	UFUNCTION()
	void DeathCameraTimelineEnd();

	/*���������� ��� �� ������ ����� �� �����*/
	bool IsWallRunning = false;
	/*������� ��������� ����� �� ������ (������ ��� �����)*/
	EWallRunSide CurrentWallRunSide = EWallRunSide::None;
	/*������� ����������� ��������*/
	FVector CurrentWallRunDirection = FVector::ZeroVector;

	/*������ �� ���������� ��������*/
	FTimerHandle WallRunTimer;
	/*����������� ��������������� ��������� ������� ��������� ����������� ������ ������� ��� �������� ��������(� ��� ������)��� ������*/
	/*��� �� ������� ������ #include "Components/TimelineComponent.h"*/
	FTimeline CameraTiltTimeline;
	FTimeline CameraTippingTimeline;
};

