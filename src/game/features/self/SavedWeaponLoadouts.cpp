#include "SavedWeaponLoadouts.hpp"

#include "core/frontend/Notifications.hpp"
#include "game/backend/Self.hpp"
#include "game/gta/data/WeaponComponents.hpp"
#include "game/gta/data/Weapons.hpp"
#include "game/gta/Natives.hpp"

#include <algorithm>
#include <filesystem>
#include <format>
#include <fstream>

namespace YimMenu
{
	static bool IsCamoComponent(std::string_view componentName)
	{
		return componentName.find("_CAMO") != std::string_view::npos;
	}

	Folder SavedWeaponLoadouts::CheckFolder()
	{
		return FileMgr::GetProjectFolder("./saved_json_loadouts/");
	}

	std::string SavedWeaponLoadouts::SanitizeFileName(std::string fileName)
	{
		static constexpr std::string_view invalidChars = R"(<>:"/\|?*)";

		std::erase_if(fileName, [](const char c) {
			return static_cast<unsigned char>(c) < 32 || invalidChars.find(c) != std::string_view::npos;
		});

		// Windows filenames cannot begin/end with spaces or end with a period.
		while (!fileName.empty() && fileName.front() == ' ')
			fileName.erase(fileName.begin());

		while (!fileName.empty() && (fileName.back() == ' ' || fileName.back() == '.'))
		{
			fileName.pop_back();
		}

