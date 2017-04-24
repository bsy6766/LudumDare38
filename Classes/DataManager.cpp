#include "DataManager.h"

DataManager* DataManager::instance = nullptr;

DataManager::DataManager()
{
}

DataManager::~DataManager()
{
}

DataManager* DataManager::getInstance()
{
	if (instance == nullptr)
	{
		instance = new DataManager();
	}

	return instance;
}

void DataManager::deleteInstance()
{
	//check if instance is alive pointer
	if (instance != nullptr)
	{
		//if data unordered_map is not empty, delete everything
		if (!instance->data.empty())
		{
			for (auto sdTree : instance->data)
			{
				delete sdTree.second;
			}
		}

		instance->filePath.clear();

		delete instance;
	}
}

void DataManager::loadDataTree(const std::string& filePath, const std::string& dataName)
{
	DataTree* sdTreeData = DataTree::create(filePath);
	if (sdTreeData != nullptr)
	{
		this->data[dataName] = sdTreeData;
		this->filePath[dataName] = filePath;
	}
}

DataTree* DataManager::getData(const std::string& dataName)
{
	auto sdTreeData = this->data.find(dataName);
	if (sdTreeData == this->data.end())
	{
		return nullptr;
	}
	else
	{
		return sdTreeData->second;
	}
}

void DataManager::encrypt(std::string& data)
{
	for (unsigned i = 0; i<data.size(); i++)
		data[i] ^= this->accessKey[i%this->accessKey.size()];
}

void DataManager::decrypt(std::string& data)
{
	for (unsigned i = 0; i<data.size(); i++)
		data[i] ^= this->accessKey[i%this->accessKey.size()];
}
bool DataManager::saveData(const std::string& dataName)
{
	auto data = this->data.find(dataName);
	if (data == this->data.end())
	{
		//cocos2d::log("WARNING! DataManager can't find data called %s", dataName.c_str());
		return false;
	}
	else
	{
		data->second->save(this->filePath[dataName]);
	}
}