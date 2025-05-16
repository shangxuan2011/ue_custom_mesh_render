// Copyright Epic Games, Inc. All Rights Reserved.

#include "CustomShader.h"
#include "Interfaces/IPluginManager.h"

#define LOCTEXT_NAMESPACE "FCustomShaderModule"

void FCustomShaderModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	 // 将插件目录下的Shader目录添加到Shader库，并映射为"/Plugins/CustomShader"前缀
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