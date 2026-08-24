#pragma once

#include "core/frontend/manager/Category.hpp"
#include "core/util/Joaat.hpp"
#include "game/frontend/items/Items.hpp"

namespace YimMenu::Submenus
{
	enum class WeaponManagementPanel
	{
		None,
		Editor,
		AddRemoveWeapons,
		Loadouts
	};

	struct WeaponDisplay
	{
		std::string name;
		std::string desc;
		joaat_t hash;
	};

	bool BeginWeaponManagementPanel(WeaponManagementPanel panel, const char* label);

	void EnsureWeaponDisplayCache();

	const std::vector<WeaponDisplay>& GetWeaponDisplayCache();

	const WeaponDisplay* FindWeaponDisplay(joaat_t weaponHash);

	std::string GetWeaponDisplayName(joaat_t weaponHash);

	std::shared_ptr<Category> BuildWeaponsMenu();
}
