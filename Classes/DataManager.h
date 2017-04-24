#ifndef DATA_MANAGER_H
#define DATA_MANAGER_H

#include <string>
#include <unordered_map>
#include "DataTree.h"


class DataManager
{
private:
	DataManager();
	static DataManager* instance;

	std::unordered_map<std::string/*data name*/, DataTree*> data;
	std::unordered_map<std::string/*data name*/, std::string/*file path*/> filePath;

	const std::string accessKey = "ABCDEFGH";

	void encrypt(std::string& data);
	void decrypt(std::string& data);

public:
	static DataManager* getInstance();
	static void deleteInstance();

	DataManager(DataManager const&) = delete;
	void operator=(DataManager const&) = delete;

	~DataManager();

	void loadDataTree(const std::string& filePath, const std::string& dataName);

	DataTree* getData(const std::string& dataName);

	bool saveData(const std::string& dataName);

};

#endif