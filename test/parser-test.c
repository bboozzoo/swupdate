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
}

int main(int argc, char *argv[])
{
	struct swupdate_cfg swup;
	bzero(&swup, sizeof(swup));

	parse_cfg(&swup, TEST_LIBCONFIG_FILE, NULL);

	return 0;
}
