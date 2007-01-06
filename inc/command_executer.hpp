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

#ifndef _SOBBY_COMMAND_EXECUTER_HPP_
#define _SOBBY_COMMAND_EXECUTER_HPP_

#include <sigc++/trackable.h>
#include <net6/non_copyable.hpp>
#include <obby/command.hpp>
#include "buffer_def.hpp"

// Defines some commands for sobby
namespace Sobby
{

class CommandExecuter: public sigc::trackable, private net6::non_copyable
{
public:
	CommandExecuter(ServerBuffer& buffer);

protected:
	obby::command_result on_remove(const obby::user& from,
	                               const std::string& paramlist);

	ServerBuffer& m_buffer;
};

}

#endif // _SOBBY_COMMAND_EXECUTER_HPP_

