#pragma once

#include "core/util/Joaat.hpp"

#include <span>
#include <string_view>

namespace YimMenu::WeaponComponents
{
	struct WeaponComponentData
	{
		std::string_view Name;
		joaat_t Hash;
	};

	constexpr WeaponComponentData MakeComponent(std::string_view name)
	{
		return {name, Joaat(name)};
	}

	// Static per-weapon component mappings ported from Enhanced Native Trainer:
	// FIying-Scotsman/GTAV-EnhancedNativeTrainer
	// commit 1d3bbcf3d771462c2867c7fc57fcaa6d7883b8cb

	inline constexpr WeaponComponentData kPistolComponents[] = {
	    MakeComponent("COMPONENT_PISTOL_CLIP_01"),
	    MakeComponent("COMPONENT_PISTOL_CLIP_02"),
	    MakeComponent("COMPONENT_AT_PI_FLSH"),
	    MakeComponent("COMPONENT_AT_PI_SUPP_02"),
	    MakeComponent("COMPONENT_PISTOL_VARMOD_LUXE"),
	};

	inline constexpr WeaponComponentData kHeavypistolComponents[] = {
	    MakeComponent("COMPONENT_HEAVYPISTOL_CLIP_01"),
	    MakeComponent("COMPONENT_HEAVYPISTOL_CLIP_02"),
	    MakeComponent("COMPONENT_AT_PI_FLSH"),
	    MakeComponent("COMPONENT_AT_PI_SUPP"),
	    MakeComponent("COMPONENT_HEAVYPISTOL_VARMOD_LUXE"),
	};

	inline constexpr WeaponComponentData kCombatpistolComponents[] = {
	    MakeComponent("COMPONENT_COMBATPISTOL_CLIP_01"),
	    MakeComponent("COMPONENT_COMBATPISTOL_CLIP_02"),
	    MakeComponent("COMPONENT_AT_PI_FLSH"),
	    MakeComponent("COMPONENT_AT_PI_SUPP"),
	    MakeComponent("COMPONENT_COMBATPISTOL_VARMOD_LOWRIDER"),
	};

	inline constexpr WeaponComponentData kAppistolComponents[] = {
	    MakeComponent("COMPONENT_APPISTOL_CLIP_01"),
	    MakeComponent("COMPONENT_APPISTOL_CLIP_02"),
	    MakeComponent("COMPONENT_AT_PI_FLSH"),
	    MakeComponent("COMPONENT_AT_PI_SUPP"),
	    MakeComponent("COMPONENT_APPISTOL_VARMOD_LUXE"),
	};

	inline constexpr WeaponComponentData kPistol50Components[] = {
	    MakeComponent("COMPONENT_PISTOL50_CLIP_01"),
	    MakeComponent("COMPONENT_PISTOL50_CLIP_02"),
	    MakeComponent("COMPONENT_AT_PI_FLSH"),
	    MakeComponent("COMPONENT_AT_AR_SUPP_02"),
	    MakeComponent("COMPONENT_PISTOL50_VARMOD_LUXE"),
	};

	inline constexpr WeaponComponentData kMicrosmgComponents[] = {
	    MakeComponent("COMPONENT_MICROSMG_CLIP_01"),
	    MakeComponent("COMPONENT_MICROSMG_CLIP_02"),
	    MakeComponent("COMPONENT_AT_PI_FLSH"),
	    MakeComponent("COMPONENT_AT_SCOPE_MACRO"),
	    MakeComponent("COMPONENT_AT_AR_SUPP_02"),
	    MakeComponent("COMPONENT_MICROSMG_VARMOD_LUXE"),
	};

	inline constexpr WeaponComponentData kSmgComponents[] = {
	    MakeComponent("COMPONENT_SMG_CLIP_01"),
	    MakeComponent("COMPONENT_SMG_CLIP_02"),
	    MakeComponent("COMPONENT_SMG_CLIP_03"),
	    MakeComponent("COMPONENT_AT_AR_FLSH"),
	    MakeComponent("COMPONENT_AT_SCOPE_MACRO_02"),
	    MakeComponent("COMPONENT_AT_PI_SUPP"),
	    MakeComponent("COMPONENT_SMG_VARMOD_LUXE"),
	};

	inline constexpr WeaponComponentData kAssaultsmgComponents[] = {
	    MakeComponent("COMPONENT_ASSAULTSMG_CLIP_01"),
	    MakeComponent("COMPONENT_ASSAULTSMG_CLIP_02"),
	    MakeComponent("COMPONENT_AT_AR_FLSH"),
	    MakeComponent("COMPONENT_AT_SCOPE_MACRO"),
	    MakeComponent("COMPONENT_AT_AR_SUPP_02"),
	    MakeComponent("COMPONENT_ASSAULTSMG_VARMOD_LOWRIDER"),
	};

	inline constexpr WeaponComponentData kAssaultrifleComponents[] = {
	    MakeComponent("COMPONENT_ASSAULTRIFLE_CLIP_01"),
	    MakeComponent("COMPONENT_ASSAULTRIFLE_CLIP_02"),
	    MakeComponent("COMPONENT_ASSAULTRIFLE_CLIP_03"),
	    MakeComponent("COMPONENT_AT_AR_FLSH"),
	    MakeComponent("COMPONENT_AT_SCOPE_MACRO"),
	    MakeComponent("COMPONENT_AT_AR_SUPP_02"),
	    MakeComponent("COMPONENT_AT_AR_AFGRIP"),
	    MakeComponent("COMPONENT_ASSAULTRIFLE_VARMOD_LUXE"),
	};

	inline constexpr WeaponComponentData kCarbinerifleComponents[] = {
	    MakeComponent("COMPONENT_CARBINERIFLE_CLIP_01"),
	    MakeComponent("COMPONENT_CARBINERIFLE_CLIP_02"),
	    MakeComponent("COMPONENT_CARBINERIFLE_CLIP_03"),
	    MakeComponent("COMPONENT_AT_AR_FLSH"),
	    MakeComponent("COMPONENT_AT_SCOPE_MEDIUM"),
	    MakeComponent("COMPONENT_AT_AR_SUPP"),
	    MakeComponent("COMPONENT_AT_AR_AFGRIP"),
	    MakeComponent("COMPONENT_CARBINERIFLE_VARMOD_LUXE"),
	};

	inline constexpr WeaponComponentData kAdvancedrifleComponents[] = {
	    MakeComponent("COMPONENT_ADVANCEDRIFLE_CLIP_01"),
	    MakeComponent("COMPONENT_ADVANCEDRIFLE_CLIP_02"),
	    MakeComponent("COMPONENT_AT_AR_FLSH"),
	    MakeComponent("COMPONENT_AT_SCOPE_SMALL"),
	    MakeComponent("COMPONENT_AT_AR_SUPP"),
	    MakeComponent("COMPONENT_ADVANCEDRIFLE_VARMOD_LUXE"),
	};

	inline constexpr WeaponComponentData kMgComponents[] = {
	    MakeComponent("COMPONENT_MG_CLIP_01"),
	    MakeComponent("COMPONENT_MG_CLIP_02"),
	    MakeComponent("COMPONENT_AT_SCOPE_SMALL_02"),
	    MakeComponent("COMPONENT_MG_VARMOD_LOWRIDER"),
	};

