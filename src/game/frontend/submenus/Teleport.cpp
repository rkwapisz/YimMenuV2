#include "Teleport.hpp"

#include "core/frontend/Notifications.hpp"
#include "core/backend/FiberPool.hpp"
#include "game/backend/SavedLocations.hpp"
#include "game/backend/Self.hpp"
#include "game/frontend/items/Items.hpp"
#include "game/backend/TeleportService.hpp"

namespace YimMenu::Submenus
{
	static float GetDistanceFromLocation(const SavedLocation& t)
	{
		return rage::fvector3(t.x, t.y, t.z).GetDistance(Self::GetPed().GetPosition());
	}

	void RenderCustomTeleport()
	{
		ImGui::BeginGroup();
		static std::string newLocationName{};
		static std::string category = "Default";
		static SavedLocation locationToDelete;

		if (!std::string(locationToDelete.name).empty())
			ImGui::OpenPopup("##deletelocation");

		if (ImGui::BeginPopupModal("##deletelocation", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove))
		{
			ImGui::Text("Are you sure you want to delete %s?", locationToDelete.name.data());

			ImGui::Spacing();

			if (ImGui::Button("Yes"))
			{
				SavedLocations::DeleteSavedLocation(category, locationToDelete.name);
				locationToDelete.name = "";
				ImGui::CloseCurrentPopup();
			}
			ImGui::SameLine();
			if (ImGui::Button("No"))
			{
				locationToDelete.name = "";
				ImGui::CloseCurrentPopup();
			}

			ImGui::EndPopup();
		}

		ImGui::PushItemWidth(300);
		InputTextWithHint("Category", "Category", &category).Draw();

		ImGui::PushItemWidth(200);
		InputTextWithHint("Location Name", "New location", &newLocationName).Draw();
		ImGui::PopItemWidth();

		if (ImGui::Button("Save Current Location"))
		{
			FiberPool::Push([=] {
				if (newLocationName.empty())
				{
					Notifications::Show("Custom Teleport", "Please enter a valid name", NotificationType::Warning);
				}
				else if (SavedLocations::GetSavedLocationByName(newLocationName))
				{
					Notifications::Show("Custom Teleport", std::format("Location with name {} already exists", newLocationName));
				}
				else
				{
					SavedLocation teleportLocation;
					Entity teleportEntity = Self::GetPed();
					if (auto vehicle = Self::GetVehicle())
						teleportEntity = vehicle;

					auto coords = teleportEntity.GetPosition();
					teleportLocation.name = newLocationName;
					teleportLocation.x = coords.x;
					teleportLocation.y = coords.y;
					teleportLocation.z = coords.z;
					teleportLocation.yaw = teleportEntity.GetHeading();
					teleportLocation.pitch = 0.0f; // why do we need pitch and roll anyway?
					teleportLocation.roll = 0.0f;
					SavedLocations::SaveNewLocation(category, teleportLocation);
				}
			});
		};


		ImGui::Separator();

		ImGui::Text("Double click to teleport\nShift click to delete");

		ImGui::Spacing();

		static std::string filter{};
		InputTextWithHint("##filter", "Search", &filter).Draw();

		ImGui::BeginGroup();
		ImGui::Text("Categories");
		if (ImGui::BeginListBox("##categories", {200, -1}))
		{
			for (auto& l : SavedLocations::GetAllSavedLocations() | std::ranges::views::keys)
			{
				if (ImGui::Selectable(l.data(), l == category))
				{
					category = l;
				}

				if (category.empty())
				{
					category = l;
				}
			}
			ImGui::EndListBox();
		}
		ImGui::EndGroup();
		ImGui::SameLine();
		ImGui::BeginGroup();
		ImGui::Text("Locations");
		if (ImGui::BeginListBox("##saved_locs", {200, -1})) // Need automatic dimensions instead of hard coded
		{
			if (SavedLocations::GetAllSavedLocations().find(category) != SavedLocations::GetAllSavedLocations().end())
			{
				std::vector<SavedLocation> current_list{};

				if (!filter.empty())
					current_list = SavedLocations::SavedLocationsFilteredList(filter);
				else
					current_list = SavedLocations::GetAllSavedLocations().at(category);

				for (const auto& l : current_list)
				{
					if (ImGui::Selectable(l.name.data(), false, ImGuiSelectableFlags_AllowDoubleClick))
					{
						if (GetAsyncKeyState(VK_SHIFT) & 0x8000)
						{
							locationToDelete = l;
						}
						else
						{
							if (ImGui::IsMouseDoubleClicked(0))
							{
								FiberPool::Push([l] {
									rage::fvector3 position = {l.x, l.y, l.z};
									TeleportService::TeleportTo(position, l.yaw, false);
								});
							}
						}
					}

					if (ImGui::IsItemHovered())
					{
						ImGui::BeginTooltip();
						if (l.name.length() > 27)
							ImGui::Text("%s", l.name.data());
						ImGui::Text("Distance: %f", GetDistanceFromLocation(l));
						ImGui::EndTooltip();
					}
				}
			}

			ImGui::EndListBox();
		}

		ImGui::EndGroup();

		ImGui::EndGroup();
	}

