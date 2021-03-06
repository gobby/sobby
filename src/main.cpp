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
#include <glibmm/exception.h>
#include <glibmm/miscutils.h>
#include "server.hpp"

#include <csignal>
#include <cstring>
#include <cerrno>

namespace
{
	Sobby::Server* server_;

	void on_signal_usr1(int sig)
	{
		if(server_) server_->save();
	}

	void on_signal_term(int sig)
	{
		if(server_)
		{
			server_->save();
			server_->quit();
		}
	}
}

int main(int argc, char* argv[]) try
{
	// Create server, parse command line options
	Sobby::Server server(argc, argv);
	// Set global server_ variable for use in signal handler
	server_ = &server;
	// Register SIGUSR1 to save session there
	if(signal(SIGUSR1, on_signal_usr1) != 0)
	{
		int save_errno = errno;
		std::cerr << "Could not register SIGUSR1: "
		          << std::strerror(save_errno) << std::endl;
	}
	// Register SIGTERM to save session there
	if(signal(SIGTERM, on_signal_term) != 0)
	{
		int save_errno = errno;
		std::cerr << "Could not register SIGTERM: "
		          << std::strerror(save_errno) << std::endl;
	}
	// Run it
	server.run();
	// Success, if no exception has been thrown
	return EXIT_SUCCESS;
}
catch(Sobby::Server::Exit& exit)
{
	// Server exit
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