	inline constexpr WeaponComponentData kCombatmgComponents[] = {
	    MakeComponent("COMPONENT_COMBATMG_CLIP_01"),
	    MakeComponent("COMPONENT_COMBATMG_CLIP_02"),
	    MakeComponent("COMPONENT_AT_SCOPE_MEDIUM"),
	    MakeComponent("COMPONENT_AT_AR_AFGRIP"),
	    MakeComponent("COMPONENT_COMBATMG_VARMOD_LOWRIDER"),
	};

	inline constexpr WeaponComponentData kSawnoffshotgunComponents[] = {
	    MakeComponent("COMPONENT_SAWNOFFSHOTGUN_VARMOD_LUXE"),
	};

	inline constexpr WeaponComponentData kPumpshotgunComponents[] = {
	    MakeComponent("COMPONENT_AT_AR_FLSH"),
	    MakeComponent("COMPONENT_AT_SR_SUPP"),
	    MakeComponent("COMPONENT_PUMPSHOTGUN_VARMOD_LOWRIDER"),
	};

	inline constexpr WeaponComponentData kAssaultshotgunComponents[] = {
	    MakeComponent("COMPONENT_ASSAULTSHOTGUN_CLIP_01"),
	    MakeComponent("COMPONENT_ASSAULTSHOTGUN_CLIP_02"),
	    MakeComponent("COMPONENT_AT_AR_FLSH"),
	    MakeComponent("COMPONENT_AT_AR_SUPP"),
	    MakeComponent("COMPONENT_AT_AR_AFGRIP"),
	};

	inline constexpr WeaponComponentData kBullpupshotgunComponents[] = {
	    MakeComponent("COMPONENT_AT_AR_FLSH"),
	    MakeComponent("COMPONENT_AT_AR_SUPP_02"),
	    MakeComponent("COMPONENT_AT_AR_AFGRIP"),
	};

	inline constexpr WeaponComponentData kSniperrifleComponents[] = {
	    MakeComponent("COMPONENT_SNIPERRIFLE_CLIP_01"),
	    MakeComponent("COMPONENT_AT_AR_SUPP_02"),
	    MakeComponent("COMPONENT_AT_SCOPE_LARGE"),
	    MakeComponent("COMPONENT_AT_SCOPE_MAX"),
	    MakeComponent("COMPONENT_SNIPERRIFLE_VARMOD_LUXE"),
	};

	inline constexpr WeaponComponentData kHeavysniperComponents[] = {
	    MakeComponent("COMPONENT_AT_SCOPE_LARGE"),
	    MakeComponent("COMPONENT_AT_SCOPE_MAX"),
	};

	inline constexpr WeaponComponentData kGrenadelauncherComponents[] = {
	    MakeComponent("COMPONENT_GRENADELAUNCHER_CLIP_01"),
	    MakeComponent("COMPONENT_AT_AR_FLSH"),
	    MakeComponent("COMPONENT_AT_AR_AFGRIP"),
	    MakeComponent("COMPONENT_AT_SCOPE_SMALL"),
	};

	inline constexpr WeaponComponentData kBullpuprifleComponents[] = {
	    MakeComponent("COMPONENT_BULLPUPRIFLE_CLIP_01"),
	    MakeComponent("COMPONENT_BULLPUPRIFLE_CLIP_02"),
	    MakeComponent("COMPONENT_AT_AR_FLSH"),
	    MakeComponent("COMPONENT_AT_SCOPE_SMALL"),
	    MakeComponent("COMPONENT_AT_AR_SUPP"),
	    MakeComponent("COMPONENT_AT_AR_AFGRIP"),
	    MakeComponent("COMPONENT_BULLPUPRIFLE_VARMOD_LOW"),
	};

	inline constexpr WeaponComponentData kGusenbergComponents[] = {
	    MakeComponent("COMPONENT_GUSENBERG_CLIP_01"),
	    MakeComponent("COMPONENT_GUSENBERG_CLIP_02"),
	};

	inline constexpr WeaponComponentData kHeavyshotgunComponents[] = {
	    MakeComponent("COMPONENT_HEAVYSHOTGUN_CLIP_01"),
	    MakeComponent("COMPONENT_HEAVYSHOTGUN_CLIP_02"),
	    MakeComponent("COMPONENT_HEAVYSHOTGUN_CLIP_03"),
	    MakeComponent("COMPONENT_AT_AR_FLSH"),
	    MakeComponent("COMPONENT_AT_AR_SUPP_02"),
	    MakeComponent("COMPONENT_AT_AR_AFGRIP"),
	};

	inline constexpr WeaponComponentData kMarksmanrifleComponents[] = {
	    MakeComponent("COMPONENT_MARKSMANRIFLE_CLIP_01"),
	    MakeComponent("COMPONENT_MARKSMANRIFLE_CLIP_02"),
	    MakeComponent("COMPONENT_AT_AR_FLSH"),
	    MakeComponent("COMPONENT_AT_AR_SUPP"),
	    MakeComponent("COMPONENT_AT_AR_AFGRIP"),
	    MakeComponent("COMPONENT_MARKSMANRIFLE_VARMOD_LUXE"),
	};

	inline constexpr WeaponComponentData kSnspistolComponents[] = {
	    MakeComponent("COMPONENT_SNSPISTOL_CLIP_01"),
	    MakeComponent("COMPONENT_SNSPISTOL_CLIP_02"),
	    MakeComponent("COMPONENT_SNSPISTOL_VARMOD_LOWRIDER"),
	};

	inline constexpr WeaponComponentData kSpecialcarbineComponents[] = {
	    MakeComponent("COMPONENT_SPECIALCARBINE_CLIP_01"),
	    MakeComponent("COMPONENT_SPECIALCARBINE_CLIP_02"),
	    MakeComponent("COMPONENT_SPECIALCARBINE_CLIP_03"),
	    MakeComponent("COMPONENT_AT_AR_FLSH"),
	    MakeComponent("COMPONENT_AT_SCOPE_MEDIUM"),
	    MakeComponent("COMPONENT_AT_AR_SUPP_02"),
	    MakeComponent("COMPONENT_AT_AR_AFGRIP"),
	    MakeComponent("COMPONENT_SPECIALCARBINE_VARMOD_LOWRIDER"),
	};

	inline constexpr WeaponComponentData kVintagepistolComponents[] = {
	    MakeComponent("COMPONENT_VINTAGEPISTOL_CLIP_01"),
	    MakeComponent("COMPONENT_VINTAGEPISTOL_CLIP_02"),
	    MakeComponent("COMPONENT_AT_PI_SUPP"),
	};

	inline constexpr WeaponComponentData kCombatpdwComponents[] = {
	    MakeComponent("COMPONENT_COMBATPDW_CLIP_01"),
	    MakeComponent("COMPONENT_COMBATPDW_CLIP_02"),
	    MakeComponent("COMPONENT_COMBATPDW_CLIP_03"),
	    MakeComponent("COMPONENT_AT_AR_FLSH"),
	    MakeComponent("COMPONENT_AT_AR_AFGRIP"),
	    MakeComponent("COMPONENT_AT_SCOPE_SMALL"),
	};

	inline constexpr WeaponComponentData kMachinepistolComponents[] = {
	    MakeComponent("COMPONENT_MACHINEPISTOL_CLIP_01"),
	    MakeComponent("COMPONENT_MACHINEPISTOL_CLIP_02"),
	    MakeComponent("COMPONENT_MACHINEPISTOL_CLIP_03"),
	    MakeComponent("COMPONENT_AT_PI_SUPP"),
	};

