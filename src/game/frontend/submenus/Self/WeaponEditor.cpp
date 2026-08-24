#include "WeaponEditor.hpp"
#include "Weapons.hpp"

#include "core/backend/FiberPool.hpp"
#include "game/backend/Self.hpp"
#include "game/gta/data/WeaponComponents.hpp"
#include "game/gta/Natives.hpp"

namespace YimMenu::Submenus
{
	enum class WeaponComponentSlot
	{
		WeaponTint,
		Magazine,
		Scope,
		Muzzle,
		Barrel,
		Grip,
		Flashlight,
		Finish,
		SlideFinish,
		Other
	};

	struct WeaponComponentSlotData
	{
		WeaponComponentSlot Slot;
		std::string Name;
		std::vector<WeaponComponents::WeaponComponentData> Components;
	};

	static WeaponComponentSlot GetComponentSlot(const WeaponComponents::WeaponComponentData& component)
	{
		const auto name = component.Name;

		if (name.find("_CAMO") != std::string_view::npos && name.find("_SLIDE") != std::string_view::npos)
		{
			return WeaponComponentSlot::SlideFinish;
		}

		if (name.find("_CAMO") != std::string_view::npos || name.find("_VARMOD_") != std::string_view::npos)
		{
			return WeaponComponentSlot::Finish;
		}

		if (name.find("_CLIP_") != std::string_view::npos)
			return WeaponComponentSlot::Magazine;

		if (name.find("_SCOPE_") != std::string_view::npos || name.find("_SIGHT") != std::string_view::npos || name.find("_SIGHTS") != std::string_view::npos || name.find("_RAIL") != std::string_view::npos)
		{
			return WeaponComponentSlot::Scope;
		}

		if (name.find("_SUPP") != std::string_view::npos || name.find("_MUZZLE_") != std::string_view::npos || name.find("_PI_COMP") != std::string_view::npos)
		{
			return WeaponComponentSlot::Muzzle;
		}

		if (name.find("_BARREL_") != std::string_view::npos)
			return WeaponComponentSlot::Barrel;

		if (name.find("_AFGRIP") != std::string_view::npos)
			return WeaponComponentSlot::Grip;

		if (name.find("_FLSH") != std::string_view::npos)
			return WeaponComponentSlot::Flashlight;

		return WeaponComponentSlot::Other;
	}

	static std::string GetSlotName(WeaponComponentSlot slot)
	{
		switch (slot)
		{
		case WeaponComponentSlot::WeaponTint: return "Weapon Tint";
		case WeaponComponentSlot::Magazine: return "Magazine / Ammo";
		case WeaponComponentSlot::Scope: return "Optics";
		case WeaponComponentSlot::Muzzle: return "Muzzle";
		case WeaponComponentSlot::Barrel: return "Barrel";
		case WeaponComponentSlot::Grip: return "Grip";
		case WeaponComponentSlot::Flashlight: return "Flashlight";
		case WeaponComponentSlot::Finish: return "Finish / Camo";
		case WeaponComponentSlot::SlideFinish: return "Slide Camo";
		case WeaponComponentSlot::Other: return "Other";
		}

		return "Unknown";
	}

	static bool IsOptionalSlot(WeaponComponentSlot slot)
	{
		switch (slot)
		{
		case WeaponComponentSlot::WeaponTint:
		case WeaponComponentSlot::Magazine: return false;
		default: return true;
		}
	}

	static bool IsCamoComponent(std::string_view name)
	{
		return name.find("_CAMO") != std::string_view::npos;
	}

