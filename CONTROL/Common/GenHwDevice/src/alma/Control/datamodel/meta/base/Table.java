/*
 * ALMA - Atacama Large Millimeter Array
 * (c) European Southern Observatory, 2002
 * (c) Associated Universities Inc., 2002
 * Copyright by ESO (in the framework of the ALMA collaboration),
 * Copyright by AUI (in the framework of the ALMA collaboration),
 * All rights reserved.
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY, without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307  USA
 *
 * File Table.java
 */
package alma.Control.datamodel.meta.base;

import java.util.Hashtable;
import java.util.ArrayList;

public class Table {

	public static final String DepChar = "^";
	public static final String CelsiusToKelvin = "CelsiusToKelvin";
	public static ArrayList sheetNames = null;
	public static ArrayList colNames = null;
	public static ArrayList tagNames = null;
	public static Hashtable toCPP = null;
	public static Hashtable toIDL = null;
	public static Hashtable toIDLSeq = null;
	public static Hashtable toCORBA = null;
	public static Hashtable toCORBASeq = null;
	public static Hashtable toCORBADevIO = null;
	public static Hashtable toBACI = null;
	public static Hashtable toDatabase = null;
	public static Hashtable rawToCPP = null;
	public static Hashtable rawToByteSwapped = null;
	public static Hashtable rawBytes = null;
	public static Hashtable worldToCPP = null;
	public static Hashtable worldToIDL = null;
	public static Hashtable worldToIDLSeq = null;
	public static Hashtable worldToCORBA = null;
	public static Hashtable worldToCORBASeq = null;
	public static Hashtable worldToCORBADevIO = null;
	public static Hashtable worldToBACI = null;
	public static Hashtable worldToJava = null;
	public static Hashtable worldToDatabase = null;

	public static int getSheetNum(String name)
	{
		int i;
		for(i = 0; i < sheetNames.size(); i++)
			if(((String)sheetNames.get(i)).compareTo(name) == 0)
				return i;
		return -1;
	}

	public static int getColNum(int sheet, String name)
	{
		int i;
		for(i = 0; i < ((ArrayList)colNames.get(sheet)).size(); i++)
			if(((String)((ArrayList)colNames.get(sheet)).get(i)).compareTo(name) == 0)
				return i;
		return -1;
	}

	public static void addSheet(String name)
	{
		sheetNames.add(name);
		colNames.add(new ArrayList());
	}

	public static void addCol(int sheet, String name)
	{
		((ArrayList)colNames.get(sheet)).add(name);
	}

