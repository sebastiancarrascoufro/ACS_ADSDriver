package alma.Control.datamodel.meta.base;

import org.openarchitectureware.core.constraint.DesignError;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileReader;
import java.io.IOException;
import java.io.LineNumberReader;
import java.io.PrintStream;
import java.io.StringReader;
import java.util.ArrayList;

public class SpreadsheetParser {

	private String xml;
	private String refPath;
	private boolean refStatus = false;
	private String xsdFile;
	private static String newline = System.getProperty("line.separator");

	public SpreadsheetParser(String xml) {
		this.xml = xml;
	}
	public static String getSpreadsheet(String dirName, String fileName) {
		File dir = new File(dirName);
		if (!dir.isDirectory())
			throw new DesignError ("Directory " + dirName + " does not exist.");
		File file = new File(dir, fileName);
		if (!file.exists())
			throw new DesignError ("File " + fileName + " in directory " + dirName + " does not exist.");
		BufferedReader in = null;
		StringBuffer xmlDoc = null;
		String line = null;
		try {
			in = new BufferedReader(new FileReader(file));
			//Read the entire file and store in it xmlDoc.
			xmlDoc = new StringBuffer();
			line = in.readLine();
			while (line != null) {
				xmlDoc.append(line + newline);
				//xmlDoc.append(line);
				line = in.readLine();
			}
			in.close();
		} catch (IOException e) {
			throw new DesignError(e.getMessage());
		}
		String xml = new String(xmlDoc);
		// Now, the entire xml file is in the string: xml.
		return xml;
	}
	private static Pair getBoundedContent(String s,
			String beginPart1, String beginPart2, String end) {
		int b1 = s.indexOf(beginPart1);
		if (b1 == -1)
			return null;
		int b2 = s.indexOf(beginPart2,b1);
		if (b2 == -1)
			return null;
		int e = s.indexOf(end,b2);
		if (e == -1)
			return null;
		String content = s.substring(b2 + 1,e).trim();
		int n = e + end.length();
		String newS = null;
		if (n < s.length())
			newS = s.substring(n);
		return new Pair(content,newS);
	}
	private static String[] getDataCells(String row) {
		ArrayList list = new ArrayList ();
		String data = null;
		String content = null;
		while (row != null && row.length() > 0) {
			Pair p = getBoundedContent(row,"<Cell",">","</Cell>");
			if (p == null)
				break;
			data = p.content;
			row = p.newString;
			p = getBoundedContent(data,"<Data",">","</Data>");
			if (p == null)
				continue;
			content = p.content;
			list.add(content);
		}
		if(list.size() == 0)
			return null;
		String[] x = new String [list.size()];
		return (String[])list.toArray(x);
	}
	public String[][][] getWorksheets() {
		ArrayList rowList = null;
		ArrayList sheetList = null;
		String sheet = null;
		String row = null;
		String[] data = null;
		sheetList = new ArrayList ();
		String doc = new String (xml);
		if (doc.startsWith("@reference")) {
			refStatus = true;
			return filter(doc);
		}
		Pair p = null;
		while (doc != null && doc.length() > 0) {
			p = getBoundedContent(doc,"<Worksheet",">","</Worksheet>");
			if (p == null)
				break;
			sheet = p.content;
			doc = p.newString;
			rowList = new ArrayList ();
			while (sheet != null && sheet.length() > 0) {
				Pair p2 = getBoundedContent(sheet,"<Row",">","</Row>");
				if (p2 == null)
					break;
				row = p2.content;
				sheet = p2.newString;
				data = getDataCells(row);
				if (data != null)
					rowList.add(data);
			}
			sheetList.add(rowList);
		}

		if(p == null){
			doc = new String(xml);
			while (doc != null && doc.length() > 0) {
				p = getBoundedContent(doc,"<ss:Worksheet",">","</ss:Worksheet>");
				if(p == null)
					break;
				sheet = p.content;
				doc = p.newString;
				rowList = new ArrayList ();
				while (sheet != null && sheet.length() > 0) {
					Pair p2 = getBoundedContent(sheet,"<Row",">","</Row>");
					if (p2 == null)
						break;
					row = p2.content;
					sheet = p2.newString;
					data = getDataCells(row);
					if (data != null)
						rowList.add(data);
				}
				sheetList.add(rowList);
			}
		}


		String[][][] result = new String [sheetList.size()][][];
		for (int i = 0; i < result.length; ++i) {
			ArrayList x = (ArrayList)sheetList.get(i);
			result[i] = new String [x.size()][];
			for (int j = 0; j < result[i].length; ++j) {
				String[] y = (String[])x.get(j);
				result[i][j] = new String [y.length];
				for (int k = 0; k < y.length; ++k)
					result[i][j][k] = y[k];
			}
		}
		return result;
	}	
	public boolean isReference(){
		return refStatus;
	}
	public String getReference(){
		return refPath;
	}
	public void setXSD(String xsd){
		xsdFile = xsd;
	}