	void RenderDirectionalTp()
	{
		FloatCommandItem("directionaltpdistance"_J).Draw();

		ImGui::BeginGroup();
		CommandItem("directionaltpforward"_J).Draw();
		CommandItem("directionaltpbackward"_J).Draw();
		ImGui::EndGroup();

		ImGui::SameLine();

		ImGui::BeginGroup();
		CommandItem("directionaltpright"_J).Draw();
		CommandItem("directionaltpleft"_J).Draw();
		ImGui::EndGroup();

		ImGui::SameLine();

		ImGui::BeginGroup();
		CommandItem("directionaltpup"_J).Draw();
		CommandItem("directionaltpdown"_J).Draw();
		ImGui::EndGroup();
	}

	void RenderPreviousLocations()
	{
		const auto history = TeleportService::GetHistory();

		if (history.empty())
		{
			ImGui::TextDisabled("No previous locations.");
			return;
		}

		// Keep the panel compact and scroll when the history grows.
		if (ImGui::BeginChild("##previous_teleport_locations", ImVec2(0.0f, 220.0f), true))
		{
			// History is stored oldest -> newest, so render it in reverse.
			for (std::size_t i = history.size(); i-- > 0;)
			{
				const auto& location = history[i];

				const auto label = std::format(
				    "{:.2f}, {:.2f}, {:.2f}##previous_location_{}",
				    location.Position.x,
				    location.Position.y,
				    location.Position.z,
				    i);

				if (ImGui::Selectable(label.c_str()))
				{
					// Navigating history should not itself create another
					// history entry.
					FiberPool::Push([location] {
						TeleportService::TeleportTo(
						    location.Position,
						    location.Heading,
						    false);
					});
				}

				if (ImGui::IsItemHovered())
				{
					ImGui::BeginTooltip();
					ImGui::Text(
					    "X: %.3f\nY: %.3f\nZ: %.3f\nHeading: %.1f",
					    location.Position.x,
					    location.Position.y,
					    location.Position.z,
					    location.Heading);
					ImGui::EndTooltip();
				}
			}
		}

		ImGui::EndChild();
	}

	Teleport::Teleport() :
#define ICON_FA_TELEPORT "\xef\x8f\x85"
	    Submenu::Submenu("Teleport", ICON_FA_TELEPORT)
	{
		auto main = std::make_shared<Category>("Main");

		auto quickTeleport = std::make_shared<Group>("Quick Teleport", 8);

		quickTeleport->AddItem(std::make_shared<ConditionalItem>("autotptowaypoint"_J, std::make_shared<CommandItem>("tptowaypoint"_J), true));
		quickTeleport->AddItem(std::make_shared<BoolCommandItem>("autotptowaypoint"_J));
		quickTeleport->AddItem(std::make_shared<CommandItem>("tptoobjective"_J));
		quickTeleport->AddItem(std::make_shared<CommandItem>("tpback"_J));
		quickTeleport->AddItem(std::make_shared<CommandItem>("tptonearestroad"_J));
		quickTeleport->AddItem(std::make_shared<CommandItem>("tptocrosshair"_J));
		quickTeleport->AddItem(std::make_shared<CommandItem>("tptopersonalvehicle"_J));
		quickTeleport->AddItem(std::make_shared<CommandItem>("tptolastvehicle"_J));

		auto previousLocations = std::make_shared<Group>("Previous Locations");
		previousLocations->AddItem(std::make_shared<ImGuiItem>([] {
			RenderPreviousLocations();
		}));

		// Quick Teleport on the left, Previous Locations on the right.
		main->AddItem(std::make_shared<ImGuiItem>([quickTeleport, previousLocations] {
			if (ImGui::BeginTable("##quick_teleport_history_layout", 2, ImGuiTableFlags_SizingStretchProp))
			{
				ImGui::TableSetupColumn("##quick_teleport_column", ImGuiTableColumnFlags_WidthStretch, 0.55f);
				ImGui::TableSetupColumn("##previous_locations_column", ImGuiTableColumnFlags_WidthStretch, 0.45f);

				ImGui::TableNextRow();

				ImGui::TableSetColumnIndex(0);
				quickTeleport->Draw();

				ImGui::TableSetColumnIndex(1);
				previousLocations->Draw();

				ImGui::EndTable();
			}
		}));

		auto coordinates = std::make_shared<Group>("Coordinates");

		coordinates->AddItem(std::make_shared<ImGuiItem>([] {
			ImGui::BeginGroup();

			FloatCommandItem("tpcoordx"_J, "X", false).Draw();
			FloatCommandItem("tpcoordy"_J, "Y", false).Draw();
			FloatCommandItem("tpcoordz"_J, "Z", false).Draw();

			CommandItem("usecurrenttpcoords"_J).Draw();
			CommandItem("tptocoordinates"_J).Draw();

			ImGui::EndGroup();
		}));

		main->AddItem(coordinates);

		auto directional = std::make_shared<Group>("Directional");

		directional->AddItem(std::make_shared<ImGuiItem>([] {
			RenderDirectionalTp();
		}));

		main->AddItem(directional);

		auto customteleport = std::make_shared<Category>("Saved");
		customteleport->AddItem(std::make_shared<ImGuiItem>([] {
			RenderCustomTeleport();
		}));

		AddCategory(std::move(main));
		AddCategory(std::move(customteleport));
	}

}