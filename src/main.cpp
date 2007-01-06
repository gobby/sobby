/* sobby - A standalone server for obby
 * Copyright (C) 2005 0x539 dev group
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with this program; if not, write to the Free
 * Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <memory>
#include <iostream>
#include <stdexcept>
#include <signal.h>
#include <glibmm/exception.h>
#include <glibmm/miscutils.h>
#include "config.hpp"
#include "server.hpp"

namespace
{
	std::auto_ptr<Sobby::Config> config;
}

void sig_int_handler(int signal)
{
	// Free config - this makes sure the config is written in case
	// of SIGINT.
	config.reset(NULL);
	// TODO: Should also shutdown server properly?
	exit(EXIT_SUCCESS);
}

int main(int argc, char* argv[]) try
{
	signal(SIGINT, sig_int_handler);

	// Load config
	config.reset(
		new Sobby::Config(Glib::get_home_dir() + "/.sobby/config.xml")
	);

	// Create server, parse command line options
	Sobby::Server server(*config, argc, argv);

	// Run it
	server.run();
	// Success, if no exception has been thrown
	return EXIT_SUCCESS;
}
catch(std::exception& e)
{
	std::cerr << e.what() << std::endl;
	return EXIT_FAILURE;
}
catch(Glib::Exception& e)
{
	std::cerr << e.what() << std::endl;
	return EXIT_FAILURE;
}
