// TUIManager.cpp (전체 교체용)  ✅ Android 안정화 버전
#include "UI/InGame/TUIManager.h"

#include "TPlayerUIWidget.h"
#include "Blueprint/UserWidget.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "GameFramework/PlayerController.h"
#include "TimerManager.h"

#include "Character/TCharacterBase.h"
#include "Character/TPlayerCharacter.h"
#include "Item/TWeaponBase.h"
#include "Area/TCapturePoint.h"
#include "Character/TNonPlayerCharacter.h"
#include "Character/TNonPlayerCharacterSword.h"
#include "Spawner/TEnemySpawner.h"
#include "Game/TGameMode.h"

//////////////////////////////////////////////////////////////////////////
// 내부 헬퍼: Subsystem에서 안전하게 World 얻기
//////////////////////////////////////////////////////////////////////////

UWorld* UTUIManager::GetWorldSafe() const
{
	if (CachedPC.IsValid())
	{
		return CachedPC->GetWorld();
	}
	return nullptr;
}

//////////////////////////////////////////////////////////////////////////
// Subsystem LifeCycle
//////////////////////////////////////////////////////////////////////////

void UTUIManager::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}

void UTUIManager::Deinitialize()
{
	if (UWorld* World = GetWorldSafe())
	{
		World->GetTimerManager().ClearTimer(UIUpdateTimerHandle);
		World->GetTimerManager().ClearTimer(MonsterMonitorTimer);
		World->GetTimerManager().ClearTimer(HitDetectionTimer);
		World->GetTimerManager().ClearTimer(RestartMissionTimer);
	}

	Super::Deinitialize();
}

//////////////////////////////////////////////////////////////////////////
// UI 생성 (핵심 수정: PC를 받아서 CreateWidget의 OwningObject로 사용)
//////////////////////////////////////////////////////////////////////////

void UTUIManager::CreatePlayerUI(APlayerController* OwningPC)
{
	if (!IsValid(OwningPC))
	{
		UE_LOG(LogTemp, Error, TEXT("[UI] CreatePlayerUI: OwningPC is NULL"));
		return;
	}

	// ✅ PC 캐시 (Subsystem에서 World를 직접 믿지 않음)
	CachedPC = OwningPC;

	UWorld* World = OwningPC->GetWorld();
	if (!World)
	{
		UE_LOG(LogTemp, Error, TEXT("[UI] CreatePlayerUI: World is NULL"));
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("[UI] CreatePlayerUI: PC=%s World=%s"),
		*GetNameSafe(OwningPC), *GetNameSafe(World));

	// 위젯 클래스 로드
	if (!PlayerUIWidgetClass)
	{
		PlayerUIWidgetClass = LoadClass<UTPlayerUIWidget>(nullptr, TEXT("/Game/UI/InGame/WBP_PlayerUI.WBP_PlayerUI_C"));
		if (!PlayerUIWidgetClass)
		{
			UE_LOG(LogTemp, Error, TEXT("[UI] PlayerUIWidgetClass Load FAILED"));
			return;
		}
	}

	// GameMode 참조
	GameModeRef = Cast<ATGameMode>(World->GetAuthGameMode());

	// ✅ CreateWidget의 OwningObject에 PC를 넣는다 (모바일에서 가장 안전)
	PlayerUIWidget = CreateWidget<UTPlayerUIWidget>(OwningPC, PlayerUIWidgetClass);
	if (!PlayerUIWidget)
	{
		UE_LOG(LogTemp, Error, TEXT("[UI] CreateWidget FAILED"));
		return;
	}

	PlayerUIWidget->AddToViewport();

	// UI 업데이트 타이머
	World->GetTimerManager().SetTimer(
		UIUpdateTimerHandle,
		this,
		&UTUIManager::UpdateAllUI,
		0.1f,
		true
	);

	StartHitDetection();
	FindAndRegisterCapturePoints();
	FindAndRegisterEnemySpawners();
	StartMonitoringMonsters();

	CurrentMissionObjective = TEXT("Mission:");

	// ✅ 람다에서 GetWorld() 직접 호출 금지 → WeakPC로 World 재획득
	TWeakObjectPtr<UTUIManager> WeakThis(this);
	TWeakObjectPtr<APlayerController> WeakPC(OwningPC);

	FTimerHandle InitMissionTimer;
	World->GetTimerManager().SetTimer(
		InitMissionTimer,
		[WeakThis, WeakPC]()
		{
			if (!WeakThis.IsValid() || !WeakPC.IsValid()) return;

			UWorld* W = WeakPC->GetWorld();
			if (!W) return;

			UTUIManager* Self = WeakThis.Get();

			if (Self->PlayerUIWidget && Self->PlayerUIWidget->IsWidgetFullyInitialized())
			{
				Self->PlayerUIWidget->UpdateMissionObjective(TEXT("Mission:"));
			}

			FTimerHandle MissionStateTimer;
			W->GetTimerManager().SetTimer(
				MissionStateTimer,
				[WeakThis]()
				{
					if (!WeakThis.IsValid()) return;
					WeakThis->UpdateMissionState();
				},
				1.3f,
				false
			);
		},
		0.2f,
		false
	);
}

