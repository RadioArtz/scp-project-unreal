

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "HttpBlueprintLibrary.generated.h"

USTRUCT(BlueprintType)
struct FHttpOption
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite)
		FString Key;

	UPROPERTY(BlueprintReadWrite)
		FString Value;

public:
	FHttpOption()
	{
		this->Key = "";
		this->Value = "";
	}

	FHttpOption(FString NewKey, FString NewValue)
	{
		this->Key = NewKey.TrimStartAndEnd();
		this->Value = NewValue.TrimStartAndEnd();
	}

	FString ToUrlString()
	{
		return this->Key + "=" + this->Value;
	}
};

UDELEGATE(BlueprintCallable, meta = (AutoCreateRefTerm = "Headers, Content"))
DECLARE_DYNAMIC_DELEGATE_ThreeParams(FHttpReqeustDone, int32, StatusCode, const TArray<FHttpOption>&, Headers, const FString&, Content);

UCLASS()
class SCPPU_API UHttpBlueprintLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Make HTTP GET Request", AutoCreateRefTerm = "Headers, Options, OnDone"))
		static void MakeHttpGetRequest(FString Url, TArray<FHttpOption> Headers, TArray<FHttpOption> Options, FHttpReqeustDone OnDone);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Make HTTP HEAD Request", AutoCreateRefTerm = "Headers, Options, OnDone"))
		static void MakeHttpHeadRequest(FString Url, TArray<FHttpOption> Headers, TArray<FHttpOption> Options, FHttpReqeustDone OnDone);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Make HTTP POST Request", AutoCreateRefTerm = "Headers, Options, OnDone"))
		static void MakeHttpPostRequest(FString Url, TArray<FHttpOption> Headers, TArray<FHttpOption> Options, FString Content, FHttpReqeustDone OnDone);


private:
	static void MakeHttpRequest_Internal(FString Verb, FString Url, TArray<FHttpOption> Headers, TArray<FHttpOption> Options, FString Content, FHttpReqeustDone OnDone);
};
