// Copyright Epic Games, Inc. All Rights Reserved.

#include "MyPingleProjectCharacter.h"
#include "MyPingleProjectProjectile.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/InputSettings.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "MotionControllerComponent.h"
#include "XRMotionControllerBase.h" // for FXRMotionControllerBase::RightHandSourceId
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/TimelineComponent.h"

DEFINE_LOG_CATEGORY_STATIC(LogFPChar, Warning, All);

//////////////////////////////////////////////////////////////////////////
// AMyPingleProjectCharacter

AMyPingleProjectCharacter::AMyPingleProjectCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->SetRelativeLocation(FVector(-39.56f, 1.75f, 64.f)); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->SetupAttachment(FirstPersonCameraComponent);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	Mesh1P->SetRelativeRotation(FRotator(1.9f, -19.19f, 5.2f));
	Mesh1P->SetRelativeLocation(FVector(-0.5f, -4.4f, -155.7f));

	// Create a gun mesh component
	FP_Gun = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FP_Gun"));
	FP_Gun->SetOnlyOwnerSee(false);			// otherwise won't be visible in the multiplayer
	FP_Gun->bCastDynamicShadow = false;
	FP_Gun->CastShadow = false;
	// FP_Gun->SetupAttachment(Mesh1P, TEXT("GripPoint"));
	FP_Gun->SetupAttachment(RootComponent);

	FP_MuzzleLocation = CreateDefaultSubobject<USceneComponent>(TEXT("MuzzleLocation"));
	FP_MuzzleLocation->SetupAttachment(FP_Gun);
	FP_MuzzleLocation->SetRelativeLocation(FVector(0.2f, 48.4f, -10.6f));

	// Default offset from the character location for projectiles to spawn
	GunOffset = FVector(100.0f, 0.0f, 10.0f);

	// Note: The ProjectileClass and the skeletal mesh/anim blueprints for Mesh1P, FP_Gun, and VR_Gun 
	// are set in the derived blueprint asset named MyCharacter to avoid direct content references in C++.
}

void AMyPingleProjectCharacter::Tick(float DeltaSeconds)
{
	/*При перегрузке виртуального метода нужно не забыть вызвать реализацию базавого метода ,что бы не сломать базовый функционал*/
	Super::Tick(DeltaSeconds);
	/*Проверка вызывать апдейт только тогда когда бежим по стене*/
	if (IsWallRunning)
	{
		UpdateWallrun();
	}
	/*Сообщаем таймлайну что он должен тикать*/
	CameraTiltTimeline.TickTimeline(DeltaSeconds);
}
	/*Переназначаем прыжок для отталкивания от стены во время бега по стене*/
void AMyPingleProjectCharacter::Jump()
{
	if (IsWallRunning)
	{
		/*Хотим найти направления отскока от стены ,нужно отскачить перпендикуляно стене*/
		FVector JumpDirection = FVector::ZeroVector;
		/*В Случае если стена находится ссправ*/
		if (CurrentWallRunSide == EWallRunSide::Right)
		{
			/*Векторное произведение между текущим направлением бега по стене и вертикальным вектором Z*/
			JumpDirection = FVector::CrossProduct(CurrentWallRunDirection, FVector::UpVector).GetSafeNormal();
		}
		else
		{
			/*GetSafeNormal() ---- используется для приведение к еденичной длине (нормализации)*/
			/*Если бежим по левой стене порядок векторов меняется от вертикального вектора к текущему вектору напрвления*/
			JumpDirection = FVector::CrossProduct(FVector::UpVector, CurrentWallRunDirection).GetSafeNormal();
		}
		/*К этому компоненту добавляем вертикальный вектор и запускаем игрока в нужном направлении*/
		JumpDirection += FVector::UpVector;
		/*В качестве модуля возьмем пораметр с GetCharacterMovement ,в качестве силы JumpZVelocity,также нормализуем JumpDirection,по X Y значения не перекрываем а по Z перекроем*/
		LaunchCharacter(GetCharacterMovement()->JumpZVelocity * JumpDirection.GetSafeNormal(), false, true);
		StopWallRun();
	}
	else
	{
		//Если по стене не бежим то остается базовая реализация прижыка
		Super::Jump();
	}
}

void AMyPingleProjectCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	//Attach gun mesh component to Skeleton, doing it here because the skeleton is not yet created in the constructor
	FP_Gun->AttachToComponent(Mesh1P, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("GripPoint"));

	// Show or hide the two versions of the gun based on whether or not we're using motion controllers.
	
	Mesh1P->SetHiddenInGame(false, true);

	//Событие которое отывечает за контакт капсулы с каким либо припятсвием 
	GetCapsuleComponent()->OnComponentHit.AddDynamic(this, &AMyPingleProjectCharacter::OnPlayerCapsuleHit);

	if (IsValid(CameraTiltCurve))
	{
		/*Проверяем что выставлена наша кривая дальше нужно создать колбек для тайм лайна что бы вызвать нашу функцию апдейта*/
		FOnTimelineFloat TimelineCallBack;
		/*привязывает к тайм лайну привязываем функцию через BindUFunction где первый аргумент указатель на тек.класс а второй имя функции в строковом формате */
		TimelineCallBack.BindUFunction(this, FName("UpdateCameraTilt"));
		/*К тайм лайну привязываем колбек и крувую по которой будут сменятся значения в зависимости от времени*/
		CameraTiltTimeline.AddInterpFloat(CameraTiltCurve, TimelineCallBack);
	}
}

