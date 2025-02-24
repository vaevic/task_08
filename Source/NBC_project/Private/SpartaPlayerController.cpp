#include "SpartaPlayerController.h"
#include "SpartaGameState.h"
#include "SpartaGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "EnhancedInputSubsystems.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "NewWaveGameState.h"
#include "Kismet/KismetSystemLibrary.h"

ASpartaPlayerController::ASpartaPlayerController()
	: InputMappingContext(nullptr),
	MoveAction(nullptr),
	JumpAction(nullptr),
	LookAction(nullptr),
	SprintAction(nullptr),
	WaveUIWidgetClass(nullptr),
	WaveUIWidgetInstance(nullptr),
	NewMenuWidgetClass(nullptr),
	NewMenuWidgetInstance(nullptr)
{

}

void ASpartaPlayerController::BeginPlay() //IMC활성화
{
	Super::BeginPlay();
	if (ULocalPlayer* LocalPlayer = GetLocalPlayer()) //플레이의 입력이나 화면을 관리하는 객체, 사용자
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
			//입력시스템을 관리함, IMC를 호출
		{
			if (InputMappingContext)
			{
				Subsystem->AddMappingContext(InputMappingContext, 0); //0= 가장 높은 우선순위
			}
		}
	}

	OpenMainMenu();
}

UUserWidget* ASpartaPlayerController::GetHUDWidget() const
{
	return WaveUIWidgetInstance;
}

void ASpartaPlayerController::ShowNewMenu(bool bIsRestart)
{
	if (WaveUIWidgetInstance)
	{
		WaveUIWidgetInstance->RemoveFromParent();
		WaveUIWidgetInstance = nullptr;
	}

	if (NewMenuWidgetInstance)
	{
		NewMenuWidgetInstance->RemoveFromParent();
		NewMenuWidgetInstance = nullptr;
	}

	if (NewMenuWidgetClass)
	{
		NewMenuWidgetInstance = CreateWidget<UUserWidget>(this, NewMenuWidgetClass);
		if (NewMenuWidgetInstance)
		{
			NewMenuWidgetInstance->AddToViewport();

			bShowMouseCursor = true;
			SetInputMode(FInputModeUIOnly());
		}

		if (UTextBlock* ButtonText = Cast<UTextBlock>(NewMenuWidgetInstance->GetWidgetFromName(TEXT("StartWave"))))
		{
			if (bIsRestart)
			{				
				ButtonText->SetText(FText::FromString(TEXT("Restart")));
			}
			else
			{
				ButtonText->SetText(FText::FromString(TEXT("Start")));
			}
		}

		if (UTextBlock* ButtonText = Cast<UTextBlock>(NewMenuWidgetInstance->GetWidgetFromName(TEXT("EndWave"))))
		{
			if (UButton* EndButton = Cast<UButton>(NewMenuWidgetInstance->GetWidgetFromName(TEXT("EndButton"))))
			{
				if (bIsRestart)
				{
					ButtonText->SetText(FText::FromString(TEXT("MainMenu")));
					EndButton->OnClicked.AddDynamic(this, &ASpartaPlayerController::OpenMainMenu);
				}
				else
				{
					ButtonText->SetText(FText::FromString(TEXT("End Game")));
					EndButton->OnClicked.AddDynamic(this, &ASpartaPlayerController::EndGame);
				}
			}
			
		}

		if (bIsRestart)
		{
			UFunction* PlayAnimFunc = NewMenuWidgetInstance->FindFunction(FName("PlayGameOverAnim"));
			if (PlayAnimFunc)
			{
				NewMenuWidgetInstance->ProcessEvent(PlayAnimFunc, nullptr);
			}

			if (UTextBlock* TotalScoreText = Cast<UTextBlock>(NewMenuWidgetInstance->GetWidgetFromName(TEXT("TotalScore"))))
			{
				if (USpartaGameInstance* SpartaGameInstance = Cast<USpartaGameInstance>(UGameplayStatics::GetGameInstance(this)))
				{
					TotalScoreText->SetText(FText::FromString(FString::Printf(TEXT("Total Score: %d"), SpartaGameInstance->TotalScore)));
				}
			}
		}
	}
}

void ASpartaPlayerController::ShowWaveUI()
{
	if (WaveUIWidgetInstance)
	{
		WaveUIWidgetInstance->RemoveFromParent();
		WaveUIWidgetInstance = nullptr;
	}

	if (NewMenuWidgetInstance)
	{
		NewMenuWidgetInstance->RemoveFromParent();
		NewMenuWidgetInstance = nullptr;
	}

	if (WaveUIWidgetClass)
	{
		WaveUIWidgetInstance = CreateWidget<UUserWidget>(this, WaveUIWidgetClass);
		if (WaveUIWidgetInstance)
		{
			WaveUIWidgetInstance->AddToViewport();

			bShowMouseCursor = false;
			SetInputMode(FInputModeGameOnly());
		}

		ANewWaveGameState* NewWaveGameState = GetWorld() ? GetWorld()->GetGameState<ANewWaveGameState>() : nullptr;
		if (NewWaveGameState)
		{
			NewWaveGameState->UpdateWaveUI();
		}
	}
}

void ASpartaPlayerController::StartGame()
{
	if (USpartaGameInstance* SpartaGameInstance = Cast<USpartaGameInstance>(UGameplayStatics::GetGameInstance(this)))
	{
		SpartaGameInstance->CurrentLevelIndex = 0;
		SpartaGameInstance->TotalScore = 0;
	}

	UGameplayStatics::OpenLevel(GetWorld(), FName("BasicLevel"));
	SetPause(false);
}

void ASpartaPlayerController::EndGame()
{
	if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
	{
		if (ASpartaPlayerController* SpartaPlayerController = Cast<ASpartaPlayerController>(PlayerController))
		{
			UKismetSystemLibrary::QuitGame(GetWorld(), SpartaPlayerController, EQuitPreference::Quit, false);
		}
	}
}

void ASpartaPlayerController::OpenMainMenu()
{
	FString CurrentMapName = GetWorld()->GetMapName();
	if (CurrentMapName.Contains("MenuLevel"))
	{
		ShowNewMenu(false);
	}
}