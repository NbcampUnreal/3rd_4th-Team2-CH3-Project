# [REDLine]

> 언리얼 엔진 5로 개발한 3인칭 점령전 TPS 게임 프로젝트입니다.

<br>

<p align="center">
  <img src="https://media.githubusercontent.com/media/NbcampUnreal/3rd_4th-Team2-CH3-Project/dev/Source/Team02/Feature/2TELLIGENCE_Logo.png">
</p>

---

## 📋 프로젝트 기본 정보 (Project Specification)

이 프로젝트를 열거나 빌드하기 위해 필요한 **필수 환경 정보**입니다. 버전이 맞지 않으면 프로젝트가 정상적으로 열리지 않을 수 있습니다.

* **언리얼 엔진 버전 (UE Version):** `5.5.4`
* **타겟 플랫폼 (Target Platform):** `Windows 64-bit`
* **IDE:** `Visual Studio 2022 and Rider`

<br>

## ✨ 주요 기능 (Features)

* **플레이어 캐릭터 (Player Character)**
    * 이동 (WASD, Jump) 및 3인칭 카메라 시스템
    * TPS 조준 (Aiming) 및 사격 (Shooting) 메커니즘
    * 재장전 및 무기 교체 시스템
* **AI 시스템 (AI System)**
    * 플레이어를 추적하고 공격하는 원거리 AI 및 근거리 AI
    * Behavior Tree와 Blackboard를 사용한 행동 로직
* **게임플레이 시스템 (Gameplay System)**
    * 체력 및 데미지 처리 시스템
    * 기본 UI (HUD): 체력, 총알 수, 크로스헤어 표시
* **캡처 포인트 (CapturePoint)**
    * 플레이어/적 진입·이탈 감지, 적 존재 시 캡처 중지, 적 삭제 시도 대비 OnDestroyed 바인딩으로 상태 정합성 유지
    * 100% 달성 시 한 번만 CompleteCapture() 호출, 연결된 벽(LinkedWalls) 동작 트리거, 특정 존(ZoneIndex==0)에서 지정 메시 파괴 가능
* **게임 모드 (GameMode)**
    * 웨이브 상태 관리: StartWave, EndWave, OnCapturePointCompleted, OnZoneOverlap
    * AI 블랙보드 키 일괄 토글로 웨이브 시작/종료 신호 동기화
    * UI 재시작 훅(Subsystem 연동) 및 전체 상태 초기화 루틴 제공 (InitializeGameState, RestartGameFromUI)
    * 리스폰: 마지막 점령 지점(LastCapturedPoint) 기준 스폰, 없으면 PlayerStart 사용
* **적 스포너 (EnemySpawner)**
    * 타이머 기반 주기 스폰, MaxSpawnCount 도달 시 자동 비활성화
    * 스폰 영역(Box) 내 랜덤 위치 계산, 활성/비활성 안전 제어 제공
* **아이템 스포너 (ItemSpawner)**
    * 최초/파괴 후 지연 스폰, 쿨다운 링 나이아가라 FX 로 남은 시간 시각화 (StartCooldownFX/StopCooldownFX)
    * 스폰된 아이템 파괴 시점에 다음 스폰 예약 및 링 재시작
* **아이템/무기 베이스 (ItemBase 및 WeaponBase)**
    * 아이템 베이스: 박스+스피어 오버랩 구성, 줍기 트리거 진입점 제공
    * 무기 베이스: 카메라 조준점을 기준으로 패럴랙스 보정 후 머즐에서 라인트레이스 판정, 데미지 적용/FX/탄약/쿨다운 일괄 처리(FireFrom, CanFire, Reload

<br>

## 🚀 프로젝트 설정 및 실행 (Project Setup & Launch)

이 프로젝트를 로컬 환경에서 설정하고 실행하는 방법입니다.

1.  이 프로젝트를 `git clone` 받거나 ZIP 파일로 다운로드합니다.

2.  프로젝트 폴더 안에 있는 `.uproject` 파일을 우클릭합니다.

3.  **`Generate Visual Studio project files`** 메뉴를 클릭하여 Visual Studio 혹은 Rider 솔루션(.sln) 파일을 생성합니다.
    * *만약 이 메뉴가 보이지 않는다면, Epic Games Launcher에서 해당 엔진 버전을 설치했는지 확인하세요.*

4.  생성된 `.sln` 파일을 Visual Studio 2022혹은 Rider로 엽니다.

5.  Visual Studio 혹은 Rider 상단의 솔루션 구성을 `Development Editor`, `Win64` 로 맞춘 후, 솔루션 탐색기에서 프로젝트를 우클릭하여 **`Build`** 합니다.

6.  빌드가 성공적으로 완료되면, Visual Studio 혹은 Rider에서 `F5` 키를 눌러 디버깅과 함께 에디터를 실행하거나, 프로젝트 폴더의 `.uproject` 파일을 더블클릭하여 언리얼 에디터를 엽니다.

<br>
 
## 🛠 주요 클래스 및 블루프린트 (Key Classes & Blueprints)

프로젝트의 핵심 로직을 담고 있는 주요 파일들입니다.

* **C++ Classes**
    * `Source/Team02/Character/TPlayerCharacter.h`: 플레이어 캐릭터의 기반 로직
    * `Source/Team02/Character/Input/TInputConfig 및 TPlayerCharacter`: 플레이어 입력 처리
    * `Source/Team02/Item/Weapon/WeaponBase.h`: 무기의 기본 클래스
    * `Source/Team02/Character/TNonPlayerCharacter.h`: 원거리 AI 캐릭터의 기반 로직
    * `Source/Team02/Character/TNonPlayerCharacterSword.h`: 근거리 AI 캐릭터의 기반 로직
    * `Source/Team02/UI/InGame/TUIManager.h`: InGame 및 OutGame 기반 로직
* **Blueprints**
    * `Content/Team02/Character/PlayerCharacter/BP_PlayerCharacter`: C++ 캐릭터 클래스를 기반으로 한 메인 플레이어 블루프린트
    * `Content/Team02/Monster/GunNPC/Blueprint/BP_GunNPC`: 원거리 AI 캐릭터 블루프린트
    * `Content/Team02/Monster/SwordNPC/Blueprint/BP_SwordNPC`: 근거리 AI 캐릭터 블루프린트
    * `Content/Team02/Weapon/BP_TPistol, BP_TRifle, BP_TShotgun`: 무기 블루프린트
    * `Content/Team02/UI/InGame/WBP_PlayerUI`: 플레이어 캐릭터의 UI
    * `Content/Team02/UI/OutGame/WBP_Title`: 타이틀 레벨의 UI

<br>

## 🔌 필수 플러그인 (Required Plugins)

이 프로젝트는 다음 플러그인을 사용하고 있습니다. 에디터의 `플러그인` 메뉴에서 활성화해야 합니다.

* [플러그인 이름 1] (예: `Enhanced Input`)
* [플러그인 이름 2] (예: `Gameplay Abilities`)
* [마켓플레이스에서 받은 에셋 플러그인 등]

<br>

## 📄 플레이영상

(https://youtu.be/hDtpiY93B_4)
