#include "structureBlockData.h"
void structureBlockData::serializeValue(nbtSerializer& s)
{
	s.serializeValue(std::wstring(L"structure name"), structureName);
	s.serializeValue(std::wstring(L"relative structure position"), relativeStructurePosition);
	s.serializeValue(std::wstring(L"structure size"), structureSize);
	s.serializeValue(std::wstring(L"include entities"), includeEntities);
	s.serializeValue(std::wstring(L"mirror x"), mirrorX);
	s.serializeValue(std::wstring(L"structure rotation"), structureRotation);
	s.serializeValue(std::wstring(L"structure integrity"), structureIntegrity);
	s.serializeValue(std::wstring(L"structure seed"), structureSeed);
	s.serializeValue(std::wstring(L"show bounding box"), showBoundingBox);
	s.serializeValue(std::wstring(L"structure block mode"), (int&)mode);
}