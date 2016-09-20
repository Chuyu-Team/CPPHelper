#ifndef RAPIDXML_PRINT_HPP_INCLUDED
#define RAPIDXML_PRINT_HPP_INCLUDED

// Copyright (C) 2006, 2009 Marcin Kalicinski
// Version 1.13
// Revision $DateTime: 2009/05/13 01:46:17 $
//! \file rapidxml_print.hpp This file contains rapidxml printer implementation

#include "rapidxml.hpp"
#include <atlstr.h>

namespace rapidxml
{

    ///////////////////////////////////////////////////////////////////////
    // Printing flags

    const int print_no_indenting = 0x1;   //!< Printer flag instructing the printer to suppress indenting of XML. See print() function.

    ///////////////////////////////////////////////////////////////////////
    // Internal

    //! \cond internal
    namespace internal
    {
        
        ///////////////////////////////////////////////////////////////////////////
        // Internal character operations
    
        // Copy characters from given range to given output iterator
		template<class Ch, class CStringT>
		inline void copy_chars(const Ch *Str, DWORD cStr, CStringT& out)
		{
			DWORD OldLength= out.GetLength();

			memcpy(out.GetBufferSetLength(OldLength + cStr)+ OldLength, Str, cStr*sizeof(Ch));
		}
   //     template<class Ch, class CStringT>
   //     inline void copy_chars(const Ch *begin, const Ch *end, CStringT& out)
   //     {
			//copy_chars(begin, end- begin, out);
   //         //while (begin != end)
   //         //    out+= *begin++;
   //         //return out;
   //     }
        
        // Copy characters from given range to given output iterator and expand
        // characters into references (&lt; &gt; &apos; &quot; &amp;)
        template<class Ch, class CStringT>
		inline void copy_and_expand_chars(const Ch *begin, const Ch *end, Ch noexpand, CStringT& out)
        {
			const Ch *CopyStart = begin;

            while (begin != end)
            {
                if (*begin == noexpand)
                {
                    //*out++ = *begin;    // No expansion, copy character
					//out += *begin;
					++begin;    // Step to next character
                }
                else
                {
                    switch (*begin)
                    {
                    case Ch('<'):
                        //*out++ = Ch('&'); *out++ = Ch('l'); *out++ = Ch('t'); *out++ = Ch(';');
						//out += L"&lt;";
						copy_chars(CopyStart, begin - CopyStart, out);
						//CopyStart = begin+1;
						copy_chars(L"&lt;",4, out);
                        break;
                    case Ch('>'): 
                        //*out++ = Ch('&'); *out++ = Ch('g'); *out++ = Ch('t'); *out++ = Ch(';');
						//out += L"&gt;";
						copy_chars(CopyStart, begin - CopyStart, out);
						copy_chars(L"&gt;", 4, out);
                        break;
                    case Ch('\''): 
                        //*out++ = Ch('&'); *out++ = Ch('a'); *out++ = Ch('p'); *out++ = Ch('o'); *out++ = Ch('s'); *out++ = Ch(';');
						//out += L"&apos;";
						copy_chars(CopyStart, begin - CopyStart, out);
						copy_chars(L"&apos;", 6, out);
                        break;
                    case Ch('"'): 
                        //*out++ = Ch('&'); *out++ = Ch('q'); *out++ = Ch('u'); *out++ = Ch('o'); *out++ = Ch('t'); *out++ = Ch(';');
						//out += L"&quot;";
						copy_chars(CopyStart, begin - CopyStart, out);
						copy_chars(L"&quot;", 6, out);
                        break;
                    case Ch('&'): 
                        //*out++ = Ch('&'); *out++ = Ch('a'); *out++ = Ch('m'); *out++ = Ch('p'); *out++ = Ch(';'); 
						//out += L"&amp;";
						copy_chars(CopyStart, begin - CopyStart, out);
						copy_chars(L"&amp;", 5, out);
                        break;
                    default:
                        //out+= *begin;    // No expansion, copy character
						++begin;
						continue;
						break;
                    }

					CopyStart = ++begin;
                }
                
            }

			copy_chars(CopyStart, begin- CopyStart, out);
            //return out;
        }

        // Fill given output iterator with repetitions of the same character
        template<class Ch, class CStringT>
		inline void fill_chars(CStringT& out, int n, Ch ch)
        {
			//加大缓冲区，防止内存抖动
			out.GetBuffer(out.GetLength() + n);
            for (int i = 0; i != n; ++i)
				out+= ch;
           // return out;
        }

        // Find character
       /* template<class Ch, Ch ch>
        inline bool find_char(const Ch *begin, const Ch *end)
        {
            while (begin != end)
                if (*begin++ == ch)
                    return true;
            return false;
        }*/

        ///////////////////////////////////////////////////////////////////////////
        // Internal printing operations
    
