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

#include <iostream>
#include <glibmm/optionentry.h>
#include <glibmm/optiongroup.h>
#include <glibmm/optioncontext.h>
#include "server.hpp"

namespace
{
	const Sobby::Server::CommandMap& create_cmd_map()
	{
		static Sobby::Server::CommandMap map;

		map["exit"] = &Sobby::Server::on_cmd_exit;

		return map;
	}
}

const Sobby::Server::CommandMap& Sobby::Server::m_cmd_map = create_cmd_map();

Sobby::Server::Server(int argc, char* argv[])
 : m_main_loop(Glib::MainLoop::create() )
{
	Glib::ustring name;
	Glib::ustring password;

	Glib::OptionEntry opt_common_name;
	Glib::OptionEntry opt_common_interactive;
	Glib::OptionEntry opt_net_port;
	Glib::OptionEntry opt_auth_password;
	
	opt_common_name.set_short_name('n');
	opt_common_name.set_long_name("name");
	opt_common_name.set_description("Published server name");

	opt_common_interactive.set_short_name('i');
	opt_common_interactive.set_long_name("Interactive");
	opt_common_interactive.set_description("Show prompt to enter commands "
	                                       "at run-time");

	opt_net_port.set_short_name('p');
	opt_net_port.set_long_name("port");
	opt_net_port.set_description("Port to run the obby server on");
	
	opt_auth_password.set_long_name("password");
	opt_auth_password.set_description(
		"Global password required to join the session");
	
	Glib::OptionGroup opt_group_common("common", "Common options",
		"General options");
	Glib::OptionGroup opt_group_net("net", "Networking options",
		"Options to set up the network");
	Glib::OptionGroup opt_group_auth("auth", "Authentication options",
		"Options to secure the obby server");
	
	opt_group_common.add_entry(opt_common_name, name);
	opt_group_common.add_entry(opt_common_interactive, m_interactive);

	opt_group_net.add_entry(opt_net_port, m_port);
	
	opt_group_auth.add_entry(opt_auth_password, password);
	
	Glib::OptionContext opt_ctx;
	opt_ctx.set_help_enabled(true);

	opt_ctx.add_group(opt_group_common);
	opt_ctx.add_group(opt_group_net);
	opt_ctx.add_group(opt_group_auth);
	
	opt_ctx.parse(argc, argv);
	
	// Default settings
	if(m_port == 0) m_port = 6522;
	if(name.empty() ) name = "Standalone obby server";

	// Start server
	std::cout << "Generating RSA key pair..." << std::endl;
	m_server.reset(new obby::io::server_buffer(m_port) );
}

Sobby::Server::~Server()
{
}

void Sobby::Server::run()
{
	std::cout << "Running obby server " << "0.2.0" /*PACKAGE_VERSION*/
	          << " on port " << m_port << std::endl;

	// Install signal handler on stdin
	Glib::RefPtr<Glib::IOChannel> stdin_channel =
		Glib::IOChannel::create_from_fd(0);

	// Connect to signal_io()
	Glib::signal_io().connect(
		sigc::mem_fun(*this, &Server::on_stdin),
		stdin_channel,
		Glib::IO_IN
	);

	// Show prompt
	if(m_interactive) std::cout << "sobby > "; std::cout.flush();

	// Run main loop
	m_main_loop->run();
}

bool Sobby::Server::on_stdin(Glib::IOCondition condition)
{
	std::string line;

	// ^D
	if(!std::getline(std::cin, line) )
	{
		if(m_interactive) std::cout << std::endl;
		m_main_loop->quit();
		return true;
	}

	// Do not evaluate command if not interactive
	if(!m_interactive) return true;

	// Lookup command in cmdmap
	CommandMap::const_iterator iter = m_cmd_map.find(line);
	if(iter == m_cmd_map.end() )
	{
		// Command not found
		std::cerr << line << ": Command not found" << std::endl;
		std::cout << "sobby > "; std::cout.flush();
		return true;
	}

	// TODO: Split into command and arguments, execute command
	ArgList list;
	if ((this->*(iter->second))(list) )
	{
		// Reshow prompt
		std::cout << "sobby > "; std::cout.flush();
	}

	return true;
}

bool Sobby::Server::on_cmd_exit(const ArgList& args)
{
	// 'exit'-command: Exit application
	m_main_loop->quit();
	return false;
}

