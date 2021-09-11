// Fill out your copyright notice in the Description page of Project Settings.


#include "MyCharacter.h"
#include "HealthWidget.h"

#include <Camera/CameraComponent.h>
#include <Components/CapsuleComponent.h>
#include <Components/InputComponent.h>
#include <Components/WidgetComponent.h>

#include <GameFramework/CharacterMovementComponent.h>
#include <GameFramework/Controller.h>
#include <GameFramework/SpringArmComponent.h>

#include <Kismet/GameplayStatics.h>
#include "GameFramework/Character.h"
#include "DrawDebugHelpers.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Engine/StaticMeshActor.h"
#include "Net/UnrealNetwork.h"
// Sets default values


AMyCharacter::AMyCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Don't rotate when the controller rotates. Let that just affect the camera.
	// instead of
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;
	// use. Whenever you write a comment to explain a block of code think if you can move this code to
	// a function which name will explain the code
	SetControllerRotation();
	
	//?????????? 
	// move to a method InitCharacterState or smth
	this->bDead=false;
	this->FireDmg = 30;
	this->MaxHealth = 100;
	this->CurrentHealth = this->MaxHealth;

	// Configure character movement
	// move to a method InitMovementParameters or smth
	GetCharacterMovement()->bOrientRotationToMovement = false; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 0.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	// move to a method InitCameraBoom or smth
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	// move to a method InitFollowCamera or smth
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm
   
	HealthWidgetComp = CreateDefaultSubobject<UWidgetComponent>(TEXT("HealthBar"));
	HealthWidgetComp->SetupAttachment(RootComponent);
	
}

void AMyCharacter::SetControllerRotation(bool Pitch = false, bool Yaw = true, bool Roll = false)
{
	this->bUseControllerRotationPitch = Pitch
	this->bUseControllerRotationYaw = Yaw
	this->bUseControllerRotationRoll = Roll
}

// Called to bind functionality to input
// why this is not called in init?
void AMyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	// Set up gameplay key bindings
	check(PlayerInputComponent);
	
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &AMyCharacter::Fire);

	PlayerInputComponent->BindAction("Target", IE_Pressed, this, &AMyCharacter::Target);
	PlayerInputComponent->BindAction("StopTarget", IE_Pressed, this, &AMyCharacter::StopTarget);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &AMyCharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &AMyCharacter::StopJumping);

	PlayerInputComponent->BindAxis("MoveForward", this, &AMyCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AMyCharacter::MoveRight);

	PlayerInputComponent->BindAxis("Turn", this, &AMyCharacter::ControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp", this, &AMyCharacter::ControllerPitchInput);
}

// Called when the game starts or when spawned
void AMyCharacter::BeginPlay()
{
	Super::BeginPlay();
	this->UpdateUW();
}

void AMyCharacter::Fire_Implementation(bool Debug = false, int CollisionVectorSpeed = 1000)
{
	FHitResult OutHit;
	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(this);
	
	FVector TraceStart = this->FollowCamera->GetComponentLocation();
	FVector TraceEnd = TraceStart+(this->FollowCamera->GetForwardVector())*CollisionVectorSpeed; 
	
	bool bHit = GetWorld()->LineTraceSingleByChannel(OutHit, TraceStart, TraceEnd, ECC_Visibility, CollisionParams);
	
	if (Debug)
		DrawDebugLine(GetWorld(),TraceStart,TraceEnd,FColor::Red,true,-1,0,10);
	
	if(bHit)
	{
		AMyCharacter* HitCharacter = Cast<AMyCharacter>(OutHit.GetActor());
		if(HitCharacter)
		{
			HitCharacter->TakeFireDamage(this->FireDmg);
			HitCharacter->UpdateUW(); // seems like not Fire_Implementation responsobility to update UW
			// mb this should be called every time TakeFireDamage is called?
		}
	}
}

void AMyCharacter::TakeFireDamage(float dmg)
{
	if (IsDead)
		return;

	this->CurrentHealth -= dmg;

	// move this to something like HandleDeath. You already have OnRep_bDead that do very similar thing
	// also 
	if(this->CurrentHealth <= 0)  // direct field check -__-
	{
		this->CurrentHealth=0;
		bDead=true;
		GetCharacterMovement()->DisableMovement();
		bUseControllerRotationYaw = false;
		if(M_Dead)
		{
			GetMesh()->PlayAnimation(M_Dead,false);
		}
	}

}

bool AMyCharacter::IsDead()
{
// Why do you even have 2 checks for death?
	if (bDead)
		return true;
	if (this->CurrentHealth>0):  // it's generaly not a good idea to check fields directly
		return true;
	return false;
}

void AMyCharacter::OnRep_bDead()
{
	this->bDead=true;
	GetCharacterMovement()->DisableMovement();
	bUseControllerRotationYaw = false;
	if(M_Dead) // what is this check about?
	{
		GetMesh()->PlayAnimation(M_Dead,false);
		UpdateUW();
	}
}