	static std::string GetComponentDisplayName(const WeaponComponents::WeaponComponentData& component)
	{
		const auto name = component.Name;

		if (name.find("_CLIP_TRACER") != std::string_view::npos)
			return "Tracer Rounds";
		if (name.find("_CLIP_INCENDIARY") != std::string_view::npos)
			return "Incendiary Rounds";
		if (name.find("_CLIP_HOLLOWPOINT") != std::string_view::npos)
			return "Hollow Point Rounds";
		if (name.find("_CLIP_ARMORPIERCING") != std::string_view::npos)
			return "Armor Piercing Rounds";
		if (name.find("_CLIP_FMJ") != std::string_view::npos)
			return "FMJ Rounds";
		if (name.find("_CLIP_EXPLOSIVE") != std::string_view::npos)
			return "Explosive Rounds";

		if (name.ends_with("_CLIP_01"))
			return "Default Magazine";
		if (name.ends_with("_CLIP_02"))
			return "Extended Magazine";
		if (name.ends_with("_CLIP_03"))
			return "Drum Magazine";

		if (name.find("_SUPP") != std::string_view::npos)
			return "Suppressor";
		if (name.find("_PI_COMP") != std::string_view::npos)
			return "Compensator";

		if (const auto pos = name.find("_MUZZLE_"); pos != std::string_view::npos)
		{
			return std::format("Muzzle Brake {}", name.substr(pos + 8));
		}

		if (name.ends_with("_BARREL_01"))
			return "Default Barrel";
		if (name.ends_with("_BARREL_02"))
			return "Heavy Barrel";

		if (name.find("_FLSH") != std::string_view::npos)
			return "Flashlight";
		if (name.find("_AFGRIP") != std::string_view::npos)
			return "Grip";

		if (name.find("_THERMAL") != std::string_view::npos)
			return "Thermal Scope";
		if (name.find("_SCOPE_NV") != std::string_view::npos)
			return "Night Vision Scope";
		if (name.find("_SCOPE_MAX") != std::string_view::npos)
			return "Advanced Scope";
		if (name.find("_LARGE_FIXED_ZOOM") != std::string_view::npos)
			return "Marksman Scope";
		if (name.find("_SCOPE_LARGE") != std::string_view::npos)
			return "Large Scope";
		if (name.find("_SCOPE_MEDIUM") != std::string_view::npos)
			return "Medium Scope";
		if (name.find("_SCOPE_SMALL") != std::string_view::npos)
			return "Small Scope";
		if (name.find("_SCOPE_MACRO") != std::string_view::npos)
			return "Compact Scope";

		if (name.find("_SIGHTS") != std::string_view::npos || name.find("_SIGHT_") != std::string_view::npos)
		{
			return "Holographic Sight";
		}

		if (name.find("_RAIL") != std::string_view::npos)
			return "Mounted Sight";

		if (name.find("_VARMOD_LUXE") != std::string_view::npos)
			return "Luxury Finish";

		if (name.find("_VARMOD_LOWRIDER") != std::string_view::npos || name.find("_VARMOD_LOW") != std::string_view::npos)
		{
			return "Lowrider Finish";
		}

		if (name.find("_VARMOD_BOSS") != std::string_view::npos)
			return "Boss Finish";
		if (name.find("_VARMOD_GOON") != std::string_view::npos)
			return "Goon Finish";

		if (name.find("_VARMOD_") != std::string_view::npos)
		{
			const auto pos = name.find("_VARMOD_");
			return std::string(name.substr(pos + 8));
		}

		if (name.find("_CAMO") != std::string_view::npos)
		{
			const bool slide = name.find("_SLIDE") != std::string_view::npos;

			if (name.find("_CAMO_IND_01") != std::string_view::npos)
				return slide ? "Camo 11 - Slide" : "Camo 11";

			const auto pos = name.find("_CAMO_");

			if (pos == std::string_view::npos)
				return slide ? "Camo 1 - Slide" : "Camo 1";

			const auto indexStart = pos + 6;

			if (indexStart + 2 <= name.size())
			{
				const auto index = name.substr(indexStart, 2);
				return std::format("Camo {}{}", index, slide ? " - Slide" : "");
			}
		}

		std::string result(component.Name);

		if (result.starts_with("COMPONENT_"))
			result.erase(0, 10);

		std::ranges::replace(result, '_', ' ');

		return result;
	}

