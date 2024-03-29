#pragma once
#include "audioCollection.h"
#include "soundhandler2d.h"
struct tickableBlockContainer;
struct musicCollection : audioCollection
{
	std::vector<std::wstring> audioPaths = std::vector<std::wstring>();
	void addAudioFile(const std::wstring& path) override;
	musicCollection(const std::wstring& path);
	musicCollection() {}
	std::shared_ptr<music2d> playRandomAudio(cfp& volume = 1, cfp& pitch = 1);
	std::shared_ptr<music2d> playAudio(csize_t& index, cfp& volume = 1, cfp& pitch = 1);
	std::shared_ptr<music2d> playAudio(csize_t& index, tickableBlockContainer* containerIn, cvec2& position, cfp& volume = 1, cfp& pitch = 1);
	std::shared_ptr<music2d> playRandomAudio(tickableBlockContainer* containerIn, cvec2& position, cfp& volume = 1, cfp& pitch = 1);
	void addMusic(std::shared_ptr<musicCollection> music);
};