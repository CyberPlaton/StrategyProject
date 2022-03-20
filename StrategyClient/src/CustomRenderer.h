#pragma once

#include <CherrySoda/Renderers/EverythingRenderer.h>

namespace cherrysoda
{
	class CustomRenderer : public EverythingRenderer
	{
	public:
		CustomRenderer() : EverythingRenderer(){}
		void Render(Scene* scene) override
		{
			Graphics::BeginRenderPass(RenderPass());
			Graphics::SetEffect(GetEffect());
			Graphics::UseCurrentRenderPass()->SetCamera(GetCamera());
			Draw::GetSpriteBatch()->Begin();

			// Replace the next three lines with what you want
			scene->Entities()->Render();
#if defined DEBUG || RELEASE
			scene->Entities()->DebugRender(GetCamera());
#endif

			Draw::GetSpriteBatch()->End();
			Graphics::SetEffect(nullptr);
			Graphics::EndRenderPass(RenderPass());

		}
	};

}