


#include "Utilities/ReflectionCaptureRotationSubsystem.h"
#include "Engine/Level.h"
#include "Engine/MapBuildDataRegistry.h"
#include "Engine/LevelStreaming.h"
#include "Engine/TextureRenderTargetCube.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Components/ReflectionCaptureComponent.h"
#include "Engine/Canvas.h" 
#include "Blueprint/AsyncTaskDownloadImage.h"

DEFINE_LOG_CATEGORY(LogReflectionCaptureSubsystem);

bool UReflectionCaptureRotationSubsystem::DoesSupportWorldType(EWorldType::Type WorldType) const
{
	TSet<EWorldType::Type> SupportedWorlds;
	SupportedWorlds.Append({ EWorldType::Game, EWorldType::PIE, EWorldType::GamePreview });
	return SupportedWorlds.Contains(WorldType);
}

void UReflectionCaptureRotationSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	FWorldDelegates::LevelAddedToWorld.AddUObject(this, &UReflectionCaptureRotationSubsystem::OnLevelAddedToWorldCallback);
	FWorldDelegates::LevelRemovedFromWorld.AddUObject(this, &UReflectionCaptureRotationSubsystem::OnLevelRemovedFromWorldCallback);
	
	UMaterialInterface* RotatorMaterialBase = GetDefault<UReflectionSubsystemSettings>()->RotatorMaterialAsset.LoadSynchronous();
	if (IsValid(RotatorMaterialBase))
	{
		this->RotatorMaterial = UMaterialInstanceDynamic::Create(RotatorMaterialBase, this);
	}
	else
	{
		UE_LOG(LogReflectionCaptureSubsystem, Error, TEXT("%s: Supplied rotator material asset is invalid!"), *this->GetName());
	}

	UE_LOG(LogReflectionCaptureSubsystem, Log, TEXT("%s: Initialized!"), *this->GetName());
}

void UReflectionCaptureRotationSubsystem::Tick(float DeltaTime)
{

}

void UReflectionCaptureRotationSubsystem::Deinitialize()
{
	UE_LOG(LogReflectionCaptureSubsystem, Log, TEXT("%s: Deinitialized!"), *this->GetName());
}

