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

#ifndef _SOBBY_SERVER_HPP_
#define _SOBBY_SERVER_HPP_

#include <net6/non_copyable.hpp>
#include <sigc++/trackable.h>
#include <glibmm/refptr.h>
#include <glibmm/main.h>
#include "io/buffer_wrapper.hpp"

#ifdef WITH_HOWL
# include <obby/zeroconf.hpp>
#endif

namespace Sobby
{

class Server : private net6::non_copyable,
               public sigc::trackable
{
public:
	// TODO: Extra class cmdmap?
	typedef std::vector<std::string> ArgList;
	typedef bool(Server::*ArgFunc)(const ArgList&);
	typedef std::map<std::string, ArgFunc> CommandMap;

	Server(int argc, char* argv[]);
	~Server();

        void run();

	bool on_cmd_exit(const ArgList& args);
	bool on_cmd_help(const ArgList& args);
	bool on_cmd_users(const ArgList& args);
	bool on_cmd_documents(const ArgList& args);
protected:
	virtual bool on_stdin(Glib::IOCondition condition);

	int m_port;
	bool m_interactive;

	Glib::RefPtr<Glib::MainLoop> m_main_loop;
	std::auto_ptr<obby::server_buffer> m_server;
#ifdef WITH_HOWL
	std::auto_ptr<obby::zeroconf> m_zeroconf;
#endif

	static const CommandMap& m_cmd_map;
};

}

#endif // _SOBBY_SERVER_HPP_