        // Print node
        template<class Ch,class CStringT>
		inline void print_node(CStringT& out, const xml_node<Ch> *node, int flags, int indent)
        {
            // Print proper node type
            switch (node->type())
            {

            // Document
            case node_document:
                print_children(out, node, flags, indent);
                break;

            // Element
            case node_element:
                print_element_node(out, node, flags, indent);
                break;
            
            // Data
            case node_data:
                print_data_node(out, node, flags, indent);
                break;
            
            // CDATA
            case node_cdata:
                print_cdata_node(out, node, flags, indent);
                break;

            // Declaration
            case node_declaration:
                print_declaration_node(out, node, flags, indent);
                break;

            // Comment
            case node_comment:
                print_comment_node(out, node, flags, indent);
                break;
            
            // Doctype
            case node_doctype:
                print_doctype_node(out, node, flags, indent);
                break;

            // Pi
            case node_pi:
                print_pi_node(out, node, flags, indent);
                break;

                // Unknown
            default:
                assert(0);
                break;
            }
            
            // If indenting not disabled, add line break after node
            if (!(flags & print_no_indenting))
                out += Ch('\n');

            // Return modified iterator
            //return out;
        }
        
        // Print children of the node                               
        template<class Ch, class CStringT>
		inline void print_children(CStringT& out, const xml_node<Ch> *node, int flags, int indent)
        {
            for (xml_node<Ch> *child = node->first_node(); child; child = child->next_sibling())
                print_node(out, child, flags, indent);
            //return out;
        }

        // Print attributes of the node
        template<class Ch, class CStringT>
		inline void print_attributes(CStringT& out, const xml_node<Ch> *node, int flags)
        {
            for (xml_attribute<Ch> *attribute = node->first_attribute(); attribute; attribute = attribute->next_attribute())
            {
                if (attribute->name() && attribute->value())
                {
                    // Print attribute name
                    //out += Ch(' ');
                    //out.Append(attribute->name(), attribute->name_size());

					//out += Ch('=');// , ++out;

					DWORD OldLength = out.GetLength();
					DWORD NameSize= attribute->name_size();

					Ch* pData=out.GetBufferSetLength(OldLength+3+ NameSize)+ OldLength;

					*pData = Ch(' ');
					++pData;
					memcpy(pData, attribute->name(), NameSize*sizeof(Ch));
					pData += NameSize;
					*pData = Ch('=');
					++pData;
					*pData = Ch('"');

                    // Print attribute value using appropriate quote type
                   // if (/*find_char<Ch, Ch('"')>(attribute->value(), attribute->value() + attribute->value_size())*/StrChrNW(attribute->value(),L'"', attribute->value_size()))
                    //{
						//out += Ch('\'');// , ++out;
      //                  copy_and_expand_chars(attribute->value(), attribute->value() + attribute->value_size(), Ch('"'), out);
						//out += Ch('\'');// , ++out;
      //              }
      //              else
                    {
						//out += Ch('"');// , ++out;
                        copy_and_expand_chars(attribute->value(), attribute->value() + attribute->value_size(), Ch('\''), out);
						out += Ch('"');// , ++out;
                    }
                }
            }
           // return out;
        }

        // Print data node
        template<class Ch, class CStringT>
        inline void print_data_node(CStringT& out, const xml_node<Ch> *node, int flags, int indent)
        {
            assert(node->type() == node_data);
            if (!(flags & print_no_indenting))
                fill_chars(out, indent, Ch('\t'));

            copy_and_expand_chars(node->value(), node->value() + node->value_size(), Ch(0), out);
            //return out;
        }

        // Print data node
        template<class Ch, class CStringT>
        inline void print_cdata_node(CStringT& out, const xml_node<Ch> *node, int flags, int indent)
        {
            assert(node->type() == node_cdata);
            if (!(flags & print_no_indenting))
                fill_chars(out, indent, Ch('\t'));

            /**out += Ch('<'); ++out;
            *out = Ch('!'); ++out;
            *out = Ch('['); ++out;
            *out = Ch('C'); ++out;
            *out = Ch('D'); ++out;
            *out = Ch('A'); ++out;
            *out = Ch('T'); ++out;
            *out = Ch('A'); ++out;
            *out = Ch('['); ++out;*/
			out += L"<![CDATA[";
			out.Append(node->value() , node->value_size());
           /* *out = Ch(']'); ++out;
            *out = Ch(']'); ++out;
            *out = Ch('>'); ++out;*/
			out += L"]]>";
            //return out;
        }