void UReflectionCaptureRotationSubsystem::OnLevelAddedToWorldCallback(ULevel* Level, UWorld* World)
{
	FTransform LevelTransform = this->FindLevelTransform(Level);

	if (!IsValid(Level->MapBuildData))
		return;

	for (AActor* Actor : Level->Actors)
	{
		if (!IsValid(Actor))
		{
			continue;
		}

		for (UActorComponent* Component : Actor->GetComponentsByClass(UReflectionCaptureComponent::StaticClass()))
		{
			UReflectionCaptureComponent* RefComponent = (UReflectionCaptureComponent*)Component;
			FReflectionCaptureMapBuildData* Data = Level->MapBuildData->GetReflectionCaptureBuildData(RefComponent->MapBuildDataId);
			TArray<uint8> Backup = Data->FullHDRCapturedData;
			//UTextureRenderTargetCube* RenderTarget = NewObject<UTextureRenderTargetCube>(this, FName(), RF_Transient);
			UTextureRenderTarget2D* RenderTarget = NewObject<UTextureRenderTarget2D>(this, FName(), RF_Transient);
			//RenderTarget->Init(256, EPixelFormat::PF_FloatRGBA);
			RenderTarget->InitCustomFormat(256, 256 * 6, EPixelFormat::PF_FloatRGBA, false);
			RenderTarget->ClearColor = FLinearColor::Green;
			RenderTarget->CompressionSettings = TC_HDR;
			RenderTarget->CompressionQuality = TCQ_Highest;
			RenderTarget->SRGB = false;
			RenderTarget->Filter = TF_Trilinear;
			RenderTarget->UpdateResourceImmediate(true);

			//UTextureCube* InTexture = NewObject<UTextureCube>(this);
			//InTexture->Source.Init(256, 256, 6, 1, TSF_RGBA16F);
			//InTexture->CompressionSettings = TC_HDR;
			//InTexture->CompressionQuality = TCQ_Highest;
			//InTexture->SRGB = false;
			//InTexture->Filter = TF_Trilinear;
			//InTexture->PostEditChange();


			UTexture2D* InTexture = NewObject<UTexture2D>(this);
			InTexture->Source.Init(256, 256 * 6, 1, 1, TSF_RGBA16F);
			InTexture->CompressionSettings = TC_HDR;
			InTexture->CompressionQuality = TCQ_Highest;
			InTexture->SRGB = false;
			InTexture->Filter = TF_Trilinear;
			InTexture->PostEditChange();

			this->AllCubemaps.Add(InTexture);

			ENQUEUE_RENDER_COMMAND(WriteRawDataToTextureCube)(
				[InTexture, Data](FRHICommandListImmediate& RHICmdList)
				{
					//FRHITextureCube* TextureRHI = InTexture->Resource->TextureRHI->GetTextureCube();
					//int32 SizeW = TextureRHI->GetSize();
					//int32 SizeH = TextureRHI->GetSize();

					FRHITexture2D * TextureRHI = InTexture->Resource->TextureRHI->GetTexture2D();
					int32 SizeW = TextureRHI->GetSizeX();
					int32 SizeH = TextureRHI->GetSizeY();



					uint8* SrcData = Data->FullHDRCapturedData.GetData();
					//uint8 SrcDataArr[256][256 * 6];
					//FMemory::Memcpy(SrcDataArr, SrcData, sizeof(SrcDataArr));



					for (int32 i = 0; i < 1; i++)
					{
						uint32 DestStride = 0;
						//uint8* DestData = reinterpret_cast<uint8*>(RHILockTextureCubeFace(TextureRHI, i, 0, 0, RLM_WriteOnly, DestStride, false));
						uint8* DestData = reinterpret_cast<uint8*>(RHILockTexture2D(TextureRHI, 0, RLM_WriteOnly, DestStride, false, false));
						//uint8 DestDataArr[256 * 6][256];
						//FMemory::Memcpy(DestDataArr, DestData, sizeof(DestDataArr));
					



						for (int32 y = 0; y < SizeW; y++)
						{
							for (int32 x = 0; x < SizeH; x++)
							{
								FFloat16Color SrcCol;
								FMemory::Memcpy(&SrcCol, SrcData, sizeof(SrcCol));
								SrcData += sizeof(SrcCol);
					
								FFloat16Color DestCol;
								DestCol.R = SrcCol.R;
								DestCol.G = SrcCol.G;
								DestCol.B = SrcCol.B;
								DestCol.A = SrcCol.A;
					
								FMemory::Memcpy(DestData, &DestCol, sizeof(DestCol));
								DestData += sizeof(DestCol);
							}
						}
					
						//RHIUnlockTextureCubeFace(TextureRHI, i, 0, 0, false);
						//FMemory::Memcpy(DestData, DestDataArr, sizeof(DestData));
						RHIUnlockTexture2D(TextureRHI, 0, false);
					}
				});

			//FlushRenderingCommands();

			World->FlushDeferredParameterCollectionInstanceUpdates();

			FTextureRenderTargetResource* RenderTargetResource = RenderTarget->GameThread_GetRenderTargetResource();

			UCanvas* Canvas = World->GetCanvasForDrawMaterialToRenderTarget();

			FCanvas RenderCanvas(
				RenderTargetResource,
				nullptr,
				World,
				World->FeatureLevel);

			Canvas->Init(RenderTarget->SizeX, RenderTarget->SizeX, nullptr, &RenderCanvas);
			Canvas->Update();

			FDrawEvent* DrawMaterialToTargetEvent = new FDrawEvent();

			FName RTName = RenderTarget->GetFName();
			ENQUEUE_RENDER_COMMAND(BeginDrawEventCommand)(
				[RTName, DrawMaterialToTargetEvent, RenderTargetResource](FRHICommandListImmediate& RHICmdList)
				{
					RenderTargetResource->FlushDeferredResourceUpdate(RHICmdList);

					BEGIN_DRAW_EVENTF(
						RHICmdList,
						DrawCanvasToTarget,
						(*DrawMaterialToTargetEvent),
						*RTName.ToString());
				});

			UMaterialInstanceDynamic* MyMaterial = UMaterialInstanceDynamic::Create(GetDefault<UReflectionSubsystemSettings>()->RotatorMaterialAsset.LoadSynchronous(), this);
			MyMaterial->SetTextureParameterValue("SourceTexture", InTexture);
			MyMaterial->SetScalarParameterValue("RotationAngle", LevelTransform.GetRotation().Euler().Z);
			Canvas->K2_DrawMaterial(MyMaterial, FVector2D(0, 0), FVector2D(RenderTarget->SizeX, RenderTarget->SizeY), FVector2D(0, 0));

			RenderCanvas.Flush_GameThread();
			Canvas->Canvas = nullptr;

			//UpdateResourceImmediate must be called here to ensure mips are generated.
			RenderTarget->UpdateResourceImmediate(false);
			ENQUEUE_RENDER_COMMAND(CanvasRenderTargetResolveCommand)(
				[DrawMaterialToTargetEvent](FRHICommandList& RHICmdList)
				{
					STOP_DRAW_EVENT((*DrawMaterialToTargetEvent));
					delete DrawMaterialToTargetEvent;
				}
			);


			ENQUEUE_RENDER_COMMAND(ReadRawDataFromTextureCube)(
				[RenderTarget, Data](FRHICommandListImmediate& RHICmdList)
				{
					//FRHITextureCube* TextureRHI = InTexture->Resource->TextureRHI->GetTextureCube();
					//int32 SizeW = TextureRHI->GetSize();
					//int32 SizeH = TextureRHI->GetSize();

					FRHITexture2D* TextureRHI = RenderTarget->Resource->TextureRHI->GetTexture2D();
					int32 SizeW = TextureRHI->GetSizeX();
					int32 SizeH = TextureRHI->GetSizeY();



					uint8* DestData = Data->FullHDRCapturedData.GetData();
					//uint8 SrcDataArr[256][256 * 6];
					//FMemory::Memcpy(SrcDataArr, SrcData, sizeof(SrcDataArr));



					for (int32 i = 0; i < 1; i++)
					{
						uint32 DestStride = 0;
						//uint8* DestData = reinterpret_cast<uint8*>(RHILockTextureCubeFace(TextureRHI, i, 0, 0, RLM_WriteOnly, DestStride, false));
						uint8* SrcData = reinterpret_cast<uint8*>(RHILockTexture2D(TextureRHI, 0, RLM_WriteOnly, DestStride, false, false));
						//uint8 DestDataArr[256 * 6][256];
						//FMemory::Memcpy(DestDataArr, DestData, sizeof(DestDataArr));




						for (int32 y = 0; y < SizeW; y++)
						{
							for (int32 x = 0; x < SizeH; x++)
							{
								FFloat16Color SrcCol;
								FMemory::Memcpy(&SrcCol, SrcData, sizeof(SrcCol));
								SrcData += sizeof(SrcCol);

								FFloat16Color DestCol;
								DestCol.R = SrcCol.R;
								DestCol.G = SrcCol.G;
								DestCol.B = SrcCol.B;
								DestCol.A = SrcCol.A;

								FMemory::Memcpy(DestData, &DestCol, sizeof(DestCol));
								DestData += sizeof(DestCol);
							}
						}

						//RHIUnlockTextureCubeFace(TextureRHI, i, 0, 0, false);
						//FMemory::Memcpy(DestData, DestDataArr, sizeof(DestData));
						RHIUnlockTexture2D(TextureRHI, 0, false);
					}
				});


			World->Scene->ReleaseReflectionCubemap(RefComponent);
			RefComponent->DestroyRenderState_Concurrent();
			RefComponent->OnUnregister();
			//UTextureCube* OutTexture = RenderTarget->ConstructTextureCube(this, FString(), RF_Transient);
			//UTexture2D* OutTexture = RenderTarget->ConstructTexture2D(this, FString(), RF_Transient);
			//OutTexture->CompressionSettings = TC_HDR;
			//OutTexture->CompressionQuality = TCQ_Highest;
			//OutTexture->SRGB = false;
			//OutTexture->Filter = TF_Trilinear;
			//OutTexture->PostEditChange();
			//this->AllCubemaps.Add(OutTexture);
			TArray<uint8> TestData = Data->FullHDRCapturedData;

			//for (int i = 0; i < Data->FullHDRCapturedData.Num(); i++)
			//{
			//	FFloat16 Black = 0.0f;
			//	if (i % (sizeof(FFloat16) * 4) == 0)
			//	{
			//		FMemory::Memcpy(&(Data->FullHDRCapturedData[i]), &Black, sizeof(FFloat16));
			//	}
			//}

			//for (int i = 0; i < Data->FullHDRCapturedData.Num(); i++)
			//{
			//	Data->FullHDRCapturedData[i] = TestData[(i + (TestData.Num() / 2)) % TestData.Num()];
			//}

			TArray<UReflectionCaptureComponent*> Comps;
			Comps.Add(RefComponent);
			RefComponent->OnRegister();
			RefComponent->CreateRenderState_Concurrent(nullptr);
			World->Scene->AllocateReflectionCaptures(Comps, TEXT("IDK"), false, false);
			GetMutableDefault<UReflectionSubsystemSettings>()->ReflectionComponentPath = RefComponent->GetPathName();
			GetMutableDefault<UReflectionSubsystemSettings>()->InTexture = InTexture;
			GetMutableDefault<UReflectionSubsystemSettings>()->InAngle = LevelTransform.GetRotation().Euler().Z;
			GetMutableDefault<UReflectionSubsystemSettings>()->RenderTarget = RenderTarget;
			//GetMutableDefault<UReflectionSubsystemSettings>()->OutTexture = OutTexture;

			Data->FullHDRCapturedData = Backup;
			UE_LOG(LogReflectionCaptureSubsystem, Log, TEXT("Done for level: %s"), *Level->GetPathName());
		}
	}
}

void UReflectionCaptureRotationSubsystem::OnLevelRemovedFromWorldCallback(ULevel* Level, UWorld* World)
{
}

FTransform UReflectionCaptureRotationSubsystem::FindLevelTransform(ULevel* Level)
{
	for (auto StreamingLevel : this->GetWorld()->GetStreamingLevels())
	{
		if (StreamingLevel->GetLoadedLevel() == Level)
		{
			return StreamingLevel->LevelTransform;
		}
	}

	return FTransform::Identity;
}

UTextureCube* UReflectionCaptureRotationSubsystem::CopyAndRotateTextureCube(UTextureCube* SourceTexture, float RotationAngle)
{
	return nullptr;
}
