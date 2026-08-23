#include "Vehicle.hpp"
#include "game/frontend/items/Items.hpp"
#include "game/frontend/submenus/Vehicle/SpawnVehicle.hpp"
#include "Vehicle/VehicleEditor.hpp"
#include "Vehicle/SavedVehicles.hpp"

namespace YimMenu::Submenus
{
	Vehicle::Vehicle() :
		#define ICON_FA_CAR "\xef\x86\xb9"
	    Submenu::Submenu("Vehicle", ICON_FA_CAR)
	{
		auto main = std::make_shared<Category>("Main");

		auto vehicle_health = std::make_shared<Group>("Vehicle Health");
		auto allvehicles = std::make_shared<Group>("All Vehicles");
		auto personal_vehicle = std::make_shared<Group>("Personal Vehicle");
		auto recovery = std::make_shared<Group>("Recovery");
		auto misc = std::make_shared<Group>("Misc", 4);

		vehicle_health->AddItem(std::make_shared<CommandItem>("repairvehicle"_J));
		vehicle_health->AddItem(std::make_shared<BoolCommandItem>("vehiclegodmode"_J, "God Mode"));
		vehicle_health->AddItem(std::make_shared<BoolCommandItem>("keeprepaired"_J, "Keep Repaired"));

		personal_vehicle->AddItem(std::make_shared<CommandItem>("requestpv"_J));
		personal_vehicle->AddItem(std::make_shared<CommandItem>("despawnpv"_J));
		personal_vehicle->AddItem(std::make_shared<CommandItem>("savepersonalvehicle"_J));

		recovery->AddItem(std::make_shared<CommandItem>("enterlastvehicle"_J));
		recovery->AddItem(std::make_shared<CommandItem>("callmechanic"_J));
		recovery->AddItem(std::make_shared<CommandItem>("recoverallvehicles"_J));

		misc->AddItem(std::make_shared<BoolCommandItem>("hornboost"_J));
		misc->AddItem(std::make_shared<BoolCommandItem>("modifyboostbehavior"_J));
		misc->AddItem(std::make_shared<ConditionalItem>("modifyboostbehavior"_J, std::make_shared<ListCommandItem>("boostbehavior"_J)));
		misc->AddItem(std::make_shared<BoolCommandItem>("speedometer"_J));
		misc->AddItem(std::make_shared<BoolCommandItem>("seatbelt"_J));
		misc->AddItem(std::make_shared<BoolCommandItem>("lowervehiclestance"_J, "Lower Stance"));
		misc->AddItem(std::make_shared<BoolCommandItem>("allowhatsinvehicles"_J));
		misc->AddItem(std::make_shared<BoolCommandItem>("lsccustomsbypass"_J));
		misc->AddItem(std::make_shared<BoolCommandItem>("dlcvehicles"_J));

		main->AddItem(vehicle_health);
		main->AddItem(personal_vehicle);
		main->AddItem(recovery);
		main->AddItem(misc);

		AddCategory(std::move(main));
		AddCategory(BuildSpawnVehicleMenu());
		AddCategory(BuildVehicleEditorMenu());
		AddCategory(BuildSavedVehiclesMenu());
	}
}