	inline constexpr WeaponComponentData kSwitchbladeComponents[] = {
	    MakeComponent("COMPONENT_SWITCHBLADE_VARMOD_BASE"),
	    MakeComponent("COMPONENT_SWITCHBLADE_VARMOD_VAR1"),
	    MakeComponent("COMPONENT_SWITCHBLADE_VARMOD_VAR2"),
	};

	inline constexpr WeaponComponentData kRevolverComponents[] = {
	    MakeComponent("COMPONENT_REVOLVER_VARMOD_BOSS"),
	    MakeComponent("COMPONENT_REVOLVER_VARMOD_GOON"),
	};

	inline constexpr WeaponComponentData kCompactrifleComponents[] = {
	    MakeComponent("COMPONENT_COMPACTRIFLE_CLIP_01"),
	    MakeComponent("COMPONENT_COMPACTRIFLE_CLIP_02"),
	    MakeComponent("COMPONENT_COMPACTRIFLE_CLIP_03"),
	};

	inline constexpr WeaponComponentData kMinismgComponents[] = {
	    MakeComponent("COMPONENT_MINISMG_CLIP_01"),
	    MakeComponent("COMPONENT_MINISMG_CLIP_02"),
	};

	inline constexpr WeaponComponentData kPistolMk2Components[] = {
	    MakeComponent("COMPONENT_PISTOL_MK2_CLIP_01"),
	    MakeComponent("COMPONENT_PISTOL_MK2_CLIP_02"),
	    MakeComponent("COMPONENT_PISTOL_MK2_CLIP_TRACER"),
	    MakeComponent("COMPONENT_PISTOL_MK2_CLIP_INCENDIARY"),
	    MakeComponent("COMPONENT_PISTOL_MK2_CLIP_HOLLOWPOINT"),
	    MakeComponent("COMPONENT_PISTOL_MK2_CLIP_FMJ"),
	    MakeComponent("COMPONENT_AT_PI_RAIL"),
	    MakeComponent("COMPONENT_AT_PI_FLSH_02"),
	    MakeComponent("COMPONENT_AT_PI_SUPP_02"),
	    MakeComponent("COMPONENT_AT_PI_COMP"),
	    MakeComponent("COMPONENT_PISTOL_MK2_CAMO"),
	    MakeComponent("COMPONENT_PISTOL_MK2_CAMO_02"),
	    MakeComponent("COMPONENT_PISTOL_MK2_CAMO_03"),
	    MakeComponent("COMPONENT_PISTOL_MK2_CAMO_04"),
	    MakeComponent("COMPONENT_PISTOL_MK2_CAMO_05"),
	    MakeComponent("COMPONENT_PISTOL_MK2_CAMO_06"),
	    MakeComponent("COMPONENT_PISTOL_MK2_CAMO_07"),
	    MakeComponent("COMPONENT_PISTOL_MK2_CAMO_08"),
	    MakeComponent("COMPONENT_PISTOL_MK2_CAMO_09"),
	    MakeComponent("COMPONENT_PISTOL_MK2_CAMO_10"),
	    MakeComponent("COMPONENT_PISTOL_MK2_CAMO_IND_01"),
	    MakeComponent("COMPONENT_PISTOL_MK2_CAMO_SLIDE"),
	    MakeComponent("COMPONENT_PISTOL_MK2_CAMO_02_SLIDE"),
	    MakeComponent("COMPONENT_PISTOL_MK2_CAMO_03_SLIDE"),
	    MakeComponent("COMPONENT_PISTOL_MK2_CAMO_04_SLIDE"),
	    MakeComponent("COMPONENT_PISTOL_MK2_CAMO_05_SLIDE"),
	    MakeComponent("COMPONENT_PISTOL_MK2_CAMO_06_SLIDE"),
	    MakeComponent("COMPONENT_PISTOL_MK2_CAMO_07_SLIDE"),
	    MakeComponent("COMPONENT_PISTOL_MK2_CAMO_08_SLIDE"),
	    MakeComponent("COMPONENT_PISTOL_MK2_CAMO_09_SLIDE"),
	    MakeComponent("COMPONENT_PISTOL_MK2_CAMO_10_SLIDE"),
	    MakeComponent("COMPONENT_PISTOL_MK2_CAMO_IND_01_SLIDE"),
	};

	inline constexpr WeaponComponentData kSmgMk2Components[] = {
	    MakeComponent("COMPONENT_SMG_MK2_CLIP_01"),
	    MakeComponent("COMPONENT_SMG_MK2_CLIP_02"),
	    MakeComponent("COMPONENT_SMG_MK2_CLIP_TRACER"),
	    MakeComponent("COMPONENT_SMG_MK2_CLIP_INCENDIARY"),
	    MakeComponent("COMPONENT_SMG_MK2_CLIP_HOLLOWPOINT"),
	    MakeComponent("COMPONENT_SMG_MK2_CLIP_FMJ"),
	    MakeComponent("COMPONENT_AT_AR_FLSH"),
	    MakeComponent("COMPONENT_AT_SIGHTS_SMG"),
	    MakeComponent("COMPONENT_AT_SCOPE_MACRO_02_SMG_MK2"),
	    MakeComponent("COMPONENT_AT_SCOPE_SMALL_SMG_MK2"),
	    MakeComponent("COMPONENT_AT_PI_SUPP"),
	    MakeComponent("COMPONENT_AT_MUZZLE_01"),
	    MakeComponent("COMPONENT_AT_MUZZLE_02"),
	    MakeComponent("COMPONENT_AT_MUZZLE_03"),
	    MakeComponent("COMPONENT_AT_MUZZLE_04"),
	    MakeComponent("COMPONENT_AT_MUZZLE_05"),
	    MakeComponent("COMPONENT_AT_MUZZLE_06"),
	    MakeComponent("COMPONENT_AT_MUZZLE_07"),
	    MakeComponent("COMPONENT_AT_SB_BARREL_01"),
	    MakeComponent("COMPONENT_AT_SB_BARREL_02"),
	    MakeComponent("COMPONENT_SMG_MK2_CAMO"),
	    MakeComponent("COMPONENT_SMG_MK2_CAMO_02"),
	    MakeComponent("COMPONENT_SMG_MK2_CAMO_03"),
	    MakeComponent("COMPONENT_SMG_MK2_CAMO_04"),
	    MakeComponent("COMPONENT_SMG_MK2_CAMO_05"),
	    MakeComponent("COMPONENT_SMG_MK2_CAMO_06"),
	    MakeComponent("COMPONENT_SMG_MK2_CAMO_07"),
	    MakeComponent("COMPONENT_SMG_MK2_CAMO_08"),
	    MakeComponent("COMPONENT_SMG_MK2_CAMO_09"),
	    MakeComponent("COMPONENT_SMG_MK2_CAMO_10"),
	    MakeComponent("COMPONENT_SMG_MK2_CAMO_IND_01"),
	};

