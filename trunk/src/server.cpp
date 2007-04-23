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

#include <iostream>
#include <glibmm/fileutils.h>
#include <glibmm/optionentry.h>
#include <glibmm/optiongroup.h>
#include <glibmm/optioncontext.h>
#include "features.hpp"
#include "server.hpp"
#include "common.hpp"

namespace
{
	const Sobby::Server::CommandMap& create_cmd_map()
	{
		static Sobby::Server::CommandMap map;

		map["exit"] = &Sobby::Server::on_cmd_exit;
		map["help"] = &Sobby::Server::on_cmd_help;
		map["users"] = &Sobby::Server::on_cmd_users;
		map["documents"] = &Sobby::Server::on_cmd_documents;

		return map;
	}

	std::vector<std::string> split(const std::string& line,
	                               char separator)
	{
		std::vector<std::string> res;
		std::string::size_type pos = 0, prev = 0;
		while( (pos = line.find(separator, pos)) != std::string::npos)
		{
			res.push_back(line.substr(prev, pos - prev) );
			prev = ++ pos;
		}
		res.push_back(line.substr(prev) );
		return res;
	}
}

const Sobby::Server::CommandMap& Sobby::Server::m_cmd_map = create_cmd_map();

Sobby::Server::Server(int argc, char* argv[]):
	m_interactive(false),
	m_main_loop(Glib::MainLoop::create() )
#ifdef WITH_AVAHI
	,m_glib_poll(NULL)
