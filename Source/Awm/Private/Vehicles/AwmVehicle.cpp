// Copyright 2015 Mail.Ru Group. All Rights Reserved.

#include "Awm.h"

AAwmVehicle::AAwmVehicle(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer) //.SetDefaultSubobjectClass<UAwmVehicleMovement>(ACharacter::CharacterMovementComponentName))
{
	GetMesh()->bReceivesDecals = false;
	GetMesh()->SetCollisionObjectType(ECC_Vehicle);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	GetMesh()->SetCollisionResponseToChannel(COLLISION_WEAPON, ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(COLLISION_PROJECTILE, ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	GetMesh()->MeshComponentUpdateFlag = EMeshComponentUpdateFlag::AlwaysTickPoseAndRefreshBones;

	bIsTargeting = false;
	bWantsToFire = false;

	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;
}

//////////////////////////////////////////////////////////////////////////
// Initialization

void AAwmVehicle::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (Role == ROLE_Authority)
	{
		Health = GetMaxHealth();
		SpawnDefaultInventory();
	}

	// create material instance for setting team colors (3rd person view)
	for (int32 iMat = 0; iMat < GetMesh()->GetNumMaterials(); iMat++)
	{
		MeshMIDs.Add(GetMesh()->CreateAndSetMaterialInstanceDynamic(iMat));
	}

	// play respawn effects
	if (GetNetMode() != NM_DedicatedServer)
	{
		// @todo Play respawn FX and sound
	}
}

void AAwmVehicle::PawnClientRestart()
{
	Super::PawnClientRestart();

	// reattach weapon if needed
	SetCurrentWeapon(CurrentWeapon);
}

void AAwmVehicle::PossessedBy(class AController* InController)
{
	Super::PossessedBy(InController);

	// [server] as soon as PlayerState is assigned, set team colors of this pawn for local player
	UpdateTeamColorsAllMIDs();
}

void AAwmVehicle::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	// [client] as soon as PlayerState is assigned, set team colors of this pawn for local player
	if (PlayerState != NULL)
	{
		UpdateTeamColorsAllMIDs();
	}
}

void AAwmVehicle::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	// Health regen
	AAwmPlayerController* MyPC = Cast<AAwmPlayerController>(Controller);
	if (MyPC && MyPC->HasHealthRegen())
	{
		if (this->Health < this->GetMaxHealth())
		{
			this->Health += 5 * DeltaSeconds;
			if (Health > this->GetMaxHealth())
			{
				Health = this->GetMaxHealth();
			}
		}
	}
}

void AAwmVehicle::Destroyed()
{
	Super::Destroyed();

	DestroyInventory();
}


//////////////////////////////////////////////////////////////////////////
// Meshes

void AAwmVehicle::UpdateTeamColors(UMaterialInstanceDynamic* UseMID)
{
	if (UseMID)
	{
		AAwmPlayerState* MyPlayerState = Cast<AAwmPlayerState>(PlayerState);
		if (MyPlayerState != NULL)
		{
			float MaterialParam = (float)MyPlayerState->GetTeamNum();
			UseMID->SetScalarParameterValue(TEXT("Team Color Index"), MaterialParam);
		}
	}
}

void AAwmVehicle::UpdateTeamColorsAllMIDs()
{
	for (int32 i = 0; i < MeshMIDs.Num(); ++i)
	{
		UpdateTeamColors(MeshMIDs[i]);
	}
}


//////////////////////////////////////////////////////////////////////////
// Damage & death

void AAwmVehicle::FellOutOfWorld(const class UDamageType& dmgType)
{
	Die(Health, FDamageEvent(dmgType.GetClass()), NULL, NULL);
}

void AAwmVehicle::Suicide()
{
	KilledBy(this);
}

void AAwmVehicle::KilledBy(APawn* EventInstigator)
{
	if (Role == ROLE_Authority && !bIsDying)
	{
		AController* Killer = NULL;
		if (EventInstigator != NULL)
		{
			Killer = EventInstigator->Controller;
			LastHitBy = NULL;
		}

		Die(Health, FDamageEvent(UDamageType::StaticClass()), Killer, NULL);
	}
}

