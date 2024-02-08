


#include "Utilities/FileIOBlueprintLibrary.h"
#include "Misc/FileHelper.h" 

bool UFileIOBlueprintLibrary::LoadFileToArray(FString Filename, TArray<uint8>& Result)
{
	return FFileHelper::LoadFileToArray(Result, *Filename);
}

bool UFileIOBlueprintLibrary::LoadFileToString(FString Filename, FString& Result)
{
	return FFileHelper::LoadFileToString(Result, *Filename);
}

bool UFileIOBlueprintLibrary::SaveArrayToFile(FString Filename, bool bUseAppendMode, TArray<uint8> Array)
{
	return FFileHelper::SaveArrayToFile(Array, *Filename, &IFileManager::Get(), (bUseAppendMode ? FILEWRITE_Append : FILEWRITE_None));
}

bool UFileIOBlueprintLibrary::SaveStringToFile(FString Filename, bool bUseAppendMode, FString String)
{
	return FFileHelper::SaveStringToFile(String, *Filename, FFileHelper::EEncodingOptions::AutoDetect, &IFileManager::Get(), (bUseAppendMode ? FILEWRITE_Append : FILEWRITE_None));
}

void UFileIOBlueprintLibrary::FindFiles(FString Directory, bool bFindFiles, bool bFindDirectories, TArray<FString>& Result)
{ 
	IFileManager::Get().FindFiles(Result, *(Directory + "/*"), bFindFiles, bFindDirectories);
}