#endif
{
	Glib::ustring name;
	Glib::ustring password;
	const char* session = NULL;

	std::string autosave_file;
	std::string autosave_folder;
	int autosave_interval;

	std::string config_file_read;
	std::string config_file_write;

	std::auto_ptr<Config> config;

	// Parse command line once to read config file
	Glib::OptionEntry opt_config_file_read;
	opt_config_file_read.set_short_name('c');
	opt_config_file_read.set_long_name("configfile");
	opt_config_file_read.set_arg_description("FILE");
	opt_config_file_read.set_description(
		"Read FILE for further configuration options"
	);

	Glib::OptionGroup opt_group_pre("pre", "You will never see this", "Really.");

	opt_group_pre.add_entry_filename(
		opt_config_file_read,
		config_file_read
	);

	Glib::OptionContext opt_config_ctx;
	opt_config_ctx.set_help_enabled(false);
	opt_config_ctx.set_ignore_unknown_options(true);

	opt_config_ctx.add_group(opt_group_pre);
	opt_config_ctx.parse(argc, argv);

	if(!config_file_read.empty() )
		config.reset(new Config(config_file_read) );

	// Set default options from config
	if(config.get() != NULL)
	{
		Config::ParentEntry& entry = config->get_root()["settings"];

		m_port = entry.get_value<unsigned int>(
			"port", 6522
		);

		name = entry.get_value<Glib::ustring>(
			"name", "Standalone obby server"
		);
		//m_interactive = config->get_value<bool>(
		//	"interactive", false
		//);

		autosave_folder = entry.get_value<std::string>(
			"autosave_directory", ""
		);

		autosave_file = entry.get_value<std::string>(
			"autosave_file", "autosave.obby"
		);

		autosave_interval = entry.get_value<unsigned int>(
			"autosave_interval", 0
		);
		password = entry.get_value<Glib::ustring>("password", "");

		m_command_dir = entry.get_value<Glib::ustring>(
			"command_directory", ""
		);
	}
	else
	{
		m_port = 6522;
		name = "Standalone obby server";
		autosave_folder = "";
		autosave_file = "autosave.obby";
		autosave_interval = 0;
		password = "";
		m_command_dir = "";
	}

	// Parse another time to get remaining options

	Glib::OptionGroup opt_group_common("common", "Common options",
		"General options");
	Glib::OptionGroup opt_group_net("net", "Networking options",
		"Options to set up the network");
	Glib::OptionGroup opt_group_auth("auth", "Authentication options",
		"Options to secure the obby server");

	Glib::OptionEntry opt_common_name;
	opt_common_name.set_short_name('n');
	opt_common_name.set_long_name("name");
	opt_common_name.set_arg_description("NAME");
	opt_common_name.set_description("Published server name");

	Glib::OptionEntry opt_common_interactive;
	opt_common_interactive.set_short_name('i');
	opt_common_interactive.set_long_name("interactive");
	opt_common_interactive.set_description(
		"Show prompt to enter commands at run-time"
	);

	Glib::OptionEntry opt_common_autosave_folder;
	opt_common_autosave_folder.set_long_name("autosave-directory");
	opt_common_autosave_folder.set_arg_description("FOLDER");
	opt_common_autosave_folder.set_description(
		"Folder where to store autosaved documents. "
	);

	Glib::OptionEntry opt_common_autosave_file;
	opt_common_autosave_file.set_long_name("autosave-file");
	opt_common_autosave_file.set_arg_description("FILE");
	opt_common_autosave_file.set_description(
		"File where to store autosaved sessions. "
		"Ignored if autosave-directory is set."
	);

	Glib::OptionEntry opt_common_autosave_interval;
	opt_common_autosave_interval.set_long_name("autosave-interval");
	opt_common_autosave_interval.set_arg_description("INTERVAL");
	opt_common_autosave_interval.set_description(
		"Interval (in seconds) between autosaves; 0 disables autosave"
	);

	Glib::OptionEntry opt_common_command_dir;
	opt_common_command_dir.set_long_name("command-directory");
	opt_common_command_dir.set_arg_description("DIRECTORY");
	opt_common_command_dir.set_description(
		"Directory where to find executable scripts which clients "
		"might execute"
	);

	Glib::OptionEntry opt_net_port;
	opt_net_port.set_short_name('p');
	opt_net_port.set_long_name("port");
	opt_net_port.set_arg_description("PORT");
	opt_net_port.set_description(
		"Port to run the obby server on"
	);

	Glib::OptionEntry opt_auth_password;
	opt_auth_password.set_long_name("password");
	opt_auth_password.set_arg_description("PASSWORD");
	opt_auth_password.set_description(
		"Global password required to join the session"
	);

	Glib::OptionEntry opt_common_config_file_write;
	opt_common_config_file_write.set_long_name("write-configfile");
	opt_common_config_file_write.set_arg_description("FILE");
	opt_common_config_file_write.set_description(
		"Store settings into FILE and exit"
	);

	opt_group_common.add_entry(opt_common_name, name);
	opt_group_common.add_entry(opt_common_interactive, m_interactive);

	opt_group_common.add_entry_filename(
		opt_common_autosave_folder,
		autosave_folder
	);

	opt_group_common.add_entry_filename(
		opt_common_autosave_file,
		autosave_file
	);

	opt_group_common.add_entry(
		opt_common_autosave_interval,
		autosave_interval
	);

	opt_group_common.add_entry_filename(
		opt_common_command_dir,
		m_command_dir
	);

	// To display config file in help output
	opt_group_common.add_entry_filename(
		opt_config_file_read,
		config_file_read
	);

	opt_group_common.add_entry_filename(
		opt_common_config_file_write,
		config_file_write
	);

	opt_group_net.add_entry(opt_net_port, m_port);

	opt_group_auth.add_entry(opt_auth_password, password);

	Glib::OptionContext opt_ctx("[file1 or session] [file2] [...]");
	opt_ctx.set_help_enabled(true);

	opt_ctx.set_main_group(opt_group_common);
	opt_ctx.add_group(opt_group_net);
	opt_ctx.add_group(opt_group_auth);

	opt_ctx.parse(argc, argv);

	// Get session file
	if(argc > 1)
		session = argv[1];

	// Set default settings
	// TODO: Should set them before parsing, but the parser resets
	// the value if the option is not given at all. Patch
	// is committed in glibmm CVS.
	if(config.get() != NULL)
	{
		Config::ParentEntry& entry = config->get_root()["settings"];

		if(m_port == 0)
			m_port = entry.get_value<unsigned int>("port", 6522);

		if(name.empty() )
			name = entry.get_value<std::string>(
				"name", "Standalone obby server"
			);

		if(autosave_folder.empty())
			autosave_folder = entry.get_value<std::string>(
				"autosave_directory", ""
			);

		if(autosave_file.empty() )
			autosave_file = entry.get_value<std::string>(
				"autosave_file", "autosave.obby"
			);

		if(autosave_interval == 0)
			autosave_interval = entry.get_value<unsigned int>(
				"autosave_interval", 0
			);

		if(password.empty() )
			password = entry.get_value<Glib::ustring>(
				"password", ""
			);

		if(m_command_dir.empty())
			m_command_dir = entry.get_value<Glib::ustring>(
				"command_directory", ""
			);
	}
	else
	{
		if(m_port == 0) m_port = 6522;
		if(name.empty() ) name = "Standalone obby server";
		if(autosave_folder.empty()) autosave_folder = "";
		if(autosave_file.empty() ) autosave_file = "autosave.obby";
		if(autosave_interval == 0) autosave_interval = 0;
		if(password.empty() ) password = "";
		if(m_command_dir.empty()) m_command_dir = "";
	}

	if(!config_file_write.empty() )
	{
		std::cout << "Sobby " << sobby_version() << " writing "
		          << "config file " << config_file_write << "..."
		          << std::endl;

		Config config;
		Config::ParentEntry& entry = config.get_root()["settings"];

		entry.set_value("port", m_port);
		entry.set_value("name", name);
		entry.set_value("autosave_file", autosave_file);
		entry.set_value("autosave_directory", autosave_folder);
		entry.set_value("autosave_interval", autosave_interval);
		entry.set_value("password", password);
		entry.set_value("command_directory", m_command_dir);

		config.save(config_file_write);

		// Should be better than calling std::exit() to ensure cleanup
		throw Exit();
	}

	std::cout << "Sobby " << sobby_version() << " starting up..."
	          << std::endl;

	// Start server
	m_server.reset(new ServerBuffer);
	m_server->set_enable_keepalives(true);

	if(session == NULL)
	{
		m_server->open(m_port);
	}
	else
	{
		try
		{
			m_server->open(session, m_port);
		}
		catch(std::exception& e)
		{
			// It is not an obby session file, so load it as
			// normal document.
			if(m_server->is_open()) m_server->close();
			m_server->open(m_port);

			for(int i = 1; i < argc; ++ i)
			{
				std::string content = Glib::file_get_contents(
					argv[i]
				);

				if(!g_utf8_validate(content.c_str(), content.length(), NULL))
				{
					obby::format_string errstring("The file '%0%' does not contain valid UTF-8. Please convert it to UTF-8 before loading it into sobby.");
					errstring << argv[i];
					throw std::runtime_error(errstring.str());
				}

				m_server->document_create(argv[i], "UTF-8", content);
			}
		}
	}

	m_server->set_global_password(password);

	if(autosave_interval > 0)
	{
		if(autosave_folder.empty())
		{
			m_autosaver.reset(
				new AutoSaver(
					*m_server,
					autosave_file,
					autosave_interval
				)
			);

			m_autosaver->error_event().connect(
				sigc::mem_fun(*this, &Server::on_autosave_error) );
		}
		else
		{
			if(!Glib::file_test(autosave_folder, Glib::FILE_TEST_IS_DIR))
			{
				obby::format_string str("'%0%' is not a directory");
				str << autosave_folder;
				throw std::runtime_error(str.str()); 
			}

			m_autosave_folder.reset(
				new AutoSaveFolder(
					*m_server,
					autosave_folder,
					autosave_interval
				)
			);

			m_autosave_folder->error_event().connect(
				sigc::mem_fun(*this, &Server::on_autosave_error) );
		}
	}

	m_command_executer.reset(new CommandExecuter(*this, *m_server) );

#ifdef WITH_ZEROCONF
	try
	{
#ifdef WITH_AVAHI
		m_glib_poll = avahi_glib_poll_new(NULL, G_PRIORITY_DEFAULT);
		m_zeroconf.reset(new obby::zeroconf_avahi(avahi_glib_poll_get(m_glib_poll)));
#else
		m_zeroconf.reset(new obby::zeroconf);
		// Process zeroconf events periodically
		Glib::signal_timeout().connect(sigc::bind(sigc::mem_fun(*m_zeroconf.get(), &obby::zeroconf_base::select), 0), 1500);
#endif
		m_zeroconf->publish(name, m_port);
	}
	catch(std::runtime_error&)
	{
#ifdef WITH_AVAHI
		std::cerr << "ERROR: Avahi initialisation failed. Please run "
		          << "avahi-daemon prior to Sobby." << std::endl;
#else
		std::cerr << "ERROR: Howl initialisation failed. Please run "
		          << "mDNSResponder prior to Sobby." << std::endl;
#endif
		std::cerr << "       Zeroconf support is thus deactivated for "
		          << "this session." << std::endl;
		m_zeroconf.reset();
	}
#endif
}

