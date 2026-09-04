#include "Self.hpp"

#include "core/util/Joaat.hpp"
#include "game/frontend/items/Items.hpp"
#include "Self/OutfitEditor.hpp"
#include "Self/Weapons.hpp"

namespace YimMenu::Submenus
{
	namespace
	{
		std::shared_ptr<Group> BuildPlayerGroup()
		{
			// Four rows per column for compactcness
			auto playerGroup = std::make_shared<Group>("Player", 4);

			// Survival
			playerGroup->AddItem(std::make_shared<BoolCommandItem>("godmode"_J));
			playerGroup->AddItem(std::make_shared<BoolCommandItem>("noragdoll"_J));
			playerGroup->AddItem(std::make_shared<BoolCommandItem>("disablecriticalhits"_J));
			playerGroup->AddItem(std::make_shared<BoolCommandItem>("unlimitedoxygen"_J));

			// Visibility and session
			auto visibilityGroup = std::make_shared<Group>("", 1);
			visibilityGroup->AddItem(std::make_shared<BoolCommandItem>("invis"_J));
			visibilityGroup->AddItem(std::make_shared<ConditionalItem>("invis"_J, std::make_shared<BoolCommandItem>("localvis"_J)));

			playerGroup->AddItem(std::move(visibilityGroup));
			playerGroup->AddItem(std::make_shared<BoolCommandItem>("otr"_J));
			playerGroup->AddItem(std::make_shared<BoolCommandItem>("noidlekick"_J));
			playerGroup->AddItem(std::make_shared<BoolCommandItem>("keepplayerclean"_J));

			// General QoL
			playerGroup->AddItem(std::make_shared<BoolCommandItem>("mobileradio"_J));
			playerGroup->AddItem(std::make_shared<BoolCommandItem>("formatmoney"_J));

			return playerGroup;
		}

		std::shared_ptr<Group> BuildQuickActionsGroup()
		{
			auto quickActionsGroup = std::make_shared<Group>("Quick Actions", 2);
			quickActionsGroup->AddItem(std::make_shared<CommandItem>("heal"_J));
			quickActionsGroup->AddItem(std::make_shared<CommandItem>("cleardamage"_J));
			quickActionsGroup->AddItem(std::make_shared<CommandItem>("fillinventory"_J));
			quickActionsGroup->AddItem(std::make_shared<CommandItem>("openwardrobe"_J));
			quickActionsGroup->AddItem(std::make_shared<CommandItem>("skipcutscene"_J));
			quickActionsGroup->AddItem(std::make_shared<CommandItem>("skipconversation"_J));
			quickActionsGroup->AddItem(std::make_shared<CommandItem>("suicide"_J));

			return quickActionsGroup;
		}

		std::shared_ptr<Group> BuildAbilitiesAndWantedGroup()
		{
			// We want one ability column and one wanted level column.
			auto gameplayGroup = std::make_shared<Group>("Abilities & Wanted", 2);

			gameplayGroup->AddItem(std::make_shared<BoolCommandItem>("infspecialability"_J));

			auto specialAbilityGroup = std::make_shared<Group>("", 1);
			specialAbilityGroup->AddItem(std::make_shared<BoolCommandItem>("mpspecialability"_J, "Enable in MP"));
			specialAbilityGroup->AddItem(std::make_shared<ConditionalItem>("mpspecialability"_J, std::make_shared<ListCommandItem>("selspecialability"_J, "##specialselect")));
			gameplayGroup->AddItem(std::move(specialAbilityGroup));

			auto clearWantedGroup = std::make_shared<Group>("", 1);
			clearWantedGroup->AddItem(std::make_shared<ConditionalItem>("neverwanted"_J, std::make_shared<CommandItem>("clearwanted"_J), true));
			clearWantedGroup->AddItem(std::make_shared<BoolCommandItem>("neverwanted"_J));

			auto setWantedGroup = std::make_shared<Group>("", 1);
			setWantedGroup->AddItem(std::make_shared<IntCommandItem>("wantedslider"_J, "Level"));
			setWantedGroup->AddItem(std::make_shared<ConditionalItem>("freezewanted"_J, std::make_shared<CommandItem>("setwanted"_J), true));
			setWantedGroup->AddItem(std::make_shared<BoolCommandItem>("freezewanted"_J));

			gameplayGroup->AddItem(std::make_shared<ConditionalItem>("freezewanted"_J, std::move(clearWantedGroup), true));
			gameplayGroup->AddItem(std::make_shared<ConditionalItem>("neverwanted"_J, std::move(setWantedGroup), true));

			return gameplayGroup;
		}

		std::shared_ptr<Group> BuildMovementGroup()
		{
			// The first column contains simple toggles and the second contains features with settings that expand directly beneath their parent toggle.
			auto movementGroup = std::make_shared<Group>("Movement", 3);
			movementGroup->AddItem(std::make_shared<BoolCommandItem>("standonvehicles"_J));
			movementGroup->AddItem(std::make_shared<BoolCommandItem>("disableactionmode"_J));
			movementGroup->AddItem(std::make_shared<BoolCommandItem>("superjump"_J));

			auto superRunGroup = std::make_shared<Group>("", 1);
			superRunGroup->AddItem(std::make_shared<BoolCommandItem>("superrun"_J));
			superRunGroup->AddItem(std::make_shared<ConditionalItem>("superrun"_J, std::make_shared<FloatCommandItem>("moverateoverride"_J, std::nullopt, false)));
			superRunGroup->AddItem(std::make_shared<ConditionalItem>("superrun"_J, std::make_shared<FloatCommandItem>("runsprintswimmultiplier"_J, std::nullopt, false)));

			auto noClipGroup = std::make_shared<Group>("", 1);
			noClipGroup->AddItem(std::make_shared<BoolCommandItem>("noclip"_J));
			noClipGroup->AddItem(std::make_shared<ConditionalItem>("noclip"_J, std::make_shared<FloatCommandItem>("noclipspeed"_J)));

			auto freecamGroup = std::make_shared<Group>("", 1);
			freecamGroup->AddItem(std::make_shared<BoolCommandItem>("freecam"_J));
			freecamGroup->AddItem(std::make_shared<ConditionalItem>("freecam"_J, std::make_shared<FloatCommandItem>("freecamspeed"_J)));

			movementGroup->AddItem(std::move(superRunGroup));
			movementGroup->AddItem(std::move(noClipGroup));
			movementGroup->AddItem(std::move(freecamGroup));

			return movementGroup;
		}
	}

	Self::Self() :
#define ICON_FA_USER "\xef\x80\x87"
	    Submenu::Submenu("Self", ICON_FA_USER)
	{
		auto main = std::make_shared<Category>("Main");
		main->AddItem(BuildPlayerGroup());
		main->AddItem(BuildQuickActionsGroup());
		main->AddItem(BuildAbilitiesAndWantedGroup());
		main->AddItem(BuildMovementGroup());
		AddCategory(std::move(main));

		AddCategory(BuildWeaponsMenu());

		auto outfitEditorCategory = YimMenu::CreateOutfitsMenu();
		AddCategory(std::move(outfitEditorCategory));
	}
}