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

#ifndef _SOBBY_AUTOSAVE_FOLDER_HPP_
#define _SOBBY_AUTOSAVE_FOLDER_HPP_

#include <stdexcept>
#include <map>
#include <net6/non_copyable.hpp>
#include <sigc++/trackable.h>
#include <sigc++/connection.h>
#include <sigc++/signal.h>
#include "buffer_def.hpp"

namespace Sobby
{

/** This class handles autosaving: It automatically saves every document
 * within the session in regular intervals into a given folder.
 */
class AutoSaveFolder: private net6::non_copyable, public sigc::trackable
{
public:
	typedef sigc::signal<void, const std::exception&> signal_error_type;

	AutoSaveFolder(const ServerBuffer& buffer,
	               const std::string& folder,
	               unsigned int interval);
	~AutoSaveFolder();

	signal_error_type error_event() const;
protected:
	typedef std::map<DocumentInfo*, bool> map_type;

	void on_document_insert(DocumentInfo& document);
	void on_document_remove(DocumentInfo& document);
	void on_document_change(DocumentInfo& document);

	bool on_timer();

	const ServerBuffer& m_buffer;
	std::string m_folder;
	sigc::connection m_conn_timer;

	map_type m_map;

	signal_error_type m_signal_error;
};

}

#endif // _SOBBY_AUTOSAVE_FOLDER_HPP_