Sobby::Server::~Server()
{
#ifdef WITH_AVAHI
	if(m_glib_poll != NULL)
	{
		avahi_glib_poll_free(m_glib_poll);
	}
#endif
}

void Sobby::Server::run()
{
	std::cout << "Running server on port " << m_port
	          << " using obby " << obby_version() << std::endl;

	if(m_interactive)
	{
		// Install signal handler on stdin
		Glib::RefPtr<Glib::IOChannel> stdin_channel =
			Glib::IOChannel::create_from_fd(0);

		// Connect to signal_io()
		Glib::signal_io().connect(
			sigc::mem_fun(*this, &Server::on_stdin),
			stdin_channel,
			Glib::IO_IN
		);

		// TODO: Readline, somehow?

		// Show prompt
		std::cout << "sobby > "; std::cout.flush();
	}

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

	// Get arglist
	ArgList arglist = split(line, ' ');

	// First is command
	std::string command = arglist[0];
	arglist.erase(arglist.begin() );

	// Lookup command in cmdmap
	CommandMap::const_iterator iter = m_cmd_map.find(command);
	if(iter == m_cmd_map.end() )
	{
		// Command not found
		std::cerr << command << ": Command not found" << std::endl;
		std::cout << "sobby > "; std::cout.flush();
		return true;
	}

	// Split into command and arguments, execute command
	if ((this->*(iter->second))(arglist) )
	{
		// Reshow prompt
		std::cout << "sobby > "; std::cout.flush();
	}

	return true;
}

