#include "NewWaveGameState.h"
#include "SpartaGameInstance.h"
#include "SpartaPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "SpawnVolume.h"
#include "CoinItem.h"
#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"
#include "Blueprint/UserWidget.h"

ANewWaveGameState::ANewWaveGameState()
{
	Score = 0;
	SpawnedCoinCount = 0;
	CollectedCoinCount = 0;
	WaveDuration = 30.0;
	MaxWave = 5;
	CurrentWave = 1;
	PlusSpawnNum = 10;
	DeclineWaveDuratoinRate = 0.8f;
}

void ANewWaveGameState::BeginPlay()
{
	Super::BeginPlay();

	UpdateWaveUI();

	StartWave();

	GetWorldTimerManager().SetTimer(
		WaveUIUpdateTimerHandel,
		this,
		&ANewWaveGameState::UpdateWaveUI,
		0.1f,
		true);
}

int32 ANewWaveGameState::GetScore() const
{
	return Score;
}

void ANewWaveGameState::AddScore(int32 Amount)
{
	if (UGameInstance* GameInstance = GetGameInstance())
	{
		USpartaGameInstance* SpartaGameInstance = Cast<USpartaGameInstance>(GameInstance);
		if (SpartaGameInstance)
		{
			SpartaGameInstance->AddToScore(Amount);
		}
	}
}

void ANewWaveGameState::StartWave()
{
	if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
	{
		if (ASpartaPlayerController* SpartaPlayerController = Cast<ASpartaPlayerController>(PlayerController))
		{
			SpartaPlayerController->ShowWaveUI();
		}
	}

	SpawnedCoinCount = 0;
	CollectedCoinCount = 0;
	CurrentWave = 1;

	TArray<AActor*> FoundVolumes;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASpawnVolume::StaticClass(), FoundVolumes);

	ItemToSpawn = 20;

	for (int32 i = 0; i < ItemToSpawn; i++)
	{
		if (FoundVolumes.Num() > 0)
		{
			ASpawnVolume* SpawnVolume = Cast<ASpawnVolume>(FoundVolumes[0]);
			if (SpawnVolume)
			{
				AActor* SpawnedActor = SpawnVolume->SpawnRandomItem();
				if (SpawnedActor && SpawnedActor->IsA(ACoinItem::StaticClass()))
				{
					SpawnedCoinCount++;
				}
			}
		}
	}

	UpdateWaveUI();

	GetWorldTimerManager().SetTimer(
		WaveTimerHandle,
		this,
		&ANewWaveGameState::OnWaveTimeUp,
		WaveDuration,
		false);

	UE_LOG(LogTemp, Warning, TEXT("Wave %d Start!, Spawned %d coin"), CurrentWave, SpawnedCoinCount);

}

void ANewWaveGameState::OnWaveTimeUp()
{
	EndWave();
}

void ANewWaveGameState::OnCoinCollected()
{
	CollectedCoinCount++;
	UE_LOG(LogTemp, Warning, TEXT("Coin Collected: %d / %d"),CollectedCoinCount,SpawnedCoinCount)
	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green, FString::Printf(TEXT("Coin Collected: %d / %d"), CollectedCoinCount, SpawnedCoinCount));

		if (SpawnedCoinCount > 0 && CollectedCoinCount >= SpawnedCoinCount)
		{
			EndWave();
		}
}

void ANewWaveGameState::EndWave()
{
	GetWorldTimerManager().ClearTimer(WaveTimerHandle);
	
	TArray<AActor*> SpawnedActor;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABaseItem::StaticClass(), SpawnedActor);
	for (AActor* ActorFound : SpawnedActor)
	{
		ActorFound->Destroy();
	}

	if (UGameInstance* GameInstance = GetGameInstance())
	{
		USpartaGameInstance* SpartaGameInstance = Cast<USpartaGameInstance>(GameInstance);
		if (SpartaGameInstance)
		{
			AddScore(Score);
		}
	}

	if (CurrentWave == MaxWave)
	{
		OnGameOver();
		return;
	}
	
	else
	{
		NextWaveStart();
	}

}