void AMyCharacter::UpdateUW()
{
	UHealthWidget* MyHealthWidget = Cast<UHealthWidget>(this->HealthWidgetComp->GetUserWidgetObject());
	// looks weird for me
	if(MyHealthWidget) // Do you really need this check? Mb just always use UHealthWidget?
	{
		MyHealthWidget->MaxHealth->SetText(FText::FromString(FString::FromInt(this->MaxHealth)));
		MyHealthWidget->CurrentHealth->SetText(FText::FromString(FString::FromInt(this->CurrentHealth)));
		MyHealthWidget->HealthPoint->SetPercent((this->CurrentHealth)/MaxHealth);
	}
}

// Strange, but seems it's an interface.
void AMyCharacter::OnRep_CurrentHealth() 
{
	this->UpdateUW();
}

void AMyCharacter::MoveForward(float Value)
{
	if (this->Controller == nullptr) // strange check
		return;
	if (Value == 0.0f)
		return;

	// find out which way is forward
	const FRotator Rotation = this->Controller->GetControlRotation();
	const FRotator YawRotation(0, Rotation.Yaw, 0);

	// get forward vector
	const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	AddMovementInput(Direction, Value);
}

void AMyCharacter::MoveRight(float Value)
{
	if ((this->Controller != nullptr) && (Value != 0.0f))
	{
		// find out which way is right
		const FRotator Rotation = this->Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Value);
	}
}

void AMyCharacter::StopTarget() // didn't understand this method. Seems like it's not Target but Targeting
{
	this->bTarget = false;
	this->TargetObjects.Empty();
	this->TargetObjectsLengths = 0;
}

void AMyCharacter::Target(int TargetShpereShift = 500, float SphereRadius = 500.0f, bool Debug = false)
{
	if (!this->bTarget)  // what is this checks?
	{
		TArray<FHitResult> OutHits;
		// complicated code, mb give it a method?
		FVector SphereStart = this->FollowCamera->GetComponentLocation() + this->FollowCamera->GetForwardVector().GetSafeNormal() * TargetShpereShift;
		FCollisionShape MyColSphere = FCollisionShape::MakeSphere(SphereRadius);

		if (Debub)
			DrawDebugSphere(GetWorld(), SphereStart, MyColSphere.GetSphereRadius(), 50, FColor::Purple, true);

		// can't find a usage of bHit. Mb just  GetWorld()->SweepMultiByChannel(...); ?
		bool bHit = GetWorld()->SweepMultiByChannel(OutHits, SphereStart, SphereStart + FVector(0, 0, 0.01), FQuat::Identity, ECC_Visibility, MyColSphere);
		CollectTargetObjects(OutHits, Debug);
	}
	
	if (this->TargetObjectsLengths == 0)
		return;
	UpdateNearestTarget();
	if (this->NearestTargetObject != nullptr)
		this->bTarget = true;
}

void AMyCharacter::UpdateNearestTarget()
{
	for (auto& oneHit : this->TargetObjects)
	{
		// why not do this comparison when you first calculate hit objects?
		if ((GetActorLocation() - this->NearestTargetObject->GetActorLocation()).Size() > (oneHit->GetActorLocation() - GetActorLocation()).Size())
		{
			this->NearestTargetObject = oneHit;
		}
	}
}

// Should be reworked. The true idea of this method is to find NearestTargetObject 
void AMyCharacter::CollectTargetObjects(TArray<FHitResult> OutHits, bool Debug)
{
	for (auto& oneHit : OutHits)
		{
			ACharacter* HitCharacter = Cast<ACharacter>(oneHit.GetActor());  // not sure if it's pointer
			if (!HitCharacter)  // It's a check that the cast faild, isn't it?
				continue;
			if (HitCharacter == this)
				continue;
			if (this->TargetObjects.Contains(HitCharacter))  // if you clear TargetObjects after each call you don't need this check
				continue;

			this->NearestTargetObject = HitCharacter;  // Why do you need this field? + Seems like not a collecting resposobility
			this->TargetObjects.Add(HitCharacter);
			this->TargetObjectsLengths++;

			if (Debub)
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("Hit Result: %s"), *HitCharacter->GetName()));
		}
}

void AMyCharacter::Jump()
{
	ACharacter::Jump();
}

void AMyCharacter::StopJumping()
{
	ACharacter::StopJumping();
}

void AMyCharacter::ControllerYawInput(float Value)
{
	if (!this->bTarget)
	{
		AddControllerYawInput(Value);
	}
}

void AMyCharacter::ControllerPitchInput(float Value)
{
	if (!this->bTarget)
	{
		AddControllerPitchInput(Value);
	}
}

// Called every frame
void AMyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (this->bTarget&&GetController())
	{
		const FRotator CameraBoomRotation = (this->NearestTargetObject->GetActorLocation()-GetActorLocation()).Rotation();
		GetController()->SetControlRotation(FRotator(CameraBoomRotation.Pitch - 20, CameraBoomRotation.Yaw, 0));
		if((this->NearestTargetObject->GetActorLocation() - this->GetActorLocation()).Size() >=1500)
		{
			this->StopTarget();
		}
	}

}

void AMyCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AMyCharacter, CurrentHealth);
	DOREPLIFETIME(AMyCharacter, bDead);
}
