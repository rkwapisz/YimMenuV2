#include "core/commands/LoopedCommand.hpp"
#include "game/backend/Self.hpp"
#include "game/gta/Natives.hpp"

namespace YimMenu::Features
{
	class InfiniteAmmo : public LoopedCommand
	{
		using LoopedCommand::LoopedCommand;

		static void RefillCurrentWeaponAmmo()
		{
			auto ped = Self::GetPed();

			if (!ped)
				return;

			const auto handle = ped.GetHandle();
			const Hash weaponHash = WEAPON::GET_SELECTED_PED_WEAPON(handle);

			if (weaponHash == "WEAPON_UNARMED"_J)
				return;

			const Hash ammoType = WEAPON::GET_PED_AMMO_TYPE_FROM_WEAPON(handle, weaponHash);

			if (ammoType == 0)
				return;

			int maxAmmo = 0;

			if (!WEAPON::GET_MAX_AMMO_BY_TYPE(handle, ammoType, &maxAmmo))
			{
				return;
			}

			WEAPON::SET_PED_AMMO_BY_TYPE(handle, ammoType, maxAmmo);

			const int maxClipAmmo = WEAPON::GET_MAX_AMMO_IN_CLIP(handle, weaponHash, false);

			if (maxClipAmmo <= 0)
				return;

			int clipAmmo = 0;

			if (!WEAPON::GET_AMMO_IN_CLIP(handle, weaponHash, &clipAmmo))
			{
				return;
			}

			if (clipAmmo < maxClipAmmo)
			{
				WEAPON::SET_AMMO_IN_CLIP(handle, weaponHash, maxClipAmmo);

				WEAPON::MAKE_PED_RELOAD(handle);
				WEAPON::REFILL_AMMO_INSTANTLY(handle);
			}
		}

		virtual void OnTick() override
		{
			auto ped = Self::GetPed();

			if (!ped)
				return;

			ped.SetInfiniteAmmo(true);

			RefillCurrentWeaponAmmo();
		}

		virtual void OnDisable() override
		{
			if (auto ped = Self::GetPed())
				ped.SetInfiniteAmmo(false);
		}
	};

	static InfiniteAmmo _InfiniteAmmo{"infiniteammo", "Infinite Ammo", "Infinite ammo for all weapons."};
}