//////////////////////////////////////////////////////////////////////////
// Input

void AMyPingleProjectCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// set up gameplay key bindings
	check(PlayerInputComponent);

	// Bind jump events
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	// Bind fire event
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &AMyPingleProjectCharacter::OnFire);

	// Bind movement events
	PlayerInputComponent->BindAxis("MoveForward", this, &AMyPingleProjectCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AMyPingleProjectCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &AMyPingleProjectCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AMyPingleProjectCharacter::LookUpAtRate);
}

void AMyPingleProjectCharacter::OnPlayerCapsuleHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	FVector HitNormal = Hit.ImpactNormal;
	/*Проверка ,если мы уже бежим по стене то все остальные проверки не нужны*/
	if (IsWallRunning)
	{
		return;
	}
	/*Проверка если не возноможно передвигаться по данной поверхности*/
	if (!IsSurfaceWallRuneble(HitNormal))
	{
		return;
	}
	/*Првоерка если игрок не падает то ничего не происходит и завершаем выполнение метода*/
	if (!GetCharacterMovement()->IsFalling())
	{
		return;
	}

	EWallRunSide Side = EWallRunSide::None;
	/*Добовляем вектор направлени*/
	FVector Direction = FVector::ZeroVector;
	GetWallRunSideAndDirection(HitNormal, Side, Direction);

	/*Если кнопки не зажаты */
	if (!AreRequiredKeysDown(Side))
	{
		return;
	}
	/*Вызываем метод старта и передаем сторону и направление*/
	StartWallRun(Side, Direction);
}

void AMyPingleProjectCharacter::GetWallRunSideAndDirection(const FVector & HitNormal, EWallRunSide & OutSide, FVector & OutDirection) const
{	/*Скалярное произведение нормали поверхности с вектором который с мотрит в одну сторону игрока (право)*/
	if (FVector::DotProduct(HitNormal, GetActorRightVector()) > 0)
	{	/*Если скалярное произведение больше нуля наша сторона левая*/
		OutSide = EWallRunSide::Left;
		/*Вектор нормали умножит на вектор Z (вверх).Важно!!! Длина ровняется произведению векторов на косинус угла между ними и итоговое значение нужно привести к единичной длине нормализирвоать)*/
		OutDirection = FVector::CrossProduct(HitNormal, FVector::UpVector).GetSafeNormal();
	}
	else
	{	/*В противном случает сторона правая*/
		OutSide = EWallRunSide::Right;
		/*Меняем местами параметры */
		OutDirection = FVector::CrossProduct(FVector::UpVector, HitNormal).GetSafeNormal(); 
	}
}

bool AMyPingleProjectCharacter::IsSurfaceWallRuneble(const FVector& SurfaceNormal) const
{
	if (SurfaceNormal.Z > GetCharacterMovement()->GetWalkableFloorZ() || SurfaceNormal.Z < -0.005f)
	{
		return false;
	}
	return true;
}

bool AMyPingleProjectCharacter::AreRequiredKeysDown(EWallRunSide Side) const
{	/*Если значени меньше позитивного значения ,мы не двигавемся*/ 
	if (ForwardAxis < 0.1f)
	{
		return false;
	}
	/*Если стена по которой двигаемся справа и пытаемся двигатся влево */
	if (Side == EWallRunSide::Right && RightAxis < -0.1f)
	{
		return false;
	}
	/*Если стена по которой двигаемся слева и пытаемся двигатся влево*/
	if (Side == EWallRunSide::Left && RightAxis > 0.1f)
	{
		return false;
	}
	return true;
}

void AMyPingleProjectCharacter::StartWallRun(EWallRunSide Side, const FVector& Direction)
{
	GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Red, TEXT("wall run started!"));
	/*Бульчик что мы бежим по стене*/
	IsWallRunning = true;
	/*Вызываем метод в начале бега*/
	BeginCameraTilt();
	/*Сторона по которой бежим*/ 
	CurrentWallRunSide = Side;
	/*Направление бега*/
	CurrentWallRunDirection = Direction;
	/*Перемещение только в горизонтальной плоскости (бег)*/
	GetCharacterMovement()->SetPlaneConstraintEnabled(true);
	/*Задаем плоскость эта плоскость паралельно полу (перпендикулярно вектору Z) задаем через ее нормаль*/ 
	GetCharacterMovement()->SetPlaneConstraintNormal(FVector::UpVector);
	/*Стартуем таймер который работает 1 секу и по окончанию останавливается бег по стене (Передаем таймер ,указатель на текущий клас ,сылка на функцию что выполнится по окончанию таймера и длительность*/ 
	GetWorld()->GetTimerManager().SetTimer(WallRunTimer, this, &AMyPingleProjectCharacter::StopWallRun, MaxWallRunTime, false); /*таймера, таймер не зацыклен потому фолс)*/
}