	inline constexpr WeaponComponentData kAssaultrifleMk2Components[] = {
	    MakeComponent("COMPONENT_ASSAULTRIFLE_MK2_CLIP_01"),
	    MakeComponent("COMPONENT_ASSAULTRIFLE_MK2_CLIP_02"),
	    MakeComponent("COMPONENT_ASSAULTRIFLE_MK2_CLIP_TRACER"),
	    MakeComponent("COMPONENT_ASSAULTRIFLE_MK2_CLIP_INCENDIARY"),
	    MakeComponent("COMPONENT_ASSAULTRIFLE_MK2_CLIP_ARMORPIERCING"),
	    MakeComponent("COMPONENT_ASSAULTRIFLE_MK2_CLIP_FMJ"),
	    MakeComponent("COMPONENT_AT_AR_AFGRIP_02"),
	    MakeComponent("COMPONENT_AT_AR_FLSH"),
	    MakeComponent("COMPONENT_AT_SIGHTS"),
	    MakeComponent("COMPONENT_AT_SCOPE_MACRO_MK2"),
	    MakeComponent("COMPONENT_AT_SCOPE_MEDIUM_MK2"),
	    MakeComponent("COMPONENT_AT_AR_SUPP_02"),
	    MakeComponent("COMPONENT_AT_MUZZLE_01"),
	    MakeComponent("COMPONENT_AT_MUZZLE_02"),
	    MakeComponent("COMPONENT_AT_MUZZLE_03"),
	    MakeComponent("COMPONENT_AT_MUZZLE_04"),
	    MakeComponent("COMPONENT_AT_MUZZLE_05"),
	    MakeComponent("COMPONENT_AT_MUZZLE_06"),
	    MakeComponent("COMPONENT_AT_MUZZLE_07"),
	    MakeComponent("COMPONENT_AT_AR_BARREL_01"),
	    MakeComponent("COMPONENT_AT_AR_BARREL_02"),
	    MakeComponent("COMPONENT_ASSAULTRIFLE_MK2_CAMO"),
	    MakeComponent("COMPONENT_ASSAULTRIFLE_MK2_CAMO_02"),
	    MakeComponent("COMPONENT_ASSAULTRIFLE_MK2_CAMO_03"),
	    MakeComponent("COMPONENT_ASSAULTRIFLE_MK2_CAMO_04"),
	    MakeComponent("COMPONENT_ASSAULTRIFLE_MK2_CAMO_05"),
	    MakeComponent("COMPONENT_ASSAULTRIFLE_MK2_CAMO_06"),
	    MakeComponent("COMPONENT_ASSAULTRIFLE_MK2_CAMO_07"),
	    MakeComponent("COMPONENT_ASSAULTRIFLE_MK2_CAMO_08"),
	    MakeComponent("COMPONENT_ASSAULTRIFLE_MK2_CAMO_09"),
	    MakeComponent("COMPONENT_ASSAULTRIFLE_MK2_CAMO_10"),
	    MakeComponent("COMPONENT_ASSAULTRIFLE_MK2_CAMO_IND_01"),
	};

	inline constexpr WeaponComponentData kCarbinerifleMk2Components[] = {
	    MakeComponent("COMPONENT_CARBINERIFLE_MK2_CLIP_01"),
	    MakeComponent("COMPONENT_CARBINERIFLE_MK2_CLIP_02"),
	    MakeComponent("COMPONENT_CARBINERIFLE_MK2_CLIP_TRACER"),
	    MakeComponent("COMPONENT_CARBINERIFLE_MK2_CLIP_INCENDIARY"),
	    MakeComponent("COMPONENT_CARBINERIFLE_MK2_CLIP_ARMORPIERCING"),
	    MakeComponent("COMPONENT_CARBINERIFLE_MK2_CLIP_FMJ"),
	    MakeComponent("COMPONENT_AT_AR_AFGRIP_02"),
	    MakeComponent("COMPONENT_AT_AR_FLSH"),
	    MakeComponent("COMPONENT_AT_SIGHTS"),
	    MakeComponent("COMPONENT_AT_SCOPE_MACRO_MK2"),
	    MakeComponent("COMPONENT_AT_SCOPE_MEDIUM_MK2"),
	    MakeComponent("COMPONENT_AT_AR_SUPP"),
	    MakeComponent("COMPONENT_AT_MUZZLE_01"),
	    MakeComponent("COMPONENT_AT_MUZZLE_02"),
	    MakeComponent("COMPONENT_AT_MUZZLE_03"),
	    MakeComponent("COMPONENT_AT_MUZZLE_04"),
	    MakeComponent("COMPONENT_AT_MUZZLE_05"),
	    MakeComponent("COMPONENT_AT_MUZZLE_06"),
	    MakeComponent("COMPONENT_AT_MUZZLE_07"),
	    MakeComponent("COMPONENT_AT_CR_BARREL_01"),
	    MakeComponent("COMPONENT_AT_CR_BARREL_02"),
	    MakeComponent("COMPONENT_CARBINERIFLE_MK2_CAMO"),
	    MakeComponent("COMPONENT_CARBINERIFLE_MK2_CAMO_02"),
	    MakeComponent("COMPONENT_CARBINERIFLE_MK2_CAMO_03"),
	    MakeComponent("COMPONENT_CARBINERIFLE_MK2_CAMO_04"),
	    MakeComponent("COMPONENT_CARBINERIFLE_MK2_CAMO_05"),
	    MakeComponent("COMPONENT_CARBINERIFLE_MK2_CAMO_06"),
	    MakeComponent("COMPONENT_CARBINERIFLE_MK2_CAMO_07"),
	    MakeComponent("COMPONENT_CARBINERIFLE_MK2_CAMO_08"),
	    MakeComponent("COMPONENT_CARBINERIFLE_MK2_CAMO_09"),
	    MakeComponent("COMPONENT_CARBINERIFLE_MK2_CAMO_10"),
	    MakeComponent("COMPONENT_CARBINERIFLE_MK2_CAMO_IND_01"),
	};

	inline constexpr WeaponComponentData kCombatmgMk2Components[] = {
	    MakeComponent("COMPONENT_COMBATMG_MK2_CLIP_01"),
	    MakeComponent("COMPONENT_COMBATMG_MK2_CLIP_02"),
	    MakeComponent("COMPONENT_COMBATMG_MK2_CLIP_TRACER"),
	    MakeComponent("COMPONENT_COMBATMG_MK2_CLIP_INCENDIARY"),
	    MakeComponent("COMPONENT_COMBATMG_MK2_CLIP_ARMORPIERCING"),
	    MakeComponent("COMPONENT_COMBATMG_MK2_CLIP_FMJ"),
	    MakeComponent("COMPONENT_AT_AR_AFGRIP_02"),
	    MakeComponent("COMPONENT_AT_SIGHTS"),
	    MakeComponent("COMPONENT_AT_SCOPE_SMALL_MK2"),
	    MakeComponent("COMPONENT_AT_SCOPE_MEDIUM_MK2"),
	    MakeComponent("COMPONENT_AT_MUZZLE_01"),
	    MakeComponent("COMPONENT_AT_MUZZLE_02"),
	    MakeComponent("COMPONENT_AT_MUZZLE_03"),
	    MakeComponent("COMPONENT_AT_MUZZLE_04"),
	    MakeComponent("COMPONENT_AT_MUZZLE_05"),
	    MakeComponent("COMPONENT_AT_MUZZLE_06"),
	    MakeComponent("COMPONENT_AT_MUZZLE_07"),
	    MakeComponent("COMPONENT_AT_MG_BARREL_01"),
	    MakeComponent("COMPONENT_AT_MG_BARREL_02"),
	    MakeComponent("COMPONENT_COMBATMG_MK2_CAMO"),
	    MakeComponent("COMPONENT_COMBATMG_MK2_CAMO_02"),
	    MakeComponent("COMPONENT_COMBATMG_MK2_CAMO_03"),
	    MakeComponent("COMPONENT_COMBATMG_MK2_CAMO_04"),
	    MakeComponent("COMPONENT_COMBATMG_MK2_CAMO_05"),
	    MakeComponent("COMPONENT_COMBATMG_MK2_CAMO_06"),
	    MakeComponent("COMPONENT_COMBATMG_MK2_CAMO_07"),
	    MakeComponent("COMPONENT_COMBATMG_MK2_CAMO_08"),
	    MakeComponent("COMPONENT_COMBATMG_MK2_CAMO_09"),
	    MakeComponent("COMPONENT_COMBATMG_MK2_CAMO_10"),
	    MakeComponent("COMPONENT_COMBATMG_MK2_CAMO_IND_01"),
	};

