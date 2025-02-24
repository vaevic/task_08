#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "NewWaveGameState.generated.h"


UCLASS()
class NBC_PROJECT_API ANewWaveGameState : public AGameState
{
	GENERATED_BODY()
	
public:
	ANewWaveGameState();

	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Score")
	int32 Score;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Coin")
	int32 SpawnedCoinCount;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Coin")
	int32 CollectedCoinCount;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Wave")
	float WaveDuration;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Wave")
	int32 CurrentWave;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Wave")
	int32 MaxWave;

	int32 ItemToSpawn;
	int32 PlusSpawnNum;
	float DeclineWaveDuratoinRate;
	FTimerHandle WaveTimerHandle;
	FTimerHandle WaveUIUpdateTimerHandel;

	UFUNCTION(BlueprintPure, Category = "Score")
	int32 GetScore() const;
	UFUNCTION(BlueprintCallable, Category = "Score")
	void AddScore(int32 Amount);
	UFUNCTION(BlueprintCallable, Category = "Wave")
	void OnGameOver();

	void StartWave();
	void OnWaveTimeUp();
	void OnCoinCollected();
	void EndWave();
	void NextWaveStart();
	void UpdateWaveUI();
};
