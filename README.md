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
    * 이동 (걷기, 달리기, 점프) 및 3인칭 카메라 시스템
    * TPS 조준 (Aiming) 및 사격 (Shooting) 메커니즘
    * 재장전 및 무기 교체 시스템
* **AI 시스템 (AI System)**
    * 플레이어를 추적하고 공격하는 기본 적 AI
    * Behavior Tree와 Blackboard를 사용한 행동 로직
* **게임플레이 시스템 (Gameplay System)**
    * 체력 및 데미지 처리 시스템
    * 기본 UI (HUD): 체력, 총알 수, 크로스헤어 표시
* **[추가한 특별한 기능, 예: 인벤토리, 엄폐 시스템 등]**

<br>

## 🚀 프로젝트 설정 및 실행 (Project Setup & Launch)

이 프로젝트를 로컬 환경에서 설정하고 실행하는 방법입니다.

1.  이 프로젝트를 `git clone` 받거나 ZIP 파일로 다운로드합니다.

2.  프로젝트 폴더 안에 있는 `.uproject` 파일을 우클릭합니다.

3.  **`Generate Visual Studio project files`** 메뉴를 클릭하여 Visual Studio 솔루션(.sln) 파일을 생성합니다.
    * *만약 이 메뉴가 보이지 않는다면, Epic Games Launcher에서 해당 엔진 버전을 설치했는지 확인하세요.*

4.  생성된 `.sln` 파일을 Visual Studio 2022로 엽니다.

5.  Visual Studio 상단의 솔루션 구성을 `Development Editor`, `Win64` 로 맞춘 후, 솔루션 탐색기에서 프로젝트를 우클릭하여 **`Build`** 합니다.

6.  빌드가 성공적으로 완료되면, Visual Studio에서 `F5` 키를 눌러 디버깅과 함께 에디터를 실행하거나, 프로젝트 폴더의 `.uproject` 파일을 더블클릭하여 언리얼 에디터를 엽니다.

<br>

## 🛠 주요 클래스 및 블루프린트 (Key Classes & Blueprints)

프로젝트의 핵심 로직을 담고 있는 주요 파일들입니다.

* **C++ Classes**
    * `Source/[ProjectName]/Public/Player/TPSCharacter.h`: 플레이어 캐릭터의 핵심 로직
    * `Source/[ProjectName]/Public/Player/TPSPlayerController.h`: 플레이어 입력 처리
    * `Source/[ProjectName]/Public/Weapon/WeaponBase.h`: 무기의 기본 클래스
* **Blueprints**
    * `Content/Blueprints/Player/BP_TPSCharacter`: C++ 캐릭터 클래스를 기반으로 한 메인 플레이어 블루프rint
    * `Content/Blueprints/AI/BP_EnemyAI`: 적 AI 캐릭터 블루프린트
    * `Content/UI/WBP_HUD`: 게임 플레이 HUD 위젯 블루프린트

<br>

## 🔌 필수 플러그인 (Required Plugins)

이 프로젝트는 다음 플러그인을 사용하고 있습니다. 에디터의 `플러그인` 메뉴에서 활성화해야 합니다.

* [플러그인 이름 1] (예: `Enhanced Input`)
* [플러그인 이름 2] (예: `Gameplay Abilities`)
* [마켓플레이스에서 받은 에셋 플러그인 등]

<br>

## 📄 라이선스 (License)

이 프로젝트는 [라이선스 이름] 라이선스를 따릅니다.
