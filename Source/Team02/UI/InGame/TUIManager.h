#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "TPlayerUIWidget.h"
#include "Area/TCapturePoint.h"
#include "TUIManager.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnVictoryDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnGameOverDelegate);

class ATCharacterBase;
class ATWeaponBase;
class ATAIBossMonster;

UCLASS()
class TEAM02_API UTUIManager : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	// OutGameUI 개발자와 협업 델리게이트 코드
	UPROPERTY(BLueprintAssignable)
	FOnVictoryDelegate OnVictoryEvent;

	UPROPERTY(BlueprintAssignable)
	FOnGameOverDelegate OnGameOverEvent;
	
	
	// initializce Subsystem
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	//create UI
	UFUNCTION(BlueprintCallable)
	void CreatePlayerUI();

	// update ui functions
	UFUNCTION(BlueprintCallable)
	void UpdatePlayerHP();
	
	UFUNCTION(BlueprintCallable)
	void UpdatePlayerAmmo();
	
	// Player references
	UFUNCTION(BlueprintCallable)
	void SetPlayerCharacter(ATCharacterBase* PlayerChar);

	// 거점 관련 함수
	UFUNCTION(BlueprintCallable)
	void ShowCaptureUI(const FString& AreaName);

	UFUNCTION(BlueprintCallable)
	void HideCaptureUI();

	UFUNCTION(BlueprintCallable)
	void UpdateCaptureProgress(float Progress);

	// 거점 연동 함수
	UFUNCTION(BlueprintCallable)
	void RegisterCapturePoint(class ATCapturePoint* CapturePoint);

	// 거점 자동 검색 함수
	UFUNCTION(BlueprintCallable)
	void FindAndRegisterCapturePoints();

	// 임무 관련 함수
	UFUNCTION(BLueprintCallable)
	void SetMissionObjective(const FString& NewObjective);
	
	UFUNCTION(BlueprintCallable)
	void UpdateMissionProgress();

	
	// 몬스터 감지 및 추적
	UFUNCTION(BlueprintCallable)
	void StartMonitoringMonsters();

	UFUNCTION(BlueprintCallable)
	void StopMonitoringMonsters();

	// spawner 연동(읽기 전용)
	UFUNCTION(BlueprintCallable)
	void FindAndRegisterEnemySpawners();
	

protected:
	// UI widget class
	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	TSubclassOf<UTPlayerUIWidget> PlayerUIWidgetClass;

	//UI widget instance
	UPROPERTY()
	TObjectPtr<UTPlayerUIWidget> PlayerUIWidget;

	//Player reference
	UPROPERTY()
	TObjectPtr<ATCharacterBase> PlayerCharacter;

	// current weapon reference
	UPROPERTY()
	TObjectPtr<ATWeaponBase> CurrentWeapon;

	// Timer
	FTimerHandle UIUpdateTimerHandle;
	
	// 게임 플로우 반영 변수 ( 웨이브 시작> 몬스터 처치 (웨이브클리어) >거점 점령 >보스전 순서 )
	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	bool bWaveActive=false; // 웨이브 활성화 여부
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bWaveCompleted = false;  // 웨이브 완료 여부

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bCapturePhase = false;   // 점령 페이즈 여부

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bCaptureCompleted = false; // 점령완료
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bWeaponUnlocked=false;  // 무기 해금

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bBossPhase=false; // 보스전 페이즈

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 RemainingMonsters = 0;  // 남은 몬스터 수 (가상)

	// 현재 활성 거점
	UPROPERTY()
	TObjectPtr<class ATCapturePoint> CurrentCapturePoint;

	//거점 이름
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	FString CapturePointName=TEXT("Control Point");

	//임무 관련 변수들
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	FString CurrentMissionObjective=TEXT("Kill Monsters"); // 임무 목표 표시
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	int32 MonsterKillCount=0; // KillCountText UI 업데이트용
	
	// 임무 상태 플래그
	//거점 감지용
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	bool bNearCapturePoint=false; 

	// 몬스터 감시 타이머
	FTimerHandle MonsterMonitorTimer;

	// 등록된 스포너들
	UPROPERTY()
	TArray<class ATEnemySpawner*> RegisteredSpawners;

	//현재 추적 중인 몬스터들
	UPROPERTY()
	TArray<class ATNonPlayerCharacter*> TrackedMonsters;

	// 이전 프레임 몬스터,보스 수(변화 감지용)
	UPROPERTY()
	int32 LastFrameMonsterCount=0;

	UPROPERTY()
	int32 LastFrameBossCount=0;
	

	// 보스 추적용
	UPROPERTY()
	TArray<class ATAIBossMonster*> TrackedBosses;
	
	// regularly ui update
	void UpdateAllUI();

private:
	// 임무 상태 업데이트
	void UpdateMissionState();

	// 몬스터 상태 모니터링
	void UpdateMonsterStatus();

	// 월드에서 몬스터 찾기
	void FindAllMonstersInWorld();

	//스포너에서 웨이브 정보 가져오기
	void UpdateWaveInfoFromSpawners();

	//보스 찾기 함수
	void FindAllBossesInWorld();
};

//TCapturePoint의 Tick() 함수에서 이미 점령률이 계산되고 있으니
//UIManager가 CapturePoint를 감시
//플레이어 진입/이탈 감지
//점령률 변화 자동 감지
//웨이브 를 다 마치고 점령 100% 채우면 점령이 되는 방식