void AMyPingleProjectCharacter::StopWallRun()
{
	GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Green, TEXT("wall run ended!"));
	/*Вызываем метод в конце бега*/
	EndCameraTilt();
	/*Противоположность Старту*/
	IsWallRunning = false;
	/*Отключаем*/
	GetCharacterMovement()->SetPlaneConstraintEnabled(false);
	GetCharacterMovement()->SetPlaneConstraintNormal(FVector::ZeroVector);
}

void AMyPingleProjectCharacter::UpdateWallrun()
{
	/*Првоерка если клавиши не нажаты для текущей стороны бега то остановим бег и исполнение функции*/ 
	if (!AreRequiredKeysDown(CurrentWallRunSide))
	{
		StopWallRun();
		return;
	}
	/*обьявили переменную в которой будет хранится результат лайн трейса*/
	FHitResult HitResult;
	/*Вычисляем направление вектора ,если стена по которой бег справа то вектор вправо и наоборот*/
	FVector LineTraceDirecton = CurrentWallRunSide == EWallRunSide::Right ? GetActorRightVector() : -GetActorRightVector();

	float LineTraceLenth = 200.0f;
	/*Стартовая позиция лайн трейса это центр игрока*/ 
	FVector StartPosition = GetActorLocation();
	/*Конечная позиция старт позиция + вектор в нужном направлении определенной длины */
	FVector EndPosition = StartPosition + LineTraceLenth * LineTraceDirecton;
	/*переменная для параметра игнора мешки персонажа*/
	FCollisionQueryParams QueryParams;
	/*Добвляем сущность которую будем игнорировать (себя)*/
	QueryParams.AddIgnoredActor(this);
	/*Добовляем трасировку луча получаем указатель на мир и юзаем метод LineTraceSingleByChannel. для юза обьявляем переменную в которой будет хранится результат (булевая переменная),старт конец */
	if (GetWorld()->LineTraceSingleByChannel(HitResult, StartPosition, EndPosition, ECC_Visibility, QueryParams))/*канал по которому хоим делать проверку(видимые обьекты) и некоротые параметры для игнора мешки персонажа*/
	{
		EWallRunSide Side = EWallRunSide::None;
		FVector Direction = FVector::ZeroVector;
		/*с рузультата получаем нормаль к поверхности */
		GetWallRunSideAndDirection(HitResult.ImpactNormal, Side, Direction);
		/*В случае если сторона отличается от той стороны по корой бежим */
		if (Side != CurrentWallRunSide)
		{
			StopWallRun();
		}
		else
		{
			/*Если все нормально */
			CurrentWallRunDirection = Direction;
			GetCharacterMovement()->Velocity = GetCharacterMovement()->GetMaxSpeed() * CurrentWallRunDirection;
		}
	}
	else
	{
		/*Если условия не выполняются то стопим бег*/
			StopWallRun();
	}
}

void AMyPingleProjectCharacter::UpdateCameraTilt(float Value)
{	 
	/*метод берет текущее значение поворота камеры через метод GetControlRotation*/
	FRotator CurrentControlRotation = GetControlRotation();
	/*Выставляем крен если стена справа камеру наклоняем влево и наоборот*/
	CurrentControlRotation.Roll = CurrentWallRunSide == EWallRunSide::Left ? Value : -Value;
	/*И выставляет обновленное значение в камеру*/
	GetController()->SetControlRotation(CurrentControlRotation);
}

void AMyPingleProjectCharacter::OnFire()
{
	// try and fire a projectile
	if (ProjectileClass != nullptr)
	{
		UWorld* const World = GetWorld();
		if (World != nullptr)
		{
				const FRotator SpawnRotation = GetControlRotation();
				// MuzzleOffset is in camera space, so transform it to world space before offsetting from the character location to find the final muzzle position
				const FVector SpawnLocation = ((FP_MuzzleLocation != nullptr) ? FP_MuzzleLocation->GetComponentLocation() : GetActorLocation()) + SpawnRotation.RotateVector(GunOffset);

				//Set Spawn Collision Handling Override
				FActorSpawnParameters ActorSpawnParams;
				ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;

				// spawn the projectile at the muzzle
				World->SpawnActor<AMyPingleProjectProjectile>(ProjectileClass, SpawnLocation, SpawnRotation, ActorSpawnParams);
	
		}
	}

	// try and play the sound if specified
	if (FireSound != nullptr)
	{
		UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
	}

	// try and play a firing animation if specified
	if (FireAnimation != nullptr)
	{
		// Get the animation object for the arms mesh
		UAnimInstance* AnimInstance = Mesh1P->GetAnimInstance();
		if (AnimInstance != nullptr)
		{
			AnimInstance->Montage_Play(FireAnimation, 1.f);
		}
	}
}


void AMyPingleProjectCharacter::MoveForward(float Value)
{
	ForwardAxis = Value;
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorForwardVector(), Value);
	}
}

void AMyPingleProjectCharacter::MoveRight(float Value)
{
	RightAxis = Value;
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorRightVector(), Value);
	}
}

void AMyPingleProjectCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AMyPingleProjectCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}
