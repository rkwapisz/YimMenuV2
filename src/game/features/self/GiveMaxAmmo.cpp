#include "core/commands/Command.hpp"
#include "game/backend/Self.hpp"
#include "game/gta/data/Weapons.hpp"
#include "game/gta/Natives.hpp"

namespace YimMenu::Features
{
	class GiveMaxAmmo : public Command
	{
		using Command::Command;

		virtual void OnCall() override
		{
			auto ped = Self::GetPed();

			if (!ped)
				return;

			const auto handle = ped.GetHandle();
			const Hash selectedWeapon = WEAPON::GET_SELECTED_PED_WEAPON(handle);

			for (const auto weaponHash : g_WeaponHashes)
			{
				if (weaponHash == "WEAPON_UNARMED"_J)
					continue;

				if (!WEAPON::HAS_PED_GOT_WEAPON(handle, weaponHash, false))
					continue;

				const Hash ammoType = WEAPON::GET_PED_AMMO_TYPE_FROM_WEAPON(handle, weaponHash);

				int maxAmmo = 0;

				// Mk2 weapons need to be refilled with the currently selected ammo type (e.g., incendiary, FMJ), not the generic ammo
				if (ammoType != 0)
				{
					if (!WEAPON::GET_MAX_AMMO_BY_TYPE(handle, ammoType, &maxAmmo))
					{
						continue;
					}

					WEAPON::SET_PED_AMMO_BY_TYPE(handle, ammoType, maxAmmo);
				}
				else
				{
					if (!WEAPON::GET_MAX_AMMO(handle, weaponHash, &maxAmmo))
					{
						continue;
					}

					WEAPON::SET_PED_AMMO(handle, weaponHash, maxAmmo, false);
				}

				const int maxClipAmmo =
				    WEAPON::GET_MAX_AMMO_IN_CLIP(handle, weaponHash, false);

				if (maxClipAmmo > 0)
				{
					WEAPON::SET_AMMO_IN_CLIP(handle, weaponHash, maxClipAmmo);
				}
			}

			// Mk2 weapons need to be handled differently and require a reload
			if (selectedWeapon != "WEAPON_UNARMED"_J && WEAPON::HAS_PED_GOT_WEAPON(handle, selectedWeapon, false))
			{
				WEAPON::MAKE_PED_RELOAD(handle);
				WEAPON::REFILL_AMMO_INSTANTLY(handle);
			}
		}
	};

	static GiveMaxAmmo _GiveMaxAmmo{"givemaxammo", "Refill Ammo", "Refills your ammo. Mk2 weapons must be equipped for this to work."};
}