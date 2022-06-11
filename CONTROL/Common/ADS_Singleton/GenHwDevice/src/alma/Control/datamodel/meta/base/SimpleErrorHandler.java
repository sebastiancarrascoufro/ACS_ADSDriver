package alma.Control.datamodel.meta.base;

import org.xml.sax.ErrorHandler;
import org.xml.sax.SAXParseException;

public class SimpleErrorHandler implements ErrorHandler {
	private boolean errors = false;
	public void error(SAXParseException exception) {
		System.out.println("error: line "+ exception.getLineNumber() + ": " + exception.getMessage());
		errors = true;
	}
	public void fatalError(SAXParseException exception) {
		System.out.println("fatalError: line "+ exception.getLineNumber() + ": " + exception.getMessage());
		errors = true;
	}
	public void warning(SAXParseException exception) {
		System.out.println("warning: line "+ exception.getLineNumber() + ": " + exception.getMessage());
		errors = true;
	}
	public boolean areErrors(){
		return errors;
	}
}