	inline constexpr WeaponComponentData kHeavysniperMk2Components[] = {
	    MakeComponent("COMPONENT_HEAVYSNIPER_MK2_CLIP_01"),
	    MakeComponent("COMPONENT_HEAVYSNIPER_MK2_CLIP_02"),
	    MakeComponent("COMPONENT_HEAVYSNIPER_MK2_CLIP_INCENDIARY"),
	    MakeComponent("COMPONENT_HEAVYSNIPER_MK2_CLIP_ARMORPIERCING"),
	    MakeComponent("COMPONENT_HEAVYSNIPER_MK2_CLIP_FMJ"),
	    MakeComponent("COMPONENT_HEAVYSNIPER_MK2_CLIP_EXPLOSIVE"),
	    MakeComponent("COMPONENT_AT_SCOPE_LARGE_MK2"),
	    MakeComponent("COMPONENT_AT_SCOPE_MAX"),
	    MakeComponent("COMPONENT_AT_SCOPE_NV"),
	    MakeComponent("COMPONENT_AT_SCOPE_THERMAL"),
	    MakeComponent("COMPONENT_AT_SR_SUPP_03"),
	    MakeComponent("COMPONENT_AT_MUZZLE_08"),
	    MakeComponent("COMPONENT_AT_MUZZLE_09"),
	    MakeComponent("COMPONENT_AT_SR_BARREL_01"),
	    MakeComponent("COMPONENT_AT_SR_BARREL_02"),
	    MakeComponent("COMPONENT_HEAVYSNIPER_MK2_CAMO"),
	    MakeComponent("COMPONENT_HEAVYSNIPER_MK2_CAMO_02"),
	    MakeComponent("COMPONENT_HEAVYSNIPER_MK2_CAMO_03"),
	    MakeComponent("COMPONENT_HEAVYSNIPER_MK2_CAMO_04"),
	    MakeComponent("COMPONENT_HEAVYSNIPER_MK2_CAMO_05"),
	    MakeComponent("COMPONENT_HEAVYSNIPER_MK2_CAMO_06"),
	    MakeComponent("COMPONENT_HEAVYSNIPER_MK2_CAMO_07"),
	    MakeComponent("COMPONENT_HEAVYSNIPER_MK2_CAMO_08"),
	    MakeComponent("COMPONENT_HEAVYSNIPER_MK2_CAMO_09"),
	    MakeComponent("COMPONENT_HEAVYSNIPER_MK2_CAMO_10"),
	    MakeComponent("COMPONENT_HEAVYSNIPER_MK2_CAMO_IND_01"),
	};

	inline constexpr WeaponComponentData kSnspistolMk2Components[] = {
	    MakeComponent("COMPONENT_SNSPISTOL_MK2_CLIP_01"),
	    MakeComponent("COMPONENT_SNSPISTOL_MK2_CLIP_02"),
	    MakeComponent("COMPONENT_SNSPISTOL_MK2_CLIP_TRACER"),
	    MakeComponent("COMPONENT_SNSPISTOL_MK2_CLIP_INCENDIARY"),
	    MakeComponent("COMPONENT_SNSPISTOL_MK2_CLIP_HOLLOWPOINT"),
	    MakeComponent("COMPONENT_SNSPISTOL_MK2_CLIP_FMJ"),
	    MakeComponent("COMPONENT_AT_PI_FLSH_03"),
	    MakeComponent("COMPONENT_AT_PI_RAIL_02"),
	    MakeComponent("COMPONENT_AT_PI_SUPP_02"),
	    MakeComponent("COMPONENT_AT_PI_COMP_02"),
	    MakeComponent("COMPONENT_SNSPISTOL_MK2_CAMO"),
	    MakeComponent("COMPONENT_SNSPISTOL_MK2_CAMO_02"),
	    MakeComponent("COMPONENT_SNSPISTOL_MK2_CAMO_03"),
	    MakeComponent("COMPONENT_SNSPISTOL_MK2_CAMO_04"),
	    MakeComponent("COMPONENT_SNSPISTOL_MK2_CAMO_05"),
	    MakeComponent("COMPONENT_SNSPISTOL_MK2_CAMO_06"),
	    MakeComponent("COMPONENT_SNSPISTOL_MK2_CAMO_07"),
	    MakeComponent("COMPONENT_SNSPISTOL_MK2_CAMO_08"),
	    MakeComponent("COMPONENT_SNSPISTOL_MK2_CAMO_09"),
	    MakeComponent("COMPONENT_SNSPISTOL_MK2_CAMO_10"),
	    MakeComponent("COMPONENT_SNSPISTOL_MK2_CAMO_IND_01"),
	    MakeComponent("COMPONENT_SNSPISTOL_MK2_CAMO_SLIDE"),
	    MakeComponent("COMPONENT_SNSPISTOL_MK2_CAMO_02_SLIDE"),
	    MakeComponent("COMPONENT_SNSPISTOL_MK2_CAMO_03_SLIDE"),
	    MakeComponent("COMPONENT_SNSPISTOL_MK2_CAMO_04_SLIDE"),
	    MakeComponent("COMPONENT_SNSPISTOL_MK2_CAMO_05_SLIDE"),
	    MakeComponent("COMPONENT_SNSPISTOL_MK2_CAMO_06_SLIDE"),
	    MakeComponent("COMPONENT_SNSPISTOL_MK2_CAMO_07_SLIDE"),
	    MakeComponent("COMPONENT_SNSPISTOL_MK2_CAMO_08_SLIDE"),
	    MakeComponent("COMPONENT_SNSPISTOL_MK2_CAMO_09_SLIDE"),
	    MakeComponent("COMPONENT_SNSPISTOL_MK2_CAMO_10_SLIDE"),
	    MakeComponent("COMPONENT_SNSPISTOL_MK2_CAMO_IND_01_SLIDE"),
	};

	inline constexpr WeaponComponentData kRevolverMk2Components[] = {
	    MakeComponent("COMPONENT_REVOLVER_MK2_CLIP_01"),
	    MakeComponent("COMPONENT_REVOLVER_MK2_CLIP_TRACER"),
	    MakeComponent("COMPONENT_REVOLVER_MK2_CLIP_INCENDIARY"),
	    MakeComponent("COMPONENT_REVOLVER_MK2_CLIP_HOLLOWPOINT"),
	    MakeComponent("COMPONENT_REVOLVER_MK2_CLIP_FMJ"),
	    MakeComponent("COMPONENT_AT_SIGHTS"),
	    MakeComponent("COMPONENT_AT_SCOPE_MACRO_MK2"),
	    MakeComponent("COMPONENT_AT_PI_FLSH"),
	    MakeComponent("COMPONENT_AT_PI_COMP_03"),
	    MakeComponent("COMPONENT_REVOLVER_MK2_CAMO"),
	    MakeComponent("COMPONENT_REVOLVER_MK2_CAMO_02"),
	    MakeComponent("COMPONENT_REVOLVER_MK2_CAMO_03"),
	    MakeComponent("COMPONENT_REVOLVER_MK2_CAMO_04"),
	    MakeComponent("COMPONENT_REVOLVER_MK2_CAMO_05"),
	    MakeComponent("COMPONENT_REVOLVER_MK2_CAMO_06"),
	    MakeComponent("COMPONENT_REVOLVER_MK2_CAMO_07"),
	    MakeComponent("COMPONENT_REVOLVER_MK2_CAMO_08"),
	    MakeComponent("COMPONENT_REVOLVER_MK2_CAMO_09"),
	    MakeComponent("COMPONENT_REVOLVER_MK2_CAMO_10"),
	    MakeComponent("COMPONENT_REVOLVER_MK2_CAMO_IND_01"),
	};