//////////////////////////////////////////////////////////////////////////
// Player 세팅
//////////////////////////////////////////////////////////////////////////

void UTUIManager::SetPlayerCharacter(ATCharacterBase* PlayerChar)
{
	PlayerCharacter = PlayerChar;

	if (IsValid(PlayerCharacter))
	{
		if (ATPlayerCharacter* PC = Cast<ATPlayerCharacter>(PlayerCharacter))
		{
			CurrentWeapon = PC->CurrentWeapon;
		}
	}
	else
	{
		PlayerCharacter = nullptr;
		CurrentWeapon = nullptr;
	}
}

//////////////////////////////////////////////////////////////////////////
// UI 업데이트들 (World 접근은 전부 GetWorldSafe로)
//////////////////////////////////////////////////////////////////////////

void UTUIManager::UpdatePlayerHP()
{
	if (!PlayerUIWidget || !PlayerUIWidget->IsWidgetFullyInitialized())
		return;

	UWorld* World = GetWorldSafe();
	if (!World) return;

	if (APlayerController* PC = World->GetFirstPlayerController())
	{
		if (APawn* CurrentPawn = PC->GetPawn())
		{
			if (ATPlayerCharacter* CurrentPlayer = Cast<ATPlayerCharacter>(CurrentPawn))
			{
				const float CurrentHP = CurrentPlayer->GetCurrentHP();
				const float MaxHP = CurrentPlayer->GetMaxHP();
				PlayerUIWidget->UpdateHPBar(CurrentHP, MaxHP);
				return;
			}
		}
	}
}

void UTUIManager::UpdatePlayerAmmo()
{
	if (!PlayerUIWidget || !PlayerUIWidget->IsWidgetFullyInitialized())
		return;

	if (PlayerUIWidget && CurrentWeapon)
	{
		PlayerUIWidget->UpdateAmmoInfo(CurrentWeapon->GetCurrentAmmo(), CurrentWeapon->GetTotalAmmo());
	}
}

void UTUIManager::UpdateWeaponInfo()
{
	if (!PlayerUIWidget || !PlayerUIWidget->IsWidgetFullyInitialized())
		return;

	if (CurrentWeapon != PreviousWeapon)
	{
		if (CurrentWeapon && PlayerUIWidget)
		{
			FString WeaponName = CurrentWeapon->GetWeaponTypeString();
			PlayerUIWidget->UpdateWeaponName(WeaponName);

			if (bFirstCaptureCompleted && !bWeaponPickedUp)
			{
				if (WeaponName != TEXT("Pistol") && WeaponName != TEXT("No Weapon"))
				{
					bWeaponPickedUp = true;
					bWeaponUnlocked = true;
					MoveToNextCapturePoint();
					UpdateMissionState();
				}
			}
			else if (bWeaponPickedUp && !bSecondWeaponPickedUp && CurrentCaptureIndex == 1)
			{
				if (WeaponName != TEXT("Pistol") && WeaponName != TEXT("No Weapon"))
				{
					if (PreviousWeapon && WeaponName != PreviousWeapon->GetWeaponTypeString())
					{
						bSecondWeaponPickedUp = true;
						UpdateMissionState();
					}
				}
			}
		}
		else if (!CurrentWeapon && PlayerUIWidget)
		{
			PlayerUIWidget->UpdateWeaponName(TEXT("No Weapon"));
		}

		PreviousWeapon = CurrentWeapon;
	}
}