float AAwmVehicle::TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, class AActor* DamageCauser)
{
	AAwmPlayerController* MyPC = Cast<AAwmPlayerController>(Controller);
	if (MyPC && MyPC->HasGodMode())
	{
		return 0.f;
	}

	if (Health <= 0.f)
	{
		return 0.f;
	}

	// Modify based on game rules.
	AAwmGameMode* const Game = GetWorld()->GetAuthGameMode<AAwmGameMode>();
	Damage = Game ? Game->ModifyDamage(Damage, this, DamageEvent, EventInstigator, DamageCauser) : 0.f;

	const float ActualDamage = Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);
	if (ActualDamage > 0.f)
	{
		Health -= ActualDamage;
		if (Health <= 0)
		{
			Die(ActualDamage, DamageEvent, EventInstigator, DamageCauser);
		}
		else
		{
			PlayHit(ActualDamage, DamageEvent, EventInstigator ? EventInstigator->GetPawn() : NULL, DamageCauser);
		}

		MakeNoise(1.0f, EventInstigator ? EventInstigator->GetPawn() : this);
	}

	return ActualDamage;
}

bool AAwmVehicle::CanDie(float KillingDamage, FDamageEvent const& DamageEvent, AController* Killer, AActor* DamageCauser) const
{
	if (bIsDying										// already dying
		|| IsPendingKill()								// already destroyed
		|| Role != ROLE_Authority						// not authority
		|| GetWorld()->GetAuthGameMode() == NULL
		|| GetWorld()->GetAuthGameMode()->GetMatchState() == MatchState::LeavingMap)	// level transition occurring
	{
		return false;
	}

	return true;
}

bool AAwmVehicle::Die(float KillingDamage, FDamageEvent const& DamageEvent, AController* Killer, AActor* DamageCauser)
{
	if (!CanDie(KillingDamage, DamageEvent, Killer, DamageCauser))
	{
		return false;
	}

	Health = FMath::Min(0.0f, Health);

	// if this is an environmental death then refer to the previous killer so that they receive credit (knocked into lava pits, etc)
	UDamageType const* const DamageType = DamageEvent.DamageTypeClass ? DamageEvent.DamageTypeClass->GetDefaultObject<UDamageType>() : GetDefault<UDamageType>();
	Killer = GetDamageInstigator(Killer, *DamageType);

	AController* const KilledPlayer = (Controller != NULL) ? Controller : Cast<AController>(GetOwner());
	GetWorld()->GetAuthGameMode<AAwmGameMode>()->Killed(Killer, KilledPlayer, this, DamageType);

	NetUpdateFrequency = GetDefault<AAwmVehicle>()->NetUpdateFrequency;
	// @todo
	//GetCharacterMovement()->ForceReplicationUpdate();

	OnDeath(KillingDamage, DamageEvent, Killer ? Killer->GetPawn() : NULL, DamageCauser);
	return true;
}

void AAwmVehicle::OnDeath(float KillingDamage, struct FDamageEvent const& DamageEvent, class APawn* PawnInstigator, class AActor* DamageCauser)
{
	if (bIsDying)
	{
		return;
	}

	bReplicateMovement = false;
	bTearOff = true;
	bIsDying = true;

	if (Role == ROLE_Authority)
	{
		ReplicateHit(KillingDamage, DamageEvent, PawnInstigator, DamageCauser, true);

		// play the force feedback effect on the client player controller
		APlayerController* PC = Cast<APlayerController>(Controller);
		if (PC && DamageEvent.DamageTypeClass)
		{
			UAwmDamageType *DamageType = Cast<UAwmDamageType>(DamageEvent.DamageTypeClass->GetDefaultObject());
			if (DamageType && DamageType->KilledForceFeedback)
			{
				PC->ClientPlayForceFeedback(DamageType->KilledForceFeedback, false, "Damage");
			}
		}
	}

	// remove all weapons
	DestroyInventory();

	DetachFromControllerPendingDestroy();

	// Use a local timer handle as we don't need to store it for later but we don't need to look for something to clear
	FTimerHandle TimerHandle;
	GetWorldTimerManager().SetTimer(TimerHandle, this, &AAwmVehicle::DeathCleanup, FMath::Min(0.1f, 0.1f/*DeathAnimDuration*/), false);
}