	inline constexpr WeaponComponentData kSpecialcarbineMk2Components[] = {
	    MakeComponent("COMPONENT_SPECIALCARBINE_MK2_CLIP_01"),
	    MakeComponent("COMPONENT_SPECIALCARBINE_MK2_CLIP_02"),
	    MakeComponent("COMPONENT_SPECIALCARBINE_MK2_CLIP_TRACER"),
	    MakeComponent("COMPONENT_SPECIALCARBINE_MK2_CLIP_INCENDIARY"),
	    MakeComponent("COMPONENT_SPECIALCARBINE_MK2_CLIP_ARMORPIERCING"),
	    MakeComponent("COMPONENT_SPECIALCARBINE_MK2_CLIP_FMJ"),
	    MakeComponent("COMPONENT_AT_AR_FLSH"),
	    MakeComponent("COMPONENT_AT_SIGHTS"),
	    MakeComponent("COMPONENT_AT_SCOPE_MACRO_MK2"),
	    MakeComponent("COMPONENT_AT_SCOPE_MEDIUM_MK2"),
	    MakeComponent("COMPONENT_AT_AR_SUPP_02"),
	    MakeComponent("COMPONENT_AT_MUZZLE_01"),
	    MakeComponent("COMPONENT_AT_MUZZLE_02"),
	    MakeComponent("COMPONENT_AT_MUZZLE_03"),
	    MakeComponent("COMPONENT_AT_MUZZLE_04"),
	    MakeComponent("COMPONENT_AT_MUZZLE_05"),
	    MakeComponent("COMPONENT_AT_MUZZLE_06"),
	    MakeComponent("COMPONENT_AT_MUZZLE_07"),
	    MakeComponent("COMPONENT_AT_AR_AFGRIP_02"),
	    MakeComponent("COMPONENT_AT_SC_BARREL_01"),
	    MakeComponent("COMPONENT_AT_SC_BARREL_02"),
	    MakeComponent("COMPONENT_SPECIALCARBINE_MK2_CAMO"),
	    MakeComponent("COMPONENT_SPECIALCARBINE_MK2_CAMO_02"),
	    MakeComponent("COMPONENT_SPECIALCARBINE_MK2_CAMO_03"),
	    MakeComponent("COMPONENT_SPECIALCARBINE_MK2_CAMO_04"),
	    MakeComponent("COMPONENT_SPECIALCARBINE_MK2_CAMO_05"),
	    MakeComponent("COMPONENT_SPECIALCARBINE_MK2_CAMO_06"),
	    MakeComponent("COMPONENT_SPECIALCARBINE_MK2_CAMO_07"),
	    MakeComponent("COMPONENT_SPECIALCARBINE_MK2_CAMO_08"),
	    MakeComponent("COMPONENT_SPECIALCARBINE_MK2_CAMO_09"),
	    MakeComponent("COMPONENT_SPECIALCARBINE_MK2_CAMO_10"),
	    MakeComponent("COMPONENT_SPECIALCARBINE_MK2_CAMO_IND_01"),
	};

	inline constexpr WeaponComponentData kBullpuprifleMk2Components[] = {
	    MakeComponent("COMPONENT_BULLPUPRIFLE_MK2_CLIP_01"),
	    MakeComponent("COMPONENT_BULLPUPRIFLE_MK2_CLIP_02"),
	    MakeComponent("COMPONENT_BULLPUPRIFLE_MK2_CLIP_TRACER"),
	    MakeComponent("COMPONENT_BULLPUPRIFLE_MK2_CLIP_INCENDIARY"),
	    MakeComponent("COMPONENT_BULLPUPRIFLE_MK2_CLIP_ARMORPIERCING"),
	    MakeComponent("COMPONENT_BULLPUPRIFLE_MK2_CLIP_FMJ"),
	    MakeComponent("COMPONENT_AT_AR_FLSH"),
	    MakeComponent("COMPONENT_AT_SIGHTS"),
	    MakeComponent("COMPONENT_AT_SCOPE_MACRO_02_MK2"),
	    MakeComponent("COMPONENT_AT_SCOPE_SMALL_MK2"),
	    MakeComponent("COMPONENT_AT_BP_BARREL_01"),
	    MakeComponent("COMPONENT_AT_BP_BARREL_02"),
	    MakeComponent("COMPONENT_AT_AR_SUPP"),
	    MakeComponent("COMPONENT_AT_MUZZLE_01"),
	    MakeComponent("COMPONENT_AT_MUZZLE_02"),
	    MakeComponent("COMPONENT_AT_MUZZLE_03"),
	    MakeComponent("COMPONENT_AT_MUZZLE_04"),
	    MakeComponent("COMPONENT_AT_MUZZLE_05"),
	    MakeComponent("COMPONENT_AT_MUZZLE_06"),
	    MakeComponent("COMPONENT_AT_MUZZLE_07"),
	    MakeComponent("COMPONENT_AT_AR_AFGRIP_02"),
	    MakeComponent("COMPONENT_BULLPUPRIFLE_MK2_CAMO"),
	    MakeComponent("COMPONENT_BULLPUPRIFLE_MK2_CAMO_02"),
	    MakeComponent("COMPONENT_BULLPUPRIFLE_MK2_CAMO_03"),
	    MakeComponent("COMPONENT_BULLPUPRIFLE_MK2_CAMO_04"),
	    MakeComponent("COMPONENT_BULLPUPRIFLE_MK2_CAMO_05"),
	    MakeComponent("COMPONENT_BULLPUPRIFLE_MK2_CAMO_06"),
	    MakeComponent("COMPONENT_BULLPUPRIFLE_MK2_CAMO_07"),
	    MakeComponent("COMPONENT_BULLPUPRIFLE_MK2_CAMO_08"),
	    MakeComponent("COMPONENT_BULLPUPRIFLE_MK2_CAMO_09"),
	    MakeComponent("COMPONENT_BULLPUPRIFLE_MK2_CAMO_10"),
	    MakeComponent("COMPONENT_BULLPUPRIFLE_MK2_CAMO_IND_01"),
	};