//////////////////////////////////////////////////////////////////////////
// 거점 UI
//////////////////////////////////////////////////////////////////////////

void UTUIManager::ShowCaptureUI(const FString& AreaName)
{
	if (PlayerUIWidget && PlayerUIWidget->IsWidgetFullyInitialized())
	{
		PlayerUIWidget->ShowCaptureUI(AreaName);
	}
}

void UTUIManager::HideCaptureUI()
{
	if (PlayerUIWidget && PlayerUIWidget->IsWidgetFullyInitialized())
	{
		PlayerUIWidget->HideCaptureUI();
	}
}

void UTUIManager::UpdateCaptureProgress(float Progress)
{
	if (PlayerUIWidget && PlayerUIWidget->IsWidgetFullyInitialized())
	{
		PlayerUIWidget->UpdateCaptureProgress(Progress);
	}
}

void UTUIManager::RegisterCapturePoint(ATCapturePoint* CapturePoint)
{
	if (CapturePoint)
	{
		CurrentCapturePoint = CapturePoint;
	}
}

void UTUIManager::FindAndRegisterCapturePoints()
{
	UWorld* World = GetWorldSafe();
	if (!World) return;

	AllCapturePoints.Empty();
	ATCapturePoint* FirstPoint = nullptr;
	ATCapturePoint* SecondPoint = nullptr;

	for (TActorIterator<ATCapturePoint> ActorItr(World); ActorItr; ++ActorItr)
	{
		ATCapturePoint* CapturePoint = *ActorItr;
		if (!CapturePoint) continue;

		const FString ActorName = CapturePoint->GetName();

		if (ActorName.Contains(TEXT("First")) || ActorName.Contains(TEXT("1")) || ActorName.Contains(TEXT("One")))
		{
			FirstPoint = CapturePoint;
		}
		else if (ActorName.Contains(TEXT("Second")) || ActorName.Contains(TEXT("2")) || ActorName.Contains(TEXT("Two")))
		{
			SecondPoint = CapturePoint;
		}
	}

	if (FirstPoint) AllCapturePoints.Add(FirstPoint);
	if (SecondPoint) AllCapturePoints.Add(SecondPoint);

	if (AllCapturePoints.Num() >= 2)
	{
		CurrentCapturePoint = AllCapturePoints[CurrentCaptureIndex];
		CapturePointName = CurrentCapturePoint->GetName();
	}
}

//////////////////////////////////////////////////////////////////////////
// 임무
//////////////////////////////////////////////////////////////////////////

void UTUIManager::SetMissionObjective(const FString& NewObjective)
{
	CurrentMissionObjective = NewObjective;

	if (PlayerUIWidget && PlayerUIWidget->IsWidgetFullyInitialized())
	{
		PlayerUIWidget->UpdateMissionObjective(NewObjective);
	}
}

void UTUIManager::UpdateMissionProgress()
{
	UpdateMissionState();
}

