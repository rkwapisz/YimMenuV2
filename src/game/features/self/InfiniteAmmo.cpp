#include "core/commands/LoopedCommand.hpp"
#include "game/backend/Self.hpp"
#include "game/gta/data/Weapons.hpp"
#include "game/gta/Natives.hpp"

namespace YimMenu::Features
{
	class InfiniteAmmo : public LoopedCommand
	{
		using LoopedCommand::LoopedCommand;

		static void RefillAllWeaponAmmo()
		{
			auto ped = Self::GetPed();

			if (!ped)
				return;

			const auto handle = ped.GetHandle();

			for (const auto weaponHash : g_WeaponHashes)
			{
				if (!WEAPON::IS_WEAPON_VALID(weaponHash) || !WEAPON::HAS_PED_GOT_WEAPON(handle, weaponHash, false))
				{
					continue;
				}

				int maxAmmo = 0;

				if (WEAPON::GET_MAX_AMMO(handle, weaponHash, &maxAmmo))
				{
					WEAPON::SET_PED_AMMO(handle, weaponHash, maxAmmo, false);
				}

				Hash ammoType = WEAPON::GET_PED_AMMO_TYPE_FROM_WEAPON(handle, weaponHash);

				int maxTypeAmmo = 0;

				if (WEAPON::GET_MAX_AMMO_BY_TYPE(handle, ammoType, &maxTypeAmmo))
				{
					WEAPON::SET_PED_AMMO_BY_TYPE(handle, ammoType, maxTypeAmmo);
				}

				int clipSize = WEAPON::GET_WEAPON_CLIP_SIZE(weaponHash);

				WEAPON::SET_AMMO_IN_CLIP(handle, weaponHash, clipSize);

			}
		}

		virtual void OnTick() override
		{
			RefillAllWeaponAmmo();
		}
	};

	static InfiniteAmmo _InfiniteAmmo{"infiniteammo", "Infinite Ammo", "Infinite ammo for all weapons."};
}