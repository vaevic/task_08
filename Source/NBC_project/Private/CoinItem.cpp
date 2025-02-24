#include "CoinItem.h"
#include "Engine/World.h"
#include "SpartaGameState.h"
#include "NewWaveGameState.h"

ACoinItem::ACoinItem()
{
	PointValue = 0;
	ItemType = "DefaultCoin";
}

void ACoinItem::ActivateItem(AActor* Activator)
{
	Super::ActivateItem(Activator);

	if (Activator && Activator->ActorHasTag("Player"))
	{
		if (UWorld* World = GetWorld())
		{
			if (ASpartaGameState* GameState = World->GetGameState<ASpartaGameState>())
			{
				GameState->AddScore(PointValue);
				GameState->OnCoinCollected();
			}
			else if (ANewWaveGameState* WaveGameState = World->GetGameState<ANewWaveGameState>())
			{
				WaveGameState->AddScore(PointValue);
				WaveGameState->OnCoinCollected();
			}
		}
		DestroyItem();
	}
}