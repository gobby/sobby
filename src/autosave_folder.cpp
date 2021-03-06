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
#include <fstream>

#include <stdexcept>
#include <glibmm/main.h>
#include <glibmm/miscutils.h>
#include "autosave_folder.hpp"

Sobby::AutoSaveFolder::AutoSaveFolder(const ServerBuffer& buffer,
                            const std::string& folder,
                            unsigned int interval):
	m_buffer(buffer), m_folder(folder)
{
	if(interval > 0)
	{
		m_conn_timer = Glib::signal_timeout().connect(
			sigc::mem_fun(*this, &AutoSaveFolder::on_timer),
			interval * 1000
		);
	}

	buffer.document_insert_event().connect(
		sigc::mem_fun(*this, &AutoSaveFolder::on_document_insert));
	buffer.document_remove_event().connect(
		sigc::mem_fun(*this, &AutoSaveFolder::on_document_remove));

	for(ServerBuffer::document_iterator iter = buffer.document_begin();
	    iter != buffer.document_end(); ++ iter)
	{
		on_document_insert(*iter);
	}
}

Sobby::AutoSaveFolder::~AutoSaveFolder()
{
	m_conn_timer.disconnect();
}

void Sobby::AutoSaveFolder::on_document_insert(DocumentInfo& document)
{
	m_map[&document] = true;

	document.get_content().changed_event().connect(
		sigc::bind(sigc::mem_fun(*this, &AutoSaveFolder::on_document_change), sigc::ref(document)));
}

void Sobby::AutoSaveFolder::on_document_remove(DocumentInfo& document)
{
	m_map.erase(&document);
}

void Sobby::AutoSaveFolder::on_document_change(DocumentInfo& document)
{
	m_map[&document] = true;
}

Sobby::AutoSaveFolder::signal_error_type Sobby::AutoSaveFolder::error_event() const
{
	return m_signal_error;
}

void Sobby::AutoSaveFolder::save()
{
	try
	{
		for(map_type::iterator iter = m_map.begin(); iter != m_map.end(); ++ iter)
		{
			if(!iter->second) continue;
			m_map[iter->first] = false;

			std::string content = iter->first->get_content().get_text();
			std::string filename = iter->first->get_title();
			std::string::size_type pos = filename.rfind('/');
			if(pos != std::string::npos)
				filename = filename.substr(pos + 1);

			std::string path = Glib::build_filename(m_folder, filename);

			// TODO: Use IOChannels here to get filename encoding
			// right?
			std::ofstream stream(path.c_str());
			if(!stream)
			{
				obby::format_string str("Could not open output file '%0%' for writing");
				str << path;
				throw std::runtime_error(str.str());
			}

			stream << content;
		}
	}
	catch(std::exception& e)
	{
		m_signal_error.emit(e);
	}

}

bool Sobby::AutoSaveFolder::on_timer()
{
	save();
	return true;
}