void UTUIManager::UpdateMissionState()
{
	FString NewObjective;

	if (bSecondCaptureCompleted)
	{
		NewObjective = TEXT("Victory!");
		OnVictoryEvent.Broadcast();
	}
	else if (bSecondWeaponPickedUp && CurrentCaptureIndex == 1)
	{
		if (bNearCapturePoint && bCapturePhase) NewObjective = TEXT("Capture Point 2");
		else if (bNearCapturePoint && !bCapturePhase) { NewObjective = TEXT("Enter Point 2"); bCapturePhase = true; }
		else NewObjective = TEXT("Move to Point 2");
	}
	else if (bWeaponPickedUp && !bSecondWeaponPickedUp && CurrentCaptureIndex == 1)
	{
		NewObjective = TEXT("Get RailGun");
	}
	else if (bFirstCaptureCompleted && !bWeaponPickedUp)
	{
		NewObjective = TEXT("Get Shotgun");
	}
	else if (bWaveCompleted && CurrentCaptureIndex == 0)
	{
		if (bNearCapturePoint && bCapturePhase) NewObjective = TEXT("Capture Point 1");
		else if (bNearCapturePoint && !bCapturePhase) { NewObjective = TEXT("Enter Point 1"); bCapturePhase = true; }
		else NewObjective = TEXT("Move to Point 1");
	}
	else if (bWaveActive || TrackedMonsters.Num() > 0)
	{
		int32 SpawnerBasedMax = 0;
		for (ATEnemySpawner* Spawner : RegisteredSpawners)
		{
			if (Spawner) SpawnerBasedMax += Spawner->MaxSpawnCount;
		}

		const int32 RemainingCount = FMath::Max(0, SpawnerBasedMax - MonsterKillCount);
		NewObjective = FString::Printf(TEXT("Kill Enemies (%d left)"), RemainingCount);
	}
	else
	{
		NewObjective = TEXT("Kill All Enemies");
	}

	if (CurrentMissionObjective != NewObjective)
	{
		SetMissionObjective(NewObjective);
	}
}

//////////////////////////////////////////////////////////////////////////
// 스포너 / 몬스터 모니터링
//////////////////////////////////////////////////////////////////////////

void UTUIManager::FindAndRegisterEnemySpawners()
{
	UWorld* World = GetWorldSafe();
	if (!World) return;

	RegisteredSpawners.Empty();

	for (TActorIterator<ATEnemySpawner> ActorItr(World); ActorItr; ++ActorItr)
	{
		ATEnemySpawner* Spawner = *ActorItr;
		if (Spawner)
		{
			RegisteredSpawners.Add(Spawner);
		}
	}

	if (RegisteredSpawners.Num() > 0)
	{
		UpdateWaveInfoFromSpawners();
	}
}

void UTUIManager::StartMonitoringMonsters()
{
	UWorld* World = GetWorldSafe();
	if (!World) return;

	World->GetTimerManager().SetTimer(
		MonsterMonitorTimer,
		this,
		&UTUIManager::UpdateMonsterStatus,
		0.5f,
		true
	);
}

void UTUIManager::StopMonitoringMonsters()
{
	if (UWorld* World = GetWorldSafe())
	{
		World->GetTimerManager().ClearTimer(MonsterMonitorTimer);
	}
}

