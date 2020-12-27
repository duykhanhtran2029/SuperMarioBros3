#include "BreakableBrick.h"
#include "PlayScence.h"
#include "MushRoom.h"

CMario* bbmario;
CPlayScene* bbscence;
void CBreakableBrick::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
	CGameObject::Update(dt);
	if (isDestroyed)
		return;
	float mLeft, mTop, mRight, mBottom;
	float oLeft, oTop, oRight, oBottom;
	bbscence = (CPlayScene*)CGame::GetInstance()->GetCurrentScene();
	if (bbscence != NULL)
		bbmario = ((CPlayScene*)bbscence)->GetPlayer();
	if (bbmario != NULL)
	{
		bbmario->GetBoundingBox(mLeft, mTop, mRight, mBottom);
		GetBoundingBox(oLeft, oTop, oRight, oBottom);
		if (isColliding(floor(mLeft), mTop, ceil(mRight), mBottom) && bbmario->level == MARIO_LEVEL_TAIL && bbmario->isTurningTail && oBottom > ceil(mTop) + MARIO_BIG_BBOX_HEIGHT - BRICK_BBOX_HEIGHT)
		{
			//DebugOut(L"[BB]	%f %f\n",ceil(mTop), oBottom);
			CAnimationSets* animation_sets = CAnimationSets::GetInstance();
			LPANIMATION_SET tmp_ani_set = animation_sets->Get(PIECE_ANI_SET_ID);

			CPiece* upleft = new CPiece(-1,-1);
			upleft->SetPosition(x, y);
			upleft->SetAnimationSet(tmp_ani_set);

			CPiece* upright = new CPiece(1, -1);
			upright->SetPosition(x + PIECE_BBOX_WIDTH, y);
			upright->SetAnimationSet(tmp_ani_set);

			CPiece* downleft = new CPiece(-1, 1);
			downleft->SetPosition(x, y + PIECE_BBOX_HEIGHT);
			downleft->SetAnimationSet(tmp_ani_set);

			CPiece* downright = new CPiece(1, 1);
			downright->SetPosition(x + PIECE_BBOX_WIDTH, y + PIECE_BBOX_HEIGHT);
			downright->SetAnimationSet(tmp_ani_set);

			bbscence->PushBack(upleft);
			bbscence->PushBack(upright);
			bbscence->PushBack(downleft);
			bbscence->PushBack(downright);

			isDestroyed = true;
			x = y = -50;
		}
	}
}
void CBreakableBrick::Render()
{
	if (isDestroyed)
		return;
	animation_set->at(0)->Render(x, y);
	RenderBoundingBox();
}