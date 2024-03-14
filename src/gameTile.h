#pragma once

#include "application/control/form.h"
#include "application/control/picturebox.h"
#include "application/control/label.h"
struct gameTile : form
{
	gameTile(const std::wstring& saveFileName);
	pictureBox* screenShot = nullptr;
	label* infoLabel;
	std::wstring gameName = std::wstring(L"");
	virtual ~gameTile() override;
	virtual void layout(crectanglei2& newRect) override;
};