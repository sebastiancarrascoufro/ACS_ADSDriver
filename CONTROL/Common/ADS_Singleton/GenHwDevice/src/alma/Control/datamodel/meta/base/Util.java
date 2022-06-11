package alma.Control.datamodel.meta.base;

import java.io.File;
import java.io.FileOutputStream;
import java.io.FileReader;
import java.io.IOException;
import java.io.LineNumberReader;
import java.io.PrintStream;
import java.util.Iterator;
import java.util.ArrayList;

import org.openarchitectureware.core.meta.core.Element;
import org.openarchitectureware.core.meta.core.ElementSet;
import org.openarchitectureware.core.meta.core.Model;
import org.openarchitectureware.core.constraint.DesignError;

import alma.hla.runtime.asdm.types.ArrayTime;

public class Util{
	static private final String newline = System.getProperty("line.separator");
	static private final int maxLength = 54;
	private static void error(String msg) {
		System.out.println(msg);
		System.exit(0);
	}
	public static void RemoveLinesFromFile(String dirName, String filename, int replace){
		PrintStream out = null;
		LineNumberReader in = null;
		String line = null;
		
		File file = new File (dirName,filename);
		if (!file.isFile())
			error(file.getAbsolutePath() + " is not a file.");
		
		try {
			// Open the input file.
			in = new LineNumberReader(new FileReader(file));
			
			// Create the output file.
			String newName = file.getAbsolutePath() + ".tmp";
			out = new PrintStream (new FileOutputStream (new File(newName)));
			
			line = in.readLine();
			// Skip all blank lines at the beginning.
			while (true) {
				if (line != null && line.trim().length() == 0)
					line = in.readLine();
				else
					break;
			}
			boolean blankFound = false;
			while (line != null) {
				if (line.trim().length() == 0) {
					blankFound = true;
				} else {
					if (blankFound) {
						blankFound = false;
						if (replace == 1)
							out.println();
					}
					out.println(line);
				}
				line = in.readLine();
			}
			// Close the input file and the output file.
			in.close();
			out.close();
			// Delete the old file and rename the new file.
			if (!file.delete())
				error("Could not delete " + file.getAbsolutePath());
			File newFile = new File (newName);
			newFile.renameTo(file);
		}catch (IOException err) {
			error(err.toString());
		}
	}
    static public String AltName(String str) {
        if (str == null || str.length() == 0)
            return null;
        String s = str;
        StringBuffer x = new StringBuffer ();
        char c = s.charAt(0);
        x.append(Character.toUpperCase(c));
        for (int i = 1; i < s.length(); ++i) {
            c = s.charAt(i);
            if (c == '_') {
                do {
                    ++i;
                    if (i == s.length())
                        break;
                    c = s.charAt(i);
                } while (c == '_');
                if (i == s.length())
                    break;
                x.append(Character.toUpperCase(c));
            } else {
                x.append(Character.toLowerCase(c));
            }
        }
        return x.toString();
    }
    static private String formatDescriptionBase(String s, String prefix) {
        if (s == null)
            return "";
        String raw = decodeHTMLChars(s);
        StringBuffer extract = new StringBuffer ();
        int begin = 0;
        int current = 0;
        int len = raw.length();
        char c = 0;
        while (current < len) {
            c = raw.charAt(current);
            if (c == '&' && (current + 4) <= len && raw.charAt(current + 1) == '#' &&
                        raw.charAt(current + 2) == '1' && raw.charAt(current + 3) == '0' &&
                        raw.charAt(current + 4) == ';') {
                c = ' ';
                current += 4;
            }
            extract.append(c);
            if ((current - begin) > maxLength && c == ' ') {
                extract.append(newline);
                extract.append(prefix);
                begin = current + 1;
            }
            ++current;
        }
        return extract.toString();
    }
    static public String formatDescription(String s) {
        return formatDescriptionBase(s,"");
    }
    static public String formatDescriptionL1(String s) {
        return formatDescriptionBase(s,"/// ");
    }
    static public String formatDescriptionL2(String s) {
        return formatDescriptionBase(s,"    /// ");
    }
    static public String formatDescriptionL4(String s) {
        return formatDescriptionBase(s,"/// ");
    }
    static public String formatDescriptionL5(String s) {
        return formatDescriptionBase(s,"    /// ");
    }

