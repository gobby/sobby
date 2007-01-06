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

Sobby::CommandExecuter::CommandExecuter(ServerBuffer& buffer):
	m_buffer(buffer)
{
	buffer.get_command_map().add_command(
		"remove",
		"Removes a document from the session",
		sigc::mem_fun(*this, &CommandExecuter::on_remove)
	);
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
		DocumentInfo* info = ctx.from_string(paramlist);

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
