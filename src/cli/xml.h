#pragma once

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/detail/xml_parser_utils.hpp>
#include <string>
#include <ostream>
#include <iomanip>

namespace boost { namespace property_tree { namespace xml_parser1
{
	template<class Ch>
	void write_xml_indent1(std::basic_ostream<Ch> &stream,
		int indent,
		const xml_writer_settings<Ch> & settings
		)
	{
		stream << std::basic_string<Ch>(indent * settings.indent_count, settings.indent_char);
	}

	template<class Ch>
	void write_xml_comment1(std::basic_ostream<Ch> &stream,
		const std::basic_string<Ch> &s, 
		int indent,
		bool separate_line,
		const xml_writer_settings<Ch> & settings
		)
	{
		typedef typename std::basic_string<Ch> Str;
		if (separate_line)
			write_xml_indent1(stream,indent,settings);
		stream << Ch('<') << Ch('!') << Ch('-') << Ch('-');
		stream << s;
		stream << Ch('-') << Ch('-') << Ch('>');
		if (separate_line)
			stream << Ch('\n');
	}

	template<class Ch>
	void write_xml_text1(std::basic_ostream<Ch> &stream,
		const std::basic_string<Ch> &s, 
		int indent, 
		bool separate_line,
		const xml_writer_settings<Ch> & settings
		)
	{
		if (separate_line)    
			write_xml_indent1(stream,indent,settings);
		stream << boost::property_tree::xml_parser::encode_char_entities(s);
		if (separate_line)
			stream << Ch('\n');
	}

	template<class Ptree>
	void write_xml_element1(std::basic_ostream<typename Ptree::key_type::value_type> &stream, 
		const std::string & parent,
		const std::basic_string<typename Ptree::key_type::value_type> &key,
		const Ptree &pt, 
		int indent,
		const xml_writer_settings<typename Ptree::key_type::value_type> & settings)
	{

		typedef typename Ptree::key_type::value_type Ch;
		typedef typename std::basic_string<Ch> Str;
		typedef typename Ptree::const_iterator It;

		bool want_pretty = settings.indent_count > 0;
		// Find if elements present
		bool has_elements = false;
		bool has_attrs_only = pt.data().empty();
		for (It it = pt.begin(), end = pt.end(); it != end; ++it)
		{
			if (it->first != boost::property_tree::xml_parser::xmlattr<Ch>() )
			{
				has_attrs_only = false;
				if (it->first != boost::property_tree::xml_parser::xmltext<Ch>())
				{
					has_elements = true;
					break;
				}
			}
		}

		// Write element
		if (pt.data().empty() && pt.empty())    // Empty key
		{
			if (indent >= 0)
			{
				write_xml_indent1(stream,indent,settings);
				stream << Ch('<') << key << 
					Ch('/') << Ch('>');
				if (want_pretty)
					stream << Ch('\n');
			}
		}
		else    // Nonempty key
		{

			// Write opening tag, attributes and data
			if (indent >= 0)
			{

				// Write opening brace and key
				write_xml_indent1(stream,indent,settings);
				stream << Ch('<') << key;

				// Write attributes
				if (optional<const Ptree &> attribs = pt.get_child_optional(boost::property_tree::xml_parser::xmlattr<Ch>()))
					for (It it = attribs.get().begin(); it != attribs.get().end(); ++it)
						stream << Ch(' ') << it->first << Ch('=')
						<< Ch('"')
						<< boost::property_tree::xml_parser::encode_char_entities(
						it->second.template get_value<std::basic_string<Ch> >())
						<< Ch('"');

				if ( has_attrs_only )
				{
					// Write closing brace
					stream << Ch('/') << Ch('>');
					if (want_pretty)
						stream << Ch('\n');
				}
				else
				{
					// Write closing brace
					stream << Ch('>');

					// Break line if needed and if we want pretty-printing
					if (has_elements && want_pretty)
						stream << Ch('\n');
				}
			}

			// Write data text, if present
			if (!pt.data().empty())
				write_xml_text1(stream,
				pt.template get_value<std::basic_string<Ch> >(),
				indent + 1, has_elements && want_pretty, settings);

			// Write elements, comments and texts
			for (It it = pt.begin(); it != pt.end(); ++it)
			{
				std::string id = it->first;

				if (it->first == boost::property_tree::xml_parser::xmlattr<Ch>())
					continue;
				else if (it->first == boost::property_tree::xml_parser::xmlcomment<Ch>())
					write_xml_comment1(stream,
					it->second.template get_value<std::basic_string<Ch> >(),
					indent + 1, want_pretty, settings);
				else if (it->first == boost::property_tree::xml_parser::xmltext<Ch>())
					write_xml_text1(stream,
					it->second.template get_value<std::basic_string<Ch> >(),
					indent + 1, has_elements && want_pretty, settings);
				else
				{
					std::string first = it->first;;
					if(id.empty())
					{
						if(parent == "devices")
						{
							first = "device";
						}
						else if(parent == "locations")
						{
							first = "location";
						}
						else if(parent == "users")
						{
							first = "user";
						}
						else if(parent == "devicelinks")
						{
							first = "link";
						}	
						else if(parent == "medialinks")
						{
							first = "link";
						}
						else if(parent == "channels")
						{
							first = "channel";
						}
						else if(parent == "scenes")
						{
							first = "scene";
						}
						else if(parent == "icons")
						{
							first = "icon";
						}
					}
					write_xml_element1(stream, id, first, it->second, indent + 1, settings);
				}
			}

			// Write closing tag
			if (indent >= 0 && !has_attrs_only)
			{
				if (has_elements)
					write_xml_indent1(stream,indent,settings);
				stream << Ch('<') << Ch('/') << key << Ch('>');
				if (want_pretty)
					stream << Ch('\n');
			}

		}
	}

	template<class Ptree>
	void write_xml_internal1(std::basic_ostream<typename Ptree::key_type::value_type> &stream, 
		const Ptree &pt,
		const std::string &filename,
		const xml_writer_settings<typename Ptree::key_type::value_type> & settings)
	{
		typedef typename Ptree::key_type::value_type Ch;
		typedef typename std::basic_string<Ch> Str;
		//stream  << detail::widen<Ch>("<?xml version=\"1.0\" encoding=\"")
		//	<< settings.encoding
		//	<< detail::widen<Ch>("\"?>\n");
		std::string parent(" ");
		write_xml_element1(stream, parent, Str(), pt, -1, settings);
		if (!stream)
			BOOST_PROPERTY_TREE_THROW(xml_parser_error("write error", filename, 0));
	}

	template<class Ptree>
	void write_xml(std::basic_ostream<typename Ptree::key_type::value_type> &stream,
		const Ptree &pt,
		const xml_writer_settings<typename Ptree::key_type::value_type> & 
			settings = xml_writer_settings<	typename Ptree::key_type::value_type>() )
	{
		write_xml_internal1(stream, pt, std::string(), settings);
	}
} } }