void AAwmVehicle::DeathCleanup()
{
	// First give a chance to blueprint to cleanup itself
	BlueprintDeathCleanup();

	// @todo
	/**
	GetCharacterMovement()->StopMovementImmediately();
	GetCharacterMovement()->DisableMovement();
	GetCharacterMovement()->SetComponentTickEnabled(false);*/

	// hide and set short lifespan
	TurnOff();
	SetActorHiddenInGame(true);
	SetLifeSpan(1.0f);
}

//Pawn::PlayDying sets this lifespan, but when that function is called on client, dead pawn's role is still SimulatedProxy despite bTearOff being true. 
void AAwmVehicle::TornOff()
{
	SetLifeSpan(25.f);
}

void AAwmVehicle::PlayHit(float DamageTaken, struct FDamageEvent const& DamageEvent, class APawn* PawnInstigator, class AActor* DamageCauser)
{
	if (Role == ROLE_Authority)
	{
		ReplicateHit(DamageTaken, DamageEvent, PawnInstigator, DamageCauser, false);

		// play the force feedback effect on the client player controller
		APlayerController* PC = Cast<APlayerController>(Controller);
		if (PC && DamageEvent.DamageTypeClass)
		{
			UAwmDamageType *DamageType = Cast<UAwmDamageType>(DamageEvent.DamageTypeClass->GetDefaultObject());
			if (DamageType && DamageType->HitForceFeedback)
			{
				PC->ClientPlayForceFeedback(DamageType->HitForceFeedback, false, "Damage");
			}
		}
	}

	if (DamageTaken > 0.f)
	{
		// @todo
		//ApplyDamageMomentum(DamageTaken, DamageEvent, PawnInstigator, DamageCauser);
	}

	AAwmPlayerController* MyPC = Cast<AAwmPlayerController>(Controller);
	AAwmHUD* MyHUD = MyPC ? Cast<AAwmHUD>(MyPC->GetHUD()) : NULL;
	if (MyHUD)
	{
		// @todo
		//MyHUD->NotifyWeaponHit(DamageTaken, DamageEvent, PawnInstigator);
	}

	if (PawnInstigator && PawnInstigator != this && PawnInstigator->IsLocallyControlled())
	{
		AAwmPlayerController* InstigatorPC = Cast<AAwmPlayerController>(PawnInstigator->Controller);
		AAwmHUD* InstigatorHUD = InstigatorPC ? Cast<AAwmHUD>(InstigatorPC->GetHUD()) : NULL;
		if (InstigatorHUD)
		{
			// @todo
			//InstigatorHUD->NotifyEnemyHit();
		}
	}
}

void AAwmVehicle::ReplicateHit(float Damage, struct FDamageEvent const& DamageEvent, class APawn* PawnInstigator, class AActor* DamageCauser, bool bKilled)
{
	const float TimeoutTime = GetWorld()->GetTimeSeconds() + 0.5f;

	FDamageEvent const& LastDamageEvent = LastTakeHitInfo.GetDamageEvent();
	if ((PawnInstigator == LastTakeHitInfo.PawnInstigator.Get()) && (LastDamageEvent.DamageTypeClass == LastTakeHitInfo.DamageTypeClass) && (LastTakeHitTimeTimeout == TimeoutTime))
	{
		// same frame damage
		if (bKilled && LastTakeHitInfo.bKilled)
		{
			// Redundant death take hit, just ignore it
			return;
		}

		// otherwise, accumulate damage done this frame
		Damage += LastTakeHitInfo.ActualDamage;
	}

	LastTakeHitInfo.ActualDamage = Damage;
	LastTakeHitInfo.PawnInstigator = Cast<AAwmVehicle>(PawnInstigator);
	LastTakeHitInfo.DamageCauser = DamageCauser;
	LastTakeHitInfo.SetDamageEvent(DamageEvent);
	LastTakeHitInfo.bKilled = bKilled;
	LastTakeHitInfo.EnsureReplication();

	LastTakeHitTimeTimeout = TimeoutTime;
}

