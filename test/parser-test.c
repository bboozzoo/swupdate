/*
 * (C) Copyright 2015
 * Maciej Borzecki, Open-RnD, maciej.borzecki@open-rnd.pl.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc.
 */
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "swupdate.h"
#include "swupdate_status.h"
#include "parsers.h"
#include "util.h"

#define TEST_LIBCONFIG_FILE "test-config.cfg"

/* enable verbose logging by default */
int verbose = 1;

int get_hw_revision(struct hw_type *hw)
{
	TRACE("called\n");
	strncpy(hw->boardname, "test-board", sizeof(hw->boardname));
	strncpy(hw->revision, "test-rev", sizeof(hw->revision));
}

void notify(RECOVERY_STATUS status, int error, const char *msg)
{
	fprintf(stderr, msg);
}

int main(int argc, char *argv[])
{
	struct swupdate_cfg swup;
	const char *test_file = TEST_LIBCONFIG_FILE;
	const char *software_set = "stable";
	const char *running_mode = "main";

	memset(&swup, 0, sizeof(swup));

	if (argc >= 2)
		test_file = argv[1];

	if (argc >= 3)
		software_set = argv[2];

	if (argc >= 4)
		running_mode = argv[3];

	strncpy(swup.software_set, software_set, sizeof(swup.software_set));
	strncpy(swup.running_mode, running_mode, sizeof(swup.running_mode));

	parse_cfg(&swup, test_file);

	return 0;
}
