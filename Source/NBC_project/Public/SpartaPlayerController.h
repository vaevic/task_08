#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "SpartaPlayerController.generated.h"

class UInputMappingContext;
class UInputAction;


UCLASS()
class NBC_PROJECT_API ASpartaPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ASpartaPlayerController();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputMappingContext* InputMappingContext;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* MoveAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* JumpAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* LookAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* SprintAction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HUD")
	TSubclassOf<UUserWidget> WaveUIWidgetClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HUD")
	UUserWidget* WaveUIWidgetInstance;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Menu")
	TSubclassOf<UUserWidget> NewMenuWidgetClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Menu")
	UUserWidget* NewMenuWidgetInstance;


	UFUNCTION(BlueprintPure, Category = "HUD")
	UUserWidget* GetHUDWidget() const;

	UFUNCTION(BlueprintCallable, Category = "Menu")
	void StartGame();
	UFUNCTION(BlueprintCallable, Category = "HUD")
	void ShowWaveUI();
	UFUNCTION(BlueprintCallable, Category = "Menu")
	void ShowNewMenu(bool bIsRestart);

	UFUNCTION(BlueprintCallable, Category = "Menu")
	void EndGame();
	UFUNCTION(BlueprintCallable, Category = "Menu")
	void OpenMainMenu();

protected:
	virtual void BeginPlay() override;

};