void ANewWaveGameState::NextWaveStart()
{
	Score = 0;
	SpawnedCoinCount = 0;
	CollectedCoinCount = 0;
	CurrentWave++;
	ItemToSpawn += PlusSpawnNum;
	WaveDuration *= DeclineWaveDuratoinRate;


	TArray<AActor*> FoundVolumes;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASpawnVolume::StaticClass(), FoundVolumes);

	for (int32 i = 0; i < ItemToSpawn; i++)
	{
		if (FoundVolumes.Num() > 0)
		{
			ASpawnVolume* SpawnVolume = Cast<ASpawnVolume>(FoundVolumes[0]);
			if (SpawnVolume)
			{
				AActor* SpawnedActor = SpawnVolume->SpawnRandomItem();
				if (SpawnedActor && SpawnedActor->IsA(ACoinItem::StaticClass()))
				{
					SpawnedCoinCount++;
				}
			}
		}
	}
	UE_LOG(LogTemp, Warning, TEXT("Wave %d Start!, Spawned %d coin"), CurrentWave, SpawnedCoinCount);


	GetWorldTimerManager().SetTimer(
		WaveTimerHandle,
		this,
		&ANewWaveGameState::OnWaveTimeUp,
		WaveDuration,
		false);
}

void ANewWaveGameState::OnGameOver()
{
	UpdateWaveUI();

	if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
	{
		if (ASpartaPlayerController* SpartaPlayerController = Cast<ASpartaPlayerController>(PlayerController))
		{
			SpartaPlayerController->SetPause(true);
			SpartaPlayerController->ShowNewMenu(true);
		}
	}
}

void ANewWaveGameState::UpdateWaveUI()
{
	if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
	{
		if (ASpartaPlayerController* SpartaPlayerController = Cast<ASpartaPlayerController>(PlayerController))
		{
			if (UUserWidget* WaveUIWidget = SpartaPlayerController->GetHUDWidget())
			{
				if (UTextBlock* TimeText = Cast<UTextBlock>(WaveUIWidget->GetWidgetFromName(TEXT("TimeText"))))
				{
					float RemainingTime = GetWorldTimerManager().GetTimerRemaining(WaveTimerHandle);
					TimeText->SetText(FText::FromString(FString::Printf(TEXT("Time: %.1f"), RemainingTime)));
				}

				if (UProgressBar* TimerBar = Cast<UProgressBar>(WaveUIWidget->GetWidgetFromName(TEXT("TimerBar"))))
				{
					float RemainingTime = GetWorldTimerManager().GetTimerRemaining(WaveTimerHandle);
					float TimerPercent = RemainingTime /= WaveDuration;
					TimerBar->SetPercent(TimerPercent);
				}

				if (UTextBlock* TotalScoreText = Cast<UTextBlock>(WaveUIWidget->GetWidgetFromName(TEXT("TotalScoreText"))))
				{
					if (UGameInstance* GameInstance = GetGameInstance())
					{
						USpartaGameInstance* SpartaGameInstance = Cast<USpartaGameInstance>(GameInstance);
						if (SpartaGameInstance)
						{
							TotalScoreText->SetText(FText::FromString(FString::Printf(TEXT("Total : %d"), SpartaGameInstance->TotalScore)));
						}
					}
				}
					
				
				if (UTextBlock* WaveText = Cast<UTextBlock>(WaveUIWidget->GetWidgetFromName(TEXT("WaveText"))))
				{
					WaveText->SetText(FText::FromString(FString::Printf(TEXT("Wave: %d"), CurrentWave)));
				}

				if (UTextBlock* CoinText = Cast<UTextBlock>(WaveUIWidget->GetWidgetFromName(TEXT("CoinText"))))
				{
					CoinText->SetText(FText::FromString(FString::Printf(TEXT("Coin %d / %d"), CollectedCoinCount, SpawnedCoinCount)));
				}
			}
		}
	}
}