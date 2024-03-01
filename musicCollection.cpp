#include "musicCollection.h"
#include "include/math/random/random.h"
#include "entity.h"
#include "tickableBlockContainer.h"
void musicCollection::addAudioFile(const std::wstring& path)
{
	audioPaths.push_back(path);
}

musicCollection::musicCollection(const std::wstring& path)
{
	addAudioFileName(path);
}

std::shared_ptr<music2d> musicCollection::playRandomAudio(cfp& volume, cfp& pitch)
{
	return playAudio(randIndex(currentRandom, audioPaths.size()), volume, pitch);
}

std::shared_ptr < music2d> musicCollection::playAudio(csize_t& index, cfp& volume, cfp& pitch)
{//TODO: send music to client
	std::shared_ptr<music2d> musicToPlay = std::make_shared<music2d>(audioPaths[index], cvec2(), volume, pitch, false);
	handler->playAudio(musicToPlay);
	return musicToPlay;
}

std::shared_ptr<music2d> musicCollection::playAudio(csize_t& index, tickableBlockContainer* containerIn, cvec2& position, cfp& volume, cfp& pitch)
{
	//TODO: send sound strings to the client (fe entity.enderdragon.roar)
	//if (containerIn->rootDimension == currentPlayableCharachter->newDimension)
	//{
	//	std::shared_ptr<music2d> musicToPlay = std::make_shared<music2d>(audioPaths[index], containerIn->containerToRootTransform.multPointMatrix(position), volume, pitch, true);
	//	handler->playAudio(musicToPlay);
	//	return musicToPlay;
	//}
	return nullptr;
}

std::shared_ptr<music2d> musicCollection::playRandomAudio(tickableBlockContainer* containerIn, cvec2& position, cfp& volume, cfp& pitch)
{
	return playAudio(randIndex(currentRandom, audioPaths.size()), containerIn, position, volume, pitch);
}

void musicCollection::addMusic(std::shared_ptr<musicCollection> music)
{
	audioPaths.insert(audioPaths.end(), music->audioPaths.begin(), music->audioPaths.end());
}