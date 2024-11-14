// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CustomCamera.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "FIT3039/Public/AutoChess/Chessboard.h"
#include "Blueprint/UserWidget.h"
#include "FIT3039Character.generated.h"

class UInputComponent;
class USkeletalMeshComponent;
class UCameraComponent;
class UInputAction;
class UInputMappingContext;
struct FInputActionValue;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnThirdPersonViewActivated);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnFirstPersonViewActivated);

UCLASS(config=Game)
class AFIT3039Character : public ACharacter
{
	GENERATED_BODY()

	/** Pawn mesh: 1st person view (arms; seen only by self) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Mesh, meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* Mesh1P;

	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FirstPersonCameraComponent;

public:
	AFIT3039Character();

	// 切换视角功能
	void SwitchToCustomCamera();  // 切换到自定义相机
	void SwitchToPlayerCamera();  // 切换回玩家相机
	
	UPROPERTY(BlueprintAssignable, Category = "Camera")
	FOnThirdPersonViewActivated OnThirdPersonViewActivated;

	UPROPERTY(BlueprintAssignable, Category = "Camera")
	FOnFirstPersonViewActivated OnFirstPersonViewActivated;


protected:
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override; // 重写 Tick 以处理射线检测

public:
	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* LookAction;

private:
	UCameraComponent* CurrentCamera;

	// 用于 widget 显示的变量
	UWidgetComponent* DetectedWidget;
	bool bIsThirdPersonActive;

	// 检测和显示 widget 方法
	void PerformRayTrace();
	void ShowWidgetOnScreen(UWidgetComponent* Widget);
	void HideWidget();

public:
	/** Returns Mesh1P subobject **/
	USkeletalMeshComponent* GetMesh1P() const { return Mesh1P; }
	/** Returns FirstPersonCameraComponent subobject **/
	UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ACustomCamera* CustomCameraActor;

	// Pointer to the currently displayed UMG widget
	UPROPERTY()
	UUserWidget* ScreenWidget;

	// Widget class to spawn the UMG widget
	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<UUserWidget> ScreenWidgetClass;

	// Base material used for rendering the WidgetComponent’s Render Target as a texture
	UPROPERTY(EditAnywhere, Category = "UI")
	UMaterialInterface* CardRenderMaterialInterface;
};