	static std::vector<WeaponComponentSlotData> BuildComponentSlots(joaat_t weaponHash)
	{
		std::vector<WeaponComponentSlotData> result;

		const int tintCount = WEAPON::GET_WEAPON_TINT_COUNT(weaponHash);

		if (tintCount > 0)
		{
			result.push_back({WeaponComponentSlot::WeaponTint, "Weapon Tint", {}});
		}

		constexpr std::array slotOrder{WeaponComponentSlot::Magazine, WeaponComponentSlot::Scope, WeaponComponentSlot::Muzzle, WeaponComponentSlot::Barrel, WeaponComponentSlot::Grip, WeaponComponentSlot::Flashlight, WeaponComponentSlot::Finish, WeaponComponentSlot::SlideFinish, WeaponComponentSlot::Other};

		const auto components = WeaponComponents::GetComponentsForWeapon(weaponHash);

		for (const auto slot : slotOrder)
		{
			WeaponComponentSlotData slotData{slot, GetSlotName(slot), {}};

			for (const auto& component : components)
			{
				if (GetComponentSlot(component) == slot)
					slotData.Components.push_back(component);
			}

			if (!slotData.Components.empty())
				result.push_back(std::move(slotData));
		}

		return result;
	}

	static bool HasComponent(::Ped handle, joaat_t weaponHash, joaat_t componentHash)
	{
		return WEAPON::HAS_PED_GOT_WEAPON_COMPONENT(handle, weaponHash, componentHash);
	}

	static joaat_t GetInstalledComponent(::Ped handle, joaat_t weaponHash, const WeaponComponentSlotData& slot)
	{
		for (const auto& component : slot.Components)
		{
			if (HasComponent(handle, weaponHash, component.Hash))
				return component.Hash;
		}

		return 0;
	}

	static void SetExclusiveComponent(joaat_t weaponHash, WeaponComponentSlotData slot, joaat_t componentHash)
	{
		FiberPool::Push([weaponHash, slot = std::move(slot), componentHash] {
			auto ped = Self::GetPed();

			if (!ped)
				return;

			const auto handle = ped.GetHandle();

			for (const auto& candidate : slot.Components)
			{
				if (candidate.Hash == componentHash)
					continue;

				if (WEAPON::HAS_PED_GOT_WEAPON_COMPONENT(handle, weaponHash, candidate.Hash))
				{
					WEAPON::REMOVE_WEAPON_COMPONENT_FROM_PED(handle, weaponHash, candidate.Hash);
				}
			}

			if (componentHash != 0 && !WEAPON::HAS_PED_GOT_WEAPON_COMPONENT(handle, weaponHash, componentHash))
			{
				WEAPON::GIVE_WEAPON_COMPONENT_TO_PED(handle, weaponHash, componentHash);
			}
		});
	}

	static void SetComponentEnabled(joaat_t weaponHash, joaat_t componentHash, bool enabled)
	{
		FiberPool::Push([weaponHash, componentHash, enabled] {
			auto ped = Self::GetPed();

			if (!ped)
				return;

			const auto handle = ped.GetHandle();

			if (enabled)
			{
				WEAPON::GIVE_WEAPON_COMPONENT_TO_PED(handle, weaponHash, componentHash);
			}
			else
			{
				WEAPON::REMOVE_WEAPON_COMPONENT_FROM_PED(handle, weaponHash, componentHash);
			}
		});
	}