	/**
	 * Syntax for a filter on a previously defined spreadsheet:
	 * 
	 * 		@reference = <base-directory-path-name> <base-assembly-name> = <new-assembly-name>
	 *  	Main
	 *  		[<row-entry>...]
	 *  	EndMain
	 *  	Monitor
	 *  		[<row-entry>...]
	 *  	EndMonitor
	 *  	Control
	 *      	[<row-entry>...]
	 *  	EndControl
	 *  	Archive
	 *      	[<row-entry>...]
	 *  	EndArchive
	 *  
	 *  where,
	 * 		row-entry := <monitor-or-control-point-name> <column-override>... |
	 * 					 ~<monitor-or-control-point-name>
	 *  	column-override := '<' column-name = value '>'
	 *  
	 *  Note:
	 *  	1. There must be no imbedded spaces or tabs in <base-directory-path-name>,
	 *  	<base-assembly-name>, or <new-assembly-name>.  
	 *  	2. There can be embedded spaces or tabs in column-name or value.
	 *  	3. The name '@refernce', 'Main', etc. mus begin in column 1 of the line.
	 *  	4. By default, all rows are included.  In the row-entry, the '~' in front of 
	 *  	the name means to delete that entire row.  The other option is used to modify 
	 *  	values of columns in a row.    
	 *  
	 *  Example:
	 *  	@reference = ../../FrontEnd/idl ColdCart = ColdCart6
 	 * 		Main
	 *		ColdCart <Base Address = 0x02000>
	 *		EndMain
	 *		Monitor
	 *		LO_SIS_VOLTAGE <Min = 4> <Max = 6>
	 *		~LO_SIS_CURRENT
	 *		EndMonitor
	 *		Control
	 *		SET_LO_VOLTAGE <Min = 4> <Max = 6>
	 *		EndControl
	 *		Archive
	 *		EndArchive
	 *
	 * This text must be in a file whose name is ColdCart6_spreadsheet.ext.
	 *
	 * @param doc A string containing the entire text as returned by 'getSpreadsheet'.
	 * @return The filtered worksheets as an array of strings.
	 */
	private String[][][] filter(String doc) {
		// A. Get the directory that is referenced, and the old and new assembly names.
		// Syntax: @reference = <base-directory-path-name> <base-assembly-name> = <new-assembly-name>
		LineNumberReader text = new LineNumberReader(new StringReader(doc));
		String line = null;
		try {
			line = text.readLine();
		} catch (IOException err) {
			throw new DesignError("Cannot read file: " + err.toString());
		}
		String[] tmp = line.split("[ |\t]+");
		if (!tmp[0].equals("@reference") || !tmp[1].equals("=") || !tmp[4].equals("=")) {
			throw new DesignError("Invalid syntax at line number " + text.getLineNumber() + " in file filter.");
		}
		String baseDirectory = tmp[2];
		String baseAssemblyName = tmp[3];
		String newAssemblyName = tmp[5];
		System.out.println("Applying filter in directory " + baseDirectory + 
				" to assembly " + baseAssemblyName + ".  New assembly is " + newAssemblyName + ".");
		
		// B. Get the contents of this base spreadsheet.
		String referenceXml = SpreadsheetParser.getSpreadsheet(baseDirectory, baseAssemblyName + "_spreadsheet.xml");
		refPath = baseDirectory + "/" + baseAssemblyName + "_spreadsheet.xml";
		SpreadsheetParser p = new SpreadsheetParser(referenceXml);
		SpreadsheetValidator v = new SpreadsheetValidator();
		if (!v.validate(refPath,xsdFile)) {
			String s = "Spreadsheet " + baseAssemblyName + "_spreadsheet.xml is not a valid spreadsheet.";
			throw new DesignError(s);
		}
		System.out.println("Base spreadsheet " + baseAssemblyName + " has been validated.");
		String[][][] base = p.getWorksheets();
		
		// C. Change the assembly name in the base spreadsheet.
		for (int i = 0; i < base.length; ++i) {
			for (int j = 0; j < base[i].length; ++j) {
				if (base[i][j][0].equals(baseAssemblyName)) {
					base[i][j][0] = newAssemblyName;
				}
			}
		}
			
		// D. Apply the filter to the base.
		//    Any deleted rows we will simply make null.
		String[][] worksheet = null;
		while (true) {
			try {
		 		line = text.readLine();
			} catch (IOException err) {
				throw new DesignError("Cannot read file: " + err.toString());
			}
			if (line == null)
				break;
			if (line.trim().length() == 0)
				continue;
			if (line.startsWith("Main"))
				worksheet = base[0];
			else if (line.startsWith("EndMain"))
				worksheet = null;
			else if (line.startsWith("Monitor"))
				worksheet = base[1];
			else if (line.startsWith("EndMonitor"))
				worksheet = null;
			else if (line.startsWith("Control"))
				worksheet = base[2];
			else if (line.startsWith("EndControl"))
				worksheet = null;
			else if (line.startsWith("Archive"))
				worksheet = base[3];
			else if (line.startsWith("EndArchive"))
				worksheet = null;
			else {
				if (worksheet == null)
					throw new DesignError("Invalid syntax at line number " + text.getLineNumber() + " in file filter. (Missing statement)");
				tmp = line.split("[ |\t]+");
				if (line.startsWith("~")) {
					deleteRow(worksheet,tmp[0].substring(1));
				} else {
					String s = line.substring(tmp[0].length());
					modifyRow(worksheet,text.getLineNumber(),tmp[0],s);
				}
			}
		}
		
		// E. Assign the non-null array.
		String[][][] result = new String [base.length] [] [];
		for (int i = 0; i < result.length; ++i) {
			int nrow = 0;
			for (int j = 0; j < base[i].length; ++j) {
				if (base[i][j] != null)
					++nrow;
			}
			result[i] = new String [nrow] [];
			int jj = 0;
			for (int j = 0; j < base[i].length; ++j) {
				if (base[i][j] != null) {
					result[i][jj] = new String [base[i][j].length];
					for (int k = 0; k < base[i][j].length; ++k)
						result[i][jj][k] = base[i][j][k];
					++jj;
				}
			}
		}
		
		// F. Return the new spreadsheet. 
		return result;
	}
	private void deleteRow(String[][] worksheet, String name) {
		if (worksheet[0][0].equals("Hardware Device")) {
			for (int i = 2; i < worksheet.length; ++i) {
				if (worksheet[i] == null)
					continue;
				if (worksheet[i][0].equals(name)) {
					worksheet[i] = null;
					return;
				}
			}
			throw new DesignError("The name " + name + " is not the name of a row in the Main worksheet.");
		}
		for (int i = 2; i < worksheet.length; ++i) {
			if (worksheet[i] == null)
				continue;
			if (worksheet[i][1].equals(name)) {
				worksheet[i] = null;
				return;
			}
		}		
		throw new DesignError("The name " + name + " is not the name of a row in the " + worksheet[0][0] + " worksheet.");
	}
	private void modifyRow(String[][] worksheet, int lineNumber, String rowName, String s) {
		// Find the row.
		int row = 0;
		if (worksheet[0][0].equals("Hardware Device")) {
			int i = 2;
			for (; i < worksheet.length; ++i) {
				if (worksheet[i] == null)
					continue;
				if (worksheet[i][0].equals(rowName)) {
					row = i;
					break;
				}
			}
			if (i == worksheet.length)
				throw new DesignError("The name " + rowName + " is not the name of a row in the Main worksheet.");
		} else {
			int i = 2;
			for (; i < worksheet.length; ++i) {
				if (worksheet[i] == null)
					continue;
				if (worksheet[i][1].equals(rowName)) {
					row = i;
					break;
				}
			}
			if (i == worksheet.length)
				throw new DesignError("The name " + rowName + " is not the name of a row in the " + worksheet[0][0] + " worksheet.");
		}
		String name;
		String value;
		int b;
		int m;
		int e;        				
		while(true) {
			b = s.indexOf('<');
			m = s.indexOf('=');
			e = s.indexOf('>');
			if (b == -1 || m == -1 || e == -1 || m < b || e < m) {
				throw new DesignError("Invalid syntax at line number " + lineNumber + " in file filter.");
			}
			name = s.substring(b + 1, m).trim();
			value = s.substring(m + 1, e).trim();
			// Apply the change.
			int j = 0;
			for (; j < worksheet[row].length; ++j) {
				if (worksheet[1][j].equals(name)) {
					worksheet[row][j] = value;
					break;
				}
			}
			if (j == worksheet[row].length)
				throw new DesignError("The name " + name + " is not the name of a column in the " + worksheet[0][0] + " worksheet.");
			// Get the next change.
			++e;
			while (e < s.length() && (s.charAt(e) == ' ' || s.charAt(e) == '\t'))
				++e;
			if (e == s.length())
				break;
			s = s.substring(e);
		}
		
	}
		
        
}
class Pair {
	Pair(String c,String n) {
		content = c;
		newString = n;
	}
	public String content;
	public String newString;
}