		return fileName;
	}

	void SavedWeaponLoadouts::RefreshList(std::vector<std::string>& files)
	{
		files.clear();

		const auto folder = CheckFolder();

		for (const auto& entry : std::filesystem::directory_iterator(folder.Path()))
		{
			if (!entry.is_regular_file())
				continue;

			if (entry.path().extension() != ".json")
				continue;

			files.push_back(entry.path().filename().generic_string());
		}

		std::ranges::sort(files);
	}

	nlohmann::json SavedWeaponLoadouts::GetJson()
	{
		nlohmann::json loadout;

		auto ped = Self::GetPed();

		if (!ped)
			return loadout;

		const auto handle = ped.GetHandle();

		loadout["selected_weapon"] = WEAPON::GET_SELECTED_PED_WEAPON(handle);
		loadout["weapons"] = nlohmann::json::array();

		for (const auto weaponHash : g_WeaponHashes)
		{
			if (weaponHash == "WEAPON_UNARMED"_J)
				continue;

			if (!WEAPON::HAS_PED_GOT_WEAPON(handle, weaponHash, false))
				continue;

			const Hash ammoType = WEAPON::GET_PED_AMMO_TYPE_FROM_WEAPON(handle, weaponHash);

			int ammo = 0;

			if (ammoType != 0)
			{
				ammo = WEAPON::GET_PED_AMMO_BY_TYPE(handle, ammoType);
			}
			else
			{
				ammo = WEAPON::GET_AMMO_IN_PED_WEAPON(handle, weaponHash);
			}

			int clipAmmo = 0;

			WEAPON::GET_AMMO_IN_CLIP(handle, weaponHash, &clipAmmo);

			nlohmann::json weapon;

			weapon["hash"] = weaponHash;
			weapon["ammo"] = std::max(0, ammo);
			weapon["ammo_type"] = ammoType;
			weapon["clip_ammo"] = std::max(0, clipAmmo);
			weapon["tint"] = WEAPON::GET_PED_WEAPON_TINT_INDEX(handle, weaponHash);

			weapon["components"] = nlohmann::json::array();

			for (const auto& component : WeaponComponents::GetComponentsForWeapon(weaponHash))
			{
				if (!WEAPON::HAS_PED_GOT_WEAPON_COMPONENT(handle, weaponHash, component.Hash))
				{
					continue;
				}

				nlohmann::json componentJson;

				componentJson["hash"] = component.Hash;
				componentJson["name"] = std::string(component.Name);

				if (IsCamoComponent(component.Name))
				{
					const int componentTint = WEAPON::GET_PED_WEAPON_COMPONENT_TINT_INDEX(handle, weaponHash, component.Hash);

					if (componentTint >= 0)
						componentJson["tint"] = componentTint;
				}

				weapon["components"].push_back(std::move(componentJson));
			}

			loadout["weapons"].push_back(std::move(weapon));
		}

		return loadout;
	}

	bool SavedWeaponLoadouts::Save(std::string fileName)
	{
		fileName = SanitizeFileName(std::move(fileName));

		if (fileName.empty())
		{
			Notifications::Show("Weapon Loadouts", "Please enter a valid loadout name.", NotificationType::Warning);

			return false;
		}

		auto ped = Self::GetPed();

		if (!ped)
		{
			Notifications::Show("Weapon Loadouts", "Unable to find the current player.", NotificationType::Warning);

			return false;
		}

		fileName += ".json";

		try
		{
			const auto file = CheckFolder().GetFile(fileName);

			std::ofstream stream(file.Path(), std::ios::out | std::ios::trunc);

			if (!stream.is_open())
			{
				Notifications::Show("Weapon Loadouts", "Unable to open the loadout file for writing.", NotificationType::Error);

				return false;
			}

			const auto loadout = GetJson();

			if (!loadout.contains("weapons"))
			{
				Notifications::Show("Weapon Loadouts", "Unable to capture the current weapon loadout.", NotificationType::Error);

				return false;
			}

			stream << loadout.dump(4);
			stream.close();

			Notifications::Show("Weapon Loadouts", std::format("Saved {}", fileName), NotificationType::Success);

			return true;
		}
		catch (const std::exception& e)
		{
			LOG(WARNING) << "Failed to save weapon loadout: " << e.what();

			Notifications::Show("Weapon Loadouts", "Failed to save the weapon loadout.", NotificationType::Error);

			return false;
		}
	}

	bool SavedWeaponLoadouts::ApplyJson(const nlohmann::json& loadout)
	{
		auto ped = Self::GetPed();

		if (!ped)
			return false;

		if (!loadout.contains("weapons") || !loadout["weapons"].is_array())
		{
			return false;
		}

		const auto handle = ped.GetHandle();

		WEAPON::REMOVE_ALL_PED_WEAPONS(handle, false);

		for (const auto& weapon : loadout["weapons"])
		{
			if (!weapon.contains("hash"))
				continue;

			const auto weaponHash = weapon["hash"].get<Hash>();

			if (!WEAPON::IS_WEAPON_VALID(weaponHash))
				continue;

			const int ammo = std::max(0, weapon.value("ammo", 0));
			const int clipAmmo = std::max(0, weapon.value("clip_ammo", 0));
			const int weaponTint = std::max(0, weapon.value("tint", 0));

			WEAPON::GIVE_WEAPON_TO_PED(handle, weaponHash, 0, false, false);

			WEAPON::SET_PED_WEAPON_TINT_INDEX(handle, weaponHash, weaponTint);

			if (weapon.contains("components") && weapon["components"].is_array())
			{
				for (const auto& component : weapon["components"])
				{
					if (!component.contains("hash"))
						continue;

					const auto componentHash = component["hash"].get<Hash>();

					WEAPON::GIVE_WEAPON_COMPONENT_TO_PED(handle, weaponHash, componentHash);

					if (component.contains("tint"))
					{
						const int componentTint = std::max(0, component["tint"].get<int>());

						WEAPON::SET_PED_WEAPON_COMPONENT_TINT_INDEX(handle, weaponHash, componentHash, componentTint);
					}
				}
			}

			const Hash currentAmmoType = WEAPON::GET_PED_AMMO_TYPE_FROM_WEAPON(handle, weaponHash);

			if (currentAmmoType != 0)
			{
				WEAPON::SET_PED_AMMO_BY_TYPE(handle, currentAmmoType, ammo);
			}
			else
			{
				WEAPON::SET_PED_AMMO(handle, weaponHash, ammo, false);
			}

			WEAPON::SET_AMMO_IN_CLIP(handle, weaponHash, clipAmmo);
		}

		const Hash selectedWeapon = loadout.value("selected_weapon", static_cast<Hash>("WEAPON_UNARMED"_J));

		if (selectedWeapon == "WEAPON_UNARMED"_J || WEAPON::HAS_PED_GOT_WEAPON(handle, selectedWeapon, false))
		{
			WEAPON::SET_CURRENT_PED_WEAPON(handle, selectedWeapon, true);
		}
		else
		{
			WEAPON::SET_CURRENT_PED_WEAPON(handle, "WEAPON_UNARMED"_J, true);
		}

		return true;
	}

	bool SavedWeaponLoadouts::Load(const std::string& fileName)
	{
		if (fileName.empty())
		{
			Notifications::Show("Weapon Loadouts", "Select a loadout first.", NotificationType::Warning);

			return false;
		}

		try
		{
			const auto path = CheckFolder().GetFile(fileName).Path();

			if (!std::filesystem::exists(path))
			{
				Notifications::Show("Weapon Loadouts", "The selected loadout file does not exist.", NotificationType::Error);

				return false;
			}

			std::ifstream stream(path);

			if (!stream.is_open())
			{
				Notifications::Show("Weapon Loadouts", "Unable to open the selected loadout.", NotificationType::Error);

				return false;
			}

			nlohmann::json loadout;

			stream >> loadout;
			stream.close();

			if (!loadout.is_object())
			{
				Notifications::Show("Weapon Loadouts", "The selected file is not a valid weapon loadout.", NotificationType::Error);

				return false;
			}

			if (!ApplyJson(loadout))
			{
				Notifications::Show("Weapon Loadouts", "The selected file is not a valid weapon loadout.", NotificationType::Error);

				return false;
			}

			Notifications::Show("Weapon Loadouts", std::format("Applied {}", fileName), NotificationType::Success);

			return true;
		}
		catch (const std::exception& e)
		{
			LOG(WARNING) << "Failed to load weapon loadout: " << e.what();

			Notifications::Show("Weapon Loadouts", "Failed to load the weapon loadout.", NotificationType::Error);

			return false;
		}
	}

	bool SavedWeaponLoadouts::Delete(const std::string& fileName)
	{
		if (fileName.empty())
			return false;

		try
		{
			const auto path = CheckFolder().GetFile(fileName).Path();

			if (!std::filesystem::exists(path))
				return false;

			if (!std::filesystem::remove(path))
				return false;

			Notifications::Show("Weapon Loadouts", std::format("Deleted {}", fileName), NotificationType::Success);

			return true;
		}
		catch (const std::exception& e)
		{
			LOG(WARNING) << "Failed to delete weapon loadout: " << e.what();

			Notifications::Show("Weapon Loadouts", "Failed to delete the weapon loadout.", NotificationType::Error);

			return false;
		}
	}
}