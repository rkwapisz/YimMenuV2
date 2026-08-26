#include "World.hpp"
#include "game/frontend/items/Items.hpp"
#include "World/SpawnPed.hpp"

namespace YimMenu::Submenus
{
	World::World() :
#define ICON_FA_GLOBE "\xef\x82\xac"
	    Submenu::Submenu("World", ICON_FA_GLOBE)
	{
		auto main = std::make_shared<Category>("Main");
		auto iplsGroup = std::make_shared<Category>("IPLs");

		auto killPeds = std::make_shared<Group>("Kill", 1);
		killPeds->AddItem(std::make_shared<CommandItem>("killallpeds"_J));
		killPeds->AddItem(std::make_shared<CommandItem>("killallenemies"_J));
		auto deleteOpts = std::make_shared<Group>("Delete", 2);
		deleteOpts->AddItem(std::make_shared<CommandItem>("delpeds"_J));
		deleteOpts->AddItem(std::make_shared<CommandItem>("delvehs"_J));
		deleteOpts->AddItem(std::make_shared<CommandItem>("delobjs"_J));
		deleteOpts->AddItem(std::make_shared<CommandItem>("delcams"_J));
		auto bringOpts = std::make_shared<Group>("Bring", 1);
		bringOpts->AddItem(std::make_shared<CommandItem>("bringpeds"_J));
		bringOpts->AddItem(std::make_shared<CommandItem>("bringvehs"_J));
		bringOpts->AddItem(std::make_shared<CommandItem>("bringobjs"_J));

		auto weatherOpts = std::make_shared<Group>("Weather", 1);
		weatherOpts->AddItem(std::make_shared<ListCommandItem>("weather"_J));
		weatherOpts->AddItem(std::make_shared<ConditionalItem>("forceweather"_J, std::make_shared<CommandItem>("setweather"_J), true));
		weatherOpts->AddItem(std::make_shared<BoolCommandItem>("forceweather"_J));

		auto timeGroup = std::make_shared<Group>("Time");

		// Passing false disables the slider and uses a numeric input instead.
		auto timeControls = std::make_shared<Group>("", 1);
		timeControls->AddItem(std::make_shared<IntCommandItem>("networktimehour"_J, "Hour", false));
		timeControls->AddItem(std::make_shared<IntCommandItem>("networktimeminute"_J, "Minute", false));
		timeControls->AddItem(std::make_shared<CommandItem>("setnetworktime"_J, "Apply"));
		timeControls->AddItem(std::make_shared<BoolCommandItem>("freezenetworktime"_J, "Freeze"));

		timeGroup->AddItem(std::move(timeControls));

		auto timePresets = std::make_shared<Group>("Presets", 1);
		timePresets->AddItem(std::make_shared<CommandItem>("networktimedawn"_J));
		timePresets->AddItem(std::make_shared<CommandItem>("networktimenoon"_J));
		timePresets->AddItem(std::make_shared<CommandItem>("networktimesunset"_J));
		timePresets->AddItem(std::make_shared<CommandItem>("networktimemidnight"_J));

		timeGroup->AddItem(std::move(timePresets));

		auto otherOpts = std::make_shared<Group>("Other", 1);
		otherOpts->AddItem(std::make_shared<BoolCommandItem>("pedsignore"_J));
		otherOpts->AddItem(std::make_shared<BoolCommandItem>("PedRiotMode"_J));
		otherOpts->AddItem(std::make_shared<BoolCommandItem>("CopsDispatch"_J));
		otherOpts->AddItem(std::make_shared<BoolCommandItem>("enablecreatordevmode"_J));
		otherOpts->AddItem(std::make_shared<BoolCommandItem>("infiniteboundary"_J));

		main->AddItem(std::move(killPeds));
		main->AddItem(std::move(deleteOpts));
		main->AddItem(std::move(bringOpts));
		main->AddItem(std::move(weatherOpts));
		main->AddItem(std::move(otherOpts));
		main->AddItem(std::move(timeGroup));

		iplsGroup->AddItem(std::make_shared<ListCommandItem>("iplselector"_J));
		iplsGroup->AddItem(std::make_shared<CommandItem>("loadipl"_J));
		iplsGroup->AddItem(std::make_shared<CommandItem>("unloadipl"_J));
		iplsGroup->AddItem(std::make_shared<CommandItem>("ipltp"_J));

		AddCategory(std::move(main));
		AddCategory(std::move(BuildSpawnPedMenu()));
		AddCategory(std::move(iplsGroup));
	}
};