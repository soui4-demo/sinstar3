#pragma once

class SkinPreview : public SHostWnd
{
public:
	SkinPreview(void);
	~SkinPreview(void);

	void SetPreview(IBitmapS *pImg);
};
