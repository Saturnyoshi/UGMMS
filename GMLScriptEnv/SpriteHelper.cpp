#include "stdafx.h"
#include "gml.h"
#include "GMLInternals.h"
#include "SpriteHelper.h"
#include "DebugTools.h"
#include "HelperHelper.h"

namespace SpriteHelper {
	int spriteCount = -1;

	void __InitialSetup() {
		if (spriteCount != -1) return;
		spriteCount = HelperHelper::countResource(GMLInternals::getFunctionID("sprite_exists"));
	}

	int getSpriteCount() {
		return spriteCount;
	}

	void spriteLoaderMod() {
		// TODO - clean up any memory leaks here
		// Fetch function IDs
		int sprite_get_name = GMLInternals::getFunctionID("sprite_get_name");
		int sprite_add = GMLInternals::getFunctionID("sprite_add");
		int sprite_assign = GMLInternals::getFunctionID("sprite_assign");
		int sprite_get_number = GMLInternals::getFunctionID("sprite_get_number");
		int sprite_get_xoffset = GMLInternals::getFunctionID("sprite_get_xoffset");
		int sprite_get_yoffset = GMLInternals::getFunctionID("sprite_get_yoffset");
		int sprite_get_speed = GMLInternals::getFunctionID("sprite_get_speed");
		int sprite_get_speed_type = GMLInternals::getFunctionID("sprite_get_speed_type");
		int sprite_set_speed = GMLInternals::getFunctionID("sprite_set_speed");
		int file_exists = GMLInternals::getFunctionID("file_exists");
		int ini_open = GMLInternals::getFunctionID("ini_open");
		int ini_read_real = GMLInternals::getFunctionID("ini_read_real");
		int ini_close = GMLInternals::getFunctionID("ini_close");
		// Sprite speed isn't a thing in GMS1 so we need to account for that
		bool supportSpeed = (sprite_set_speed != -1);
		// Loop over every sprite ID
		for (int i = 0; i < SpriteHelper::getSpriteCount(); i++) {
			// Get the name of the sprite
			GMLVar* argsGet[] = { &GMLVar(i) };
			GMLVar* name = GMLInternals::callGMLFunction(sprite_get_name, 1, argsGet, false);
			// See if the file exists
			GMLVar fileName = GMLVar(("sprites/" + name->getString() + ".png").c_str());
			GMLVar* argsExists[] = { &fileName };
			GMLVar* exists = GMLInternals::callGMLFunction(file_exists, 1, argsExists, false);

			// If the file exists...
			if (exists->truthy()) {
				// Fetch sprite properties
				GMLVar* speed = NULL;
				GMLVar* speedType = NULL;
				GMLVar* xorigin = GMLInternals::callGMLFunction(sprite_get_xoffset, 1, argsGet, false);
				GMLVar* yorigin = GMLInternals::callGMLFunction(sprite_get_yoffset, 1, argsGet, false);
				if (supportSpeed) {
					speed = GMLInternals::callGMLFunction(sprite_get_speed, 1, argsGet, false);
					speedType = GMLInternals::callGMLFunction(sprite_get_speed_type, 1, argsGet, false);
				}
				GMLVar* number = GMLInternals::callGMLFunction(sprite_get_number, 1, argsGet, false);

				// Look for ini
				GMLVar iniFile = GMLVar(("sprites/" + name->getString() + ".ini").c_str());
				GMLVar* argsIni[] = { &iniFile };
				GMLVar* iniExists = GMLInternals::callGMLFunction(file_exists, 1, argsIni, false);
				// If the ini exists...
				if (iniExists->truthy()) {
					// Open it
					GMLInternals::callGMLFunction(ini_open, 1, argsIni, true);
					// Store original properties to delete
					GMLVar* speedOld = NULL;
					GMLVar* speedTypeOld = NULL;
					GMLVar* xoriginOld = xorigin;
					GMLVar* yoriginOld = yorigin;
					if (supportSpeed) {
						speedOld = speed;
						speedTypeOld = speedType;
					}
					GMLVar* numberOld = number;
					// Read from the ini
					GMLVar* argsIniXoffset[] = { &GMLVar("sprite"), &GMLVar("xoffset"), xoriginOld };
					xorigin = GMLInternals::callGMLFunction(ini_read_real, 3, argsIniXoffset, false);
					GMLVar* argsIniYoffset[] = { &GMLVar("sprite"), &GMLVar("yoffset"), yoriginOld };
					yorigin = GMLInternals::callGMLFunction(ini_read_real, 3, argsIniYoffset, false);
					if (supportSpeed) {
						GMLVar* argsIniSpeed[] = { &GMLVar("sprite"), &GMLVar("speed"), speedOld };
						speed = GMLInternals::callGMLFunction(ini_read_real, 3, argsIniSpeed, false);
						GMLVar* argsIniSpeedType[] = { &GMLVar("sprite"), &GMLVar("speedType"), speedTypeOld };
						speedType = GMLInternals::callGMLFunction(ini_read_real, 3, argsIniSpeedType, false);
					}
					GMLVar* argsIniFrames[] = { &GMLVar("sprite"), &GMLVar("frames"), numberOld };
					number = GMLInternals::callGMLFunction(ini_read_real, 3, argsIniFrames, false);
					// Close the ini
					GMLInternals::callGMLFunction(ini_close, 0, NULL, true);
					// Clean old properties
					delete xoriginOld;
					delete yoriginOld;
					if (supportSpeed) {
						delete speedOld;
						delete speedTypeOld;
					}
					delete numberOld;
				}
				// Clean up ini stuff
				iniFile.freeString();
				delete iniExists;

				// Load new sprite
				GMLVar* argsSpriteAdd[] = { &fileName, number, &GMLVar(0), &GMLVar(0), xorigin, yorigin };
				GMLVar* newSprite = GMLInternals::callGMLFunction(sprite_add, 6, argsSpriteAdd, false);
				if (newSprite->getReal() > 0) {
					// Set final properties
					if (supportSpeed) {
						GMLVar* argsSetSpeed[] = { newSprite, speed, speedType };
						GMLInternals::callGMLFunction(sprite_set_speed, 3, argsSetSpeed, true);
					}
					// Replace the old sprite!
					GMLVar* argsSpriteAssign[] = { &GMLVar(i), newSprite };
					GMLInternals::callGMLFunction(sprite_assign, 2, argsSpriteAssign, true);
				}
				else {
					// Failed to load :(
					MessageBoxA(GetActiveWindow(), ("Failed to load the sprite " + name->getString()).c_str(), "Load Error", MB_OK);
				}

				// Clean up
				delete number;
				delete xorigin;
				delete yorigin;
				if (supportSpeed) {
					delete speed;
					delete speedType;
				}
				delete newSprite;
			}

			// Clean up
			name->valueString->refCountGML -= 1;
			fileName.freeString();
			delete name;
			delete exists;
		}
	}
}
