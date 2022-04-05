package alma.Control.datamodel.meta.amb;

import alma.Control.datamodel.meta.base.SpreadsheetValidator;
import alma.Control.datamodel.meta.base.SpreadsheetParser;
import alma.Control.datamodel.meta.base.Util;

import org.openarchitectureware.core.constraint.DesignError;
import java.io.File;
/**
 * This class is on charge of look for GENERIC_spreadsheet.xml file that
 * contains all generic monitor, control and archive points defined. Furthermore
 * this class returns the spreadsheet parsed and the final worksheet with
 * generic monitor points added.
 * 
 * @version 	1.0
 * @author 	Pablo Burgos
 * @since       Aug-2008
 * 
 */

public class GenericMonitorPoints {
    private String[][][] newSpreadsheet;
    private String deviceName;
    private String dirLocation;
    
    private String[][][] getParsedGenericSpreadsheet(){
        dirLocation = Util.getInstallDir();
        if (!(new File(dirLocation + "/idl/GENERIC_spreadsheet.xml").exists())){
            System.out.println("Spreadsheet GENERIC_spreadsheet not found. Exiting....");
            System.exit(1);
        }
        
        String xml = SpreadsheetParser.getSpreadsheet(dirLocation + "/idl", "GENERIC_spreadsheet.xml");
        String xmlFile = dirLocation + "/idl/GENERIC_spreadsheet.xml";
        String xsdFile = dirLocation + "/config/schemas/amb/Workbook.xsd";
        SpreadsheetParser p = new SpreadsheetParser(xml);
        String [][][] spreadsheet = p.getWorksheets();
        SpreadsheetValidator v = new SpreadsheetValidator();
        if (!v.validate(xmlFile,xsdFile))
        {
            String s = "Spreadsheet GENERIC_spreadsheet.xml is not a valid spreadsheet.";
            throw new DesignError(s);
        }

        System.out.println("Spreadsheet GENERIC has been validated.");
        
        return spreadsheet;
     
    }
    private void setDeviceName(String s){
        deviceName = s;
    }
    public String getDeviceName(){
        return deviceName;
    }

    public String[][][] getDeviceWorksheetWithGenericPointsAdded(String[][][] spreadsheet){
        // Create the SP class
        setDeviceName(spreadsheet[0][2][0]);
        String [][][] genericSpreadsheet = getParsedGenericSpreadsheet();
        // Now we iterate through spreadsheet adding monitor, control and archive generic points.
        
        // I coded this way to make the things clear
        int[] Rows = null;
        int[] Columns = null;
        int[] GenericRows = null;
        
        Rows = new int[4];
        Columns = new int[4];
        GenericRows = new int[4];
        
        String deviceName=spreadsheet[0][2][0];
        
        int mainRows = spreadsheet[0].length;
        int monitorRows = spreadsheet[1].length;
        int controlRows = spreadsheet[2].length;
        int archiveRows = spreadsheet[3].length;
        
        int mainColumns = spreadsheet[0][1].length;
        int monitorColumns = spreadsheet[1][1].length;
        int controlColumns = spreadsheet[2][1].length;
        int archiveColumns = spreadsheet[3][1].length;
        
        int genericMonitorRows = genericSpreadsheet[1].length;
        int genericControlRows = genericSpreadsheet[2].length;
        int genericArchiveRows = genericSpreadsheet[3].length;
        
        // Well, first the newSpreadsheet is created. Arrays are static structures....
        newSpreadsheet = new String[4][][];
        newSpreadsheet[0] = new String[mainRows][mainColumns];
        newSpreadsheet[1] = new String[monitorRows+genericMonitorRows-2][monitorColumns];
        newSpreadsheet[2] = new String[controlRows+genericControlRows-2][controlColumns];
        newSpreadsheet[3] = new String[archiveRows+genericArchiveRows-2][archiveColumns];
        
	// Hardware Device
        newSpreadsheet[0][0] = new String[1];
        newSpreadsheet[0][0][0] = spreadsheet[0][0][0];
	// Monitor Point
        newSpreadsheet[1][0] = new String[1];
        newSpreadsheet[1][0][0] = spreadsheet[1][0][0];
	// Control Point
        newSpreadsheet[2][0] = new String[1];
        newSpreadsheet[2][0][0] = spreadsheet[2][0][0];
	// Archive Property
        newSpreadsheet[3][0] = new String[1];
        newSpreadsheet[3][0][0] = spreadsheet[3][0][0];
        
        Rows[0] = mainRows; // Yes here I ignore Notes on Main spreadsheet
        Rows[1] = monitorRows;
        Rows[2] = controlRows;
        Rows[3] = archiveRows;
        
        Columns[0] = mainColumns;
        Columns[1] = monitorColumns;
        Columns[2] = controlColumns;
        Columns[3] = archiveColumns;
        
        // Then I copy elements from spreadsheet to newSpreadsheet
        for (int i = 0; i < Rows.length; i++ ) {
            for (int j = 1; j < Rows[i]; j++) {
                for (int k = 0; k < Columns[i]; k++) {
                    if (spreadsheet[i][j][k].equals("Notes")){
                        newSpreadsheet[i][j] = new String[2];
                        newSpreadsheet[i][j][k] = spreadsheet[i][j][k];
                        newSpreadsheet[i][j][k+1] = spreadsheet[i][j][k+1];
                        break; 
                    }
                    newSpreadsheet[i][j][k] = spreadsheet[i][j][k];
                }

            }
        }        
        
        // Here we start the copy process from generic spreadsheet
        GenericRows[0] = 0;
        GenericRows[1] = genericMonitorRows;
        GenericRows[2] = genericControlRows;
        GenericRows[3] = genericArchiveRows;

        for (int i = 1; i < Rows.length ; i++ ) {
            for (int j = Rows[i];j < (Rows[i] + GenericRows[i] - 2); j++) {
                for (int k = 0; k < Columns[i]; k++) {
                    if ( (genericSpreadsheet[i][j-Rows[i]+2][k]).equals("GENERIC")){
                        newSpreadsheet[i][j][k] = deviceName;
                    } else {
                        newSpreadsheet[i][j][k] = genericSpreadsheet[i][j-Rows[i]+2][k];
                    }
                }
            }
        }
        return newSpreadsheet;
    }
}