	inline constexpr WeaponComponentData kPumpshotgunMk2Components[] = {
	    MakeComponent("COMPONENT_PUMPSHOTGUN_MK2_CLIP_01"),
	    MakeComponent("COMPONENT_PUMPSHOTGUN_MK2_CLIP_INCENDIARY"),
	    MakeComponent("COMPONENT_PUMPSHOTGUN_MK2_CLIP_ARMORPIERCING"),
	    MakeComponent("COMPONENT_PUMPSHOTGUN_MK2_CLIP_HOLLOWPOINT"),
	    MakeComponent("COMPONENT_PUMPSHOTGUN_MK2_CLIP_EXPLOSIVE"),
	    MakeComponent("COMPONENT_AT_SIGHTS"),
	    MakeComponent("COMPONENT_AT_SCOPE_MACRO_MK2"),
	    MakeComponent("COMPONENT_AT_SCOPE_SMALL_MK2"),
	    MakeComponent("COMPONENT_AT_AR_FLSH"),
	    MakeComponent("COMPONENT_AT_SR_SUPP_03"),
	    MakeComponent("COMPONENT_AT_MUZZLE_08"),
	    MakeComponent("COMPONENT_PUMPSHOTGUN_MK2_CAMO"),
	    MakeComponent("COMPONENT_PUMPSHOTGUN_MK2_CAMO_02"),
	    MakeComponent("COMPONENT_PUMPSHOTGUN_MK2_CAMO_03"),
	    MakeComponent("COMPONENT_PUMPSHOTGUN_MK2_CAMO_04"),
	    MakeComponent("COMPONENT_PUMPSHOTGUN_MK2_CAMO_05"),
	    MakeComponent("COMPONENT_PUMPSHOTGUN_MK2_CAMO_06"),
	    MakeComponent("COMPONENT_PUMPSHOTGUN_MK2_CAMO_07"),
	    MakeComponent("COMPONENT_PUMPSHOTGUN_MK2_CAMO_08"),
	    MakeComponent("COMPONENT_PUMPSHOTGUN_MK2_CAMO_09"),
	    MakeComponent("COMPONENT_PUMPSHOTGUN_MK2_CAMO_10"),
	    MakeComponent("COMPONENT_PUMPSHOTGUN_MK2_CAMO_IND_01"),
	};

	inline constexpr WeaponComponentData kMarksmanrifleMk2Components[] = {
	    MakeComponent("COMPONENT_MARKSMANRIFLE_MK2_CLIP_01"),
	    MakeComponent("COMPONENT_MARKSMANRIFLE_MK2_CLIP_02"),
	    MakeComponent("COMPONENT_MARKSMANRIFLE_MK2_CLIP_TRACER"),
	    MakeComponent("COMPONENT_MARKSMANRIFLE_MK2_CLIP_INCENDIARY"),
	    MakeComponent("COMPONENT_MARKSMANRIFLE_MK2_CLIP_ARMORPIERCING"),
	    MakeComponent("COMPONENT_MARKSMANRIFLE_MK2_CLIP_FMJ"),
	    MakeComponent("COMPONENT_AT_SIGHTS"),
	    MakeComponent("COMPONENT_AT_SCOPE_MEDIUM_MK2"),
	    MakeComponent("COMPONENT_AT_SCOPE_LARGE_FIXED_ZOOM_MK2"),
	    MakeComponent("COMPONENT_AT_AR_FLSH"),
	    MakeComponent("COMPONENT_AT_AR_SUPP"),
	    MakeComponent("COMPONENT_AT_MUZZLE_01"),
	    MakeComponent("COMPONENT_AT_MUZZLE_02"),
	    MakeComponent("COMPONENT_AT_MUZZLE_03"),
	    MakeComponent("COMPONENT_AT_MUZZLE_04"),
	    MakeComponent("COMPONENT_AT_MUZZLE_05"),
	    MakeComponent("COMPONENT_AT_MUZZLE_06"),
	    MakeComponent("COMPONENT_AT_MUZZLE_07"),
	    MakeComponent("COMPONENT_AT_MRFL_BARREL_01"),
	    MakeComponent("COMPONENT_AT_MRFL_BARREL_02"),
	    MakeComponent("COMPONENT_AT_AR_AFGRIP_02"),
	    MakeComponent("COMPONENT_MARKSMANRIFLE_MK2_CAMO"),
	    MakeComponent("COMPONENT_MARKSMANRIFLE_MK2_CAMO_02"),
	    MakeComponent("COMPONENT_MARKSMANRIFLE_MK2_CAMO_03"),
	    MakeComponent("COMPONENT_MARKSMANRIFLE_MK2_CAMO_04"),
	    MakeComponent("COMPONENT_MARKSMANRIFLE_MK2_CAMO_05"),
	    MakeComponent("COMPONENT_MARKSMANRIFLE_MK2_CAMO_06"),
	    MakeComponent("COMPONENT_MARKSMANRIFLE_MK2_CAMO_07"),
	    MakeComponent("COMPONENT_MARKSMANRIFLE_MK2_CAMO_08"),
	    MakeComponent("COMPONENT_MARKSMANRIFLE_MK2_CAMO_09"),
	    MakeComponent("COMPONENT_MARKSMANRIFLE_MK2_CAMO_10"),
	    MakeComponent("COMPONENT_MARKSMANRIFLE_MK2_CAMO_IND_01"),
	};

	inline constexpr WeaponComponentData kCeramicpistolComponents[] = {
	    MakeComponent("COMPONENT_CERAMICPISTOL_CLIP_01"),
	    MakeComponent("COMPONENT_CERAMICPISTOL_CLIP_02"),
	    MakeComponent("COMPONENT_CERAMICPISTOL_SUPP"),
	};

	inline constexpr WeaponComponentData kMilitaryrifleComponents[] = {
	    MakeComponent("COMPONENT_MILITARYRIFLE_CLIP_01"),
	    MakeComponent("COMPONENT_MILITARYRIFLE_CLIP_02"),
	    MakeComponent("COMPONENT_MILITARYRIFLE_SIGHT_01"),
	    MakeComponent("COMPONENT_AT_SCOPE_SMALL"),
	    MakeComponent("COMPONENT_AT_AR_FLSH"),
	    MakeComponent("COMPONENT_AT_AR_SUPP"),
	};

	inline constexpr WeaponComponentData kCombatshotgunComponents[] = {
	    MakeComponent("COMPONENT_AT_AR_FLSH"),
	    MakeComponent("COMPONENT_AT_AR_SUPP"),
	};

	inline constexpr WeaponComponentData kHeavyrifleComponents[] = {
	    MakeComponent("COMPONENT_HEAVYRIFLE_CLIP_01"),
	    MakeComponent("COMPONENT_HEAVYRIFLE_CLIP_02"),
	    MakeComponent("COMPONENT_HEAVYRIFLE_SIGHT_01"),
	    MakeComponent("COMPONENT_AT_SCOPE_MEDIUM"),
	    MakeComponent("COMPONENT_AT_AR_FLSH"),
	    MakeComponent("COMPONENT_AT_AR_SUPP"),
	    MakeComponent("COMPONENT_AT_AR_AFGRIP"),
	    MakeComponent("COMPONENT_HEAVYRIFLE_CAMO1"),
	};

	inline constexpr WeaponComponentData kTacticalrifleComponents[] = {
	    MakeComponent("COMPONENT_AT_AR_FLSH_REH"),
	    MakeComponent("COMPONENT_TACTICALRIFLE_CLIP_02"),
	    MakeComponent("COMPONENT_AT_AR_SUPP_02"),
	    MakeComponent("COMPONENT_AT_AR_AFGRIP"),
	};

	inline constexpr WeaponComponentData kPistolxm3Components[] = {
	    MakeComponent("COMPONENT_PISTOLXM3_CLIP_01"),
	    MakeComponent("COMPONENT_PISTOLXM3_SUPP"),
	};

