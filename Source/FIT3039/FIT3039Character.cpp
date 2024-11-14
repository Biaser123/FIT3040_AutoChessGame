// Copyright Epic Games, Inc. All Rights Reserved.

#include "FIT3039Character.h"

#include "CanvasItem.h"
#include "FIT3039Projectile.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/PanelWidget.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "AutoChess/Chess.h"
#include "Engine/LocalPlayer.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

AFIT3039Character::AFIT3039Character()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);

	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->SetRelativeLocation(FVector(-10.f, 0.f, 60.f)); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = false;

	// Set initial active camera
	CurrentCamera = FirstPersonCameraComponent;

	// Create a mesh component for '1st person' view
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharaZcterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->SetupAttachment(FirstPersonCameraComponent);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	Mesh1P->SetRelativeLocation(FVector(-30.f, 0.f, -150.f));
	

}

void AFIT3039Character::BeginPlay()
{
	Super::BeginPlay();
}

void AFIT3039Character::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 在第三人称视角时进行射线检测
	if (bIsThirdPersonActive)
	{
		PerformRayTrace();
	}
}

void AFIT3039Character::SwitchToCustomCamera()
{
	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		if (CustomCameraActor && CustomCameraActor->GetCameraComponent())
		{
			PC->SetViewTargetWithBlend(CustomCameraActor); // 切换到自定义相机视角
			bIsThirdPersonActive = true;
			OnThirdPersonViewActivated.Broadcast();
		}
	}
}

void AFIT3039Character::SwitchToPlayerCamera()
{
	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		PC->SetViewTargetWithBlend(this); // 切换回玩家相机视角
		bIsThirdPersonActive = false; // 关闭射线检测
		OnFirstPersonViewActivated.Broadcast();

		// 确保 ScreenWidget 有效且在视口中，然后移除并清除引用
		if (ScreenWidget && ScreenWidget->IsInViewport())
		{
			ScreenWidget->RemoveFromParent();  // 从视口移除
			ScreenWidget = nullptr;            // 清空引用
		}

		// 确保 DetectedWidget 有效，并检查是否需要清除它
		if (DetectedWidget && DetectedWidget->IsValidLowLevelFast())
		{
			DetectedWidget = nullptr;
		}
	}
}

void AFIT3039Character::PerformRayTrace()
{
	if (!CustomCameraActor || !CustomCameraActor->GetCameraComponent())
	{
		UE_LOG(LogTemplateCharacter, Warning, TEXT("CustomCameraActor or its CameraComponent is invalid."));
		return;
	}

	APlayerController* PC = Cast<APlayerController>(GetController());
	if (!PC) 
	{
		UE_LOG(LogTemplateCharacter, Warning, TEXT("PlayerController is invalid."));
		return;
	}

	FVector MouseWorldLocation, MouseWorldDirection;

	// 检测鼠标位置并进行射线检测
	if (PC->DeprojectMousePositionToWorld(MouseWorldLocation, MouseWorldDirection))
	{
		FVector Start = MouseWorldLocation;
		FVector End = Start + (MouseWorldDirection * 5000.f);

		FHitResult HitResult;
		bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility);

		if (bHit && HitResult.GetComponent())
		{
			UBoxComponent* HitBoxComponent = Cast<UBoxComponent>(HitResult.GetComponent());
            
			if (HitBoxComponent)
			{
				AChessboard* Chessboard = Cast<AChessboard>(HitBoxComponent->GetOwner());
				if (Chessboard)
				{
					UGridNode* HoveredNode = Chessboard->GetNodeFromCollisionComponent(HitBoxComponent);

					if (HoveredNode && HoveredNode->GetOccupyingChess())
					{
						AChess* OccupyingChess = HoveredNode->GetOccupyingChess();
						if (OccupyingChess)
						{
							UWidgetComponent* WidgetComponent = OccupyingChess->FindComponentByClass<UWidgetComponent>();
							
							// 检查是否需要更新显示的 Widget
							if (WidgetComponent && WidgetComponent != DetectedWidget)
							{
								ShowWidgetOnScreen(WidgetComponent);  // 显示新的 Widget
								return;
							}
							else if (WidgetComponent == DetectedWidget)
							{
								// 当前 Widget 已显示，无需更新
								return;
							}
						}
					}
				}
			}
		}
	}

	// 如果未命中或已移开，隐藏 Widget
	HideWidget();
}