void AAwmVehicle::OnRep_LastTakeHitInfo()
{
	if (LastTakeHitInfo.bKilled)
	{
		OnDeath(LastTakeHitInfo.ActualDamage, LastTakeHitInfo.GetDamageEvent(), LastTakeHitInfo.PawnInstigator.Get(), LastTakeHitInfo.DamageCauser.Get());
	}
	else
	{
		PlayHit(LastTakeHitInfo.ActualDamage, LastTakeHitInfo.GetDamageEvent(), LastTakeHitInfo.PawnInstigator.Get(), LastTakeHitInfo.DamageCauser.Get());
	}
}


//////////////////////////////////////////////////////////////////////////
// Vehicle Stats

int32 AAwmVehicle::GetMaxHealth() const
{
	return GetClass()->GetDefaultObject<AAwmVehicle>()->Health;
}

bool AAwmVehicle::IsAlive() const
{
	return Health > 0;
}


//////////////////////////////////////////////////////////////////////////
// Weapon usage

void AAwmVehicle::StartWeaponFire()
{
	if (!bWantsToFire)
	{
		bWantsToFire = true;
		if (CurrentWeapon)
		{
			CurrentWeapon->StartFire();
		}
	}
}

void AAwmVehicle::StopWeaponFire()
{
	if (bWantsToFire)
	{
		bWantsToFire = false;
		if (CurrentWeapon)
		{
			CurrentWeapon->StopFire();
		}
	}
}

bool AAwmVehicle::CanFire() const
{
	return IsAlive();
}

bool AAwmVehicle::CanReload() const
{
	return true;
}

void AAwmVehicle::SetTargeting(bool bNewTargeting)
{
	bIsTargeting = bNewTargeting;

	if (Role < ROLE_Authority)
	{
		ServerSetTargeting(bNewTargeting);
	}
}

bool AAwmVehicle::ServerSetTargeting_Validate(bool bNewTargeting)
{
	return true;
}

void AAwmVehicle::ServerSetTargeting_Implementation(bool bNewTargeting)
{
	SetTargeting(bNewTargeting);
}


//////////////////////////////////////////////////////////////////////////
// Input

void AAwmVehicle::SetupPlayerInputComponent(class UInputComponent* InputComponent)
{
	check(InputComponent);
	//InputComponent->BindAxis("MoveForward", this, &AAwmVehicle::MoveForward);
	//InputComponent->BindAxis("MoveRight", this, &AAwmVehicle::MoveRight);
	InputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	InputComponent->BindAxis("TurnRate", this, &AAwmVehicle::TurnAtRate);
	InputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	InputComponent->BindAxis("LookUpRate", this, &AAwmVehicle::LookUpAtRate);

	InputComponent->BindAction("Fire", IE_Pressed, this, &AAwmVehicle::OnStartFire);
	InputComponent->BindAction("Fire", IE_Released, this, &AAwmVehicle::OnStopFire);

	InputComponent->BindAction("Targeting", IE_Pressed, this, &AAwmVehicle::OnStartTargeting);
	InputComponent->BindAction("Targeting", IE_Released, this, &AAwmVehicle::OnStopTargeting);

	InputComponent->BindAction("NextWeapon", IE_Pressed, this, &AAwmVehicle::OnNextWeapon);
	InputComponent->BindAction("PrevWeapon", IE_Pressed, this, &AAwmVehicle::OnPrevWeapon);

	InputComponent->BindAction("Reload", IE_Pressed, this, &AAwmVehicle::OnReload);
}

void AAwmVehicle::MoveForward(float Val)
{
	GetVehicleMovementComponent()->SetThrottleInput(Val);
}

void AAwmVehicle::MoveRight(float Val)
{
	GetVehicleMovementComponent()->SetSteeringInput(Val);
}

