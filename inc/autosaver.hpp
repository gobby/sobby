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

#ifndef _SOBBY_AUTOSAVER_HPP_
#define _SOBBY_AUTOSAVER_HPP_

#include <stdexcept>
#include <net6/non_copyable.hpp>
#include <sigc++/trackable.h>
#include <sigc++/connection.h>
#include <sigc++/signal.h>
#include <obby/server_buffer.hpp>

namespace Sobby
{

/** This class handles autosaving: It automatically saves the session in
 * regular intervals to a given file.
 */
class AutoSaver: private net6::non_copyable, public sigc::trackable
{
public:
	typedef sigc::signal<void, const std::exception&> signal_error_type;

	AutoSaver(const obby::server_buffer& buffer,
	          const std::string& filename,
	          unsigned int interval);
	~AutoSaver();

	signal_error_type error_event() const;
protected:
	bool on_timer();

	const obby::server_buffer& m_buffer;
	std::string m_filename;
	sigc::connection m_conn_timer;
	signal_error_type m_signal_error;
};

}

#endif // _SOBBY_SERVER_HPP_