    static public String descriptionAsString(String s) {
        if (s == null)
            return "";
        String raw = decodeHTMLChars(s);
        StringBuffer extract = new StringBuffer ();
        int current = 0;
        int len = raw.length();
        char c = 0;
        while (current < len) {
            c = raw.charAt(current);
            if (c == '&' && (current + 4) <= len && raw.charAt(current + 1) == '#' &&
                        raw.charAt(current + 2) == '1' && raw.charAt(current + 3) == '0' &&
                        raw.charAt(current + 4) == ';') {
                c = ' ';
                current += 4;
            }
            extract.append(c);
            ++current;
        }
        return extract.toString();
    }
    private static String decodeHTMLChars(String s) {
        if (s == null || s.length() == 0)
            return s;
        StringBuffer x = new StringBuffer();
        int index = 0;
        int index1 = 0;
        int index2 = 0;
        int index3 = 0;
        int index4 = 0;
        int current = 0;
        while (true) {
            // look for any HTML encoded character
            index1 = s.indexOf("&lt;", current);
            index2 = s.indexOf("&gt;", current);
            index3 = s.indexOf("&amp;", current);
            index4 = s.indexOf("&#xd;", current);
            // if there aren't any, we're done
            if (index1 == -1 && index2 == -1 && index3 == -1 && index4 == -1) {
                x.append(s.substring(current));
                break;
            }
            // set index to the least value that is not -1
            index = index1;
            if (index == -1 || (index2 != -1 && index2 < index))
                index = index2;
            if (index == -1 || (index3 != -1 && index3 < index))
                index = index3;
            if (index == -1 || (index4 != -1 && index4 < index))
                index = index4;
            // append the current text up to index
            x.append(s.substring(current,index));
            // append the correct character and skip over the HTML encoded characters
            if (index == index1) {
                x.append('<');
                current = index + 4;
            } else if (index == index2) {
                x.append('>');
                current = index + 4;
            } else if (index == index3) {
                x.append('&');
                current = index + 5;
            } else if (index == index4) {
                x.append('\n');
                current = index + 5;
            }
        }
        return new String (x);
    }
    static public Boolean isArray(String s) {
        return s.endsWith("]") ? true : false;
    }
    static public String NumberRawDataTypeBytes(String s) {
        return (String)Table.rawBytes.get(s);
    }

