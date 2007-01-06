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

#include <cstring>
#include <cstdio>
#include <cerrno>
#include <list>

#include <stdexcept>
#include <glibmm/main.h>
#include <glibmm/miscutils.h>
#include "autosaver.hpp"

Sobby::AutoSaver::AutoSaver(const obby::server_buffer& buffer,
                            const std::string& filename,
                            unsigned int interval):
	m_buffer(buffer), m_filename(filename)
{
	m_conn_timer = Glib::signal_timeout().connect(
		sigc::mem_fun(*this, &AutoSaver::on_timer),
		interval * 1000
	);
}

Sobby::AutoSaver::~AutoSaver()
{
	m_conn_timer.disconnect();
}

Sobby::AutoSaver::signal_error_type Sobby::AutoSaver::error_event() const
{
	return m_signal_error;
}

bool Sobby::AutoSaver::on_timer()
{
	try
	{
		std::list<std::string> list;
		list.push_back(Glib::get_tmp_dir() );
		list.push_back("autosave.obby.tmp");
		std::string tmp_file(Glib::build_filename(list) );

		m_buffer.serialise(tmp_file);
		if(std::rename(tmp_file.c_str(), m_filename.c_str()) == -1)
			throw std::runtime_error(std::strerror(errno) );
	}
	catch(std::exception& e)
	{
		m_signal_error.emit(e);
	}

	return true;
}
