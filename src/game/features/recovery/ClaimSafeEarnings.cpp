#include "core/backend/ScriptMgr.hpp"
#include "core/commands/ListCommand.hpp"
#include "game/backend/Self.hpp"
#include "game/gta/ScriptGlobal.hpp"
#include "game/pointers/Pointers.hpp"
#include "types/script/globals/GPBD_FM.hpp"
#include "types/script/globals/GPBD_FM_2.hpp"

#include <chrono>
#include <cstddef>
#include <utility>
#include <vector>

namespace YimMenu::Features
{
	enum class eAppVinewoodMenuSafe
	{
		NIGHTCLUB,
		ARCADE,
		AGENCY,
		SALVAGE_YARD,
		BAIL_OFFICE,
		GARMENT_FACTORY,
		HANDS_ON_CAR_WASH
	};

	static std::vector<std::pair<int, const char*>> businessNames = {{static_cast<int>(eAppVinewoodMenuSafe::NIGHTCLUB), "Nightclub"}, {static_cast<int>(eAppVinewoodMenuSafe::ARCADE), "Arcade"}, {static_cast<int>(eAppVinewoodMenuSafe::AGENCY), "Agency"}, {static_cast<int>(eAppVinewoodMenuSafe::SALVAGE_YARD), "Salvage Yard"}, {static_cast<int>(eAppVinewoodMenuSafe::BAIL_OFFICE), "Bail Office"}, {static_cast<int>(eAppVinewoodMenuSafe::GARMENT_FACTORY), "Garment Factory"}, {static_cast<int>(eAppVinewoodMenuSafe::HANDS_ON_CAR_WASH), "Hands On Car Wash"}};

	static ListCommand _SelectedBusiness{"businesssafe", "Business", "Business to claim earnings from.", businessNames, 0};

	static bool HasBusinessSafeEarnings(eAppVinewoodMenuSafe business)
	{
		const auto playerId = Self::GetPlayer().GetId();

		switch (business)
		{
		case eAppVinewoodMenuSafe::NIGHTCLUB: return GPBD_FM::Get()->Entries[playerId].PropertyData.NightclubData.SafeCashValue > 0;
		case eAppVinewoodMenuSafe::ARCADE: return GPBD_FM::Get()->Entries[playerId].PropertyData.ArcadeData.SafeCashValue > 0;
		case eAppVinewoodMenuSafe::AGENCY: return GPBD_FM::Get()->Entries[playerId].PropertyData.FixerHQData.SafeCashValue > 0;
		case eAppVinewoodMenuSafe::SALVAGE_YARD: return GPBD_FM::Get()->Entries[playerId].PropertyData.SalvageYardData.TotalEarnings > 0;
		case eAppVinewoodMenuSafe::BAIL_OFFICE: return GPBD_FM::Get()->Entries[playerId].PropertyData.BailShopData.SafeCashValue > 0;
		case eAppVinewoodMenuSafe::GARMENT_FACTORY: return GPBD_FM::Get()->Entries[playerId].PropertyData.HackerDenData.SafeCashValue > 0;
		case eAppVinewoodMenuSafe::HANDS_ON_CAR_WASH: return GPBD_FM_2::Get()->Entries[playerId].SYVehSaleData.HOWCData.SafeCashValue > 0;
		}

		return false;
	}

	static bool ClaimBusinessSafeEarnings(eAppVinewoodMenuSafe business)
	{
		if (!HasBusinessSafeEarnings(business))
			return false;

		switch (business)
		{
		case eAppVinewoodMenuSafe::NIGHTCLUB: *ScriptGlobal(2708943).As<BOOL*>() = TRUE; break;
		case eAppVinewoodMenuSafe::ARCADE: *ScriptGlobal(2708952).As<BOOL*>() = TRUE; break;
		case eAppVinewoodMenuSafe::AGENCY: *ScriptGlobal(2708961).As<BOOL*>() = TRUE; break;
		case eAppVinewoodMenuSafe::SALVAGE_YARD: *ScriptGlobal(2708970).As<BOOL*>() = TRUE; break;
		case eAppVinewoodMenuSafe::BAIL_OFFICE: *ScriptGlobal(2708979).As<BOOL*>() = TRUE; break;
		case eAppVinewoodMenuSafe::GARMENT_FACTORY: *ScriptGlobal(2708994).As<BOOL*>() = TRUE; break;
		case eAppVinewoodMenuSafe::HANDS_ON_CAR_WASH: *ScriptGlobal(2709001).As<BOOL*>() = TRUE; break;
		default: return false;
		}

		return true;
	}

	class ClaimSafeEarnings : public Command
	{
		using Command::Command;

		void OnCall() override
		{
			if (!*Pointers.IsSessionStarted)
				return;

			ClaimBusinessSafeEarnings(static_cast<eAppVinewoodMenuSafe>(_SelectedBusiness.GetState()));
		}
	};

	class ClaimAllSafeEarnings : public Command
	{
		using Command::Command;

		void OnCall() override
		{
			if (!*Pointers.IsSessionStarted)
				return;

			std::vector<eAppVinewoodMenuSafe> businessesToClaim;
			businessesToClaim.reserve(businessNames.size());

			// Build the complete work list before claiming anything. Empty and
			// unavailable safes do not create unnecessary delays.
			for (const auto& businessEntry : businessNames)
			{
				const auto business = static_cast<eAppVinewoodMenuSafe>(businessEntry.first);

				if (HasBusinessSafeEarnings(business))
					businessesToClaim.push_back(business);
			}

			constexpr auto claimDelay = std::chrono::seconds(5);

			for (std::size_t index = 0; index < businessesToClaim.size(); ++index)
			{
				if (index != 0)
				{
					ScriptMgr::Yield(claimDelay);

					// The player may have left the session during the delay.
					if (!*Pointers.IsSessionStarted)
						return;
				}

				ClaimBusinessSafeEarnings(businessesToClaim[index]);
			}
		}
	};

	static ClaimSafeEarnings _ClaimSafeEarnings{"claimsafeearnings", "Claim Safe Earnings", "Claims your safe earnings from the selected business."};

	static ClaimAllSafeEarnings _ClaimAllSafeEarnings{"claimallsafeearnings", "Claim All Safe Earnings", "Claims all currently available business safe earnings.\nThere is a 5s delay between each collection."};
}