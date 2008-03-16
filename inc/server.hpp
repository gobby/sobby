/* sobby - A standalone server for obby
 * Copyright (C) 2005, 2006 0x539 dev group
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
#include "buffer_def.hpp"
#include "features.hpp"
#include "config.hpp"
#include "autosaver.hpp"
#include "autosave_folder.hpp"
#include "command_executer.hpp"

#ifdef WITH_ZEROCONF
# include <obby/zeroconf.hpp>
#endif

#ifdef WITH_AVAHI
# include <obby/zeroconf_avahi.hpp>
# include <avahi-glib/glib-watch.h>
#endif

namespace Sobby
{

class Server : private net6::non_copyable,
               public sigc::trackable
{
public:
	class Exit {};

	// TODO: Extra class cmdmap?
	typedef std::vector<std::string> ArgList;
	typedef bool(Server::*ArgFunc)(const ArgList&);
	typedef std::map<std::string, ArgFunc> CommandMap;

	Server(int argc, char* argv[]);
	~Server();

        void run();
	void save();

	bool on_cmd_exit(const ArgList& args);
	bool on_cmd_help(const ArgList& args);
	bool on_cmd_users(const ArgList& args);
	bool on_cmd_documents(const ArgList& args);

	const std::string& get_command_dir() const;
protected:
	virtual bool on_stdin(Glib::IOCondition condition);

	virtual void on_autosave_error(const std::exception& e);

	int m_port;
	bool m_interactive;
	std::string m_command_dir;

	Glib::RefPtr<Glib::MainLoop> m_main_loop;
	std::auto_ptr<ServerBuffer> m_server;
	std::auto_ptr<AutoSaver> m_autosaver;
	std::auto_ptr<AutoSaveFolder> m_autosave_folder;
	std::auto_ptr<CommandExecuter> m_command_executer;
#ifdef WITH_ZEROCONF
	std::auto_ptr<obby::zeroconf_base> m_zeroconf;
#endif
#ifdef WITH_AVAHI
	AvahiGLibPoll* m_glib_poll;
#endif

	static const CommandMap& m_cmd_map;
};

}

#endif // _SOBBY_SERVER_HPP_

