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
#include <glibmm/spawn.h>
#include "autosaver.hpp"

Sobby::AutoSaver::AutoSaver(const ServerBuffer& buffer,
                            const std::string& filename,
                            unsigned int interval,
			    const std::string& post_save_hook):
	m_buffer(buffer), m_filename(filename),
	m_post_save_hook(post_save_hook), m_has_modification(true)
{
	if(interval > 0)
	{
		m_conn_timer = Glib::signal_timeout().connect(
			sigc::mem_fun(*this, &AutoSaver::on_timer),
			interval * 1000
		);
	}

	buffer.document_insert_event().connect(
		sigc::mem_fun(*this, &AutoSaver::on_document_insert));
	buffer.document_remove_event().connect(
		sigc::mem_fun(*this, &AutoSaver::on_document_remove));

	for(ServerBuffer::document_iterator iter = buffer.document_begin();
	    iter != buffer.document_end(); ++ iter)
	{
		on_document_insert(*iter);
	}
}

Sobby::AutoSaver::~AutoSaver()
{
	m_conn_timer.disconnect();
}

void Sobby::AutoSaver::on_document_insert(DocumentInfo& document)
{
	document.get_content().changed_event().connect(
		sigc::mem_fun(*this, &AutoSaver::on_document_change));

	m_has_modification = true;
}

void Sobby::AutoSaver::on_document_remove(DocumentInfo& document)
{
}

void Sobby::AutoSaver::on_document_change()
{
	m_has_modification = true;
}

Sobby::AutoSaver::signal_error_type Sobby::AutoSaver::error_event() const
{
	return m_signal_error;
}

void Sobby::AutoSaver::save()
{
	try
	{
		/*std::list<std::string> list;
		list.push_back(Glib::get_tmp_dir() );
		list.push_back("autosave.obby.tmp");
		std::string tmp_file(Glib::build_filename(list) );

		m_buffer.serialise(tmp_file);
		if(std::rename(tmp_file.c_str(), m_filename.c_str()) == -1)
			throw std::runtime_error(std::strerror(errno) );*/

		m_buffer.serialise(m_filename);
		m_has_modification = false;

		if(!m_post_save_hook.empty())
		{
			std::vector<std::string> argv;
			argv.push_back(m_post_save_hook);
			argv.push_back(m_filename);

			Glib::spawn_async(".", argv);
		}
	}
	catch(std::exception& e)
	{
		m_signal_error.emit(e);
	}
	catch(Glib::Exception& e)
	{
		// TODO: Convert to locale?
		m_signal_error.emit(std::runtime_error(e.what()));
	}
}

bool Sobby::AutoSaver::on_timer()
{
	if(m_has_modification) save();
	return true;
}
