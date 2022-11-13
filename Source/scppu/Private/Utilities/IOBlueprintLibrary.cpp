


#include "Utilities/IOBlueprintLibrary.h"

bool UIOBlueprintLibrary::LoadFileToArray(FString Filename, TArray<uint8>& Result)
{
	return FFileHelper::LoadFileToArray(Result, *Filename);
}

bool UIOBlueprintLibrary::LoadFileToString(FString Filename, FString& Result)
{
	return FFileHelper::LoadFileToString(Result, *Filename);
}

bool UIOBlueprintLibrary::SaveArrayToFile(FString Filename, bool bUseAppendMode, TArray<uint8> Array)
{
	return FFileHelper::SaveArrayToFile(Array, *Filename, &IFileManager::Get(), (bUseAppendMode ? FILEWRITE_Append : FILEWRITE_None));
}

bool UIOBlueprintLibrary::SaveStringToFile(FString Filename, bool bUseAppendMode, FString String)
{
	return FFileHelper::SaveStringToFile(String, *Filename, FFileHelper::EEncodingOptions::AutoDetect, &IFileManager::Get(), (bUseAppendMode ? FILEWRITE_Append : FILEWRITE_None));
}

void UIOBlueprintLibrary::FindFiles(FString Directory, bool bFindFiles, bool bFindDirectories, TArray<FString>& Result)
{ 
	IFileManager::Get().FindFiles(Result, *(Directory + "/*"), bFindFiles, bFindDirectories);
}
