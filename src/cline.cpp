/* xoreos - A reimplementation of BioWare's Aurora engine
 *
 * xoreos is the legal property of its developers, whose names can be
 * found in the AUTHORS file distributed with this source
 * distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 3
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 *
 * The Infinity, Aurora, Odyssey, Eclipse and Lycium engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 */

/** @file cline.cpp
 *  Command line arguments parser.
 */

#include <cstdio>
#include <cstring>

#include "cline.h"

#include "common/ustring.h"
#include "common/util.h"

#include "common/configman.h"

static void displayUsage(const char *name) {
	std::printf("Usage: %s [options] [target]\n\n", name);
	std::printf("          --help              This text\n");
	std::printf("  -cFILE  --config=FILE       Load the config from file FILE\n");
	std::printf("  -pDIR   --path=DIR          Override the game path with DIR\n");
	std::printf("  -wSIZE  --width=SIZE        Set the window's width to SIZE\n");
	std::printf("  -hSIZE  --height=SIZE       Set the window's height to SIZE\n");
	std::printf("  -fBOOL  --fullscreen=BOOL   Switch fullscreen on/off\n");
	std::printf("  -kBOOL  --skipvideos=BOOL   Disable videos on/off\n");
	std::printf("  -vVOL   --volume=VOL        Set global volume to VOL\n");
	std::printf("  -mVOL   --volume_music=VOL  Set music volume to VOL\n");
	std::printf("  -sVOL   --volume_sfx=VOL    Set SFX volume to VOL\n");
	std::printf("  -oVOL   --volume_voice=VOL  Set voice volume to VOL\n");
	std::printf("  -iVOL   --volume_video=VOL  Set video volume to VOL\n");
	std::printf("  -dLVL   --debuglevel=LVL    Set the debug level to LVL\n");
	std::printf("          --debugchannel=CHAN Set the enabled debug channel(s) to CHAN.\n");
	std::printf("          --listdebug         List all available debug channels.\n");
	std::printf("          --logfile=FILE      Write all debug output into this file too.\n");
	std::printf("\n");
	std::printf("FILE: Absolute or relative path to a file.\n");
	std::printf("DIR:  Absolute or relative path to a directory.\n");
	std::printf("SIZE: A positive integer.\n");
	std::printf("BOOL: \"true\", \"yes\" and \"1\" are true, everything else is false.\n");
	std::printf("VOL:  A double ranging from 0.0 (min) - 1.0 (max).\n");
	std::printf("LVL:  A positive integer.\n");
	std::printf("CHAN: A comma-separated list of debug channels.\n");
	std::printf("      Use \"All\" to enable all debug channels.\n");
	std::printf("\n");
	std::printf("Examples:\n");
	std::printf("%s -p/path/to/nwn/\n", name);
	std::printf("  xoreos will start the game in /path/to/nwn/. Should a target with this\n");
	std::printf("  path not yet exist in the config file, xoreos will create one named \"nwn\".\n");
	std::printf("%s -p/path/to/nwn/ foobar\n", name);
	std::printf("  xoreos will start the game in /path/to/nwn/. If a target \"foobar\"\n");
	std::printf("  does not yet exist in the config file, xoreos will create it.\n");
	std::printf("%s nwn\n", name);
	std::printf("  xoreos will start the game specified by target \"nwn\", which must exit\n");
	std::printf("  in the config file already.\n");
	std::printf("\n");
}

static Common::UString convertShortToLongOption(char shortOption) {
	if (shortOption == 'p')
		return "path";
	if (shortOption == 'c')
		return "config";
	if (shortOption == 'f')
		return "fullscreen";
	if (shortOption == 'w')
		return "width";
	if (shortOption == 'h')
		return "height";
	if (shortOption == 'k')
		return "skipvideos";
	if (shortOption == 'v')
		return "volume";
	if (shortOption == 'm')
		return "volume_music";
	if (shortOption == 's')
		return "volume_sfx";
	if (shortOption == 'o')
		return "volume_voice";
	if (shortOption == 'i')
		return "volume_video";
	if (shortOption == 'd')
		return "debuglevel";

	return "";
}

static bool setOption(Common::UString &key, const Common::UString &value) {
	if (key.equalsIgnoreCase("config")) {
		ConfigMan.setConfigFile(value);
		if (!ConfigMan.load()) {
			if (!ConfigMan.fileExists())
				warning("No such config file \"%s\"", value.c_str());
			return false;
		}

		key.clear();
		return true;
	}

	ConfigMan.setCommandlineKey(key, value);
	key.clear();
	return true;
}

static bool parseOption(const char *arg, Common::UString &key) {
	if (arg[1] == '\0') {
		warning("Unrecognized command line argument \"%s\"", arg);
		return false;
	}

	Common::UString value;
	const char *start = arg + 1;
	if (*start == '-') {
		// Long option

		start++;

		const char *e = strchr(start, '=');
		if (e) {
			key = Common::UString(start, e - start);
			value = e + 1;
		} else
			key = start;

	} else {
		// Short option

		key   = convertShortToLongOption(*start);
		value = start + 1;
	}

	if (key.empty()) {
		warning("Unrecognized command line argument \"%s\"", arg);
		return false;
	}

	if (value.empty())
		return true;

	if (!setOption(key, value))
		return false;

	return true;
}

bool parseCommandline(int argc, char **argv, Common::UString &target, int &code) {
	target.clear();

	Common::UString key;

	bool stopMark = false;
	for (int i = 1; i < argc; i++) {

		if (!key.empty()) {
		// Still got one parameter missing from last time
			if (!setOption(key, argv[i])) {
				code = 1;
				return false;
			}

			continue;
		}

		if (!stopMark && (argv[i][0] == '-')) {
			if (!strcmp(argv[i], "--")) {
				stopMark = true;
				continue;
			}

			if (!strcmp(argv[i], "--help")) {
				code = 0;
				displayUsage(argv[0]);
				return false;
			}

			if (!parseOption(argv[i], key)) {
				code = 1;
				return false;
			}

			if (key == "listdebug") {
				setOption(key, "true");
				key.clear();
			}

			continue;
		}

		if (!target.empty()) {
			warning("Found multiple target (\"%s\" and \"%s\")", target.c_str(), argv[i]);
			code = 1;
			return false;
		}

		target = argv[i];
	}

	if (target.empty() && !ConfigMan.hasKey("path") && !ConfigMan.getBool("listdebug", false)) {
		displayUsage(argv[0]);
		code = 1;
		return false;
	}

	return true;
}