void Sobby::Server::on_autosave_error(const std::exception& e)
{
	std::cerr << "Autosave failed: " << e.what() << std::endl;

	if(m_interactive)
	{
		// TODO: show_prompt method?
		std::cout << "sobby > "; std::cout.flush();
	}
}

bool Sobby::Server::on_cmd_help(const ArgList& args)
{
	std::cout << "\thelp           Show this help" << std::endl;
	std::cout << "\texit           Exits the server" << std::endl;
	std::cout << "\tusers          Show currently connected users" << std::endl;
	std::cout << "\tdocuments      Show open documents" << std::endl;
	return true;
}

bool Sobby::Server::on_cmd_exit(const ArgList& args)
{
	// 'exit'-command: Exit application
	m_main_loop->quit();
	return false;
}

bool Sobby::Server::on_cmd_users(const ArgList& args)
{
	const obby::user_table& user_table = m_server->get_user_table();
	for(obby::user_table::iterator iter =
		user_table.begin(
			obby::user::flags::CONNECTED,
			obby::user::flags::NONE
		);
	    iter != user_table.end(
		obby::user::flags::CONNECTED,
		obby::user::flags::NONE
	    );
	    ++ iter)
	{
		std::cout << " * " << iter->get_name() << std::endl;
	}

	unsigned int count = user_table.count(
		obby::user::flags::CONNECTED,
		obby::user::flags::NONE
	);

	std::cout << count << " users" << std::endl;
	return true;
}

bool Sobby::Server::on_cmd_documents(const ArgList& args)
{
	for(Buffer::document_iterator iter = m_server->document_begin();
	    iter != m_server->document_end();
	    ++ iter)
	{
		std::cout << " * " << iter->get_title() << std::endl;
	}

	std::cout << m_server->document_count() << " documents" << std::endl;
	return true;
}

const std::string& Sobby::Server::get_command_dir() const
{
	return m_command_dir;
}
