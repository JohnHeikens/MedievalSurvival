#include "math/graphics/color/color.h"
#include "interface/idestructable.h"
#pragma once

template<typename r, typename i>
struct brush : IDestructable
{
	typedef i inputType;
	typedef r resultingType;
	virtual resultingType getValue(const inputType& pos) const = 0;
};

typedef brush<color, vec2> colorBrush;

struct colorBrushSizeT : public brush<color, vect2<fsize_t>>, virtual colorBrush
{
	typedef vect2<fsize_t> inputType;
	typedef color resultingType;
	inline virtual color getValue(cvect2<fsize_t>& pos) const override = 0;

	inline virtual color getValue(cvec2& pos) const
	{

		if constexpr (isDebugging)
		{
			if (pos.x() < 0)
			{
				return colorPalette::red;
			}
			else if (pos.y() < 0)
			{
				return colorPalette::green;
			}
		}
		else 
		{
			assumeInRelease((pos.x() >= 0) && (pos.y() >= 0));
		}
		return getValue(floorVector<fsize_t>(pos));
	}
};