    static public String RawDataTypeTotalBytes(String s) {
        if (isArray(s)) {
				int b;
            int n = Integer.parseInt(NumberOfItems(s));
				if(s.startsWith("&lt;"))
					return "0";
				if(s.startsWith("["))
					return "0";
				int i = s.indexOf("[");
            b = Integer.parseInt((String)Table.rawBytes.get(s.substring(0,i)));
            String x = Integer.toString(n * b);
            return x;
        }
         return (String)Table.rawBytes.get(s);
    }
    static public String NumberOfItems(String s) {
        if (s.startsWith("&lt;"))
            return "1";
        if (!isArray(s))
            return "1";
        int i = s.indexOf("[");
        if (i == -1)
            throw new DesignError("Invalid syntax in array notation: (" + s + ")");
        String x = s.substring(i + 1, s.length() - 1);
        try {
            int n = Integer.parseInt(x);
        } catch (NumberFormatException err) {
            throw new DesignError("Invalid syntax in array notation: (" + s + ")");
        }
        return x;
    }
    static private int getRawDataTypeIndex(String s) {
        // Temporary <<<<<<<<<<<Must fix this bug<<<<<<<<<<<<<<<<<<<<<<<<
        //if (s.equals("~"))
            //return 5;
        // End Temporary <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
        if (s.startsWith("&lt;"))
            return 0;
        if (s.startsWith("["))
            return 0; // This isn't really true, but it will pass the validation.
        String x = s;
        if (isArray(s)) {
            int i = s.indexOf("[");
            if (i == -1)
                throw new DesignError("Invalid syntax in array notation: (" + s + ")");
            x = s.substring(0, i);
        }
        //int i = 0;
        //for (; i < Table.RawDataType.length; ++i) {
        //    if (Table.RawDataType[i].equals(x))
        //        break;
        //}
        //if (i < Table.RawDataType.length)
        //    return i;
			return 0;
        //throw new DesignError("Unknown RawDatatype: (" + s + ")");
    }
    public static String toArrayTime(String s) {
    	//The required string date form is "YYYY-MM-DDThh:mm:ss.ssss".
    	ArrayTime t = new ArrayTime ("20" + parseDateString(s,true) + "T00:00:00.0");
    	long l = t.get();
    	return Long.toString(l);
    }
    public static String toDatabaseDate(String s) {
    	return parseDateString(s,false);
    }
	private static final String [] MONTH = {"JAN", "FEB", "MAR", "APR", "MAY", "JUN", "JUL", "AUG", "SEP", "OCT", "NOV", "DEC"};  
	private static final String [] MONTHalt = {"01", "02", "03", "04", "05", "06", "07", "08", "09", "10", "11", "12"};  
    public static String parseDateString(String s, boolean alt) {
    	int pos = 0;
    	int n = s.indexOf('-'); 	if (n == -1) return null;
    	String yy = s.substring(pos,n);
    	if (yy.startsWith("20"))
    		yy = yy.substring(2);
    	pos = n + 1;
    	n = s.indexOf('-',pos); 	if (n == -1) return null;
    	String mm = s.substring(pos,n);
    	int nmm = 0;
    	try {
    		nmm = Integer.parseInt(mm);
    	} catch (NumberFormatException err) {
    		return null;
    	}
    	if (nmm < 1 || nmm > 12) return null;
    	pos = n + 1;
    	n = s.indexOf('T',pos); 	if (n == -1) return null;
    	String dd = s.substring(pos,n);
    	return yy + '-' + (alt ? MONTHalt[nmm - 1] : MONTH[nmm - 1]) + '-' + dd;
    }
   public static String normalizeNumber(String wdt, String value) {
	   if (wdt.equals("boolean"))
		   return normalizeBoolean(value);
	   else if (wdt.equals("double") || wdt.equals("float"))
		   return normalizeFloat(value);
	   else
		   return value;
   }
	public static String normalizeFloat(String value){
	   try {
          // Does value contain a valid "double" or "float"?
		  Double.parseDouble(value);
          // Does value contain a decimal point?
          if (-1 == value.indexOf('.')) {
        	  // No decimal point found.
		      // Does value contain an exponent?
		      int i = value.indexOf('E');
		      if (-1 != i) {
			      // Insert the ".0" before the "E".
			      String parts[] = value.split("E");
			      return parts[0] + ".0E" + parts[1];
		       } else {
		    	   // No exponent found.
			       // Append the ".0" to the end.
                   return value + ".0";
		       }
	       } else {
	    	   // value contains a decimal point.  Return it without changes.  
	           return value;
	       }
       } catch (NumberFormatException de) {
           try {
               // Does this contain a valid "integer" or "long"?
        	   Integer.parseInt(value);
        	   // Add the missing decimal point and trailing "0".
        	   return value + ".0";
       	   } catch (NumberFormatException ie) {
       		   // This is not a number.
       		   // Return it as is.
       		   return value;
       	   }
       }
	}
   public static String normalizeBoolean(String value) {
	   int indexOfDot = value.indexOf('.');
	   if (-1 == indexOfDot) {
		   return value;
	   } else {
		   return value.substring(0,indexOfDot);
	   }
   }

    //2010-10-08 ntroncos: Found that this method does did not check for INTLIST, so i went and added
    //    support for it. While at it I put in some check to avoid returning null. Since:
    //    Exception in thread "main" java.lang.IllegalArgumentException: Couldn't load file null/config/workflow/workflow.oaw
    //    Looks to damn ugly. So now if everything goes bad, and at the end off the method dirLocation is still NULL 
    //    I will assert.
    //    BTW: Creating a new file object each time looks inefficient has shit (http://xkcd.com/798/) sombody 
    //    ought TODO somthing about it. 
   public static String getInstallDir() {
       String dirLocation = null;
       String acsroot = System.getenv("ACSROOT");
       String intlist = System.getenv("INTLIST");
       String introot = System.getenv("INTROOT");

       if (acsroot != null && new File(acsroot + "/lib/ControlGenHwDevice").exists()) {
           dirLocation = acsroot + "/lib/ControlGenHwDevice";
       }

       if (intlist != null ) {
           String dirs[] = intlist.split(":");
           for (int i = 0; i < dirs.length; i++) {
               if (new File(dirs[i] + "/lib/ControlGenHwDevice").exists()) {
                   dirLocation = dirs[i] + "/lib/ControlGenHwDevice";
                   break;
               }
           }

       }

       if (introot != null && new File(introot + "/lib/ControlGenHwDevice").exists()) {
           dirLocation = introot + "/lib/ControlGenHwDevice";
       }  

       assert dirLocation != null : "dirLocation is null, this means that I cannot find lib/ControlGenHwDevice";
       return dirLocation;
   }
}