void AAwmVehicle::TurnAtRate(float Val)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Val * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AAwmVehicle::LookUpAtRate(float Val)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Val * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void AAwmVehicle::OnStartFire()
{
	AAwmPlayerController* MyPC = Cast<AAwmPlayerController>(Controller);
	if (MyPC && MyPC->IsGameInputAllowed())
	{
		StartWeaponFire();
	}
}

void AAwmVehicle::OnStopFire()
{
	StopWeaponFire();
}

void AAwmVehicle::OnStartTargeting()
{
	AAwmPlayerController* MyPC = Cast<AAwmPlayerController>(Controller);
	if (MyPC && MyPC->IsGameInputAllowed())
	{
		SetTargeting(true);
	}
}

void AAwmVehicle::OnStopTargeting()
{
	SetTargeting(false);
}

void AAwmVehicle::OnNextWeapon()
{
	AAwmPlayerController* MyPC = Cast<AAwmPlayerController>(Controller);
	if (MyPC && MyPC->IsGameInputAllowed())
	{
		if (Inventory.Num() >= 2 && (CurrentWeapon == NULL || CurrentWeapon->GetCurrentState() != EWeaponState::Equipping))
		{
			const int32 CurrentWeaponIdx = Inventory.IndexOfByKey(CurrentWeapon);
			AAwmWeapon* NextWeapon = Inventory[(CurrentWeaponIdx + 1) % Inventory.Num()];
			EquipWeapon(NextWeapon);
		}
	}
}

void AAwmVehicle::OnPrevWeapon()
{
	AAwmPlayerController* MyPC = Cast<AAwmPlayerController>(Controller);
	if (MyPC && MyPC->IsGameInputAllowed())
	{
		if (Inventory.Num() >= 2 && (CurrentWeapon == NULL || CurrentWeapon->GetCurrentState() != EWeaponState::Equipping))
		{
			const int32 CurrentWeaponIdx = Inventory.IndexOfByKey(CurrentWeapon);
			AAwmWeapon* PrevWeapon = Inventory[(CurrentWeaponIdx - 1 + Inventory.Num()) % Inventory.Num()];
			EquipWeapon(PrevWeapon);
		}
	}
}

void AAwmVehicle::OnReload()
{
	AAwmPlayerController* MyPC = Cast<AAwmPlayerController>(Controller);
	if (MyPC && MyPC->IsGameInputAllowed())
	{
		if (CurrentWeapon)
		{
			CurrentWeapon->StartReload();
		}
	}
}


//////////////////////////////////////////////////////////////////////////
// Reading data

bool AAwmVehicle::IsTargeting() const
{
	return bIsTargeting;
}

bool AAwmVehicle::IsFiring() const
{
	return bWantsToFire;
};


//////////////////////////////////////////////////////////////////////////
// Inventory

AAwmWeapon* AAwmVehicle::GetWeapon() const
{
	return CurrentWeapon;
}

FName AAwmVehicle::GetWeaponAttachPoint() const
{
	return WeaponAttachPoint;
}

int32 AAwmVehicle::GetInventoryCount() const
{
	return Inventory.Num();
}

AAwmWeapon* AAwmVehicle::GetInventoryWeapon(int32 index) const
{
	return Inventory[index];
}

void AAwmVehicle::SpawnDefaultInventory()
{
	if (Role < ROLE_Authority)
	{
		return;
	}

	int32 NumWeaponClasses = DefaultInventoryClasses.Num();
	for (int32 i = 0; i < NumWeaponClasses; i++)
	{
		if (DefaultInventoryClasses[i])
		{
			FActorSpawnParameters SpawnInfo;
			SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
			AAwmWeapon* NewWeapon = GetWorld()->SpawnActor<AAwmWeapon>(DefaultInventoryClasses[i], SpawnInfo);
			AddWeapon(NewWeapon);
		}
	}

	// equip first weapon in inventory
	if (Inventory.Num() > 0)
	{
		EquipWeapon(Inventory[0]);
	}
}