void UTUIManager::UpdateMonsterStatus()
{
	FindAllMonstersInWorld();

	const int32 CurrentMonsterCount = TrackedMonsters.Num();

	bool bGameModeWaveActive = false;
	if (GameModeRef)
	{
		bGameModeWaveActive = GameModeRef->bIsWaveActive;
	}

	bool bAnySpawnerActive = false;
	int32 SpawnerBasedMax = 0;

	for (ATEnemySpawner* Spawner : RegisteredSpawners)
	{
		if (!Spawner) continue;

		const int32 CurrentSpawned = Spawner->GetCurrentSpawned();
		const int32 MaxSpawn = Spawner->MaxSpawnCount;
		SpawnerBasedMax += MaxSpawn;

		const bool bSpawnerCompleted = (CurrentSpawned >= MaxSpawn);

		if (!bSpawnerCompleted && CurrentSpawned > 0)
			bAnySpawnerActive = true;
	}

	if (!bWaveActive && !bWaveCompleted)
	{
		if (bGameModeWaveActive || bAnySpawnerActive || CurrentMonsterCount > 0)
		{
			bWaveActive = true;
			bWaveCompleted = false;
			LastWaveMonsterCount = 0;
			MonsterKillCount = 0;

			PreExistingMonsters.Empty();
			for (ATCharacterBase* Monster : TrackedMonsters)
			{
				PreExistingMonsters.Add(Monster);
			}

			WaveSpawnedMonsters.Empty();
			TotalWaveMonsters = 0;

			if (PlayerUIWidget && PlayerUIWidget->IsWidgetFullyInitialized())
			{
				PlayerUIWidget->ShoWEnemyIncomingAlarm();

				if (UWorld* World = GetWorldSafe())
				{
					FTimerHandle AlarmTimer;
					TWeakObjectPtr<UTUIManager> WeakThis(this);

					World->GetTimerManager().SetTimer(
						AlarmTimer,
						[WeakThis]()
						{
							if (!WeakThis.IsValid()) return;

							if (WeakThis->PlayerUIWidget && WeakThis->PlayerUIWidget->IsWidgetFullyInitialized())
							{
								WeakThis->PlayerUIWidget->HideEnemyIncomingAlarm();
							}
						},
						3.0f,
						false
					);
				}
			}
		}
	}

	if (bWaveActive && !bWaveCompleted)
	{
		for (ATCharacterBase* Monster : TrackedMonsters)
		{
			if (!WaveSpawnedMonsters.Contains(Monster) && !PreExistingMonsters.Contains(Monster))
			{
				WaveSpawnedMonsters.Add(Monster);
				TotalWaveMonsters++;
			}
		}

		WaveSpawnedMonsters.RemoveAll([](ATCharacterBase* Monster)
		{
			return !Monster || Monster->GetCurrentHP() <= 0;
		});

		if (CurrentMonsterCount < LastFrameMonsterCount)
		{
			const int32 KilledCount = LastFrameMonsterCount - CurrentMonsterCount;

			for (int32 i = 0; i < KilledCount; i++)
			{
				MonsterKillCount++;
			}

			if (PlayerUIWidget && PlayerUIWidget->IsWidgetFullyInitialized())
			{
				PlayerUIWidget->UpdateKillCount(MonsterKillCount, SpawnerBasedMax);
			}

			UpdateMissionState();
		}

		if (CurrentMonsterCount == 0)
		{
			bWaveCompleted = true;
			bWaveActive = false;
			WaveSpawnedMonsters.Empty();
			LastWaveMonsterCount = 0;

			UpdateMissionState();
		}
	}

	LastFrameMonsterCount = CurrentMonsterCount;
}

void UTUIManager::FindAllMonstersInWorld()
{
	UWorld* World = GetWorldSafe();
	if (!World) return;

	TrackedMonsters.Empty();

	for (TActorIterator<ATNonPlayerCharacter> ActorItr(World); ActorItr; ++ActorItr)
	{
		ATNonPlayerCharacter* Monster = *ActorItr;
		if (!Monster) continue;

		if (Monster->GetCurrentHP() > 0)
		{
			TrackedMonsters.Add(Cast<ATCharacterBase>(Monster));
		}
	}

	for (TActorIterator<ATNonPlayerCharacterSword> SwordItr(World); SwordItr; ++SwordItr)
	{
		ATNonPlayerCharacterSword* Monster = *SwordItr;
		if (!Monster) continue;

		if (Monster->GetCurrentHP() > 0)
		{
			TrackedMonsters.Add(Cast<ATCharacterBase>(Monster));
		}
	}
}

void UTUIManager::UpdateWaveInfoFromSpawners()
{
	int32 TotalMaxMonsters = 0;
	for (ATEnemySpawner* Spawner : RegisteredSpawners)
	{
		if (Spawner)
		{
			TotalMaxMonsters += Spawner->MaxSpawnCount;
		}
	}

	if (TotalMaxMonsters > 0)
	{
		RemainingMonsters = TotalMaxMonsters;
		MonsterKillCount = 0;
		bWaveCompleted = false;

		if (PlayerUIWidget && PlayerUIWidget->IsWidgetFullyInitialized())
		{
			PlayerUIWidget->UpdateKillCount(0, TotalMaxMonsters);
		}
	}
}

