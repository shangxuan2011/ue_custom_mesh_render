// Copyright Epic Games, Inc. All Rights Reserved.

#include "CustomShader.h"
#include "Interfaces/IPluginManager.h"

#define LOCTEXT_NAMESPACE "FCustomShaderModule"

void FCustomShaderModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	 // �����Ŀ¼�µ�ShaderĿ¼��ӵ�Shader�⣬��ӳ��Ϊ"/Plugins/CustomShader"ǰ׺
	FString PluginShaderDir = IPluginManager::Get().FindPlugin(TEXT("CustomShader"))->GetBaseDir();
	const FString ShaderDir = FPaths::Combine(
		PluginShaderDir,
		TEXT("Shaders")
	);
	AddShaderSourceDirectoryMapping(TEXT("/Plugins/CustomShader"), ShaderDir);
}

void FCustomShaderModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FCustomShaderModule, CustomShader)