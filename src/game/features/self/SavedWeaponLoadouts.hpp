#pragma once

#include "core/filemgr/FileMgr.hpp"

namespace YimMenu
{
	class SavedWeaponLoadouts
	{
	public:
		static void RefreshList(std::vector<std::string>& files);

		static bool Save(std::string fileName);
		static bool Load(const std::string& fileName);
		static bool Delete(const std::string& fileName);

		static bool HasLastApplied();
		static bool RestoreLastApplied();

	private:
		static Folder CheckFolder();
		static std::string SanitizeFileName(std::string fileName);

		static nlohmann::json GetJson();
		static bool ApplyJson(const nlohmann::json& loadout);
	};
}