        // Print element node
        template<class Ch, class CStringT>
        inline void print_element_node(CStringT& out, const xml_node<Ch> *node, int flags, int indent)
        {
            assert(node->type() == node_element);

            // Print element name and attributes, if any
            if (!(flags & print_no_indenting))
                fill_chars(out, indent, Ch('\t'));

			out += Ch('<');// , ++out;
			out.Append(node->name() , node->name_size());
			//copy_chars(node->name(), node->name() + node->name_size(), out);
            print_attributes(out, node, flags);
            
            // If node is childless
            if (node->value_size() == 0 && !node->first_node())
            {
                // Print childless node tag ending
				out += Ch('/');// , ++out;
				out += Ch('>');// , ++out;
            }
            else
            {
                // Print normal node tag ending
				out += Ch('>');// , ++out;

                // Test if node contains a single data node only (and no other nodes)
                xml_node<Ch> *child = node->first_node();
                if (!child)
                {
                    // If node has no children, only print its value without indenting
                    copy_and_expand_chars(node->value(), node->value() + node->value_size(), Ch(0), out);
                }
                else if (child->next_sibling() == 0 && child->type() == node_data)
                {
                    // If node has a sole data child, only print its value without indenting
                    copy_and_expand_chars(child->value(), child->value() + child->value_size(), Ch(0), out);
                }
                else
                {
                    // Print all children with full indenting
					if (!(flags & print_no_indenting))
						out += Ch('\n');// , ++out;
                    print_children(out, node, flags, indent + 1);

                    if (!(flags & print_no_indenting))
                        fill_chars(out, indent, Ch('\t'));
                }

                // Print node end
				out += Ch('<');// , ++out;
				out += Ch('/');// , ++out;
                //copy_chars(node->name(), node->name() + node->name_size(), out);
				out.Append(node->name(), node->name_size());

				out += Ch('>');// , ++out;
            }
            //return out;
        }

        // Print declaration node
        template<class Ch, class CStringT>
        inline void print_declaration_node(CStringT& out, const xml_node<Ch> *node, int flags, int indent)
        {
            // Print declaration start
            if (!(flags & print_no_indenting))
                fill_chars(out, indent, Ch('\t'));

           /* *out = Ch('<'), ++out;
            *out = Ch('?'), ++out;
            *out = Ch('x'), ++out;
            *out = Ch('m'), ++out;
            *out = Ch('l'), ++out;*/
			out += L"<?xml";

            // Print attributes
            print_attributes(out, node, flags);
            
            // Print declaration end
			out += Ch('?');// , ++out;
			out += Ch('>');// , ++out;
            
            //return out;
        }

        // Print comment node
        template<class Ch, class CStringT>
        inline void print_comment_node(CStringT& out, const xml_node<Ch> *node, int flags, int indent)
        {
            assert(node->type() == node_comment);
            if (!(flags & print_no_indenting))
                fill_chars(out, indent, Ch('\t'));

            /**out = Ch('<'), ++out;
            *out = Ch('!'), ++out;
            *out = Ch('-'), ++out;
            *out = Ch('-'), ++out;*/
			out += L"<!--";
			out.Append(node->value(), node->value_size());
            
            /**out = Ch('-'), ++out;
            *out = Ch('-'), ++out;
            *out = Ch('>'), ++out;*/
			out += L"-->";

            //return out;
        }

        // Print doctype node
        template<class Ch,class CStringT>
        inline void print_doctype_node(CStringT& out, const xml_node<Ch> *node, int flags, int indent)
        {
            assert(node->type() == node_doctype);
            if (!(flags & print_no_indenting))
                fill_chars(out, indent, Ch('\t'));

           /* *out = Ch('<'), ++out;
            *out = Ch('!'), ++out;
            *out = Ch('D'), ++out;
            *out = Ch('O'), ++out;
            *out = Ch('C'), ++out;
            *out = Ch('T'), ++out;
            *out = Ch('Y'), ++out;
            *out = Ch('P'), ++out;
            *out = Ch('E'), ++out;
            *out = Ch(' '), ++out;*/
			out += L"<!DOCTYPE ";
			out.Append(node->value(), node->value_size());
			out += Ch('>');// , ++out;
            //return out;
        }

        // Print pi node
        template<class Ch,class CStringT>
        inline void print_pi_node(CStringT& out, const xml_node<Ch> *node, int flags, int indent)
        {
            assert(node->type() == node_pi);
            if (!(flags & print_no_indenting))
                fill_chars(out, indent, Ch('\t'));

			out += Ch('<');// , ++out;
			out += Ch('?');// , ++out;
			out.Append(node->name(), node->name_size());

			out += Ch(' ');// , ++out;
			out.Append(node->value(), node->value_size());
			out += Ch('?');// , ++out;
			out += Ch('>');// , ++out;
            //return out;
        }

    }
    //! \endcond

    ///////////////////////////////////////////////////////////////////////////
    // Printing

    //! Prints XML to given output iterator.
    //! \param out Output iterator to print to.
    //! \param node Node to be printed. Pass xml_document to print entire document.
    //! \param flags Flags controlling how XML is printed.
    //! \return Output iterator pointing to position immediately after last character of printed text.
    //template<class Ch> 
    inline CString print(const xml_node<wchar_t> &node, int flags = 0)
    {
		CString Temp;

		internal::print_node(Temp, &node, flags, 0);

		return Temp;
    }

	inline CStringA print(const xml_node<char> &node, int flags = 0)
	{
		CStringA Temp;

		internal::print_node(Temp, &node, flags, 0);

		return Temp;
	}

}

#endif
