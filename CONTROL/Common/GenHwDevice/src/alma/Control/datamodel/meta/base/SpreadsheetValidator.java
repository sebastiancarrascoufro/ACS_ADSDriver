package alma.Control.datamodel.meta.base;

import java.io.IOException;
import javax.xml.parsers.DocumentBuilder;
import javax.xml.parsers.DocumentBuilderFactory;
import javax.xml.parsers.ParserConfigurationException;
import org.w3c.dom.Document;
import org.w3c.dom.Node;
import org.xml.sax.SAXException;
import javax.xml.validation.*;
import javax.xml.transform.*;
import javax.xml.transform.stream.*;
import javax.xml.transform.dom.*;
import java.io.File;

import java.util.ArrayList;
import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Method;
import org.openarchitectureware.core.constraint.DesignError;

public class SpreadsheetValidator
{
    SimpleErrorHandler seh;

    private ArrayList< String > errorList;

    public SpreadsheetValidator()
    {
        this.errorList = new ArrayList< String >();
    }

    public boolean validate(String xmlFile, String xsdFile)
    /*
     * throws ParserConfigurationException , SAXException, IOException
     */
    {
        DocumentBuilderFactory factory = DocumentBuilderFactory.newInstance();
        factory.setNamespaceAware(true);
        factory.setValidating(true);
        factory.setAttribute(
                "http://java.sun.com/xml/jaxp/properties/schemaLanguage",
                "http://www.w3.org/2001/XMLSchema");
        // Specify our own schema - this overrides the schemaLocation in the xml
        // file
        factory.setAttribute(
                "http://java.sun.com/xml/jaxp/properties/schemaSource",
                "file://" + xsdFile);
        seh = new SimpleErrorHandler();

        try
        {
            DocumentBuilder builder = factory.newDocumentBuilder();
            builder.setErrorHandler(seh);
            Document document = builder.parse(xmlFile);
        }
        catch(Exception e)
        {
            System.out.println("Validating error file " + xmlFile
                    + " with schema file " + xsdFile + " failed!");
            return false;
        }

        if(seh.areErrors())
        {
            System.out.println("Validating error file " + xmlFile
                    + " with schema file " + xsdFile + " failed!");
            return false;
        }
        else
        {
            return true;
        }
    }
}