void AFIT3039Character::ShowWidgetOnScreen(UWidgetComponent* ChessWidgetComponent)
{
    if (!ChessWidgetComponent)
    {
        UE_LOG(LogTemplateCharacter, Error, TEXT("ChessWidgetComponent is invalid."));
        return;
    }

    UUserWidget* ChessWidgetInstance = ChessWidgetComponent->GetUserWidgetObject();
    if (!ChessWidgetInstance)
    {
        UE_LOG(LogTemplateCharacter, Warning, TEXT("ChessWidgetInstance is invalid or not set in the Chess widget component."));
        return;
    }

    // 克隆 ChessWidgetInstance 来避免直接引用原始 widget
    UUserWidget* ClonedWidget = DuplicateObject<UUserWidget>(ChessWidgetInstance, this);
    if (!ClonedWidget)
    {
        UE_LOG(LogTemplateCharacter, Error, TEXT("Failed to duplicate ChessWidgetInstance."));
        return;
    }

    // 检查并创建 ScreenWidget 实例
    if (!ScreenWidget)
    {
        ScreenWidget = CreateWidget<UUserWidget>(GetWorld(), ScreenWidgetClass);
        if (!ScreenWidget)
        {
            UE_LOG(LogTemplateCharacter, Error, TEXT("Failed to create ScreenWidget!"));
            return;
        }
        ScreenWidget->AddToViewport();
    }

    // 确保 ScreenWidget 可见
    ScreenWidget->SetVisibility(ESlateVisibility::Visible);

    // 获取 PlayerController
    APlayerController* PC = Cast<APlayerController>(GetController());
    if (PC)
    {
        // 将 ClonedWidget 作为 ScreenWidget 的子组件添加
        UPanelWidget* ScreenPanel = Cast<UPanelWidget>(ScreenWidget->GetRootWidget());
        if (ScreenPanel)
        {
            ScreenPanel->ClearChildren();  // 清空之前的内容
            ScreenPanel->AddChild(ClonedWidget);  // 添加 ClonedWidget 作为子组件
            UE_LOG(LogTemplateCharacter, Log, TEXT("Added ClonedWidget to ScreenWidget."));
        }
        
        // 获取棋子的世界位置并将其转换为屏幕空间
        FVector ChessWorldLocation = ChessWidgetComponent->GetComponentLocation();
        FVector2D ChessScreenPosition;
        if (PC->ProjectWorldLocationToScreen(ChessWorldLocation, ChessScreenPosition))
        {
            FVector2D WidgetSize = ScreenWidget->GetDesiredSize();
            FVector2D AlignedPosition = ChessScreenPosition - (WidgetSize * 0.5f);  // 居中对齐

            // 获取屏幕的宽度和高度
            FVector2D ViewportSize;
            if (GEngine && GEngine->GameViewport)
            {
                GEngine->GameViewport->GetViewportSize(ViewportSize);
            }

            // 调整位置以确保 Widget 不超出屏幕边界
            AlignedPosition.X = FMath::Clamp(AlignedPosition.X, 0.f, ViewportSize.X - WidgetSize.X);
            AlignedPosition.Y = FMath::Clamp(AlignedPosition.Y, 0.f, ViewportSize.Y - WidgetSize.Y);

            // 设置 ScreenWidget 的位置
            ScreenWidget->SetPositionInViewport(AlignedPosition);
            ScreenWidget->SetVisibility(ESlateVisibility::HitTestInvisible);

            UE_LOG(LogTemplateCharacter, Log, TEXT("Widget shown centered at chess piece position: %s"), *AlignedPosition.ToString());
        }

        DetectedWidget = ChessWidgetComponent; // 更新当前检测到的 Widget
    }
}

void AFIT3039Character::HideWidget()
{
    if (ScreenWidget && ScreenWidget->IsVisible())
    {
        ScreenWidget->SetVisibility(ESlateVisibility::Hidden);  // 隐藏 ScreenWidget
        ScreenWidget->RemoveFromParent();  // 从视口中移除以完全清除引用
        ScreenWidget = nullptr;  // 清空 ScreenWidget 的引用
    }

    // 清除 DetectedWidget 引用
    DetectedWidget = nullptr;
}

