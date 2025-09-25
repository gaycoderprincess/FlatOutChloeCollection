class CMenu_Controller_Layout : public CMenuHUDElement {
public:
	const char* GetName() override { return "menu_controller_layout"; }

	void Init() override {
		PreloadTexture("data/menu/xbox_controller.tga");
	}

	enum eFontAlign {
		FONTF_LEFT,
		FONTF_CENTER,
		FONTF_RIGHT,
	};

	struct tControllerBinding {
		std::vector<int> keys;
		int pos[2];
		int align;
		std::string icon;
		int iconpos[2] = {};
		int iconsize[2] = {};
	};
	static inline tControllerBinding aBindings[] = {
			{{CONTROLLER_BUTTON_LEFT_TRIGGER}, {206,133}, FONTF_RIGHT, "icon_lt", {210,129}, {24,22}},
			{{CONTROLLER_BUTTON_EXTRA1}, {206,153}, FONTF_RIGHT, "icon_lb", {210,153}, {29,16}},
			{{CONTROLLER_BUTTON_SELECT}, {206,173}, FONTF_RIGHT, "icon_back", {210,173}, {23,16}},

			{{CONTROLLER_BUTTON_UP}, {136+30,225}, FONTF_RIGHT, "controller_lines", {141,215}, {208,78}},
			{{CONTROLLER_BUTTON_DIGITAL_LEFT, CONTROLLER_BUTTON_DIGITAL_RIGHT, CONTROLLER_BUTTON_DIGITAL_UP, CONTROLLER_BUTTON_DIGITAL_DOWN}, {136+30,244}, FONTF_RIGHT, "", {}, {}},
			{{CONTROLLER_BUTTON_EXTRA4, CONTROLLER_BUTTON_RIGHT_UP, CONTROLLER_BUTTON_RIGHT_DOWN, CONTROLLER_BUTTON_RIGHT_LEFT, CONTROLLER_BUTTON_RIGHT_RIGHT}, {136+30,281}, FONTF_RIGHT, "", {}, {}},

			{{CONTROLLER_BUTTON_RIGHT_TRIGGER}, {439,133}, FONTF_LEFT, "icon_rt", {409,129}, {24,22}},
			{{CONTROLLER_BUTTON_EXTRA2}, {439,153}, FONTF_LEFT, "icon_rb", {404,153}, {29,16}},
			{{CONTROLLER_BUTTON_START}, {439,173}, FONTF_LEFT, "icon_start", {409,173}, {23,16}},

			{{CONTROLLER_BUTTON_A}, {449,197+20*0}, FONTF_LEFT, "icon_abxy", {428,194}, {17,78}},
			{{CONTROLLER_BUTTON_B}, {449,197+20*1}, FONTF_LEFT, "", {}, {}},
			{{CONTROLLER_BUTTON_X}, {449,197+20*2}, FONTF_LEFT, "", {}, {}},
			{{CONTROLLER_BUTTON_Y}, {449,197+20*3}, FONTF_LEFT, "", {}, {}},
	};

	static const char* GetControllerBindingName(int bind) {
		switch (bind) {
			case CONFIG_HANDBRAKE: return "HANDBRAKE";
			case CONFIG_HANDBRAKE_ALT: return "HANDBRAKE";
			case CONFIG_NITRO: return "NITRO";
			case CONFIG_GEARDOWN: return "GEAR DOWN";
			case CONFIG_GEARUP: return "GEAR UP";
			case CONFIG_PAUSE: return "PAUSE";
			case CONFIG_STEERING: return "STEERING";
			case CONFIG_CAMERA: return "CAMERA";
			case CONFIG_RAGDOLL: return "NITRO / DRIVER LAUNCH";
			case CONFIG_RESET: return "RESET";
			case CONFIG_BRAKE: return "BRAKE / REVERSE";
			case CONFIG_THROTTLE: return "ACCELERATION";
			case CONFIG_LOOK: return "LOOK BACK";
			case CONFIG_THROTTLEBRAKE: return "ACCELERATION / BRAKE";
			case CONFIG_PLAYERLIST: return "PLAYER LIST";
			case CONFIG_SKIPMUSIC: return "SKIP MUSIC TRACK";
			default: return "";
		}
	}

	float fTextSize = 0.035;
	int nTextYOffset = 6;
	int nGlobalYOffset = 54;

	void Process() override {
		if (!bEnabled) return;
		if (!nControllerSupport) return;

		static auto texture = LoadTextureFromBFS("data/menu/xbox_controller.tga");
		static auto texData = LoadHUDData("data/menu/xbox_controller.bed", "xbox_controller");

		auto controller = GetHUDData(texData, "controller");

		float pos[] = {226,162+43};
		pos[1] += nGlobalYOffset;
		float size[] = {189,131};
		Draw1080pSprite(JUSTIFY_480P_CENTER, pos[0], pos[0] + size[0], pos[1], pos[1] + size[1], {255,255,255,255}, texture, controller->min, controller->max);

		for (auto& bind : aBindings) {
			std::string action;
			for (auto& key : bind.keys) {
				auto newAction = GetControllerBindingName(GetControllerBinding(key));;
				if (!newAction[0]) continue;
				if (!action.empty()) action += ", ";
				action += newAction;
			}
			if (action.empty()) action = "N/A";

			tNyaStringData data;
			data.x = bind.pos[0];
			data.y = bind.pos[1] + nGlobalYOffset + 22 + nTextYOffset;
			data.size = fTextSize;
			switch (bind.align) {
				case FONTF_LEFT:
				default:
					break;
				case FONTF_CENTER:
					data.XCenterAlign = true;
					break;
				case FONTF_RIGHT:
					data.XRightAlign = true;
					break;
			}
			Draw1080pString(JUSTIFY_480P_CENTER, data, action, &DrawStringFO2_Ingame12);

			if (bind.icon.empty()) continue;

			auto icon = GetHUDData(texData, bind.icon);
			if (!icon) continue;

			Draw1080pSprite(JUSTIFY_480P_CENTER, bind.iconpos[0], bind.iconpos[0] + bind.iconsize[0], bind.iconpos[1] + nGlobalYOffset + 22, bind.iconpos[1] + nGlobalYOffset + 22 + bind.iconsize[1], {255,255,255,255}, texture, icon->min, icon->max);
		}
	}
} Menu_Controller_Layout;