void AAwmVehicle::DestroyInventory()
{
	if (Role < ROLE_Authority)
	{
		return;
	}

	// remove all weapons from inventory and destroy them
	for (int32 i = Inventory.Num() - 1; i >= 0; i--)
	{
		AAwmWeapon* Weapon = Inventory[i];
		if (Weapon)
		{
			RemoveWeapon(Weapon);
			Weapon->Destroy();
		}
	}
}

void AAwmVehicle::AddWeapon(AAwmWeapon* Weapon)
{
	if (Weapon && Role == ROLE_Authority)
	{
		Weapon->OnEnterInventory(this);
		Inventory.AddUnique(Weapon);
	}
}

void AAwmVehicle::RemoveWeapon(AAwmWeapon* Weapon)
{
	if (Weapon && Role == ROLE_Authority)
	{
		Weapon->OnLeaveInventory();
		Inventory.RemoveSingle(Weapon);
	}
}

AAwmWeapon* AAwmVehicle::FindWeapon(TSubclassOf<AAwmWeapon> WeaponClass)
{
	for (int32 i = 0; i < Inventory.Num(); i++)
	{
		if (Inventory[i] && Inventory[i]->IsA(WeaponClass))
		{
			return Inventory[i];
		}
	}

	return NULL;
}

void AAwmVehicle::EquipWeapon(AAwmWeapon* Weapon)
{
	if (Weapon)
	{
		if (Role == ROLE_Authority)
		{
			SetCurrentWeapon(Weapon, CurrentWeapon);
		}
		else
		{
			ServerEquipWeapon(Weapon);
		}
	}
}

bool AAwmVehicle::ServerEquipWeapon_Validate(AAwmWeapon* Weapon)
{
	return true;
}

void AAwmVehicle::ServerEquipWeapon_Implementation(AAwmWeapon* Weapon)
{
	EquipWeapon(Weapon);
}

void AAwmVehicle::OnRep_CurrentWeapon(AAwmWeapon* LastWeapon)
{
	SetCurrentWeapon(CurrentWeapon, LastWeapon);
}

void AAwmVehicle::SetCurrentWeapon(AAwmWeapon* NewWeapon, AAwmWeapon* LastWeapon)
{
	AAwmWeapon* LocalLastWeapon = NULL;

	if (LastWeapon != NULL)
	{
		LocalLastWeapon = LastWeapon;
	}
	else if (NewWeapon != CurrentWeapon)
	{
		LocalLastWeapon = CurrentWeapon;
	}

	// unequip previous
	if (LocalLastWeapon)
	{
		LocalLastWeapon->OnUnEquip();
	}

	CurrentWeapon = NewWeapon;

	// equip new one
	if (NewWeapon)
	{
		NewWeapon->SetVehicleOwner(this);	// Make sure weapon's MyPawn is pointing back to us. During replication, we can't guarantee APawn::CurrentWeapon will rep after AWeapon::MyPawn!

		NewWeapon->OnEquip(LastWeapon);
	}
}


//////////////////////////////////////////////////////////////////////////
// Replication

void AAwmVehicle::PreReplication(IRepChangedPropertyTracker & ChangedPropertyTracker)
{
	Super::PreReplication(ChangedPropertyTracker);

	// Only replicate this property for a short duration after it changes so join in progress players don't get spammed with fx when joining late
	DOREPLIFETIME_ACTIVE_OVERRIDE(AAwmVehicle, LastTakeHitInfo, GetWorld() && GetWorld()->GetTimeSeconds() < LastTakeHitTimeTimeout);
}

void AAwmVehicle::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// only to local owner: weapon change requests are locally instigated, other clients don't need it
	DOREPLIFETIME_CONDITION(AAwmVehicle, Inventory, COND_OwnerOnly);

	// everyone except local owner: flag change is locally instigated
	DOREPLIFETIME_CONDITION(AAwmVehicle, bIsTargeting, COND_SkipOwner);

	DOREPLIFETIME_CONDITION(AAwmVehicle, LastTakeHitInfo, COND_Custom);

	// everyone
	DOREPLIFETIME(AAwmVehicle, CurrentWeapon);
	DOREPLIFETIME(AAwmVehicle, Health);
}
