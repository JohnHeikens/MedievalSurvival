#include "soundCollection.h"
#include "constants.h"
#include "tickableBlockContainer.h"
#include "server.h"
#include <filesystem>
#include <memory>
#include <string>
#include <vector>
#include <SFML/Audio/SoundBuffer.hpp>
#include "audioCollection.h"
#include "array/wstringFunctions.h"
#include "GlobalFunctions.h"
#include "math/random/random.h"
#include "math/vectn.h"
#include "soundhandler2d.h"
#include "soundPacket.h"
#include "human.h"
#include <fstream>
#include "filesystem/fileio.h"
#include "filesystem/textfile.h"
#include "filesystem/sfmlInputStream.h"

std::vector<soundCollection *> globalSoundCollectionList = std::vector<soundCollection *>();

soundCollection::soundCollection()
{
	soundCollectionID = (int)globalSoundCollectionList.size();
	globalSoundCollectionList.push_back(this);
	audioToChooseFrom = std::vector<std::shared_ptr<sf::SoundBuffer>>();
}

soundCollection::soundCollection(const stdPath &path) : soundCollection()
{
	addAudioFileName(path);
}

void soundCollection::addSoundCollection(const soundCollection &collection)
{
	audioToChooseFrom.insert(audioToChooseFrom.end(), collection.audioToChooseFrom.begin(), collection.audioToChooseFrom.end());
}

void audioCollection::addAudioFileName(const stdPath &path)
{
	bool loaded = false;
	stdPath defaultPath = path;
	defaultPath += L".ogg";
	if (stdFileSystem::exists(defaultPath))
	{
		addAudioFile(defaultPath);
		loaded = true;
	}
	// warning: starts with 1
	int i = 1;
	while (true)
	{
		stdPath pathNumber = path;
		pathNumber += std::to_wstring(i) + L".ogg";
		if (!stdFileSystem::exists(pathNumber))
			break;
		{
			addAudioFile(pathNumber);
			loaded = true;
		}
		i++;
	}
	if (!loaded)
	{
		handleError(std::wstring(L"no sound found in " + path.wstring()));
	}
}

void audioCollection::addAudioFile(const stdPath &path)
{
	handleError(std::wstring(L"not implemented"));
}

void soundCollection::addAudioFile(const stdPath &path)
{
    const auto buffer = std::make_shared<sf::SoundBuffer>();
    //this way, even on android we can read from an actual file instead of the internal APK storage
    auto stream = sfmlInputStream(std::make_shared<std::ifstream>(path, getOpenMode(false)));

    //std::string data = readAllText(path);

    //std::ifstream s(path, getOpenMode(false));

    //s.write(text.c_str(), (std::streamsize)text.size());
    //s.close();
    //str = std::ifstream(path,fileOpenMode::read)
    //sf::FileInputStream str;
    //str.open(path);


    //sf::InputStream str
    buffer->loadFromStream(stream);
	audioToChooseFrom.push_back(buffer);
}

std::shared_ptr<audio2d> soundCollection::playRandomSound(tickableBlockContainer *containerIn, cvec2 &position, cfp &volume, cfp &pitch)
{
	return playSound(randIndex(currentRandom, audioToChooseFrom.size()), containerIn, position, volume, pitch);
}

std::shared_ptr<audio2d> soundCollection::playRandomSound(cfp &volume, cfp &pitch)
{
	return playSound(randIndex(currentRandom, audioToChooseFrom.size()), volume, pitch);
}

std::shared_ptr<sound2d> soundCollection::playSound(csize_t &index, tickableBlockContainer *containerIn, cvec2 &position, cfp &volume, cfp &pitch)
{
	const dimension *soundDimension = containerIn->rootDimension;
	cvec2 &soundPosition = containerIn->containerToRootTransform.multPointMatrix(position);
	const auto &players = currentServer->getPlayersInRadius(soundDimension, soundPosition, soundLoadRange);
	soundPacket packet = soundPacket(position, soundCollectionID, (int)index, volume, pitch);
	for (const auto &p : players)
	{
		p->screen.dataToSend.push_back(packet);
	}

	// if (containerIn->rootDimension == currentPlayableCharachter->newDimension)
	//{
	//	std::shared_ptr<sound2d> soundToPlay = std::make_shared<sound2d>(audioToChooseFrom[index].get(), position, volume, pitch, true);
	//	handler->playAudio(soundToPlay);
	//	return soundToPlay;
	// }
	return nullptr;
}

std::shared_ptr<sound2d> soundCollection::playSound(csize_t &index, cfp &volume, cfp &pitch)
{
	std::shared_ptr<sound2d> soundToPlay = std::make_shared<sound2d>(audioToChooseFrom[index].get(), cvec2(), volume, pitch, false);
	handler->playAudio(soundToPlay);
	return soundToPlay;
}