	void RenderWeaponEditorMenu()
	{
		static joaat_t currentWeapon = 0;
		static int selectedSlot = -1;

		static joaat_t tintComponent = 0;
		static int componentTint = 0;

		static std::vector<WeaponComponentSlotData> componentSlots;

		auto ped = Self::GetPed();

		if (!ped)
		{
			ImGui::TextUnformatted("Unable to find the player.");

			currentWeapon = 0;
			selectedSlot = -1;
			componentSlots.clear();

			return;
		}

		const auto handle = ped.GetHandle();
		const joaat_t selectedWeapon = WEAPON::GET_SELECTED_PED_WEAPON(handle);

		if (selectedWeapon == "WEAPON_UNARMED"_J)
		{
			ImGui::TextUnformatted("Equip a weapon to edit it.");

			currentWeapon = 0;
			selectedSlot = -1;
			componentSlots.clear();

			return;
		}

		if (currentWeapon != selectedWeapon)
		{
			currentWeapon = selectedWeapon;
			componentSlots = BuildComponentSlots(currentWeapon);
			selectedSlot = componentSlots.empty() ? -1 : 0;

			tintComponent = 0;
			componentTint = 0;
		}

		const std::string weaponName = GetWeaponDisplayName(currentWeapon);

		ImGui::TextUnformatted(weaponName.c_str());

		if (ImGui::IsItemHovered())
		{
			ImGui::BeginTooltip();

			ImGui::Text("Weapon Hash: 0x%08X", currentWeapon);

			if (const auto weapon = FindWeaponDisplay(currentWeapon))
			{
				if (!weapon->desc.empty())
				{
					ImGui::Spacing();
					ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
					ImGui::TextUnformatted(weapon->desc.c_str());
					ImGui::PopTextWrapPos();
				}
			}

			ImGui::EndTooltip();
		}

		ImGui::SameLine();

		if (ImGui::Button("Refresh##weapon_editor"))
		{
			currentWeapon = 0;
			return;
		}

		ImGui::SeparatorText("Components");

		if (componentSlots.empty())
		{
			ImGui::TextUnformatted("This weapon has no mapped components or tints.");
			return;
		}

		ImGui::BeginGroup();
		ImGui::TextUnformatted("Slot");

		if (ImGui::BeginListBox("##weapon_component_slot", ImVec2(180.0f, 240.0f)))
		{
			for (int i = 0; i < static_cast<int>(componentSlots.size()); ++i)
			{
				if (ImGui::Selectable(componentSlots[i].Name.c_str(), selectedSlot == i))
				{
					selectedSlot = i;
					tintComponent = 0;
					componentTint = 0;
				}
			}

			ImGui::EndListBox();
		}

		ImGui::EndGroup();

		if (selectedSlot < 0 || selectedSlot >= static_cast<int>(componentSlots.size()))
		{
			return;
		}

		auto& slot = componentSlots[selectedSlot];

		ImGui::SameLine();
		ImGui::BeginGroup();
		ImGui::TextUnformatted("Option");

		if (slot.Slot == WeaponComponentSlot::WeaponTint)
		{
			const int tintCount = WEAPON::GET_WEAPON_TINT_COUNT(currentWeapon);

			const int currentTint = WEAPON::GET_PED_WEAPON_TINT_INDEX(handle, currentWeapon);

			if (ImGui::BeginListBox("##weapon_component_option", ImVec2(260.0f, 240.0f)))
			{
				for (int tint = 0; tint < tintCount; ++tint)
				{
					const std::string tintName = std::format("Tint {}", tint);

					if (ImGui::Selectable(tintName.c_str(), currentTint == tint))
					{
						const auto weaponHash = currentWeapon;

						FiberPool::Push([weaponHash, tint] {
							if (auto ped = Self::GetPed())
							{
								WEAPON::SET_PED_WEAPON_TINT_INDEX(ped.GetHandle(), weaponHash, tint);
							}
						});
					}
				}

				ImGui::EndListBox();
			}
		}
		else
		{
			const joaat_t installedComponent = GetInstalledComponent(handle, currentWeapon, slot);

			if (ImGui::BeginListBox("##weapon_component_option", ImVec2(260.0f, 240.0f)))
			{
				if (IsOptionalSlot(slot.Slot) && slot.Slot != WeaponComponentSlot::Other)
				{
					if (ImGui::Selectable("None", installedComponent == 0))
					{
						SetExclusiveComponent(currentWeapon, slot, 0);

						tintComponent = 0;
						componentTint = 0;
					}
				}

				for (const auto& component : slot.Components)
				{
					const bool installed = HasComponent(handle, currentWeapon, component.Hash);

					const auto displayName = GetComponentDisplayName(component);

					if (slot.Slot == WeaponComponentSlot::Other)
					{
						bool enabled = installed;

						if (ImGui::Checkbox(displayName.c_str(), &enabled))
						{
							SetComponentEnabled(currentWeapon, component.Hash, enabled);
						}
					}
					else
					{
						if (ImGui::Selectable(displayName.c_str(), installed))
						{
							SetExclusiveComponent(currentWeapon, slot, component.Hash);

							tintComponent = 0;
							componentTint = 0;
						}
					}

					if (ImGui::IsItemHovered())
					{
						ImGui::BeginTooltip();

						ImGui::TextUnformatted(component.Name.data(), component.Name.data() + component.Name.size());

						ImGui::Text("Hash: 0x%08X", component.Hash);

						ImGui::EndTooltip();
					}
				}

				ImGui::EndListBox();
			}
		}

		ImGui::EndGroup();

		if (slot.Slot != WeaponComponentSlot::WeaponTint)
		{
			joaat_t installedTintableComponent = 0;

			for (const auto& component : slot.Components)
			{
				if (!IsCamoComponent(component.Name))
					continue;

				if (HasComponent(handle, currentWeapon, component.Hash))
				{
					installedTintableComponent = component.Hash;
					break;
				}
			}

			if (installedTintableComponent != 0)
			{
				if (tintComponent != installedTintableComponent)
				{
					tintComponent = installedTintableComponent;

					componentTint = WEAPON::GET_PED_WEAPON_COMPONENT_TINT_INDEX(handle, currentWeapon, tintComponent);

					componentTint = std::max(0, componentTint);
				}

				ImGui::Spacing();
				ImGui::TextUnformatted("Camo Tint");
				ImGui::SetNextItemWidth(250.0f);

				int newTint = componentTint;

				if (ImGui::SliderInt("##weapon_component_tint", &newTint, 0, 31, "Tint %d"))
				{
					componentTint = newTint;

					const auto weaponHash = currentWeapon;
					const auto componentHash = tintComponent;
					const int tint = componentTint;

					FiberPool::Push([weaponHash, componentHash, tint] {
						if (auto ped = Self::GetPed())
						{
							WEAPON::SET_PED_WEAPON_COMPONENT_TINT_INDEX(ped.GetHandle(), weaponHash, componentHash, tint);
						}
					});
				}
			}
		}

		ImGui::Spacing();

		if (ImGui::Button("Reset Weapon Mods"))
		{
			const auto weaponHash = currentWeapon;

			const auto components = WeaponComponents::GetComponentsForWeapon(weaponHash);

			std::vector<joaat_t> componentHashes;
			componentHashes.reserve(components.size());

			for (const auto& component : components)
				componentHashes.push_back(component.Hash);

			FiberPool::Push([weaponHash, componentHashes = std::move(componentHashes)] {
				auto ped = Self::GetPed();

				if (!ped)
					return;

				const auto handle = ped.GetHandle();

				for (const auto componentHash : componentHashes)
				{
					if (WEAPON::HAS_PED_GOT_WEAPON_COMPONENT(handle, weaponHash, componentHash))
					{
						WEAPON::REMOVE_WEAPON_COMPONENT_FROM_PED(handle, weaponHash, componentHash);
					}
				}

				WEAPON::SET_PED_WEAPON_TINT_INDEX(handle, weaponHash, 0);
			});

			currentWeapon = 0;
		}
	}
}