// Fill out your copyright notice in the Description page of Project Settings.


#include "MyGhost.h"

#include "DrawDebugHelpers.h"
#include "MyCharacter.h"
#include "MyGhostWidget.h"
#include "Blueprint/UserWidget.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/ProgressBar.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/GameSession.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

// Sets default values
AMyGhost::AMyGhost()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 42.0f);

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = true;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;
	GetCharacterMovement()->DefaultLandMovementMode = EMovementMode::MOVE_Flying;
	
	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = false; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 0.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.9f;
	GetCharacterMovement()->BrakingFrictionFactor = 20;
	//GetCharacterMovement()->JumpOff(this);
	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller
	
	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm
}

// Called when the game starts or when spawned
void AMyGhost::BeginPlay()
{
	Super::BeginPlay();
	
}




// Called every frame
void AMyGhost::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AMyGhost::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAxis("MoveForward", this, &AMyGhost::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AMyGhost::MoveRight);


	PlayerInputComponent->BindAxis("Turn", this, &AMyGhost::ControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp", this, &AMyGhost::ControllerPitchInput);
	
	PlayerInputComponent->BindAction("StealControl", IE_Pressed, this, &AMyGhost::CallStealControl);
}

void AMyGhost::ControllerYawInput(float Value)
{
	AddControllerYawInput(Value);
}

void AMyGhost::ControllerPitchInput(float Value)
{
	AddControllerPitchInput(Value);
}

void AMyGhost::MoveForward(float Value)
{
	if ((Controller != nullptr) && (Value != 0.0f))
	{
		AddMovementInput(FollowCamera->GetForwardVector().GetSafeNormal(),Value);
	}
}

void AMyGhost::MoveRight(float Value)
{
	if ((Controller != nullptr) && (Value != 0.0f))
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Value);
	}
}

void AMyGhost::CallStealControl_Implementation()
{
	AMyPlayerController* PCRef = Cast<AMyPlayerController>(GetController());
	if(PCRef && PCRef->GhostWidget&&StaminaPoints>=50)
	{
		Cast<AMyPlayerController>(GetController())->GhostWidget->StaminaPoint->SetPercent(0);
		StaminaPoints = 0;
		StealControl();
	}
}

void AMyGhost::GetControlBack_Implementation()
{
	if(HitCharacter)
	{
		this->SwapControllersWithHim(HitCharacter);
	}
}


void AMyGhost::StealControl_Implementation()
{
	FHitResult OutHit;
	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(this);
	FVector TraceStart = FollowCamera->GetComponentLocation();
	FVector TraceEnd = TraceStart+(FollowCamera->GetForwardVector())*1000;
	bool bHit = GetWorld()->LineTraceSingleByChannel(OutHit, TraceStart, TraceEnd, ECC_Visibility,CollisionParams);
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("Steal Control")));
	DrawDebugLine(GetWorld(),TraceStart,TraceEnd,FColor::Red,true,-1,0,10);
	if(bHit)
	{
		if(Cast<AMyCharacter>(OutHit.GetActor()))
		{
			HitCharacter = Cast<AMyCharacter>(OutHit.GetActor());
			bool SwapCheck = this->SwapControllersWithHim(HitCharacter);
			
			if(SwapCheck)
			{
				FTimerHandle Handle;
				GetWorld()->GetTimerManager().SetTimer(Handle, this, &AMyGhost::GetControlBack, ControlDuration, false);
			}
		}
	}
}

bool AMyGhost::SwapControllersWithHim(AMyCharacter* OtherCharacter)
{
	AController* MyController = GetController();
	AController* HisController = OtherCharacter->GetController();
			
	if(MyController&&HisController)
	{
		const FRotator GhostRotation(MyController->GetControlRotation());
		const FRotator CharacterRotation(HisController->GetControlRotation());
		
		MyController->UnPossess();
		HisController->UnPossess();
		
		MyController->Possess(OtherCharacter);
		MyController->SetControlRotation(CharacterRotation);
		
		HisController->Possess(this);
		HisController->SetControlRotation(GhostRotation);
		return true;
	}
	return false;
}


// Replication
void AMyGhost::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}
