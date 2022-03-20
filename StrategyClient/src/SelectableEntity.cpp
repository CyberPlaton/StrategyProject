#include "SelectableEntity.h"

namespace cherrysoda
{
	void SelectableEntity::Update()
	{
		CHERRYSODA_ASSERT(GetEntity()->Get< Sprite >(), "SelectableEntity requires a Sprite component!");
		CHERRYSODA_ASSERT(GetEntity()->Get< CollidableComponent >(), "SelectableEntity requires a Collidable component!");

		auto entity = GetEntity();
		auto sprite = entity->Get< Sprite >();
		auto collider = entity->Get< CollidableComponent >();

		if (!collider->GetCollider())
		{
			auto w = sprite->Width();
			auto h = sprite->Height();
			auto box = new Hitbox(w, h, -w / 2, -h / 2);
			collider->SetCollider(box);
		}

		int x = MInput::Mouse()->Position().x;
		int y = MInput::Mouse()->Position().y;
		auto camera = sprite->GetScene()->FirstRenderer()->GetCamera();
		float screenx = camera->ScreenToWorld({ x, y }).x;
		float screeny = camera->ScreenToWorld({ x, y }).y;
		
		bool click = false;
		bool hovered = false;

		Circle point(1, { screenx, screeny });
		if (collider->GetCollider()->Collide(&point))
		{
			if (MInput::Mouse()->CheckLeftButton())
			{
				click = true;
				OnHeldLMB();
			}
			if (MInput::Mouse()->CheckRightButton())
			{
				click = true;
				OnHeldRMB();
			}
			if (MInput::Mouse()->PressedRightButton())
			{
				click = true;
				OnPressedLMB();
			}
			if (MInput::Mouse()->PressedLeftButton())
			{
				click = true;
				OnPressedRMB();
			}
			if (MInput::Mouse()->RleasedLeftButton())
			{
				click = true;
				OnReleasedLMB();
			}
			if (MInput::Mouse()->RleasedRightButton())
			{
				click = true;
				OnReleasedRMB();
			}

			hovered = (click == false) ? true : false;
			if (hovered)
			{
				OnHoverEnter();
			}
		}
	}
}