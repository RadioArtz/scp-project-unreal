

#include "Utilities/HttpBlueprintLibrary.h"
#include "Runtime/Online/HTTP/Public/Http.h"
#include "Serialization/JsonSerializerMacros.h" 

void UHttpBlueprintLibrary::MakeHttpGetRequest(FString Url, TArray<FHttpOption> Headers, TArray<FHttpOption> Options, FHttpReqeustDone OnDone)
{
	MakeHttpRequest_Internal("GET", Url, Headers, Options, "", OnDone);
}

void UHttpBlueprintLibrary::MakeHttpHeadRequest(FString Url, TArray<FHttpOption> Headers, TArray<FHttpOption> Options, FHttpReqeustDone OnDone)
{
	MakeHttpRequest_Internal("HEAD", Url, Headers, Options, "", OnDone);
}

void UHttpBlueprintLibrary::MakeHttpPostRequest(FString Url, TArray<FHttpOption> Headers, TArray<FHttpOption> Options, FString Content, FHttpReqeustDone OnDone)
{
	MakeHttpRequest_Internal("POST", Url, Headers, Options, Content, OnDone);
}

void UHttpBlueprintLibrary::MakeHttpRequest_Internal(FString Verb, FString Url, TArray<FHttpOption> Headers, TArray<FHttpOption> Options, FString Content, FHttpReqeustDone OnDone)
{
	FHttpRequestPtr Request = FHttpModule::Get().CreateRequest();
	Request->SetVerb(Verb);

	for (auto Elem : Headers)
	{
		Request->SetHeader(Elem.Key, Elem.Value);
	}

	FString FinalUrl = Url + "?";
	for (auto Elem : Options)
	{
		FinalUrl += Elem.ToUrlString() + "&";
	}

	// Remove last "&" if there were any options or remove "?" if there were none
	FinalUrl.RemoveAt(FinalUrl.Len() - 1);
	Request->SetURL(FinalUrl);

	Request->SetContentAsString(Content);
	Request->OnProcessRequestComplete().BindLambda([OnDone](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful) 
	{
		if (!bWasSuccessful)
		{
			OnDone.ExecuteIfBound(-1, TArray<FHttpOption>(), "");
			return;
		}

		TArray<FHttpOption> Headers;
		FJsonSerializableArray HeadersRaw = Request->GetAllHeaders();
		for (auto Elem : HeadersRaw)
		{
			FJsonSerializableArray Parts;
			Elem.ParseIntoArray(Parts, TEXT(":"), false);
			check(Parts.Num() == 2);

			FHttpOption Header = FHttpOption(Parts[0], Parts[1]);
			Headers.Add(Header);
		}

		OnDone.ExecuteIfBound(Response->GetResponseCode(), Headers, Response->GetContentAsString());
	});

	Request->ProcessRequest();
};
