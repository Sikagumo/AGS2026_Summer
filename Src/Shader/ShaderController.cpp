#include "ShaderController.h"
#include <algorithm>

ShaderController* ShaderController::instance_ = nullptr;

void ShaderController::CreateInstance(void)
{
    if (instance_ == nullptr)
    {
        instance_ = new ShaderController();
    }
}

ShaderController& ShaderController::GetInstance(void)
{
    return *instance_;
}

void ShaderController::DestroyInstance(void)
{
    if (instance_ != nullptr)
    {
        delete instance_;
        instance_ = nullptr;
    }
}

ShaderController::ShaderController(void)
    : shaderRenderer_(nullptr)
{
    shaderRenderer_ = std::make_unique<ShaderRenderer>();
}

ShaderController::~ShaderController(void)
{
    Release();
}

void ShaderController::Initialize(void)
{
    shaderRenderer_->Initialize();
}

void ShaderController::Release(void)
{
    shaderRenderer_->Release();
    renderCommandQueue_.clear();
}

void ShaderController::ExecuteDrawCommands(void)
{
    if (renderCommandQueue_.empty())
    {
        return;
    }

    std::sort(renderCommandQueue_.begin(), renderCommandQueue_.end(),
        [](const RenderCommand& leftCommand, const RenderCommand& rightCommand)
        {
            if (leftCommand.vertexShaderHandleId != rightCommand.vertexShaderHandleId)
            {
                return leftCommand.vertexShaderHandleId < rightCommand.vertexShaderHandleId;
            }
            return leftCommand.pixelShaderHandleId < rightCommand.pixelShaderHandleId;
        });

    shaderRenderer_->BeginBatch();

    for (const auto& renderCommand : renderCommandQueue_)
    {
        shaderRenderer_->ExecuteCommand(renderCommand);
    }

    shaderRenderer_->EndBatch();

    renderCommandQueue_.clear();
}