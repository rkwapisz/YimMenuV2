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

			for (const auto weaponHash : g_WeaponHashes)
			{
				if (!WEAPON::IS_WEAPON_VALID(weaponHash) || !WEAPON::HAS_PED_GOT_WEAPON(handle, weaponHash, false))
				{
					continue;
				}

				int maxAmmo = 0;

				if (WEAPON::GET_MAX_AMMO(handle, weaponHash, &maxAmmo))
					WEAPON::SET_PED_AMMO(handle, weaponHash, maxAmmo, false);
			}
		}
	};

	static GiveMaxAmmo _GiveMaxAmmo{"givemaxammo", "Refill Ammo", "Refills your ammo. Mk2 weapons must be equipped for this to work."};
}