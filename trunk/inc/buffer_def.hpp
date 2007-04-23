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

#ifndef _SOBBY_BUFFER_DEF_HPP_
#define _SOBBY_BUFFER_DEF_HPP_

#include "gselector.hpp"
#include <obby/host_buffer.hpp>
#include <obby/server_buffer.hpp>
#include <obby/client_buffer.hpp>
#include <obby/local_buffer.hpp>
#include <obby/buffer.hpp>
#include <obby/host_document_info.hpp>
#include <obby/server_document_info.hpp>
#include <obby/client_document_info.hpp>
#include <obby/local_document_info.hpp>
#include <obby/document_info.hpp>

namespace Sobby
{

typedef obby::basic_buffer<obby::document, GSelector> Buffer;
typedef obby::basic_local_buffer<obby::document, GSelector> LocalBuffer;
typedef obby::basic_client_buffer<obby::document, GSelector> ClientBuffer;
typedef obby::basic_server_buffer<obby::document, GSelector> ServerBuffer;
typedef obby::basic_host_buffer<obby::document, GSelector> HostBuffer;

typedef Buffer::document_info_type DocumentInfo;
typedef LocalBuffer::document_info_type LocalDocumentInfo;
typedef ClientBuffer::document_info_type ClientDocumentInfo;
typedef ServerBuffer::document_info_type ServerDocumentInfo;
typedef HostBuffer::document_info_type HostDocumentInfo;

} // namespace Sobby

#endif // _SOBBY_BUFFER_DEF_HPP_