	public static void initialize(String[][][] sh){
		int i, j;
		sheetNames = new ArrayList();
		colNames = new ArrayList();
		for(i = 0; i < sh.length; i++){
			addSheet(sh[i][0][0]);
			for(j = 0; j < sh[i][1].length; j++){
				addCol(i, sh[i][1][j]);
			}
		}
		toCPP = new Hashtable();
		toCPP.put("char", "char");
		toCPP.put("short", "short");
		toCPP.put("int", "int");
		toCPP.put("long long", "int64_t");
		toCPP.put("float", "float");
		toCPP.put("double", "double");
		toCPP.put("bool", "bool");
		toCPP.put("string", "char *");
		toCPP.put("void", "void");

		toIDL = new Hashtable();
		toIDL.put("char", "long");
		toIDL.put("short", "long");
		toIDL.put("int", "long");
		toIDL.put("long long", "long long");
		toIDL.put("float", "float");
		toIDL.put("double", "double");
		toIDL.put("bool", "boolean");
		toIDL.put("string", "string");
		toIDL.put("void", "void");

		toIDLSeq = new Hashtable();
		toIDLSeq.put("char", "Control::LongSeq");
		toIDLSeq.put("short", "Control::LongSeq");
		toIDLSeq.put("int", "Control::LongSeq");
		toIDLSeq.put("long long", "Control::LongLongSeq");
		toIDLSeq.put("float", "Control::FloatSeq");
		toIDLSeq.put("double", "Control::DoubleSeq");
		toIDLSeq.put("bool", "Control::BooleanSeq");
		toIDLSeq.put("string", "Control::StringSeq");
		toIDLSeq.put("void", "NOTHING");

		toCORBA = new Hashtable();
		toCORBA.put("char", "CORBA::Long");
		toCORBA.put("short", "CORBA::Long");
		toCORBA.put("int", "CORBA::Long");
		toCORBA.put("long long", "CORBA::LongLong");
		toCORBA.put("float", "CORBA::Float");
		toCORBA.put("double", "CORBA::Double");
		toCORBA.put("bool", "CORBA::Boolean");
		toCORBA.put("string", "char *");
		toCORBA.put("void", "void");

		toCORBADevIO = new Hashtable();
		toCORBADevIO.put("char", "CORBA::Long");
		toCORBADevIO.put("short", "CORBA::Long");
		toCORBADevIO.put("int", "CORBA::Long");
		toCORBADevIO.put("long long", "CORBA::LongLong");
		toCORBADevIO.put("float", "CORBA::Float");
		toCORBADevIO.put("double", "CORBA::Double");
		toCORBADevIO.put("bool", "CORBA::Boolean");
		toCORBADevIO.put("string", "CORBA::String");
		toCORBADevIO.put("void", "NOTHING");

		toCORBASeq = new Hashtable();
		toCORBASeq.put("char", "Control::LongSeq");
		toCORBASeq.put("short", "Control::LongSeq");
		toCORBASeq.put("int", "Control::LongSeq");
		toCORBASeq.put("long long", "Control::LongLongSeq");
		toCORBASeq.put("float", "Control::FloatSeq");
		toCORBASeq.put("double", "Control::DoubleSeq");
		toCORBASeq.put("bool", "Control::BooleanSeq");
		toCORBASeq.put("string", "Control::StringSeq");
		toCORBASeq.put("void", "NOTHING");

		toBACI = new Hashtable();
		toBACI.put("char", "long");
		toBACI.put("short", "long");
		toBACI.put("int", "long");
		toBACI.put("long long", "longLong");
		toBACI.put("float", "float");
		toBACI.put("double", "double");
		toBACI.put("bool", "boolean");
		toBACI.put("string", "string");
		toBACI.put("void", "void");

		toDatabase = new Hashtable();
		toDatabase.put("char", "char");
		toDatabase.put("short", "integer");
		toDatabase.put("int", "integer");
		toDatabase.put("long long", "integer");
		toDatabase.put("float", "float");
		toDatabase.put("double", "double");
		toDatabase.put("bool", "boolean");
		toDatabase.put("string", "string");
		toDatabase.put("void", "-none-");

		//toType = new Hashtable();
		//toType.put("char", "");
		//toType.put("short", "");
		//toType.put("int", "");
		//toType.put("long long", "");
		//toType.put("float", "");
		//toType.put("double", "");
		//toType.put("bool", "");
		//toType.put("string", "");
		//toType.put("void", "");

		rawToCPP = new Hashtable();
		rawToCPP.put("bit", "int8_t");
		rawToCPP.put("ubyte", "uint8_t");
		rawToCPP.put("int8", "char");
		rawToCPP.put("uint8", "uint8_t");
		rawToCPP.put("int16", "int16_t");
		rawToCPP.put("uint16", "uint16_t");
		rawToCPP.put("int24", "int32_t");
		rawToCPP.put("uint24", "uint32_t");
		rawToCPP.put("int32", "int32_t");
		rawToCPP.put("uint32", "uint32_t");
		rawToCPP.put("int48", "int32_t");
		rawToCPP.put("uint48", "uint64_t");
		rawToCPP.put("int64", "int64_t");
		rawToCPP.put("uint64", "uint64_t");
		rawToCPP.put("float", "float");
		rawToCPP.put("double", "double");

		rawToByteSwapped = new Hashtable();
		rawToByteSwapped.put("bit", "false");
		rawToByteSwapped.put("ubyte", "false");
		rawToByteSwapped.put("int8", "false");
		rawToByteSwapped.put("uint8", "false");
		rawToByteSwapped.put("int16", "true");
		rawToByteSwapped.put("uint16", "true");
		rawToByteSwapped.put("int24", "true");
		rawToByteSwapped.put("uint24", "true");
		rawToByteSwapped.put("int32", "true");
		rawToByteSwapped.put("uint32", "true");
		rawToByteSwapped.put("int48", "true");
		rawToByteSwapped.put("uint48", "true");
		rawToByteSwapped.put("int64", "true");
		rawToByteSwapped.put("uint64", "true");
		rawToByteSwapped.put("float", "true");
		rawToByteSwapped.put("double", "true");

		rawBytes = new Hashtable();
		rawBytes.put("bit", "1");
		rawBytes.put("ubyte", "1");
		rawBytes.put("int8", "1");
		rawBytes.put("uint8", "1");
		rawBytes.put("int16", "2");
		rawBytes.put("uint16", "2");
		rawBytes.put("int24", "3");
		rawBytes.put("uint24", "3");
		rawBytes.put("int32", "4");
		rawBytes.put("uint32", "4");
		rawBytes.put("int48", "6");
		rawBytes.put("uint48", "6");
		rawBytes.put("int64", "8");
		rawBytes.put("uint64", "8");
		rawBytes.put("float", "4");
		rawBytes.put("double", "8");

		worldToCPP = new Hashtable();
		worldToCPP.put("byte", "char");
		worldToCPP.put("ubyte", "uint8_t");
		worldToCPP.put("short", "int16_t");
		worldToCPP.put("ushort", "uint16_t");
		worldToCPP.put("int", "int32_t");
		worldToCPP.put("uint", "uint32_t");
		worldToCPP.put("long", "int64_t");
		worldToCPP.put("ulong", "uint64_t");
		worldToCPP.put("float", "float");
		worldToCPP.put("double", "double");
		worldToCPP.put("boolean", "bool");
		worldToCPP.put("string", "std::string");
		worldToCPP.put("acstime", "ACS::Time");
		worldToCPP.put("void", "void");

		worldToJava = new Hashtable();
		worldToJava.put("byte", "int");
		worldToJava.put("ubyte", "int");
		worldToJava.put("short", "int");
		worldToJava.put("ushort", "int");
		worldToJava.put("int", "int");
		worldToJava.put("uint", "long");
		worldToJava.put("long", "long");
		worldToJava.put("ulong", "long");
		worldToJava.put("float", "float");
		worldToJava.put("double", "double");
		worldToJava.put("boolean", "boolean");
		worldToJava.put("string", "String");
		worldToJava.put("acstime", "ACSTime");
		worldToJava.put("void", "void");

		/*
		 * TODO
		 * ACS does not provide property types for all CORBA types. :(
		 * So we just hope that nobody ever tries to store data which uses all
		 * bits of unsigned types since we are storing them in the signed
		 * equivalent.
		 *
		 * List of missing property types given as IDL types with the
		 * replacement given in paranthesis:
		 *
		 * byte (long)
		 * sequence < byte > (sequence< long >)
		 *
		 * octet (long)
		 * sequence< octet > (sequence< long >)
		 *
		 * unsigned short (long)
		 * sequence < unsigned short > (sequence< long >)
		 *
		 * short (long)
		 * sequence < short > (sequence< long >)
		 *
		 * unsigned long (unsigned long long)
		 * sequence< unsigned long > (sequence< long >, Data loss possible!)
		 *
		 * sequence< long long > (sequence< long >, Data loss possible!)
		 *
		 * sequence< unsigned long long > (sequence< long >, Data loss possible!)
		 */
		worldToBACI = new Hashtable();
		worldToBACI.put("byte", "long");
		worldToBACI.put("ubyte", "long");
		worldToBACI.put("short", "long");
		worldToBACI.put("ushort", "long");
		worldToBACI.put("int", "long");
		worldToBACI.put("uint", "uLong");
		worldToBACI.put("long", "longLong");
		worldToBACI.put("ulong", "uLongLong");
		worldToBACI.put("float", "float");
		worldToBACI.put("double", "double");
		worldToBACI.put("boolean", "boolean");
		worldToBACI.put("string", "string");
		worldToBACI.put("acstime", "uLongLong");
		worldToBACI.put("void", "void");

		/*
		 * TODO
		 * Same here. This is how it should be:
		worldToCORBA.put("byte", "CORBA::Byte");
		worldToCORBA.put("ubyte", "CORBA::Octet");
		worldToCORBA.put("short", "CORBA::Short");
		worldToCORBA.put("ushort", "CORBA::UShort");
		worldToCORBA.put("int", "CORBA::Long");
		worldToCORBA.put("uint", "CORBA::ULong");
		worldToCORBA.put("long", "CORBA::LongLong");
		worldToCORBA.put("ulong", "CORBA::ULongLong");
		worldToCORBA.put("float", "CORBA::Float");
		worldToCORBA.put("double", "CORBA::Double");
		worldToCORBA.put("boolean", "CORBA::Boolean");
		worldToCORBA.put("string", "char *");
		worldToCORBA.put("acstime", "ACS::Time");
		worldToCORBA.put("void", "void");
		 */
		worldToCORBA = new Hashtable();
		worldToCORBA.put("byte", "CORBA::Long");
		worldToCORBA.put("ubyte", "CORBA::Long");
		worldToCORBA.put("short", "CORBA::Long");
		worldToCORBA.put("ushort", "CORBA::Long");
		worldToCORBA.put("int", "CORBA::Long");
		worldToCORBA.put("uint", "CORBA::ULong");
		worldToCORBA.put("long", "CORBA::LongLong");
		worldToCORBA.put("ulong", "CORBA::ULongLong");
		worldToCORBA.put("float", "CORBA::Float");
		worldToCORBA.put("double", "CORBA::Double");
		worldToCORBA.put("boolean", "CORBA::Boolean");
		worldToCORBA.put("string", "char *");
		worldToCORBA.put("acstime", "ACS::Time");
		worldToCORBA.put("void", "void");

		worldToCORBADevIO = new Hashtable();
		worldToCORBADevIO.put("byte", "CORBA::Long");
		worldToCORBADevIO.put("ubyte", "CORBA::Long");
		worldToCORBADevIO.put("short", "CORBA::Long");
		worldToCORBADevIO.put("ushort", "CORBA::Long");
		worldToCORBADevIO.put("int", "CORBA::Long");
		worldToCORBADevIO.put("uint", "CORBA::ULong");
		worldToCORBADevIO.put("long", "CORBA::LongLong");
		worldToCORBADevIO.put("ulong", "CORBA::ULongLong");
		worldToCORBADevIO.put("float", "CORBA::Float");
		worldToCORBADevIO.put("double", "CORBA::Double");
		worldToCORBADevIO.put("boolean", "CORBA::Boolean");
		worldToCORBADevIO.put("string", "CORBA::String");
		worldToCORBADevIO.put("acstime", "ACS::Time");
		worldToCORBADevIO.put("void", "NOTHING");

		/*
		 * TODO
		 * Same here. This is how it should be:
		worldToCORBASeq.put("byte", "Control::ByteSeq");
		worldToCORBASeq.put("ubyte", "Control::OctetSeq");
		worldToCORBASeq.put("short", "Control::ShortSeq");
		worldToCORBASeq.put("ushort", "Control::UShortSeq");
		worldToCORBASeq.put("int", "Control::LongSeq");
		worldToCORBASeq.put("uint", "Control::ULongSeq");
		worldToCORBASeq.put("long", "Control::LongLongSeq");
		worldToCORBASeq.put("ulong", "Control::ULongLongSeq");
		worldToCORBASeq.put("float", "Control::FloatSeq");
		worldToCORBASeq.put("double", "Control::DoubleSeq");
		worldToCORBASeq.put("boolean", "Control::BooleanSeq");
		worldToCORBASeq.put("string", "Control::StringSeq");
		worldToCORBASeq.put("acstime", "Control::ACSTimeSeq");
		worldToCORBASeq.put("void", "NOTHING");
		 */
		worldToCORBASeq = new Hashtable();
		worldToCORBASeq.put("byte", "Control::LongSeq");
		worldToCORBASeq.put("ubyte", "Control::LongSeq");
		worldToCORBASeq.put("short", "Control::LongSeq");
		worldToCORBASeq.put("ushort", "Control::LongSeq");
		worldToCORBASeq.put("int", "Control::LongSeq");
		worldToCORBASeq.put("uint", "Control::ULongSeq");
		worldToCORBASeq.put("long", "Control::LongLongSeq");
		worldToCORBASeq.put("ulong", "Control::ULongLongSeq");
		worldToCORBASeq.put("float", "Control::FloatSeq");
		worldToCORBASeq.put("double", "Control::DoubleSeq");
		worldToCORBASeq.put("boolean", "Control::BooleanSeq");
		worldToCORBASeq.put("string", "Control::StringSeq");
		worldToCORBASeq.put("acstime", "Control::ACSTimeSeq");
		worldToCORBASeq.put("void", "NOTHING");

		/*
		 * TODO
		 * Same here. This is how it should be:
		worldToIDLSeq.put("byte", "Control::CharSeq");
		worldToIDLSeq.put("ubyte", "Control::OctetSeq");
		worldToIDLSeq.put("short", "Control::ShortSeq");
		worldToIDLSeq.put("ushort", "Control::UShortSeq");
		worldToIDLSeq.put("int", "Control::LongSeq");
		worldToIDLSeq.put("uint", "Control::ULongSeq");
		worldToIDLSeq.put("long", "Control::LongLongSeq");
		worldToIDLSeq.put("ulong", "Control::ULongLongSeq");
		worldToIDLSeq.put("float", "Control::FloatSeq");
		worldToIDLSeq.put("double", "Control::DoubleSeq");
		worldToIDLSeq.put("boolean", "Control::BooleanSeq");
		worldToIDLSeq.put("string", "Control::StringSeq");
		worldToIDLSeq.put("acstime", "Control::ACSTimeSeq");
		worldToIDLSeq.put("void", "NOTHING");
		*/
		worldToIDLSeq = new Hashtable();
		worldToIDLSeq.put("byte", "Control::LongSeq");
		worldToIDLSeq.put("ubyte", "Control::LongSeq");
		worldToIDLSeq.put("short", "Control::LongSeq");
		worldToIDLSeq.put("ushort", "Control::LongSeq");
		worldToIDLSeq.put("int", "Control::LongSeq");
		worldToIDLSeq.put("uint", "Control::ULongSeq");
		worldToIDLSeq.put("long", "Control::LongLongSeq");
		worldToIDLSeq.put("ulong", "Control::ULongLongSeq");
		worldToIDLSeq.put("float", "Control::FloatSeq");
		worldToIDLSeq.put("double", "Control::DoubleSeq");
		worldToIDLSeq.put("boolean", "Control::BooleanSeq");
		worldToIDLSeq.put("string", "Control::StringSeq");
		worldToIDLSeq.put("acstime", "Control::ACSTimeSeq");
		worldToIDLSeq.put("void", "NOTHING");

		/*
		 * TODO
		 * Same here. This is how it should be:
		worldToIDL.put("byte", "char");
		worldToIDL.put("ubyte", "octet");
		worldToIDL.put("short", "short");
		worldToIDL.put("ushort", "unsigned short");
		worldToIDL.put("int", "long");
		worldToIDL.put("uint", "unsigned long");
		worldToIDL.put("long", "long long");
		worldToIDL.put("ulong", "unsigned long long");
		worldToIDL.put("float", "float");
		worldToIDL.put("double", "double");
		worldToIDL.put("boolean", "boolean");
		worldToIDL.put("string", "string");
		worldToIDL.put("acstime", "ACS::Time");
		worldToIDL.put("void", "void");
		 */
		worldToIDL = new Hashtable();
		worldToIDL.put("byte", "long");
		worldToIDL.put("ubyte", "long");
		worldToIDL.put("short", "long");
		worldToIDL.put("ushort", "long");
		worldToIDL.put("int", "long");
		worldToIDL.put("uint", "unsigned long");
		worldToIDL.put("long", "long long");
		worldToIDL.put("ulong", "unsigned long long");
		worldToIDL.put("float", "float");
		worldToIDL.put("double", "double");
		worldToIDL.put("boolean", "boolean");
		worldToIDL.put("string", "string");
		worldToIDL.put("acstime", "ACS::Time");
		worldToIDL.put("void", "void");

		worldToDatabase = new Hashtable();
		worldToDatabase.put("byte", "integer");
		worldToDatabase.put("ubyte", "integer");
		worldToDatabase.put("short", "integer");
		worldToDatabase.put("ushort", "integer");
		worldToDatabase.put("int", "integer");
		worldToDatabase.put("uint", "integer");
		worldToDatabase.put("long", "integer");
		worldToDatabase.put("ulong", "integer");
		worldToDatabase.put("float", "float");
		worldToDatabase.put("double", "double");
		worldToDatabase.put("boolean", "boolean");
		worldToDatabase.put("string", "string");
		worldToDatabase.put("acstime", "integer");
		worldToDatabase.put("void", "-none-");
	}
}