void UTUIManager::MoveToNextCapturePoint()
{
	if (AllCapturePoints.IsValidIndex(CurrentCaptureIndex + 1))
	{
		CurrentCaptureIndex++;
		CurrentCapturePoint = AllCapturePoints[CurrentCaptureIndex];
		CapturePointName = CurrentCapturePoint->GetName();
	}
}

//////////////////////////////////////////////////////////////////////////
// 정기 UI 업데이트
//////////////////////////////////////////////////////////////////////////

void UTUIManager::UpdateAllUI()
{
	if (!PlayerUIWidget || !PlayerUIWidget->IsWidgetFullyInitialized())
		return;

	if (ATPlayerCharacter* PC = Cast<ATPlayerCharacter>(PlayerCharacter))
	{
		CurrentWeapon = PC->CurrentWeapon;
	}

	UpdatePlayerHP();
	UpdatePlayerAmmo();
	UpdateWeaponInfo();
	DetectTeleportDash();

	if (IsValid(CurrentCapturePoint) && PlayerUIWidget)
	{
		if (CurrentCapturePoint->bPlayerInArea)
		{
			bNearCapturePoint = true;

			if (!PlayerUIWidget->IsCaptureUIVisible())
			{
				ShowCaptureUI(CapturePointName);
			}

			const float ProgressPercent = CurrentCapturePoint->CapturePercent / 100.0f;
			UpdateCaptureProgress(ProgressPercent);

			if (CurrentCapturePoint->CapturePercent >= 100.0f)
			{
				if (CurrentCaptureIndex == 0 && !bFirstCaptureCompleted)
				{
					bFirstCaptureCompleted = true;
					bCapturePhase = false;
					HideCaptureUI();
					
					if (IsValid(GameModeRef))
					{
						GameModeRef->LastCapturedPoint = CurrentCapturePoint;
					}
					else
					{
						GameModeRef = nullptr;
					}

					UpdateMissionState();
				}
				else if (CurrentCaptureIndex == 1 && !bSecondCaptureCompleted)
				{
					bSecondCaptureCompleted = true;
					bCapturePhase = false;
					HideCaptureUI();

					if (IsValid(GameModeRef))
					{
						GameModeRef->LastCapturedPoint = CurrentCapturePoint;
					}
					else
					{
						GameModeRef = nullptr;
					}

					UpdateMissionState();
				}
			}
		}
		else
		{
			bNearCapturePoint = false;

			if (PlayerUIWidget->IsCaptureUIVisible())
			{
				HideCaptureUI();
			}
			CurrentCapturePoint = nullptr;
		}
	}
}

//////////////////////////////////////////////////////////////////////////
// Restart UI
//////////////////////////////////////////////////////////////////////////

