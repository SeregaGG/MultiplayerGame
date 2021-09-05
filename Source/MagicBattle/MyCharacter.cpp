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

#include "DrawDebugHelpers.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
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
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;

	

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = false; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 0.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm
   
	HealthWidgetComp = CreateDefaultSubobject<UWidgetComponent>(TEXT("HealthBar"));
	HealthWidgetComp->SetupAttachment(RootComponent);
	
}


// Called to bind functionality to input
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
	this->bDead=false;
	this->FireDmg = 30;
	this->MaxHealth = 100;
	this->CurrentHealth = this->MaxHealth;
	this->UpdateUW();
}

void AMyCharacter::Fire_Implementation()
{
	FHitResult OutHit;
	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(this);
	
	FVector TraceStart = this->FollowCamera->GetComponentLocation();
	FVector TraceEnd = TraceStart+(this->FollowCamera->GetForwardVector())*1000;
	
	bool bHit = GetWorld()->LineTraceSingleByChannel(OutHit, TraceStart, TraceEnd, ECC_Visibility,CollisionParams);
	
	DrawDebugLine(GetWorld(),TraceStart,TraceEnd,FColor::Red,true,-1,0,10);
	
	if(bHit)
	{
		AMyCharacter* HitCharacter = Cast<AMyCharacter>(OutHit.GetActor());
		if(HitCharacter)
		{
			HitCharacter->TakeFireDamage(this->FireDmg);
			HitCharacter->UpdateUW();
		}
	}
}

void AMyCharacter::TakeFireDamage(float dmg)
{
	if(!bDead && this->CurrentHealth>0)
	{
		this->CurrentHealth -= dmg;
		if(this->CurrentHealth <= 0)
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
}

void AMyCharacter::OnRep_bDead()
{
	this->bDead=true;
	GetCharacterMovement()->DisableMovement();
	bUseControllerRotationYaw = false;
	if(M_Dead)
	{
		GetMesh()->PlayAnimation(M_Dead,false);
	}
}


void AMyCharacter::UpdateUW()
{
	UHealthWidget* MyHealthWidget = Cast<UHealthWidget>(this->HealthWidgetComp->GetUserWidgetObject());
	if(MyHealthWidget)
	{
		MyHealthWidget->MaxHealth->SetText(FText::FromString(FString::FromInt(this->MaxHealth)));
		MyHealthWidget->CurrentHealth->SetText(FText::FromString(FString::FromInt(this->CurrentHealth)));
		MyHealthWidget->HealthPoint->SetPercent((this->CurrentHealth)/MaxHealth);
	}
}



void AMyCharacter::OnRep_CurrentHealth()
{
	this->UpdateUW();
}

void AMyCharacter::MoveForward(float Value)
{
	if ((this->Controller != nullptr) && (Value != 0.0f))
	{
		// find out which way is forward
		const FRotator Rotation = this->Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
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


void AMyCharacter::StopTarget()
{
	this->bTarget = false;
	this->TargetObjects.Empty();
	this->TargetObjectsLengths = 0;
}


void AMyCharacter::Target()
{
	if (!this->bTarget)
	{
		TArray<FHitResult> OutHits;
		//FHitResult OutHits;
		FVector SphereStart = this->FollowCamera->GetComponentLocation() + this->FollowCamera->GetForwardVector().GetSafeNormal() * 500;

		FCollisionShape MyColSphere = FCollisionShape::MakeSphere(500.0f);

		//DrawDebugSphere(GetWorld(), SphereStart, MyColSphere.GetSphereRadius(), 50, FColor::Purple, true);

		bool bHit = GetWorld()->SweepMultiByChannel(OutHits, SphereStart, SphereStart + FVector(0, 0, 0.01), FQuat::Identity, ECC_Visibility, MyColSphere);
		//if (Cast<APhysicThings>(OutHits.GetActor()))
		//{


		//	Cast<APhysicThings>(OutHits.GetActor())->MyMesh->AddImpulse((SphereStart - Cast<APhysicThings>(OutHits.GetActor())->GetActorLocation()).GetSafeNormal() * 100000);
		//}
		for (auto& oneHit : OutHits)
		{
			if (Cast<ACharacter>(oneHit.GetActor()) && Cast<ACharacter>(oneHit.GetActor()) != this)
			{
				if (!this->TargetObjects.Contains(Cast<ACharacter>(oneHit.GetActor())))
				{
					this->NearestTargetObject = Cast<ACharacter>(oneHit.GetActor());
					this->TargetObjects.Add(Cast<ACharacter>(oneHit.GetActor()));
					this->TargetObjectsLengths++;
					
				}
			}
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("Hit Result: %s"), *oneHit.GetActor()->GetName()));
			//Cast<APhysicThings>(oneHit.GetActor())->MyMesh->AddImpulse((SphereStart - Cast<APhysicThings>(oneHit.GetActor())->GetActorLocation()).GetSafeNormal() * 100000);
		}

	}
	
	if (this->TargetObjectsLengths > 0)
	{
		for (auto& oneHit : this->TargetObjects)
		{
			if ((GetActorLocation() - this->NearestTargetObject->GetActorLocation()).Size() > (oneHit->GetActorLocation() - GetActorLocation()).Size())
			{
				this->NearestTargetObject = oneHit;
			}
		}

		if (this->NearestTargetObject != nullptr)
		{
			//bUseControllerRotationPitch = false;
			//bUseControllerRotationYaw = true;++
			//bUseControllerRotationRoll = false;
			this->bTarget = true;
		}
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