	inline constexpr WeaponComponentData kTecpistolComponents[] = {
	    MakeComponent("COMPONENT_TECPISTOL_CLIP_01"),
	    MakeComponent("COMPONENT_TECPISTOL_CLIP_02"),
	    MakeComponent("COMPONENT_AT_AR_SUPP_02"),
	    MakeComponent("COMPONENT_AT_SCOPE_MACRO"),
	};

	inline constexpr WeaponComponentData kBattlerifleComponents[] = {
	    MakeComponent("COMPONENT_BATTLERIFLE_CLIP_01"),
	    MakeComponent("COMPONENT_BATTLERIFLE_CLIP_02"),
	    MakeComponent("COMPONENT_AT_AR_SUPP"),
	};

	inline constexpr WeaponComponentData kKnuckleComponents[] = {
	    MakeComponent("COMPONENT_KNUCKLE_VARMOD_BASE"),
	    MakeComponent("COMPONENT_KNUCKLE_VARMOD_PIMP"),
	    MakeComponent("COMPONENT_KNUCKLE_VARMOD_BALLAS"),
	    MakeComponent("COMPONENT_KNUCKLE_VARMOD_DOLLAR"),
	    MakeComponent("COMPONENT_KNUCKLE_VARMOD_DIAMOND"),
	    MakeComponent("COMPONENT_KNUCKLE_VARMOD_HATE"),
	    MakeComponent("COMPONENT_KNUCKLE_VARMOD_LOVE"),
	    MakeComponent("COMPONENT_KNUCKLE_VARMOD_PLAYER"),
	    MakeComponent("COMPONENT_KNUCKLE_VARMOD_KING"),
	    MakeComponent("COMPONENT_KNUCKLE_VARMOD_VAGOS"),
	};

	inline constexpr WeaponComponentData kRaypistolComponents[] = {
	    MakeComponent("COMPONENT_RAYPISTOL_VARMOD_XMAS18"),
	};

	inline std::span<const WeaponComponentData> GetComponentsForWeapon(joaat_t weaponHash)
	{
		switch (weaponHash)
		{
		case Joaat("WEAPON_PISTOL"):
			return kPistolComponents;
		case Joaat("WEAPON_HEAVYPISTOL"):
			return kHeavypistolComponents;
		case Joaat("WEAPON_COMBATPISTOL"):
			return kCombatpistolComponents;
		case Joaat("WEAPON_APPISTOL"):
			return kAppistolComponents;
		case Joaat("WEAPON_PISTOL50"):
			return kPistol50Components;
		case Joaat("WEAPON_MICROSMG"):
			return kMicrosmgComponents;
		case Joaat("WEAPON_SMG"):
			return kSmgComponents;
		case Joaat("WEAPON_ASSAULTSMG"):
			return kAssaultsmgComponents;
		case Joaat("WEAPON_ASSAULTRIFLE"):
			return kAssaultrifleComponents;
		case Joaat("WEAPON_CARBINERIFLE"):
			return kCarbinerifleComponents;
		case Joaat("WEAPON_ADVANCEDRIFLE"):
			return kAdvancedrifleComponents;
		case Joaat("WEAPON_MG"):
			return kMgComponents;
		case Joaat("WEAPON_COMBATMG"):
			return kCombatmgComponents;
		case Joaat("WEAPON_SAWNOFFSHOTGUN"):
			return kSawnoffshotgunComponents;
		case Joaat("WEAPON_PUMPSHOTGUN"):
			return kPumpshotgunComponents;
		case Joaat("WEAPON_ASSAULTSHOTGUN"):
			return kAssaultshotgunComponents;
		case Joaat("WEAPON_BULLPUPSHOTGUN"):
			return kBullpupshotgunComponents;
		case Joaat("WEAPON_SNIPERRIFLE"):
			return kSniperrifleComponents;
		case Joaat("WEAPON_HEAVYSNIPER"):
			return kHeavysniperComponents;
		case Joaat("WEAPON_GRENADELAUNCHER"):
			return kGrenadelauncherComponents;
		case Joaat("WEAPON_BULLPUPRIFLE"):
			return kBullpuprifleComponents;
		case Joaat("WEAPON_GUSENBERG"):
			return kGusenbergComponents;
		case Joaat("WEAPON_HEAVYSHOTGUN"):
			return kHeavyshotgunComponents;
		case Joaat("WEAPON_MARKSMANRIFLE"):
			return kMarksmanrifleComponents;
		case Joaat("WEAPON_SNSPISTOL"):
			return kSnspistolComponents;
		case Joaat("WEAPON_SPECIALCARBINE"):
			return kSpecialcarbineComponents;
		case Joaat("WEAPON_VINTAGEPISTOL"):
			return kVintagepistolComponents;
		case Joaat("WEAPON_COMBATPDW"):
			return kCombatpdwComponents;
		case Joaat("WEAPON_MACHINEPISTOL"):
			return kMachinepistolComponents;
		case Joaat("WEAPON_SWITCHBLADE"):
			return kSwitchbladeComponents;
		case Joaat("WEAPON_REVOLVER"):
			return kRevolverComponents;
		case Joaat("WEAPON_COMPACTRIFLE"):
			return kCompactrifleComponents;
		case Joaat("WEAPON_MINISMG"):
			return kMinismgComponents;
		case Joaat("WEAPON_PISTOL_MK2"):
			return kPistolMk2Components;
		case Joaat("WEAPON_SMG_MK2"):
			return kSmgMk2Components;
		case Joaat("WEAPON_ASSAULTRIFLE_MK2"):
			return kAssaultrifleMk2Components;
		case Joaat("WEAPON_CARBINERIFLE_MK2"):
			return kCarbinerifleMk2Components;
		case Joaat("WEAPON_COMBATMG_MK2"):
			return kCombatmgMk2Components;
		case Joaat("WEAPON_HEAVYSNIPER_MK2"):
			return kHeavysniperMk2Components;
		case Joaat("WEAPON_SNSPISTOL_MK2"):
			return kSnspistolMk2Components;
		case Joaat("WEAPON_REVOLVER_MK2"):
			return kRevolverMk2Components;
		case Joaat("WEAPON_SPECIALCARBINE_MK2"):
			return kSpecialcarbineMk2Components;
		case Joaat("WEAPON_BULLPUPRIFLE_MK2"):
			return kBullpuprifleMk2Components;
		case Joaat("WEAPON_PUMPSHOTGUN_MK2"):
			return kPumpshotgunMk2Components;
		case Joaat("WEAPON_MARKSMANRIFLE_MK2"):
			return kMarksmanrifleMk2Components;
		case Joaat("WEAPON_CERAMICPISTOL"):
			return kCeramicpistolComponents;
		case Joaat("WEAPON_MILITARYRIFLE"):
			return kMilitaryrifleComponents;
		case Joaat("WEAPON_COMBATSHOTGUN"):
			return kCombatshotgunComponents;
		case Joaat("WEAPON_HEAVYRIFLE"):
			return kHeavyrifleComponents;
		case Joaat("WEAPON_TACTICALRIFLE"):
			return kTacticalrifleComponents;
		case Joaat("WEAPON_PISTOLXM3"):
			return kPistolxm3Components;
		case Joaat("WEAPON_TECPISTOL"):
			return kTecpistolComponents;
		case Joaat("WEAPON_BATTLERIFLE"):
			return kBattlerifleComponents;
		case Joaat("WEAPON_KNUCKLE"):
			return kKnuckleComponents;
		case Joaat("WEAPON_RAYPISTOL"):
			return kRaypistolComponents;
		default:
			return {};
		}
	}
}