void UTUIManager::RestartGameUI()
{
	// 미션 관련 변수 초기화
	bWaveActive = false;
	bWaveCompleted = false;
	bFirstCaptureCompleted = false;
	bSecondCaptureCompleted = false;
	bWeaponUnlocked = false;
	bNearCapturePoint = false;
	bCapturePhase = false;

	// 몬스터 카운트 초기화
	MonsterKillCount = 0;
	RemainingMonsters = 0;
	LastFrameMonsterCount = 0;
	LastWaveMonsterCount = 0;
	TotalWaveMonsters = 0;

	// 히트 감지 변수 초기화
	LastWeaponAmmo = -1;
	LastMonsterHPs.Empty();

	// 무기 관련 초기화
	bWeaponSpawned = false;
	bWeaponPickedUp = false;
	bSecondWeaponPickedUp = false;

	// 거점 관련 초기화
	CurrentCaptureIndex = 0;
	if (AllCapturePoints.IsValidIndex(0))
	{
		CurrentCapturePoint = AllCapturePoints[0];
		CapturePointName = CurrentCapturePoint->GetName();
	}

	TrackedMonsters.Empty();
	WaveSpawnedMonsters.Empty();
	PreExistingMonsters.Empty();

	if (PlayerUIWidget && PlayerUIWidget->IsWidgetFullyInitialized())
	{
		CurrentMissionObjective = TEXT("Mission:");
		PlayerUIWidget->UpdateMissionObjective(TEXT("Mission:"));
		PlayerUIWidget->UpdateKillCount(0, 0);
		PlayerUIWidget->HideCaptureUI();
		PlayerUIWidget->HideEnemyIncomingAlarm();
		PlayerUIWidget->ShowDashReady();

		if (UWorld* World = GetWorldSafe())
		{
			if (APlayerController* PC = World->GetFirstPlayerController())
			{
				if (APawn* PlayerPawn = PC->GetPawn())
				{
					if (ATPlayerCharacter* Player = Cast<ATPlayerCharacter>(PlayerPawn))
					{
						PlayerUIWidget->UpdateHPBar(Player->GetCurrentHP(), Player->GetMaxHP());

						ATWeaponBase* PlayerWeapon = Player->GetCurrentWeapon();
						if (PlayerWeapon)
						{
							PlayerUIWidget->UpdateAmmoInfo(PlayerWeapon->GetCurrentAmmo(), PlayerWeapon->GetTotalAmmo());
							PlayerUIWidget->UpdateWeaponName(PlayerWeapon->GetWeaponTypeString());
						}
						else
						{
							PlayerUIWidget->UpdateAmmoInfo(7, 100);
							PlayerUIWidget->UpdateWeaponName(TEXT("Pistol"));
						}
					}
				}
			}
		}
	}

	if (UWorld* World = GetWorldSafe())
	{
		World->GetTimerManager().ClearTimer(MonsterMonitorTimer);
		World->GetTimerManager().ClearTimer(UIUpdateTimerHandle);
		World->GetTimerManager().ClearTimer(HitDetectionTimer);

		TWeakObjectPtr<UTUIManager> WeakThis(this);

		FTimerHandle DelayTimer;
		World->GetTimerManager().SetTimer(
			DelayTimer,
			[WeakThis]()
			{
				if (!WeakThis.IsValid()) return;

				WeakThis->StartMonitoringMonsters();
				WeakThis->StartHitDetection();

				if (UWorld* W = WeakThis->GetWorldSafe())
				{
					W->GetTimerManager().SetTimer(
						WeakThis->UIUpdateTimerHandle,
						WeakThis.Get(),
						&UTUIManager::UpdateAllUI,
						0.1f,
						true
					);
				}
			},
			0.5f,
			false
		);
	}

	FindAndRegisterEnemySpawners();
	UpdateWaveInfoFromSpawners();

	if (UWorld* World = GetWorldSafe())
	{
		TWeakObjectPtr<UTUIManager> WeakThis(this);
		World->GetTimerManager().SetTimer(
			RestartMissionTimer,
			[WeakThis]()
			{
				if (!WeakThis.IsValid()) return;
				WeakThis->UpdateMissionState();
			},
			1.5f,
			false
		);
	}
}

//////////////////////////////////////////////////////////////////////////
// Hit Marker
//////////////////////////////////////////////////////////////////////////

void UTUIManager::StartHitDetection()
{
	UWorld* World = GetWorldSafe();
	if (!World) return;

	if (!HitDetectionTimer.IsValid())
	{
		World->GetTimerManager().SetTimer(
			HitDetectionTimer,
			this,
			&UTUIManager::CheckForHits,
			0.1f,
			true
		);
	}
}

void UTUIManager::StopHitDetection()
{
	if (UWorld* World = GetWorldSafe())
	{
		if (HitDetectionTimer.IsValid())
		{
			World->GetTimerManager().ClearTimer(HitDetectionTimer);
		}
	}
}

