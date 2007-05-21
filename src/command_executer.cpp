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

#include "command_executer.hpp"
#include "server.hpp"

#include <glibmm/spawn.h>
#include <glibmm/miscutils.h>

Sobby::CommandExecuter::CommandExecuter(const Server& server,
                                        ServerBuffer& buffer):
	m_server(server), m_buffer(buffer)
{
	buffer.get_command_map().add_command(
		"remove",
		"Removes a document from the session",
		sigc::mem_fun(*this, &CommandExecuter::on_remove)
	);

	if(!server.get_command_dir().empty())
	{
		buffer.get_command_map().add_command(
			"command",
			"Executes a command on the server",
			sigc::mem_fun(*this, &CommandExecuter::on_command)
		);
	}
}

obby::command_result Sobby::CommandExecuter::
	on_remove(const obby::user& from,
                  const std::string& paramlist)
{
	obby::command_paramlist list(paramlist);
	if(list.count() == 0)
	{
		return obby::command_result(
			obby::command_result::REPLY,
			"no_doc_given"
		);
	}

	try
	{
		obby::command_context_from<DocumentInfo*> ctx(m_buffer);
		DocumentInfo* info = ctx.from_string(list.value(0));

		m_buffer.document_remove(*info);
	}
	catch(serialise::conversion_error& e)
	{
		return obby::command_result(
			obby::command_result::REPLY,
			"doc_not_found"
		);
	}

	return obby::command_result(obby::command_result::NO_REPLY);
}

obby::command_result Sobby::CommandExecuter::
	on_command(const obby::user& from,
                   const std::string& paramlist)
{
	try
	{
		obby::command_paramlist list(paramlist);
		if(list.count() == 0)
		{
			m_buffer.send_message("Usage: /command <script to execute> <params>");
		}
		else
		{
			std::vector<std::string> argv;
			argv.push_back(Glib::path_get_basename(list.value(0)));
			argv.push_back(from.get_name());
			for(int i = 1; i < list.count(); ++ i)
				argv.push_back(list.value(i));

			int out_fd;
			Glib::spawn_async_with_pipes(m_server.get_command_dir(), argv, Glib::SpawnFlags(0), sigc::slot<void>(), NULL, NULL, &out_fd, NULL);

			// Collect stdout to send it to the client eventually
			Glib::signal_io().connect(
				sigc::bind(sigc::mem_fun(*this, &CommandExecuter::on_out), out_fd),
				out_fd,
				Glib::IO_IN | Glib::IO_ERR | Glib::IO_HUP
			);
		}
	}
	catch(Glib::Exception& e)
	{
		m_buffer.send_message(e.what());
	}
	catch(std::exception& e)
	{
		m_buffer.send_message(e.what());
	}

	return obby::command_result(obby::command_result::NO_REPLY);
}

bool Sobby::CommandExecuter::on_out(Glib::IOCondition io, int fd)
{
	bool done = false;

	if(io & (Glib::IO_ERR | Glib::IO_HUP))
		done = true;

	if(!done && (io & Glib::IO_IN))
	{
		gchar buffer[2048];
		ssize_t res = ::read(fd, buffer, 2048);

		if(res <= 0) done = true;
		else m_command_out[fd].append(buffer, res);
	}

	if(done)
	{
		m_buffer.send_message(m_command_out[fd]);
		m_command_out.erase(fd);
		return false;
	}

	return true;
}
