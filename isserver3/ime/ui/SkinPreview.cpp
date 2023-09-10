#include "stdafx.h"
#include "SkinPreview.h"

SkinPreview::SkinPreview(void):SHostWnd(UIRES.LAYOUT.wnd_skin_preview)
{
}

SkinPreview::~SkinPreview(void)
{
}

void SkinPreview::SetPreview(IBitmapS *pImg)
{
	SImageWnd *pImgWnd = FindChildByID2<SImageWnd>(R.id.img_preview);
	if(pImgWnd){
		pImgWnd->SetImage(pImg);
		pImgWnd->RequestRelayout();
	}
}