void UTUIManager::CheckForHits()
{
	if (!PlayerCharacter) return;

	ATPlayerCharacter* Player = Cast<ATPlayerCharacter>(PlayerCharacter);
	if (!Player) return;

	ATWeaponBase* Weapon = Player->CurrentWeapon;
	if (!Weapon) return;

	const int32 CurrentAmmo = Weapon->GetCurrentAmmo();

	if (LastWeaponAmmo > 0 && CurrentAmmo < LastWeaponAmmo)
	{
		if (CheckMonsterHPChanges())
		{
			if (PlayerUIWidget && PlayerUIWidget->IsWidgetFullyInitialized())
			{
				PlayerUIWidget->ShowHitMarker();
			}
		}
	}

	LastWeaponAmmo = CurrentAmmo;
	UpdateMonsterHPList();
}

bool UTUIManager::CheckMonsterHPChanges()
{
	for (int32 i = 0; i < TrackedMonsters.Num(); i++)
	{
		if (TrackedMonsters[i] && IsValid(TrackedMonsters[i]))
		{
			const float CurrentHP = TrackedMonsters[i]->GetCurrentHP();
			if (i < LastMonsterHPs.Num())
			{
				if (CurrentHP < LastMonsterHPs[i])
				{
					return true;
				}
			}
		}
	}
	return false;
}

void UTUIManager::UpdateMonsterHPList()
{
	LastMonsterHPs.Empty();

	for (ATCharacterBase* Monster : TrackedMonsters)
	{
		if (Monster && IsValid(Monster))
		{
			LastMonsterHPs.Add(Monster->GetCurrentHP());
		}
	}
}

//////////////////////////////////////////////////////////////////////////
// Dash UI (원본 로직 유지)
//////////////////////////////////////////////////////////////////////////

void UTUIManager::DetectTeleportDash()
{
	if (!PlayerCharacter || !PlayerUIWidget || !PlayerUIWidget->IsWidgetFullyInitialized())
		return;

	bool bCanDashFromBP = true;

	if (ATPlayerCharacter* Player = Cast<ATPlayerCharacter>(PlayerCharacter))
	{
		UClass* PlayerClass = Player->GetClass();

		for (FProperty* Property = PlayerClass->PropertyLink; Property; Property = Property->PropertyLinkNext)
		{
			const FString PropName = Property->GetName();

			if (PropName.Contains(TEXT("CanDash")) || PropName.Contains(TEXT("Can Dash")))
			{
				if (FBoolProperty* BoolProp = CastField<FBoolProperty>(Property))
				{
					bCanDashFromBP = BoolProp->GetPropertyValue_InContainer(Player);
					break;
				}
			}
		}
	}

	static bool bPreviousCanDash = true;
	static bool bFirstRun = true;

	if (bFirstRun)
	{
		bFirstRun = false;
		bPreviousCanDash = bCanDashFromBP;

		if (PlayerUIWidget)
		{
			if (UWidget* DashWidget = PlayerUIWidget->GetWidgetFromName(TEXT("DashText")))
			{
				if (UTextBlock* DashText = Cast<UTextBlock>(DashWidget))
				{
					DashText->SetText(FText::FromString(TEXT("Dash Ready!")));
					DashText->SetColorAndOpacity(FLinearColor::Green);
				}
			}
		}
		return;
	}

	if (PlayerUIWidget)
	{
		if (UWidget* DashWidget = PlayerUIWidget->GetWidgetFromName(TEXT("DashText")))
		{
			if (UTextBlock* DashText = Cast<UTextBlock>(DashWidget))
			{
				if (bCanDashFromBP)
				{
					DashText->SetText(FText::FromString(TEXT("Dash Ready!")));
					DashText->SetColorAndOpacity(FLinearColor::Green);
				}
				else
				{
					DashText->SetText(FText::FromString(TEXT("Dash CoolTime!")));
					DashText->SetColorAndOpacity(FLinearColor::Red);
				}
			}
		}
	}

	bPreviousCanDash = bCanDashFromBP;
}
